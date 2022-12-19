#include "kr_main_head.h"

// Состояние - описание состояния КА
enum States {
	ER = 0,		// - ошибка
	H,			// - начальное
	I,			// - индентификатор
	N2,			// - двоичное число
	N8,			// - восьмиричное число
	N10,		// - десятичное число
	N16,		// - шестнадцатиричное число
	C,			// - коммент
	P1,			// - точка
	P2,			// - дробна часть числа
	B,			// - символ B
	O,			// - символ O
	D,			// - символ D
	HX,			// - символ H
	E11,		// - символ E
	E12,		// - порядок числа в экспоненциальной форме
	E13,		// - порядок числа в экспоненциальной форме
	E22,		// - порядок числа в экспоненциальной форме
	ZN,			// - знак порядка числа в экспоненциальной форме
	E21,		// - знак порядка числа в экспоненциальной форме
	OG,			// - ограничитель
	V,			// - выход
};

class Lexer {
	// Переменные (файлы)

	string user_file;						// - название файла с программой для анализа
	fstream user_prgrm;
	fstream lexeme_file;					// - файл со сгенерированными лексемами
	
	// Переменные (анализатор)
	
	char CH;								// - новый входной символ
	string STR_buff;						// - буфферная строка для формирования слов
	int d;									// - переменная для формирования целого значения константы
	enum States CS;							// - текущее состояние КА
	
	// Функции анализатора

	bool gc();								// - считывание нового символа в CH
	bool let();								// - проверка CH на буквенное значение
	bool digit();							// - проверка CH на цифру

	void add();								// - добавление символа в конец буффера
	void nill();							// - функция очистки буфера
	
	int look(enum class Tbl_index tbl);		// - функция поиска лексемы из буфера в таблице
	int put(enum class Tbl_index tbl);		// - функция записи лексемы из буфера в таблицу

	void out(enum class Tbl_index, int num);// - запись пары чисел (n,t) в файл лексем
	bool check_hex();						// - проверка CH на цифру или букву из диапазона a..f..A..F
	bool h_check();							// - проверка CH на букву h/H
	void translate(int);					// - перевод числа в буфере из системы счисления по основанию base в десятичную
	void convert();							// - перевод числа в буфере из строковой формы записи в десятичную форму

public:
	Lexer(std::string);						// - функция открытия файла с программой (конструктор)
	~Lexer();								// - деструктор
	bool scanner();							// - функция сканирования кода программы
	void lex_out();							// - функция вывода лексем в текстовый файл
};

// Служебные слова
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
// Ограничители
std::unordered_map<std::string, int> Tbl_lim = {		
	{"[",1},
	{"]",2},
	{"plus",3},				
	{"min",4},
	{"mult",5},				 
	{"div",6},
	{"NE",7},
	{"EQ",8},				
	{"LT",9},				
	{"GT",10},
	{"LE",11},
	{"GE",12},
	{"(",13},
	{")",14},
	{"{",15},				// Начало комментария
	{"}",16},				// Конец комментария
	{"%",17},				// Целый тип
	{"!",18},				// Действительный тип
	{"$",19},				// Логический тип
	{".",20},
	{",",21},
	{":",22},
	{";",23},
	{"\n", 24}
};

std::unordered_map<std::string, int> Tbl_num = {};	// Числа
std::unordered_map<std::string, int> Tbl_idnt = {};	// Идентификаторы


bool Lexer::gc() {
	if (user_prgrm.eof())
		return true;
	CH = user_prgrm.get();
	return user_prgrm.eof();
}

bool Lexer::let() {
	return (CH >= 'a' && CH <= 'z') || (CH >= 'A' && CH <= 'Z');
}

bool Lexer::digit() {
	return CH >= '0' && CH <= '9';
}

void Lexer::add() {
	STR_buff += CH;
}

void Lexer::nill() {
	STR_buff = "";
}

int Lexer::look(Tbl_index tbl) {
	switch (tbl) {
		case Tbl_index::T_kw_index: {
			if (Tbl_kw.find(STR_buff) == Tbl_kw.end())
				return 0;
			return Tbl_kw.at(STR_buff);
			break;
		}
		case Tbl_index::T_lim_index: {
			if (Tbl_lim.find(STR_buff) == Tbl_lim.end())
				return 0;
			return Tbl_lim.at(STR_buff);
			break;
		}
		case Tbl_index::T_num_index: {
			if (Tbl_num.find(STR_buff) == Tbl_num.end()) return 0;
			return Tbl_lim.at(STR_buff);
			break;
		}
		case Tbl_index::T_idnt_index: {
			if (Tbl_idnt.find(STR_buff) == Tbl_idnt.end()) return 0;
			return Tbl_lim.at(STR_buff);
			break;
		}
		default: {
			return 0;
			break;
		}
	}
}

int Lexer::put(Tbl_index tbl) {
	switch (tbl) {
		case Tbl_index::T_num_index: {
			if (Tbl_num.size() == 0) {
				Tbl_num[STR_buff] = 1;
				return 1;
			}
			else if (Tbl_num.find(STR_buff) == Tbl_num.end()) {
				Tbl_num[STR_buff] = Tbl_num.size() + 1;
				return Tbl_num.size();
			}
			else {
				return Tbl_num[STR_buff];
			}
			break;
		}
		case Tbl_index::T_idnt_index: {
			if (Tbl_idnt.size() == 0) {
				Tbl_idnt[STR_buff] = 1;
				return 1;
			}
			else if (Tbl_idnt.find(STR_buff) == Tbl_idnt.end()) {
				Tbl_idnt[STR_buff] = Tbl_idnt.size() + 1;
				return Tbl_idnt.size();
			}
			else {
				return Tbl_idnt[STR_buff];
			}
			break;
		}
		default: {
			return 0;
			break;
		}
	}
}

void Lexer::out(Tbl_index tbl, int num) {
	lexeme_file << "(" << (int)tbl << "," << num << ")" << endl;
}

bool Lexer::check_hex() {
	return digit() || (CH >= 'a' && CH <= 'f') || (CH >= 'A' && CH <= 'F');
}

bool Lexer::h_check() {
	return check_hex() || CH == 'H' || CH == 'h';
}

void Lexer::translate(int base) {
	for (int i = 0; i < STR_buff.length(); ++i) {
		d += (STR_buff[i] - 48) * pow(2, STR_buff.length() - 1 - i);
	}
	STR_buff = to_string(d);
}

void Lexer::convert() {
	d = stoi(STR_buff);
}

Lexer::Lexer(string p_filename) :
	user_file(p_filename),
	CS(States::H),
	CH(' '),
	STR_buff(""),
	d(0)
{}

Lexer::~Lexer() {
	user_prgrm.close();
	lexeme_file.close();
}

void Lexer::lex_out() {
	struct lexeme {
		int id;
		string value;
		Tbl_index tbl;
		int number;
	};

	lexeme LEX;
	string lex;
	static int number = 1;

	lexeme_file.open("lexems " + user_file);
	lexeme_file >> lex;

	while (!lexeme_file.eof()) {
		LEX.tbl = (Tbl_index)(lex[1] - 48);
		LEX.id = stoi(lex.substr(3, lex.size() - 4));
		switch (LEX.tbl) {
			case(Tbl_index::T_kw_index): {
				for (auto elem : Tbl_kw)
					if (elem.second == LEX.id) LEX.value = elem.first;
				break;
			}
			case(Tbl_index::T_lim_index): {
				for (auto elem : Tbl_lim)
					if (elem.second == LEX.id) LEX.value = elem.first;
				break;
			}
			case(Tbl_index::T_idnt_index): {
				for (auto elem : Tbl_idnt)
					if (elem.second == LEX.id) LEX.value = elem.first;
				break;
			}
			case(Tbl_index::T_num_index): {
				for (auto elem : Tbl_num)
					if (elem.second == LEX.id) LEX.value = elem.first;
				break;
			}
			default: {
				throw "Неопознанная лексема";
			}
		}
		LEX.number = number;
		++number;
		lexeme_file >> lex;
	}
	lexeme_file.close();
}


bool Lexer::scanner() {
	user_prgrm.open(user_file, ios::in);
	lexeme_file.open("lexems " + user_file, ios::out);
	gc();
	int z = 0;
	bool thn = 0;
	do {
		switch (CS) {
		case H: {
			while ((CH == ' ' || CH == '\t' || CH == '\n') && !user_prgrm.eof()) { // || CH == '\n' 
				if (thn == 1 && CH == '\n') {
					out(Tbl_index::T_lim_index, 24);
				}
				gc();
			}
			if (user_prgrm.eof()) {
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
			if (STR_buff == "NE") {
				gc();
				out(Tbl_index::T_lim_index, 7);
				CS = H;
				break;
			}
			else if (STR_buff == "EQ") {
				gc();
				out(Tbl_index::T_lim_index, 8);
				CS = H;
				break;
			}
			else if (STR_buff == "LT" || STR_buff == "LE") {
				if (STR_buff == "LT") {
					gc();
					out(Tbl_index::T_lim_index, 9);
					CS = H;
					break;
				}
				else if (STR_buff == "LE") {
					gc();
					out(Tbl_index::T_lim_index, 11);
					CS = H;
					break;
				}
			}
			else if (STR_buff == "GT" || STR_buff == "GE") {
				if (STR_buff == "GT") {
					gc();
					out(Tbl_index::T_lim_index, 10);
					CS = H;
					break;
				}
				else if (STR_buff == "GE") {
					gc();
					out(Tbl_index::T_lim_index, 12);
					CS = H;
					break;
				}
			}
			if (STR_buff == "plus") {
				gc();
				out(Tbl_index::T_lim_index, 3);
				CS = H;
				break;
			}
			if (STR_buff == "min") {
				gc();
				out(Tbl_index::T_lim_index, 4);
				CS = H;
				break;
			}
			if (STR_buff == "mult") {
				gc();
				out(Tbl_index::T_lim_index, 5);
				CS = H;
				break;
			}
			if (STR_buff == "div") {
				gc();
				out(Tbl_index::T_lim_index, 6);
				CS = H;
				break;
			}
			if (STR_buff == "+" || STR_buff == "-") {
				add();
				gc();
				CS = ZN;
			}
			if (STR_buff == "end") {
				out(Tbl_index::T_kw_index, 16);
				gc();
				CS = V;
				break;
			}
			if (STR_buff == "program") {
				string prog_var;
				gc();
				do {
					prog_var += CH;
					gc();
				} while (let());
				if (prog_var == "var")
					STR_buff += " " + prog_var;
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
			if (STR_buff == "[") thn = 1;
			if (STR_buff == "]") thn = 0;
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
	if (CS == ER) {
		cout << "\nОшибка лексического анализатора!";
		exit(ER);
	}
	user_prgrm.close();
	lexeme_file.close();
	cout << "\nЛексический анализ успешно завершен" << endl;
	return CS;
}