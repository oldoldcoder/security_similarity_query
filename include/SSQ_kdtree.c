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
RESULT kdtree_init(kd_tree * root,int dim, int n){
    root->dim = dim;
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
    BN_set_word(tmp,node->divide_dim);
    if(node->divide_val == NULL){
        node->divide_val = BN_CTX_get(CTX);
    }
    // 计算得到的分割值
    BN_div(node->divide_val,NULL,sum,tmp,CTX);

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
            int dim = da->dim;
            if(da->en_val == NULL)
                da->en_val = (eTPSS **) malloc(sizeof (eTPSS *) * dim);
            if(da->en_xi == NULL)
                da->en_xi = (eTPSS **) malloc(sizeof (eTPSS *) * dim);
            for(int i = 0 ; i < dim ; ++i){
                eTPSS * a = (eTPSS *) malloc(sizeof (eTPSS));
                eTPSS * b = (eTPSS *) malloc(sizeof (eTPSS));
                init_eTPSS(a);
                init_eTPSS(b);
                et_Share(a,da->val[i]);
                et_Share(b,da->xi[i]);
                da->en_val[i] = a;
                da->en_xi[i] = b;
;            }
            tmp = tmp->next;
        }
    }else{
        node->en_divide_dim = (eTPSS * ) malloc(sizeof (eTPSS));
        node->en_divide_val = (eTPSS * ) malloc(sizeof (eTPSS));
        init_eTPSS(node->en_divide_dim);
        init_eTPSS(node->en_divide_val);
        BIGNUM * tmp = BN_CTX_get(CTX);
        BN_set_word(tmp,node->divide_dim);
        et_Share(node->en_divide_val,node->divide_val);
        et_Share(node->en_divide_dim,tmp);

        BN_clear(tmp);

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
RESULT kdtree_init_search(search_req * req,search_resp * resp,int dim,SSQ_data * kArr){
    FILE  * file = fopen(RSQ_FILE_PATH,"r");
    if(file == NULL){
        perror("Error opening file");
        return ERROR;
    }
    req->dim = dim;
    // 填充Y的值
    req->y = (eTPSS **) malloc(dim * sizeof(eTPSS *));
    req->range = (eTPSS ***) malloc(dim * sizeof (eTPSS **));
    // 填充数值
    for(int i = 0 ; i < dim ; ++i){
        eTPSS  * d  = (eTPSS *) malloc(sizeof (eTPSS));
        init_eTPSS(d);
        // 读取内容
        req->y[i] = d;

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
    BN_clear(t);

    /*---------------读取数据y----------------*/
    char *line = NULL;
    size_t len = 0;
    getline(&line, &len, file);
    int idx = 0;
    char *token = strtok(line, ",");
    while (token != NULL) {
        BIGNUM *tmp = BN_CTX_get(CTX);
        fflush(stdout);
        // 转换为BIGNUM函数
        if(!BN_dec2bn(&tmp,token)){
            fprintf(stdout,"file have some error format about data\n");
            return ERROR;
        }
        et_Share(req->y[idx++],tmp);
        BN_clear(tmp);
        token = strtok(NULL, ",");
    }
    if(idx != dim){
        fprintf(stderr,"Error reading data dimension from file\n");
        return ERROR;
    }
    /*---------------计算范围查询的数据----------------*/
    for(int i = 0 ; i < kArr->n ; ++i){
        // kArr是否也要加密0231231531122
    }

    /*---------------设置数据resp----------------*/
    resp->root = resp->now = NULL;
    return SUCCESS;
}
// 进行查询
RESULT kdtree_range_search(kd_tree * root, search_req * req, search_resp * resp){
    //
}
// 销毁查询的参数
RESULT kdtree_free_search_param(search_req * req,search_resp * resp){

}
// 转换参数到文件中去
RESULT kdtree_tsf_resp_to_file(search_resp * resp){

}