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
    // kArr需要使用
    eTPSS ** en_data;
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
    // 加密之后的val
    eTPSS ** en_val;
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
