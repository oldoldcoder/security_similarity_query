/**
 * @author heqi
 * @date 2024/03/26
 * @email 2742598536@qq.com
 * @desc 用来测试的初版文件
 * */
#include "stdlib.h"
#include "stdio.h"
#include "SSQ_kdtree.h"
#include "SSQ.h"
#include "utils.h"

int main(){
    // 初始化常量
    initialize_Constant();
    // 开始测试
    SSQ_data data;
    SSQ_data kArr;
    // 读取数据
    RSQ_read_data(&data);
    // 生成k个点
    RSQ_generate_k_ref_points(&kArr,K,data.dim);
    // 计算vi向量的集合
    v_data ** vd = (v_data **) malloc(sizeof (v_data) * data.n);
    RSQ_compute_vi(vd,&data,&kArr);

    // 打印vd的所有数据
    for(int i = 0 ; i <data.n ; ++i){
        printf("目前是第%d个vd的点:",i);
        for(int j = 0 ; j < vd[i]->dim ; ++j){
            char * str = BN_bn2dec(vd[i]->val[j]);
            printf(" %s",str);
        }
        printf("\n");
    }
    fflush(stdout);
    /**-------------------构建树------------------*/
    int dn = data.n;
    int ddim = data.dim;
    int kn = kArr.n;
    kd_tree tree;
    kdtree_init(&tree,kn);
    // 创建树
    kdtree_create(tree.root,vd,dn,kn,0,0,dn - 1);
    // 加密树
    kdtree_upload_server(&tree);
    /**------------------- 查询 ------------------*/
    search_req req;
    search_resp resp;
    // 初始化查询的参数，计算范围查询的令牌
    kdtree_init_search(&req,&resp,&kArr,ddim,kn);
    // 进行查询
    kdtree_range_search(&tree,&req,&resp);
    kdtree_tsf_resp_to_file(&req,&resp);
    // 释放数据
    RSQ_free_vi(vd,data.n);
    RSQ_free_data(&kArr);
    RSQ_free_data(&data);
    kdtree_destory_tree(&tree);
    kdtree_free_search_param(&req,&resp);
}