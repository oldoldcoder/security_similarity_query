
#include "openssl//bn.h"
#include "eTPSS.h"
#include "utils.h"
#include "stdio.h"
#include "stdlib.h"
#include "SSQ.h"

/**------------------------方法的实现---------------------------*/
// 读取数据
RESULT RSQ_read_data(SSQ_data * data){

}
// 释放数据
RESULT RSQ_free_data(SSQ_data * data){

}
// 随机生成K个参考点
RESULT RSQ_generate_k_ref_points(SSQ_data  * kArr,int k){

}
// 计算为Vi向量的集合
RESULT RSQ_compute_vi(v_data * res, SSQ_data * data,SSQ_data * kArr){

}
// 释放vi的点
RESULT RSQ_free_vi(v_data * target){

}