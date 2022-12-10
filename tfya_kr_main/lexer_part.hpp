#include "kr_main_head.h"

//состояния конечного автомата
enum States {
	ER = 0, //ошибка(конечное состояние)
	H,		//начало
	I,		//индентификатор
	N2,		//двоичное число
	N8,		//восьмиричное число
	N10,	//десятичное число
	N16,	//шестнадцатиричное число
	C,		//коммент
	C1,		//комментарий(начало)
	C2,		//комментарий(содержание)
	C3,		//комментарий(конец)
	M1,		//меньше, меньше или равно
	M2,		//больше, больше или равно
	M3,		//неравно
	M4,		//равно
	P1,		//точка
	P2,		//дробна часть числа
	B,		//символ B
	O,		//символ O
	D,		//символ D
	HX,		//символ H
	E11,	//символ E
	E12,	//порядок числа в экспоненциальной форме
	E13,	//порядок числа в экспоненциальной форме
	E22,	//порядок числа в экспоненциальной форме
	ZN,		//знак порядка числа в экспоненциальной форме
	E21,	//знак порядка числа в экспоненциальной форме
	OG,		//ограничитель
	V,		//выход(конечное состояние)
};

class Lexer {

	int d;								//переменная для формирования целого значения константы
	char CH;							//входной символ
	string Str_buff;					//буффер для накапливания символов лексемы
	enum States CS;						//текущее состояние
	string filename;					//имя файла с кодом
	fstream lexeme_file;				//файл с лексемами
	fstream example_program;			//программа для анализа

	bool gc();							//процедура считывания очередного символа в переменную CH
	bool let();							//логическая функция, проверяющая, является ли переменная CH буквой
	bool digit();						//логическая функция, проверяющая, является ли переменная CH цифрой
	void nill();						//процедура очистки буфера S
	void add();							//процедура добавления очередного символа в конец 
	int look(enum class Tbl_index t);	//функция поиска лексемы из буфера S в таблице t
	int put(enum class Tbl_index t);	//процедура записи лексемы из буфера S в таблицу t
	void out(enum class Tbl_index, int);	//процедура записи пары чисел (n,t) в файл лексем
	bool check_hex();					//логическая функция, проверяющая, является ли переменная CH цифрой или буквой из диапазона a..fA..F
	bool AFH();							//логическая функция, подобная check_hex, но дополнительно проверяет, является ли CH буквой H или h
	void translate(int);				//процедура перевода числа в буфере из системы счисления по основанию base в десятичную СС
	void convert();						//процедура перевода числа в буфере из строковой формы записи в десятичную форму

public:
	Lexer(std::string);
	bool scanner();								//начало анализа
	void print_lexems();
	~Lexer();
};

//таблица служебных слов
std::unordered_map<std::string, int> Tbl_kw = {		
	{"true",1},
	{"false",2},
	{"not",3},
	{"program var",4},
	{"if",5},
	{"then",6},
	{"else",7},
	{"begin",8},
	{"for",9},
	{"while",10},
	{"read",11},
	{"write",12},
	{"or",13},
	{"and",14},
	{"~",15},
	{"end.",16},
	{"as", 17},
	{"to", 18},
	{"do", 19}
};
//таблица ограничителей
std::unordered_map<std::string, int> Tbl_lim = {		
	{"[",1},
	{"]",2},
	{"plus",3},				// plus | min 
	{"min",4},
	{"mult",5},				// mult | div 
	{"div",6},
	{"NE",7},
	{"EQ",8},				// NE | EQ | LT | LE | GT | GE
	{"LT",9},				// < >| =  | <  | <= | >  | >=
	{"GT",10},
	{"LE",11},
	{"GE",12},
	{"(",13},
	{")",14},
	{"{",15},				// признак начала комментария
	{"}",16},				// признак конца комментария
	{"%",17},				//целый тип
	{"!",18},				//действительный тип
	{"$",19},				//логический тип
	{".",20},
	{",",21},
	{":",22},
	{";",23},
	{"\n", 24}
};

std::unordered_map<std::string, int> Tbl_num = {};	//таблица чисел
std::unordered_map<std::string, int> Tbl_idnt = {};	//таблица идентификаторов


Lexer::Lexer(string p_filename) :
	Str_buff(""),
	d(0),
	CH(' '),
	CS(States::H),
	filename(p_filename)
{}

bool Lexer::scanner() {
	example_program.open(filename, ios::in);
	lexeme_file.open("lexems " + filename, ios::out);
	gc();
	int z = 0;
	bool thn = 0;
	do {
		switch (CS) {
		case H: {
			while ((CH == ' ' || CH == '\t' || CH == '\n') && !example_program.eof()) { // || CH == '\n' 
				if (thn == 1 && CH == '\n') {
					out(Tbl_index::T_lim_index, 24);
				}
				gc();
			}
			if (example_program.eof()) {
				CS = ER;
				break;
			}
			if (let()) {
				nill();
				add();
				gc();
				CS = I;
			}
			else if (CH == '0' || CH == '1') {
				nill();
				CS = N2;
				add();
				gc();
			}
			else if (CH >= '2' && CH <= '7') {
				nill();
				CS = N8;
				add();
				gc();
			}
			else if (CH >= '8' && CH <= '9') {
				nill();
				CS = N10;
				add();
				gc();
			}
			else if (CH == '.') {
				nill();
				add();
				gc();
				CS = P1;
			}
			else if (CH == '{') {
				gc();
				CS = C;
			}
			else if (CH == '}') {
				CS = ER;
			}
			else CS = OG;
			break;
		}

		case I: {
			while (let() || digit()) { 
				add(); 
				gc(); 
			}
			if (Str_buff == "NE") {
				gc();
				out(Tbl_index::T_lim_index, 7);
				CS = H;
				break;
			}
			else if (Str_buff == "EQ") {
				gc();
				out(Tbl_index::T_lim_index, 8);
				CS = H;
				break;
			}
			else if (Str_buff == "LT" || Str_buff == "LE") {
				if (Str_buff == "LT") {
					gc();
					out(Tbl_index::T_lim_index, 9);
					CS = H;
					break;
				}
				else if (Str_buff == "LE") {
					gc();
					out(Tbl_index::T_lim_index, 11);
					CS = H;
					break;
				}
			}
			else if (Str_buff == "GT" || Str_buff == "GE") {
				if (Str_buff == "GT") {
					gc();
					out(Tbl_index::T_lim_index, 10);
					CS = H;
					break;
				}
				else if (Str_buff == "GE") {
					gc();
					out(Tbl_index::T_lim_index, 12);
					CS = H;
					break;
				}
			}
			if (Str_buff == "plus") {
				gc();
				out(Tbl_index::T_lim_index, 3);
				CS = H;
				break;
			}
			if (Str_buff == "min") {
				gc();
				out(Tbl_index::T_lim_index, 4);
				CS = H;
				break;
			}
			if (Str_buff == "mult") {
				gc();
				out(Tbl_index::T_lim_index, 5);
				CS = H;
				break;
			}
			if (Str_buff == "div") {
				gc();
				out(Tbl_index::T_lim_index, 6);
				CS = H;
				break;
			}
			if (Str_buff == "+" || Str_buff == "-") {
				add();
				gc();
				CS = ZN;
			}
			if (Str_buff == "end") {
				out(Tbl_index::T_kw_index, 16);
				gc();
				CS = V;
				break;
			}
			/*if (Str_buff == "then") {
				thn = 1;
			}*/
			if (Str_buff == "program") {
				string prog_var;
				gc();
				do {
					prog_var += CH;
					gc();
				} while (let());
				if (prog_var == "var")
					Str_buff += " " + prog_var;
				else
					CS = ER;
			}
			z = look(Tbl_index::T_kw_index);
			if (z != 0) {
				out(Tbl_index::T_kw_index, z);
				CS = H;
			}
			else {
				z = put(Tbl_index::T_idnt_index);
				out(Tbl_index::T_idnt_index, z);
				CS = H;
			}
			
			break;
		}
		case N2: {
			while (CH == '0' || CH == '1') { 
				add(); 
				gc(); 
			}
			if (CH >= '2' && CH <= '7')
				CS = N8;
			else if (CH == '8' || CH == '9')
				CS = N10;
			else if (CH == 'A' || CH == 'a' || CH == 'C' || CH == 'c' || CH == 'F' || CH == 'f')
				CS = N16;
			else if (CH == 'E' || CH == 'e') {
				add();
				gc();
				CS = E11;
			}
			else if (CH == 'D' || CH == 'd') {
				add();
				gc();
				CS = D;
			}
			else if (CH == 'O' || CH == 'o')
				CS = O;
			else if (CH == 'H' || CH == 'h') {
				gc();
				CS = HX;
			}
			else if (CH == '.') {
				add();
				gc();
				CS = P1;
			}
			else if (CH == 'B' || CH == 'b') {
				add();
				gc();
				CS = B;
			}
			else if (let())
				CS = ER;
			else
				CS = N10;
			break;
		}

		case N8: {
			while (CH >= '2' && CH <= '7') { 
				add(); 
				gc(); 
			}
			if (CH == '8' || CH == '9') { 
				CS = N10; 
			}
			else if (CH == 'A' || CH == 'a' || CH == 'B' || CH == 'b' || CH == 'C' || CH == 'c' || CH == 'F' || CH == 'f')
				CS = N16;
			else if (CH == 'E' || CH == 'e') {
				add();
				gc();
				CS = E11;
			}
			else if (CH == 'D' || CH == 'd') {
				add();
				gc();
				CS = D;
			}
			else if (CH == 'H' || CH == 'h') {
				gc();
				CS = HX;
			}
			else if (CH == '.') {
				add();
				gc();
				CS = P1;
			}
			else if (CH == 'O' || CH == 'o') {
				gc();
				CS = O;
			}
			else if (let())
				CS = ER;
			else
				CS = N10;
			break;
		}

		case N10: {
			while (CH == '8' || CH == '9') { 
				add(); 
				gc(); 
			}
			if (CH == 'A' || CH == 'a' || CH == 'B' || CH == 'b' || CH == 'C' || CH == 'c' || CH == 'F' || CH == 'f')
				CS = N16;
			else if (CH == 'E' || CH == 'e') {
				add();
				gc();
				CS = E11;
			}
			else if (CH == 'H' || CH == 'h') {
				gc();
				CS = HX;
			}
			else if (CH == '.') {
				add();
				gc();
				CS = P1;
			}
			else if (CH == 'D' || CH == 'd') {
				add();
				gc();
				CS = D;
			}
			else if (let())
				CS = ER;
			else {
				z = put(Tbl_index::T_num_index);
				out(Tbl_index::T_num_index, z);
				CS = H;
			}
			break;
		}
		case N16: {
			while (check_hex()) { 
				add(); 
				gc();
			}
			if (CH == 'H' || CH == 'h') {
				gc();
				CS = HX;
			}
			else
				CS = ER;
			break;
		}

		case B: {
			if (check_hex())
				CS = N16;
			else if (CH == 'H' || CH == 'h') {
				gc();
				CS = HX;
			}
			else if (let())
				CS = ER;
			else {
				translate(2);
				z = put(Tbl_index::T_num_index);
				out(Tbl_index::T_num_index, z);
				CS = H;
			}
			break;
		}

		case O: {
			if (let() || digit())
				CS = ER;
			else {
				translate(8);
				z = put(Tbl_index::T_num_index);
				out(Tbl_index::T_num_index, z);
				CS = H;
			}
			break;
		}

		case D: {
			if (CH == 'H' || CH == 'h') {
				gc();
				CS = HX;
			}
			else if (check_hex())
				CS = N16;
			else if (let())
				CS = ER;
			else {
				z = put(Tbl_index::T_num_index);
				out(Tbl_index::T_num_index, z);
				CS = H;
			}
			break;
		}

		case HX: {
			if (let() || digit())
				CS = ER;
			else {
				translate(16);
				z = put(Tbl_index::T_num_index);
				out(Tbl_index::T_num_index, z);
				CS = H;
			}
			break;
		}

		case E11: {
			if (digit()) {
				add();
				gc();
				CS = E12;
			}
			else if (CH == '+' || CH == '-') {
				add();
				gc();
				CS = ZN;
			}
			else if (CH == 'H' || CH == 'h') {
				gc();
				CS = HX;
			}
			else if (check_hex()) {
				add();
				gc();
				CS = N16;
			}
			else
				CS = ER;
			break;
		}

		case ZN: {
			if (digit()) {
				add();
				gc();
				CS = E13;
			}
			else
				CS = ER;
			break;
		}

		case E12: {
			while (digit()) { add(); gc(); }
			if (check_hex())
				CS = N16;
			else if (CH == 'H' || CH == 'h') {
				gc();
				CS = HX;
			}
			else if (let())
				CS = ER;
			else {
				convert();
				z = put(Tbl_index::T_num_index);
				out(Tbl_index::T_num_index, z);
				CS = H;
			}
			break;
		}

		case E13: {
			while (digit()) { add(); gc(); }
			if (let() || CH == '.')
				CS = ER;
			else {
				convert();
				z = put(Tbl_index::T_num_index);
				out(Tbl_index::T_num_index, z);
				CS = H;
			}
			break;
		}

		case P1: {
			if (digit())
				CS = P2;
			else
				CS = ER;
			break;
		}

		case P2: {
			while (digit()) { 
				add(); 
				gc(); 
			}
			if (CH == 'E' || CH == 'e') {
				add();
				gc();
				CS = E21;
			}
			else if (let() || CH == '.') {
				CS = ER;
			}
			else {
				convert();
				z = put(Tbl_index::T_num_index);
				out(Tbl_index::T_num_index, z);
				CS = H;
			}
			break;
		}

		case E21: {
			if (CH == '+' || CH == '-') {
				add();
				gc();
				CS = ZN;
			}
			else if (digit())
				CS = E22;
			else
				CS = ER;
			break;
		}

		case E22: {
			while (digit()) { 
				add(); 
				gc(); 
			}
			if (let() || CH == '.')
				CS = ER;
			else {
				convert();
				z = put(Tbl_index::T_num_index);
				out(Tbl_index::T_num_index, z)
					; CS = H;
			}
			break;
		}
		case C: {
			bool flag = 0;	//флаг конца файла
			while (CH != '}' && !flag) {
				flag = gc();
			}
			if (flag)
				CS = ER;
			else if (CH == '}') {
				gc();
				CS = H;
			}
			break;
		}
		case OG: {
			nill();
			add();
			if (Str_buff == "[") thn = 1;
			if (Str_buff == "]") thn = 0;
			z = look(Tbl_index::T_lim_index);
			if (z != 0) {
				gc();
				out(Tbl_index::T_lim_index, z);
				CS = H;
			}
			else CS = ER;
			break;
		}
		}
	} while (CS != V && CS != ER);
	example_program.close();
	lexeme_file.close();
	cout << "Лексический анализ успешно завершен" << endl;
	return CS;
}

void Lexer::print_lexems()
{
	struct lexeme
	{
		int id;
		string value;
		Tbl_index tbl;
		int number;
	};
	lexeme LEX;
	static int number = 1;
	lexeme_file.open("lexems " + filename);
	string lex;
	lexeme_file >> lex;
	while (!lexeme_file.eof()) {
		LEX.tbl = (Tbl_index)(lex[1] - 48);
		LEX.id = stoi(lex.substr(3, lex.size() - 4));
		switch (LEX.tbl)
		{
		case(Tbl_index::T_kw_index):
			for (auto elem : Tbl_kw)
				if (elem.second == LEX.id) LEX.value = elem.first;
			break;
		case(Tbl_index::T_lim_index):
			for (auto elem : Tbl_lim)
				if (elem.second == LEX.id) LEX.value = elem.first;
			break;
		case(Tbl_index::T_idnt_index):
			for (auto elem : Tbl_idnt)
				if (elem.second == LEX.id) LEX.value = elem.first;
			break;
		case(Tbl_index::T_num_index):
			for (auto elem : Tbl_num)
				if (elem.second == LEX.id) LEX.value = elem.first;
			break;
		default:
			throw "Неопознанная лексема";
		}
		LEX.number = number;
		++number;
		std::cout << lex << " --- " << LEX.value << endl;
		lexeme_file >> lex;
	}
	lexeme_file.close();
}

bool Lexer::gc() {
	if (example_program.eof())
		return true;
	CH = example_program.get();
	return example_program.eof();
}

bool Lexer::let()
{
	return (CH >= 'a' && CH <= 'z') || (CH >= 'A' && CH <= 'Z');
}

bool Lexer::digit()
{
	return CH >= '0' && CH <= '9';
}

void Lexer::nill()
{
	Str_buff = "";
}

void Lexer::add()
{
	Str_buff += CH;
}

int Lexer::look(Tbl_index t)
{
	switch (t)
	{
	case Tbl_index::T_kw_index:
		if (Tbl_kw.find(Str_buff) == Tbl_kw.end())
			return 0;
		return Tbl_kw.at(Str_buff);
		break;
	case Tbl_index::T_lim_index:
		if (Tbl_lim.find(Str_buff) == Tbl_lim.end())
			return 0;
		return Tbl_lim.at(Str_buff);
		break;
	case Tbl_index::T_num_index:
		if (Tbl_num.find(Str_buff) == Tbl_num.end()) return 0;
		return Tbl_lim.at(Str_buff);
		break;
	case Tbl_index::T_idnt_index:
		if (Tbl_idnt.find(Str_buff) == Tbl_idnt.end()) return 0;
		return Tbl_lim.at(Str_buff);
		break;
	default:
		return 0;
		break;
	}
}

int Lexer::put(Tbl_index t)
{
	//cout << (int)t << " " << S << endl;
	switch (t)
	{
	case Tbl_index::T_num_index:
		if (Tbl_num.size() == 0) {
			Tbl_num[Str_buff] = 1;
			return 1;
		}
		else if (Tbl_num.find(Str_buff) == Tbl_num.end()) {
			Tbl_num[Str_buff] = Tbl_num.size() + 1;
			return Tbl_num.size();
		}
		else
			return Tbl_num[Str_buff];
		break;
	case Tbl_index::T_idnt_index:
		if (Tbl_idnt.size() == 0) {
			Tbl_idnt[Str_buff] = 1;
			return 1;
		}
		else if (Tbl_idnt.find(Str_buff) == Tbl_idnt.end()) {
			Tbl_idnt[Str_buff] = Tbl_idnt.size() + 1;
			return Tbl_idnt.size();
		}
		else
			return Tbl_idnt[Str_buff];
		break;
	default:
		return 0;
		break;
	}
}

void Lexer::out(Tbl_index t, int n)
{
	lexeme_file << "(" << (int)t << "," << n << ")" << endl;
	//cout << S << " -- (" << (int)t << "," << n << ")" << endl;
}

bool Lexer::check_hex()
{
	return digit() || (CH >= 'a' && CH <= 'f') || (CH >= 'A' && CH <= 'F');
}

bool Lexer::AFH()
{
	return check_hex() || CH == 'H' || CH == 'h';
}

void Lexer::translate(int base)
{
	for (int i = 0; i < Str_buff.length(); ++i) {
		d += (Str_buff[i] - 48) * pow(2, Str_buff.length() - 1 - i);
	}
	Str_buff = to_string(d);
}

void Lexer::convert()
{
	d = stoi(Str_buff);
}

Lexer::~Lexer() {
	example_program.close();
	lexeme_file.close();
}