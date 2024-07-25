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
// 计算时间差并返回毫秒
double calculate_duration(struct timespec start, struct timespec end) {
    double start_ms = start.tv_sec * 1000.0 + start.tv_nsec / 1000000.0;
    double end_ms = end.tv_sec * 1000.0 + end.tv_nsec / 1000000.0;
    return end_ms - start_ms;
}


int main() {
    struct timespec start_time, end_time;

	SSQ_data data;
	SSQ_data kArr;
	kd_tree tree;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    init_algo("/root/heqi/encryption_algorithm/ssq/data/data.txt", &data, &tree, &kArr);
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    double total_duration = calculate_duration(start_time, end_time);
    printf("初始化的总时间是：%f 秒\n", total_duration/1000);
    fflush(stdout);

    clock_gettime(CLOCK_MONOTONIC, &start_time);

    query_algo(&data, &tree, &kArr, "/root/heqi/encryption_algorithm/ssq/data/search_req.txt", "/root/heqi/encryption_algorithm/ssq/data/search_res.txt");
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    total_duration = calculate_duration(start_time, end_time);
    printf("查询的总时间是：%f 秒\n", total_duration/1000);
    fflush(stdout);
    fflush(stdout);
    free_algo(&data, &tree, &kArr);
}