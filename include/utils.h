#pragma once

/**----------------------定义常用的常量-------------------------*/
#include "eTPSS.h"
#include "openssl/bn.h"
#define SUCCESS 15465
#define ERROR 234564
#define TRUE 1
#define FALSE 0
#define RESULT int
// 定义生成参考点的数量
#define K 4
#define LEAF_NODE_MAX_CAP 1
// 随机的范围

// 定义范围查询的路径
#define RSQ_FILE_PATH "/root/heqi/encryption_algorithm/ssq/data/search_req.txt"
// 定义写入查询结果的文件路径
#define RESP_FILE_PATH "/root/heqi/encryption_algorithm/ssq/data/search_res.txt"
// 数据路径
#define SSQ_DATA_FILE "/root/heqi/encryption_algorithm/ssq/data/SSQ_data_file.txt"

void printDebugInfo(BIGNUM * res,eTPSS * et,const char * funcName,int line,char * paramName);
// 牛顿法迭代开平方

void sqrt_bignum(const BIGNUM *num,BIGNUM * x);