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

//значения таблицы лексем
enum class Tbl_index {
	T_kw_index = 1,		//таблица служебных слов
	T_lim_index = 2,	//таблица ограничителей
	T_num_index = 3,	//таблица чисел
	T_idnt_index = 4	//таблица идентификаторов
};

#endif // KR_MAIN_H