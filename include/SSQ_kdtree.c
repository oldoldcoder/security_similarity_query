#include "openssl//bn.h"
#include "eTPSS.h"
#include "utils.h"
#include "stdio.h"
#include "stdlib.h"
#include "SSQ.h"
#include "SSQ_kdtree.h"

/**----------------------方法实现-----------------------*/
static void swap_v_data(v_data ** data,int i,int j){
    v_data * t = data[i];
    data[i] = data[j];
    data[j] = t;
}

// 树的初始化
RESULT kdtree_init(kd_tree * root,int dim){
    root->dim = dim;
    root->root = (tree_node *) malloc(sizeof (tree_node) );
    kdtree_node_init(root->root,FALSE,-1,NULL,dim,0,NULL,NULL);
    return SUCCESS;
}
// 树的节点的初始化
RESULT kdtree_node_init(tree_node * node,int is_leaf_node,int divide_dim,BIGNUM * divide_val,int dim,int dir,tree_node * left,tree_node * right){
    node->is_leaf_node = is_leaf_node;
    node->dir  =dir;
    // 如果不是叶子节点
    if(is_leaf_node != TRUE) {
        node->divide_dim = divide_dim;
        node->divide_val = divide_val;
    }
    node->dim = dim;
    node->left = left;
    node->right = right;
    return SUCCESS;
}
// 创建树
RESULT kdtree_create(tree_node * node,v_data ** total,int n,int dim,int dir,int idx_lef,int idx_rig){
    // 判断是否到达我们的容量，到达容量不需要再次分割
    if(n <= LEAF_NODE_MAX_CAP){
        node->is_leaf_node = TRUE;
        // 对node进行初始化
        node->right = NULL;
        node->left = NULL;
        node->dim = dim;
        node->dir = dir;
        for(int i = idx_lef ; i <= idx_rig ; ++i){
            leaf_val * t = (leaf_val *) malloc(sizeof (leaf_val));
            t->next = NULL;
            t->val = total[i];
            if(node->data_root == NULL) {
                node->data_root = t;
                node->data_now = t;
            }else{
                node->data_now->next = t;
                node->data_now = t;
            }
        }
        // 结构体不需要再给其他的赋值，我们直接返回SUCCESS
        return SUCCESS;
    }

    // 获取切割的维度
    node->divide_dim = (dir + 1) % dim;
    BIGNUM * sum = BN_CTX_get(CTX);
    BN_set_word(sum,0);
    // 计算切割维度的平均值
    for(int i = idx_lef ; i <= idx_rig ; ++i){
        // 将目标维度的所有值加和
        BN_add(sum,sum,total[i]->val[node->divide_dim]);
    }

    BIGNUM  * tmp = BN_CTX_get(CTX);
    BN_set_word(tmp,idx_rig - idx_lef + 1);
    if(node->divide_val == NULL){
        node->divide_val = BN_CTX_get(CTX);
    }
    // 计算得到的分割值
    BN_div(node->divide_val,NULL,sum,tmp,CTX);
    // debug
    printDebugInfo(node->divide_val,NULL,__func__ ,__LINE__,"分割的divide的值");

    int idxL = idx_lef,idxR = idx_rig;
    while(idxL <= idxR){
        int res = 0;
        // 查找左边需要确定的点,大于的，需要移动过去的
        while(idxL <= idxR){
            res = BN_cmp(node->divide_val,total[idxL]->val[node->divide_dim]);
            // 如果可以放在左边
            if(res == -1){
                break;
            }else{
                ++idxL;
            }
        }
        // 查找右边的点位,小于的，需要移动到左边的
        while(idxL <= idxR){
            res = BN_cmp(node->divide_val,total[idxR]->val[node->divide_dim]);
            if(res == 1){
                break;
            }else{
                --idxR;
            }
        }
        if(idxL <= idxR){
            // 确定了两个点，进行交换位置，同时移动指针
            swap_v_data(total,idxL,idxR);
            idxL ++;
            idxR --;
        }
    }
    /**
     * idxR此刻指向的位置是下一次切分的中点
     * 目前需要填充当前内部节点的数据
     * */
    kdtree_node_init(node,FALSE,node->divide_dim,node->divide_val,dim,dir,(tree_node * ) malloc(sizeof (tree_node)),(tree_node * ) malloc(sizeof (tree_node)));

    if(node->left == NULL || node->right == NULL){
        fprintf(stderr,"Memory allocation failed.\n");
        return ERROR;
    }
    if (kdtree_create(node->left, total, idxR - idx_lef + 1, dim, dir + 1, idx_lef, idxR) != SUCCESS){
        return ERROR;
    }

    if(kdtree_create(node->right,total,idx_rig - idxL + 1,dim,dir + 1,idxL,idx_rig) != SUCCESS){
        return ERROR;
    }
    BN_clear(tmp);
    BN_clear(sum);
    return SUCCESS;
}
// 销毁树
RESULT kdtree_destory_tree(kd_tree * tree){
    // 后续再做
    // TODO 执行树的销毁
    return SUCCESS;
}
static void kdtree_encode_node(tree_node * node){
    if(node == NULL)
        return ;
    // 只需要遍历叶子节点就行
    if(node->is_leaf_node == TRUE){
        leaf_val *tmp = node->data_root;
        while(tmp != NULL){
            v_data * da = tmp->val;
            int kArr_dim = da->dim;
            int xi_dim = da->xi_dim;
            if(da->en_val == NULL)
                da->en_val = (eTPSS **) malloc(sizeof (eTPSS *) * kArr_dim);
            if(da->en_xi == NULL)
                da->en_xi = (eTPSS **) malloc(sizeof (eTPSS *) * xi_dim);
            for(int i = 0 ; i < kArr_dim ; ++i){
                eTPSS * a = (eTPSS *) malloc(sizeof (eTPSS));
                init_eTPSS(a);
                et_Share(a,da->val[i]);
                da->en_val[i] = a;
;            }
            for(int i = 0 ; i < xi_dim ; ++i){
                eTPSS * b = (eTPSS *) malloc(sizeof (eTPSS));
                init_eTPSS(b);
                et_Share(b,da->xi[i]);
                da->en_xi[i] = b;
            }
            tmp = tmp->next;
        }
    }else{
        // TODO 划分维度的选择
        node->en_divide_dim = (eTPSS ** ) malloc(node->dim * sizeof (eTPSS *));
        node->en_divide_val = (eTPSS * ) malloc(sizeof (eTPSS));
        for(int i = 0 ; i < node->dim ; ++i){
            node->en_divide_dim[i] = (eTPSS *) malloc(sizeof (eTPSS));
            init_eTPSS(node->en_divide_dim[i]);
            if(i != node->divide_dim){
                et_Share(node->en_divide_dim[i],ZERO);
            }else{
                et_Share(node->en_divide_dim[i],ONE);
            }
        }
        init_eTPSS(node->en_divide_val);
        et_Share(node->en_divide_val,node->divide_val);
        kdtree_encode_node(node->right);
        kdtree_encode_node(node->left);
    }
}
// 加密树上传服务器，假方法，只加密不上传
RESULT kdtree_upload_server(kd_tree * tree){
    // 加密过程
    tree_node * node = tree->root;
    if(node == NULL){
        fprintf(stderr,"加密树阶段树为空\n");
        return ERROR;
    }
    kdtree_encode_node(node);
    return SUCCESS;
}
// 初始化查询的参数
RESULT kdtree_init_search(search_req * req,search_resp * resp,SSQ_data * kArr,int ddim,int kn){
    FILE  * file = fopen(RSQ_FILE_PATH,"r");
    if(file == NULL){
        perror("Error opening file");
        return ERROR;
    }
    req->dim = ddim;
    req->range_dim = kn;
    // 填充Y的值
    req->y = (eTPSS **) malloc(ddim * sizeof(eTPSS *));
    req->range = (eTPSS ***) malloc(kn * sizeof (eTPSS **));
    // 填充数值
    for(int i = 0 ; i < ddim ; ++i){
        eTPSS  * d  = (eTPSS *) malloc(sizeof (eTPSS));
        init_eTPSS(d);
        // 读取内容
        req->y[i] = d;
    }
    for(int i = 0 ; i < kn ; ++i){
        req->range[i] = (eTPSS **) malloc(2 * sizeof (eTPSS *));
        req->range[i][0] = (eTPSS *) malloc(sizeof (eTPSS));
        req->range[i][1] = (eTPSS *) malloc(sizeof (eTPSS));
        init_eTPSS(req->range[i][0]);
        init_eTPSS(req->range[i][1]);
    }

    /*---------------读取数据tao----------------*/
    BIGNUM *t = BN_CTX_get(CTX);
    char tao[100];
    // 读取tao
    fscanf(file,"%s\n",tao);
    if(! BN_dec2bn(&t,tao)){
        fprintf(stdout,"file have some error format about data\n");
        return ERROR;
    }
    req->tao = (eTPSS *) malloc(sizeof (eTPSS));
    init_eTPSS(req->tao);
    et_Share(req->tao,t);

    /*---------------读取数据y----------------*/
    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, file);
    int idx = 0;
    char *token = strtok(line, ",");
    // 未加密的y的数据
    BIGNUM ** de_y_data = (BIGNUM **) malloc(sizeof (BIGNUM *) * ddim);
    while (token != NULL) {
        BIGNUM *tmp = BN_CTX_get(CTX);
        fflush(stdout);
        // 转换为BIGNUM函数
        if(!BN_dec2bn(&tmp,token)){
            fprintf(stdout,"file have some error format about data\n");
            return ERROR;
        }
        de_y_data[idx]  = tmp;
        et_Share(req->y[idx++],tmp);
        token = strtok(NULL, ",");
    }
    if(idx != ddim){
        fprintf(stderr,"Error reading data dimension from file\n");
        return ERROR;
    }
    /*---------------计算范围查询的数据----------------*/
    BIGNUM *tmp = BN_CTX_get(CTX);
    BIGNUM * sum = BN_CTX_get(CTX);
    BIGNUM * tmp2 = BN_CTX_get(CTX);
    BIGNUM * sqrt_sum = BN_CTX_get(CTX);
    for(int i = 0 ; i < kArr->n ; ++i){
        BN_set_word(sum,0);
        // 计算range 的范围
        for(int j = 0 ; j < ddim ; ++j){
            BN_sub(tmp,kArr->total_data[i]->single_data[j],de_y_data[j]);
            BN_mul(tmp2,tmp,tmp,CTX);
            BN_add(sum,sum,tmp2);
        }
        printDebugInfo(sum,NULL,__func__ ,__LINE__,"未平方根之前的值");
        // 计算减法的平方
        sqrt_bignum(sum,sqrt_sum);
        printDebugInfo(sqrt_sum,NULL,__func__ ,__LINE__,"平方根之后的数值");
        BN_sub(tmp,sqrt_sum,t);
        BN_mul(sum,tmp,tmp,CTX);
        printDebugInfo(sum,NULL,__func__ ,__LINE__,"-tao的数值");
        et_Share(req->range[i][0],sum);

        // 加法的时候为了扩大给sqrt_sum加1
        BN_add_word(sqrt_sum,1);
        BN_add(tmp,sqrt_sum,t);
        BN_mul(sum,tmp,tmp,CTX);
        printDebugInfo(sum,NULL,__func__ ,__LINE__,"+tao的数值");

        et_Share(req->range[i][1],sum);

    }
    BN_clear(tmp);
    BN_clear(tmp2);
    BN_clear(sum);
    BN_clear(t);
    BN_clear(sqrt_sum);
    for(int i = 0 ; i < ddim ; ++i){
        BN_clear(de_y_data[i]);
    }

    /*---------------设置数据resp----------------*/
    resp->root = resp->now = NULL;
    resp->dim = req->dim;
    return SUCCESS;
}
/**
 * 通过加密之后的选择维度和我们的加密量来判断大小，主要是Search的时候选择走入的子树
 * ret 1 divide_val > 最大值
 * ret 0 最小值 < divide_val < 最大值
 * ret -1 divide_val < 最小值
 * ret -2 出现了错误
 * */
static int judge_direct(eTPSS ** dim_vector,eTPSS * divide_val,eTPSS *** range,int dim){

    // res1代表了对应分割维度的最小值与val的比较结果，res2代表了对应分割维度最大值和val的比较结果
    int res1 = 0,res2 = 0;
    eTPSS tmp,sum;
    init_eTPSS(&sum);
    init_eTPSS(&tmp);
    et_Share(&sum,ZERO);
    printDebugInfo(NULL,divide_val,__func__ ,__LINE__,"divide val的数值");
    for(int i = 0 ; i < dim ; ++i){
        et_Mul(&tmp,dim_vector[i],range[i][0]);
        et_Add(&sum,&sum,&tmp);
    }

    printDebugInfo(NULL,&sum,__func__ ,__LINE__,"最小值的加和");
    // 判断大小
    et_Sub(&res1,divide_val,&sum);
    et_Share(&sum,ZERO);
    for(int i = 0 ; i < dim ; ++i){
        et_Mul(&tmp,dim_vector[i],range[i][1]);
        et_Add(&sum,&sum,&tmp);
    }

    printDebugInfo(NULL,&sum,__func__ ,__LINE__,"最大值的加和");
    et_Sub(&res2,divide_val,&sum);
    if(res1 == 0 && res2 == 0)
        return 1;
    else if((res1 == 0 &&res2 == 1) || (res1 == -1 &&res2 == 1) || (res1 == 0 && res2 == -1) || (res1 == -1 && res2 == -1))
        return 0;
    else if(res1 == 1 && res2 == 1)
        return -1;

    free_eTPSS(&sum);
    free_eTPSS(&tmp);
    return -2;
}
static void kdtree_range_search_o(tree_node * node,search_req * req,search_resp * resp){
    if(node == NULL)
        return;
    if(node->is_leaf_node == TRUE){
        // 如果是根节点的情况,判断距离的平方是否小于等于τ的平方
        leaf_val * tmp = node->data_root;
        eTPSS taoo;
        init_eTPSS(&taoo);
        et_Mul(&taoo,req->tao,req->tao);
        eTPSS t1,t2,sum;
        init_eTPSS(&t1);
        init_eTPSS(&t2);
        init_eTPSS(&sum);
        while(tmp != NULL){
            leaf_val  * next = tmp->next;
            // 计算距离
            et_Share(&sum,ZERO);
            for(int i = 0 ; i < tmp->val->xi_dim ; ++i){
                et_Sub_cal_res_o(&t1,tmp->val->en_xi[i],req->y[i]);
                // 乘法
                et_Mul(&t2,&t1,&t1);
                // 加和
                et_Add(&sum,&sum,&t2);
            }
            int ret = -2;
            // 判断大小,小于等于tao平方加入我们的结果
            et_Sub(&ret,&sum,&taoo);
            if(ret != 0){
                res_node * new = (res_node *) malloc(sizeof (res_node));
                new->next = NULL;
                // 这个节点数据的加密数据
                new->data = tmp->val->en_xi;
                if(resp->root == NULL) {
                    resp->root = new;
                    resp->now = new;
                }else{
                    resp->now->next = new;
                    resp->now = new;
                }
            }
            tmp = next;
        }

        return;
    }
    eTPSS * divide_val = node->en_divide_val;
    eTPSS ** dim_vector = node->en_divide_dim;
    // 判断树的走的方向
    int ret = judge_direct(dim_vector,divide_val,req->range,node->dim);
    if(ret == -2){
        fprintf(stderr,"判断查询走向出现了计算异常\n");
        return;
    }
    if(ret == 1){
        // 查询左子树
        kdtree_range_search_o(node->left,req,resp);
    }else if(ret == 0){
        kdtree_range_search_o(node->right,req,resp);
        kdtree_range_search_o(node->left,req,resp);
    }else {
        kdtree_range_search_o(node->right, req, resp);
    }

}
// 进行查询
RESULT kdtree_range_search(kd_tree * root, search_req * req, search_resp * resp){
    if(root->root == NULL){
        fprintf(stderr,"传递进来的树为空\n");
        return ERROR;
    }
    if(req->range == NULL){
        fprintf(stderr,"req和resp未进行初始化\n");
        return ERROR;
    }
    kdtree_range_search_o(root->root,req,resp);
    return SUCCESS;
}
// 销毁查询的参数
RESULT kdtree_free_search_param(search_req * req,search_resp * resp){
    // 主要销毁req内部的参数
    int dim = req->dim;
    free_eTPSS(req->tao);
    free(req->tao);
    for(int i = 0 ; i < dim ; ++i){
        free_eTPSS(req->y[i]);
        free(req->y[i]);
    }
    free(req->y);
    for(int i = 0 ; i < dim ; ++i){
        free_eTPSS(req->range[i][0]);
        free_eTPSS(req->range[i][1]);
        free(req->range[i][0]);
        free(req->range[i][1]);
        free(req->range[i]);
    }
    free(req->range);
    // 释放resp的参数
    res_node * node = resp->root;
    while(node != NULL){
        res_node * tmp = node->next;
        free(node);
        node = tmp;
    }
    return SUCCESS;
}
// 转换参数到文件中去
RESULT kdtree_tsf_resp_to_file(search_req  * req,search_resp * resp){
    FILE  * file = fopen(RESP_FILE_PATH,"w");
    if(file == NULL){
        perror("Error opening file");
        return ERROR;
    }
    int dim  = resp->dim;
    BIGNUM * tmp = BN_CTX_get(CTX);
    char *str ;
    fprintf(file,"The search rep\ndim:%d\nreq data: ",resp->dim);
    for(int i = 0 ; i < dim ; ++i){
        et_Recover(tmp,req->y[i]);
        str = BN_bn2dec(tmp);
        fprintf(file,"%s ",str);
    }
    et_Recover(tmp,req->tao);
    str = BN_bn2dec(tmp);
    fprintf(file,"\ntao: %s\nsearch res:\n",str);
    res_node *root = resp->root;
    int idx = 0;
    while(root != NULL){
        fprintf(file,"%d. ",idx);
        for(int i = 0 ; i < dim ; ++i){
            et_Recover(tmp,root->data[i]);
            str = BN_bn2dec(tmp);
            fprintf(file,"%s ",str);
        }
        idx++;
        fprintf(file,"\n");
        root = root->next;
    }
    // 关闭文件
    fclose(file);
    return SUCCESS;
}