#pragma once

#include "openssl//bn.h"
#include "eTPSS.h"
#include "utils.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
/*----------------常量定义-------------------*/

/*----------------结构定义-------------------*/
// 首先是吃进去数据的结构定义
typedef struct {
    int dim;
    BIGNUM ** single_data;
}dim_data;
typedef struct {
    int n;// 数据的总数
    int dim;
    dim_data **total_data; // 定义的总的数据
}SSQ_data;
// 计算得到vi的数据集合
typedef struct {
    // 合成的dim维度
    int dim;
    // 值
    BIGNUM ** val;
    // 指向两个数据点
    BIGNUM ** xi;
    // 加密之后的xi点
    eTPSS ** en_xi;
}v_data;
/*----------------读取数据的方法定义-------------------*/
// 读取数据
RESULT RSQ_read_data(SSQ_data * data);
// 释放数据
RESULT RSQ_free_data(SSQ_data * data);
// 随机生成K个参考点
RESULT RSQ_generate_k_ref_points(SSQ_data  * kArr,int k,int dim);
// 计算为Vi向量的集合
RESULT RSQ_compute_vi(v_data ** res, SSQ_data * data,SSQ_data * kArr);
// 释放vi的点
RESULT RSQ_free_vi(v_data ** target ,int vn);

// 异或查看最长公共qianzhui
char* longestCommonPrefix(char** strs, int strsSize) {
    char * arr = (char * ) malloc(sizeof (char) * 200);
    int minLen = strlen(strs[0]);
    memset(arr,0,sizeof(char) * strsSize );
    arr = strs[0];
    for(int i = 1 ; i < strsSize ; ++i){
        int len = strlen(strs[i]);
        if(len > minLen)
            minLen = len;
        // 遍历最短的进行异或操作
        for(int j = 0 ; j < minLen ; ++j){
            arr[j] = arr[j] ^ strs[i][j];
        }
    }
    char * res = malloc(sizeof (char )* minLen);
    // 充满结束符
    memset(arr,0,sizeof(char ) * minLen );
    // 在minlen内前面为0的
    for(int i = 0 ; i < minLen ; ++i){
        // 相同的字符
        if(arr[i] == 0){
            res[i] = strs[0][i];
        }else
            break;
    }
    return res;
}