/**
 * @author heqi
 * @date 2024/03/22
 * @desc 对于安全相似性查询头文件的编码，安全相似性是用来查询高纬数据的一种方法，主要使用了kdtree等
 * */
#pragma once
#include "openssl//bn.h"
#include "eTPSS.h"
#include "utils.h"
#include "stdio.h"
#include "stdlib.h"
#include "SSQ.h"

/*-------------------------数据定义-----------------------------*/
typedef struct leaf_val{
    v_data *val;
    struct leaf_val * next;
}leaf_val;

// 定义树的节点
typedef struct tree_node{
    int is_leaf_node;
    // 当前层
    int dir;
    int dim;
    union{
        struct {
            // 分割的维度
            int divide_dim;
            BIGNUM * divide_val;
            /**--------加密之后---------*/
            eTPSS * en_divide_dim;
            eTPSS * en_divide_val;
        };
        struct {
            leaf_val *data_root;
            // 当前节点
            leaf_val *data_now;
        };
    };
    struct tree_node * left;
    struct tree_node * right;
}tree_node;

// 定义一个树
typedef struct {
    // 维度
    int dim;
    tree_node  * root;
}kd_tree;

// 定义一个查询的格式
typedef struct {
    int dim;
    eTPSS ** y;
    // 定义τ的长度
    eTPSS *** range;
    eTPSS * tao;
}search_req;
// 定义查询结果，查询的结果应该都是加密的数据
typedef struct search_res_node{
    eTPSS ** data;
    struct search_res_node *next;
}res_node;

typedef struct search_resp{
    // 查询数据的维度
    int dim;
    //查询的结果链表
    res_node * root;
    res_node * now;
} search_resp;

/*-----------------------------方法定义-----------------------------*/
// 树的初始化
RESULT kdtree_init(kd_tree * root,int dim, int n);
// 树的节点的初始化
RESULT kdtree_node_init(tree_node * node,int is_leaf_node,int divide_dim,BIGNUM * divide_val,int dim,int dir,tree_node * left,tree_node * right);
// 创建树
RESULT kdtree_create(tree_node * node,v_data ** total,int n,int dim,int dir,int idx_lef,int idx_rig);
// 销毁树
RESULT kdtree_destory_tree(kd_tree * tree);
// 加密树上传服务器，假方法，只加密不上传
RESULT kdtree_upload_server(kd_tree * tree);
// 初始化查询的参数，同时计算范围查询的令牌
RESULT kdtree_init_search(search_req * req,search_resp * resp,int dim,SSQ_data * kArr);
// 进行查询
RESULT kdtree_range_search(kd_tree * root, search_req * req, search_resp * resp);
// 销毁查询的参数
RESULT kdtree_free_search_param(search_req * req,search_resp * resp);
// 转换参数到文件中去
RESULT kdtree_tsf_resp_to_file(search_resp * resp);
