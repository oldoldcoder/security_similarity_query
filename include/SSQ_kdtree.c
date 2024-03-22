#include "openssl//bn.h"
#include "eTPSS.h"
#include "utils.h"
#include "stdio.h"
#include "stdlib.h"
#include "SSQ.h"
#include "SSQ_kdtree.h"

/**----------------------方法实现-----------------------*/
// 树的初始化
RESULT kdtree_init(kd_tree * root,int dim, int n){

}
// 树的节点的初始化
RESULT kdtree_node_init(tree_node * node,int is_leaf_node,int divide_dim,int divide_val,int dim,tree_node * left,tree_node * right){

}
// 创建树
RESULT kdtree_create(tree_node * node,SSQ_data * total,int n,int dim ,int idx_lef,int idx_rig){

}
// 销毁树
RESULT kdtree_destory_tree(kd_tree * tree){

}
// 加密树上传服务器，假方法，只加密不上传
RESULT kdtree_upload_server(kd_tree * tree){

}
// 初始化查询的参数
RESULT kdtree_init_search(search_req * req,search_resp * resp){

}
// 进行查询
RESULT kdtree_range_search(kd_tree * root, search_req * req, search_resp * resp){

}
// 销毁查询的参数
RESULT kdtree_free_search_param(search_req * req,search_resp * resp){

}
// 转换参数到文件中去
RESULT kdtree_tsf_resp_to_file(search_resp * resp){

}