#ifndef KR_MAIN_H
#define KR_MAIN_H

#include <iostream>
#include <fstream>
#include <cmath>

#include <string>
#include <unordered_map>
#include <stack>
#include <vector>

extern std::unordered_map<std::string, int> Tbl_kw;
extern std::unordered_map<std::string, int> Tbl_lim;
extern std::unordered_map<std::string, int> Tbl_num;
extern std::unordered_map<std::string, int> Tbl_idnt;
using namespace std;

//�������� ������� ������
enum class Tbl_index {
	T_kw_index = 1,		//������� ��������� ����
	T_lim_index = 2,	//������� �������������
	T_num_index = 3,	//������� �����
	T_idnt_index = 4	//������� ���������������
};

#endif // KR_MAIN_H