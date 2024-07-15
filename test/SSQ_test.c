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

int main() {

	SSQ_data data;
	SSQ_data kArr;
	kd_tree tree;
	init_algo("D:\\study\\code\\ClionProject\\security_similarity_query\\data\\SSQ_data_file.txt", &data, &tree, &kArr);
	query_algo(&data, &tree, &kArr, "D:\\study\\code\\ClionProject\\security_similarity_query\\data\\search_req.txt", "D:\\study\\code\\ClionProject\\security_similarity_query\\data\\search_res.txt");
	free_algo(&data, &tree, &kArr);

}