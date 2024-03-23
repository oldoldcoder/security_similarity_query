
#include "openssl//bn.h"
#include "eTPSS.h"
#include "utils.h"
#include "stdio.h"
#include "stdlib.h"
#include "SSQ.h"
#include "string.h"
/**------------------------方法的实现---------------------------*/
// 读取数据
RESULT RSQ_read_data(SSQ_data * data){
    FILE  * file = fopen(PTREEB_DATA_FILE,"r");
    if(file == NULL){
        perror("Error opening file");
        return ERROR;
    }
    char *line = NULL;
    size_t len = 0;
    // 初始化data
    int totalLines, dim;
    fscanf(file, "%d %d\n", &totalLines, &dim); // 读取第一行中的两个数字
    data->n = totalLines;
    data->dim = dim;
    data->total_data = (dim_data **) malloc(totalLines * sizeof (dim_data *));
    if(data->total_data == NULL){
        fprintf(stderr,"Memory allocation failed.\n");
        return ERROR;
    }
    for(int i = 0 ; i < totalLines ; i++){
        data->total_data[i] = (dim_data *) malloc(sizeof (dim_data));
        data->total_data[i]->dim = dim;
        // 分配内存空间
        data->total_data[i]->single_data = (BIGNUM **) malloc(dim * sizeof (BIGNUM *));
        if(data->total_data[i] == NULL || data->total_data[i]->single_data == NULL ){
            fprintf(stderr,"Memory allocation failed.\n");
            return ERROR;
        }
    }

    int n = 0;
    while (getline(&line, &len, file) != -1) {
        int idx = 0;
        char *token = strtok(line, ",");
        while (token != NULL) {
            BIGNUM *tmp = BN_CTX_get(CTX);
            fflush(stdout);
            // 转换为BIGNUM函数
            // TODO 注意负数的处理
            if(!BN_dec2bn(&tmp,token)){
                fprintf(stdout,"file have some error format about data\n");
                return SUCCESS;
            }
            data->total_data[n]->single_data[idx++] = tmp;
            token = strtok(NULL, ",");
        }
        if(idx != dim){
            fprintf(stderr,"Error reading data dimension from file\n");
            return ERROR;
        }
        n++;
    }
    if(n != totalLines){
        fprintf(stderr,"Error reading data rows from file\n");
        return ERROR;
    }

    fclose(file);
    return SUCCESS;
}
// 释放数据
RESULT RSQ_free_data(SSQ_data * data){
    int n = data->n;
    int dim = data->dim;
    for(int i = 0 ; i < n ; ++i){
        for(int j = 0; j < dim ; ++j){
            BN_clear(data->total_data[i]->single_data[j]);
        }
        // BIGNUM ** 数组里面的值由CTX清理
        free(data->total_data[i]->single_data);
        data->total_data[i]->single_data = NULL;

        free(data->total_data[i]);
        data->total_data[i] = NULL;

    }
    free(data->total_data);
    data->total_data = NULL;
}
// 随机生成K个参考点
RESULT RSQ_generate_k_ref_points(SSQ_data  * kArr,int k,int dim){
    // 随机生成K个参考点
    kArr->n = k;
    kArr->dim = dim;
    // 生成这些点
    kArr->total_data = (dim_data **) malloc(sizeof (dim_data *) * k);
    if(kArr->total_data == NULL){
        fprintf(stderr,"Memory allocation failed.\n");
        return ERROR;
    }
    for(int i = 0 ; i < k ; i++){
        kArr->total_data[i] = (dim_data *) malloc(sizeof (dim_data));
        kArr->total_data[i]->dim = dim;
        // 分配内存空间
        kArr->total_data[i]->single_data = (BIGNUM **) malloc(dim * sizeof (BIGNUM *));
        if(kArr->total_data[i] == NULL || kArr->total_data[i]->single_data == NULL ){
            fprintf(stderr,"Memory allocation failed.\n");
            return ERROR;
        }
        // 在dim范围内给随机值
        for(int j = 0 ; j < dim ; ++j){
            BIGNUM * rn = BN_CTX_get(CTX);
            if(!BN_rand_range(rn,RANDOM_RANGE)){
                fprintf(stderr,"随机生成BIGNUM出错\n");
                return ERROR;
            }
            kArr->total_data[i]->single_data[j] = rn;
        }
    }

    return SUCCESS;
}
// 计算为Vi向量的集合
RESULT RSQ_compute_vi(v_data ** res, SSQ_data * data,SSQ_data * kArr){
    // data的全体向量对于karr进行一个计算距离，目前距离除了不开根号，没有乘二的操作
    //TODO 目前没有括号外乘2的操作
    int dn = data->n;
    int kn = kArr->n;
    BIGNUM * tmp = BN_CTX_get(CTX);
    BIGNUM * tmp2 = BN_CTX_get(CTX);
    BIGNUM * sum = BN_CTX_get(CTX);
    for(int i = 0 ; i < dn ; ++i){
        res[i] = (v_data *) malloc(sizeof (v_data));
        res[i]->dim = kn;
        res[i]->val = (BIGNUM **) malloc(sizeof (BIGNUM *) * kn);
        if(res[i]->val == NULL){
            fprintf(stderr,"malloc memory failed!\n");
            return ERROR;
        }
        // k个维度对应计算的值都是你这一个xi
        res[i]->xi = data->total_data[i]->single_data;
        // 计算每个的距离d
        for(int j = 0 ; j < kn; ++j){
            BN_set_word(sum,0);
            for(int z = 0 ; z < data->dim ; ++z){
                BIGNUM * a = data->total_data[i]->single_data[z];
                BIGNUM * b = kArr->total_data[j]->single_data[z];
                BN_sub(tmp,a,b);
                BN_mul(tmp2,tmp,tmp,CTX);
                BN_add(sum,sum,tmp2);
            }
            BIGNUM * vij = BN_CTX_get(CTX);
            BN_copy(vij,sum);
            res[i]->val[j]= vij;
        }
    }

    BN_clear(tmp);
    BN_clear(tmp2);
    BN_clear(sum);
    return SUCCESS;
}
// 释放vi的点
RESULT RSQ_free_vi(v_data ** target ,int vn){
    if(target == NULL){
        fprintf(stderr,"传递进来的参数为空\n");
        return ERROR;
    }
    for(int i = 0 ; i < vn ; ++i){
        for(int j = 0 ; j < target[0]->dim ; ++j){
            BN_clear(target[i]->val[j]);
        }
        free(target[i]->val);
    }
    free(target);
    return SUCCESS;
}