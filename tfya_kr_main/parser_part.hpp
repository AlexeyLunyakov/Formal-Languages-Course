#include "kr_main_head.h"

// Лексемы - описание
struct lexem {
    enum class Tbl_index tbl;   // - принадлежность таблице 
    int id;						// - порядковый номер в таблице
    std::string value;			// - значение лексемы
    int number;					// - номер лексемы
};

class Parser {
    // Переменные
    vector<lexem> declareted_identificators;	// - список объявленых идентификаторов
    string user_file;							// - название файла
    fstream lexeme_file;						// - файл с лексемами
    lexem LEX;									// - лексема

    // Функции
    bool gl();                                  // - функция считывания следующей лексемы
    bool EQ(std::string);                       // - функция поиска лексемы в соответсвующих таблицах
    bool ID();                                  // - функция вызова соотвествующего идентификатора из таблицы
    bool NUM();                                 // - функция вызова соотвествующего числа из таблицы 

    int check_lex();                            // - функция проверки существования лексемы
    void add();									// - фунция добавление идентификаторв в список объявленных
    int err_proc(int);							// - функция вывода ошибок

public:
    Parser(std::string);
    void scanner();
private:
    //Процедуры проверки выполнения правил, описывающих входной язык

    //<программа>::=        program var <описание> begin <оператор>{ ; <оператор> } end.
    void PROG();

    //<описание>::= 		{<идентификатор> {, <идентификатор> } : <тип> ;}
    void DESCR();

    //<оператор>::= 	    <составной> | <присваивания> | <условный> | <фиксированного_цикла> | <условного_цикла> | <ввода> | <вывода>
    void OPERATOR();

    //<тип>::= 		        % | ! | $
    void TYPE();



    //Правило, определяющее оператор программы

    //<составной>::=                «[» <оператор> { ( : | перевод строки) <оператор> } «]»
    void COMP_OP();

    //<присваивания>::=             <идентификатор> as <выражение>
    void ASSIGN_OP();

    //<условный>::=                 if <выражение> then <оператор> [ else <оператор>]
    void IF_OP();

    //<фиксированного_цикла>::=     for <присваивания> to <выражение> do<оператор>
    void FOR_CICLE();

    //<условного_цикла>::=          while <выражение> do <оператор>
    void WHILE_CICLE();

    //<ввода>::=                    read «(»<идентификатор> {, <идентификатор> } «)»
    void READ();

    //<вывода>::=                   write «(»<выражение> {, <выражение> } «)»
    void WRITE();



    //Выражения языка

    //<выражение>:: =	        <операнд>{ <операции_группы_отношения> <операнд> }
    void EXPRESS();

    //<операнд>::= 		        <слагаемое> {<операции_группы_сложения> <слагаемое>}
    void OPERAND();

    //<слагаемое>::= 	        <множитель> {<операции_группы_умножения> <множитель>}
    void SUMMAND();

    //<множитель>::= 	        <идентификатор> | <число> | <логическая_константа> | <унарная_операция> <множитель> | «(»<выражение>«)»
    void MULTIP();

    //<число>:: =		        <целое> | <действительное>
    void NUMBER();



    //Операции языка

    //<операции_группы_отношения>::=    NE | EQ | LT | LE | GT | GE            
    void RATIO();

    //<операции_группы_сложения>::=     plus | min | or
    void ADDITION();

    //<операции_группы_умножения>::=    mult | div | and
    void MULTIPLICATION();

    //<унарная_операция>::=             ~
    void UNARY();
};

bool Parser::gl() {
    static int number = 1;
    string lex;
    if (lexeme_file.eof()) {
        return true;
    }
    lexeme_file >> lex;

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
    return lexeme_file.eof();

}

bool Parser::EQ(std::string STR_buff) {
    switch (LEX.tbl) {
    case(Tbl_index::T_kw_index): {
        return Tbl_kw[STR_buff] == LEX.id;
    }
    case(Tbl_index::T_lim_index): {
        return Tbl_lim[STR_buff] == LEX.id;
    }
    case(Tbl_index::T_idnt_index): {
        return Tbl_idnt[STR_buff] == LEX.id;
    }
    case(Tbl_index::T_num_index): {
        return Tbl_num[STR_buff] == LEX.id;
    }
    default: {
        return false;
    }
    }
}

bool Parser::ID() {
    return (LEX.tbl == Tbl_index::T_idnt_index);
}

bool Parser::NUM() {
    return LEX.tbl == Tbl_index::T_num_index;
}

int Parser::check_lex() {
    for (lexem lex : declareted_identificators) {
        if (lex.id == LEX.id and lex.tbl == LEX.tbl) {
            return true;
        }
    }
    return false;
}

void Parser::add() {
    declareted_identificators.push_back(LEX);
}

int Parser::err_proc(int ER) {
    cout << "Ошибка: " << ER << ": ";
    switch (ER) {
    case 1001: {
        cout << "Ожидался идентификатор" << endl;
        break;
    }
    case 1002: {
        cout << "Ожидался ':'" << endl;
        break;
    }
    case 1003: {
        cout << "Ожидался оператор" << endl;
        break;
    }
    case 1004: {
        cout << "Ожидался тип" << endl;
        break;
    }
    case 1005: {
        cout << "Ожидался ']'" << endl;
        break;
    }
    case 1006: {
        cout << "Ожидался '['" << endl;
        break;
    }
    case 1007: {
        cout << "Ожидался 'as'" << endl;
        break;
    }
    case 1008: {
        cout << "Ожидался 'then'" << endl;
        break;
    }
    case 1009: {
        cout << "Ожидался 'end.'" << endl;
        break;
    }
    case 1010: {
        cout << "Ожидался '('" << endl;
        break;
    }
    case 1011: {
        cout << "Ожидался ';'" << endl;
        break;
    }
    case 1012: {
        cout << "Ожидался ')'" << endl;
        break;
    }
    case 1013: {
        cout << "Ожидался 'do' " << endl;
        break;
    }
    case 1014: {
        cout << "Ожидался операнд " << endl;
        break;
    }
    case 1015: {
        cout << "Ожидалось слагаемое" << endl;
        break;
    }
    case 1016: {
        cout << "Ожидался множитель" << endl;
        break;
    }
    case 1017: {
        cout << "Ожидалось число" << endl;
        break;
    }
    case 1018: {
        cout << "Ожидался оператор группы отношения" << endl;
        break;
    }
    case 1019: {
        cout << "Ожидался оператор группы сложения" << endl;
        break;
    }
    case 1020: {
        cout << "Ожидался оператор группы умножения" << endl;
        break;
    }
    case 1021: {
        cout << "Ожидался унарный оператор" << endl;
        break;
    }
    case 1101: {
        cout << "Переменная уже была объявлена ранее" << endl;
        break;
    }
    case 1102: {
        cout << "Переменная не была объявлена ранее" << endl;
        break;
    }
    default: {
        cout << "Неизвестная ошибка" << endl;
        break;
    }
    }
    cout << "Встречен " << LEX.value << ", номер лексемы: " << LEX.number;
    throw ER;

}

Parser::Parser(std::string p_filename) :
    user_file("lexems " + p_filename),
    declareted_identificators(0),
    LEX({ Tbl_index::T_lim_index, 1 })
{}

void Parser::scanner() {
    lexeme_file.open(user_file, ios::in);
    PROG();
    cout << "Синтаксический и семантический анализы успешно завершены";
    lexeme_file.close();
}

//<программа>::=        program var <описание> begin <оператор>{ ;  <оператор> } end. 
void Parser::PROG() {
    gl();
    if (EQ("program var")) {
        gl();
        while (!EQ("begin"))
            if (ID())
                DESCR();
            else break;

        if (EQ("begin")) {
            gl();
            if (EQ("if") || EQ("while") || EQ("for") || EQ("read") || EQ("write") || EQ("[") || ID())
                OPERATOR();
            else
                err_proc(1003);
            gl();
            do {
                if (EQ(";")) gl();
                if (EQ("end."))
                    break;
                if (EQ("if") || EQ("while") || EQ("for") || EQ("read") || EQ("write") || EQ("[") || ID())
                {
                    OPERATOR();
                }
                else
                    err_proc(1003);
                if (EQ("end."))
                    break;
                if (!EQ(";"))
                    err_proc(1011);
            } while (EQ(";"));
        }
        if (!(EQ("end.")))
            err_proc(1009);
    }
    else err_proc(2);
}

void Parser::DESCR() {
    if (check_lex())
        err_proc(1101);
    add();
    gl();
    while (EQ(",")) {
        gl();
        if (!ID())
            err_proc(1001);
        if (check_lex())
            err_proc(1101);
        add();
        gl();
    }
    if (EQ(":")) {
        gl();
        TYPE();
    }
    else  err_proc(1002);
    if (EQ(";")) {
        gl();
    }
    else  err_proc(1011);

}

void Parser::OPERATOR() {
    if (EQ("["))
        COMP_OP();
    else if (ID())
        ASSIGN_OP();
    else if (EQ("if"))
        IF_OP();
    else if (EQ("for"))
        FOR_CICLE();
    else if (EQ("while"))
        WHILE_CICLE();
    else if (EQ("read"))
        READ();
    else if (EQ("write"))
        WRITE();
}

void Parser::TYPE() {
    if (!EQ("%") && !EQ("!") && !EQ("$"))
        err_proc(1004);
    gl();
}

//<составной>::=        «[» <оператор> { ( : | перевод строки) <оператор> } «]»
void Parser::COMP_OP() {
    do {
        gl();
        if (EQ("]")) break;
        OPERATOR();

    } while (EQ(":") || EQ("\n"));
    if (!EQ("]"))
        err_proc(1005);
    gl();
}

//<присваивания>::=     <идентификатор> as <выражение>
void Parser::ASSIGN_OP() {
    if (!ID())
        err_proc(1001);
    gl();
    if (!EQ("as"))
        err_proc(1007);
    gl();
    EXPRESS();
}

//<условный>::=        if <выражение> then <оператор> [ else <оператор>]
void Parser::IF_OP() {
    gl();
    EXPRESS();
    if (!EQ("then"))
        err_proc(1008);
    gl();
    OPERATOR();
    if (EQ("else")) {
        gl();
        OPERATOR();
    }
}

//<фиксированного_цикла>::=     for <присваивания> to <выражение> do <оператор>
void Parser::FOR_CICLE() {
    gl();
    ASSIGN_OP();
    if (!EQ("to"))
        err_proc(1011);
    gl();
    if (ID() || NUM() || EQ("true") || EQ("false") || EQ("~") || EQ("(")) {
        if (ID() && !check_lex())
            err_proc(1102);
        EXPRESS();
    }
    if (!EQ("do"))
        err_proc(1012);
    gl();
    OPERATOR();
}

//<условного_цикла>::=      while <выражение> do <оператор>
void Parser::WHILE_CICLE() {
    gl();
    if (ID() || NUM() || EQ("true") || EQ("false") || EQ("~") || EQ("(")) {
        EXPRESS();
    }
    else
        err_proc(1111);
    if (!EQ("do"))
        err_proc(1013);
    gl();
    if (EQ("if") || EQ("while") || EQ("for") || EQ("input") || EQ("output") || EQ("[")) {
        OPERATOR();
    }
    else
        err_proc(1003);
}

//<ввода>::=        read «(»<идентификатор> {, <идентификатор> } «)»
//<ввода>::= 		input «(»<идентификатор> {пробел <идентификатор>} «)»
void Parser::READ() {
    gl();
    if (!EQ("("))
        err_proc(1010);
    gl();
    while (ID()) {
        if (!EQ(","))
            break;
        gl();
    }
    gl();
    if (!EQ(")"))
        err_proc(1012);
    gl();
}

//<вывода>::=       write «(»<выражение> {, <выражение> } «)»
//<вывода>::= 		output «(»<выражение> { пробел <выражение> } «)»
void Parser::WRITE() {
    gl();
    if (!EQ("("))
        err_proc(1010);
    gl();
    while (ID() || NUM() || EQ("true") || EQ("false") || EQ("~") || EQ("(")) {
        if (ID() && !check_lex())
            err_proc(1102);
        if (!EQ(",")) break;
        gl();
        EXPRESS();
    }
    gl();
    if (!EQ(")"))
        err_proc(1012);
    gl();
}

void Parser::EXPRESS() {
    if (ID() || NUM() || EQ("true") || EQ("false") || EQ("~") || EQ("(")) {
        if (ID() && !check_lex())
            err_proc(1102);
        OPERAND();
    }
    else err_proc(1014);
    // NE | EQ | LT | LE | GT | GE
    if (EQ("NE") || EQ("EQ") || EQ("LT") || EQ("LE") || EQ("GT") || EQ("GE")) {
        RATIO();
        if (ID() || NUM() || EQ("true") || EQ("false") || EQ("~") || EQ("(")) {
            if (ID() && !check_lex())
                err_proc(1102);
            OPERAND();
        }
        else err_proc(1014);
    }
}

void Parser::OPERAND() {
    if (ID() || NUM() || EQ("true") || EQ("false") || EQ("~") || EQ("(")) {
        if (ID() && !check_lex())
            err_proc(1102);
        SUMMAND();
    }
    else err_proc(1015);
    // plus | min | or
    if (EQ("plus") || EQ("min") || EQ("or"))
    {
        ADDITION();
        if (ID() || NUM() || EQ("true") || EQ("false") || EQ("~") || EQ("(")) {
            if (ID() && !check_lex())
                err_proc(1102);
            SUMMAND();
        }
        else err_proc(1015);
    }
}

void Parser::SUMMAND() {
    if (ID() || NUM() || EQ("true") || EQ("false") || EQ("~") || EQ("(")) {
        if (ID() && !check_lex())
            err_proc(1102);
        MULTIP();
    }
    else err_proc(1016);
    // mult | div | and
    if (EQ("mult") || EQ("div") || EQ("and"))
    {
        MULTIPLICATION();
        if (ID() || NUM() || EQ("true") || EQ("false") || EQ("~") || EQ("(")) {
            if (ID() && !check_lex())
                err_proc(1102);
            MULTIP();
        }
        else err_proc(1016);
    }
}

void Parser::MULTIP() {
    if (EQ("(")) {
        gl();
        EXPRESS();
        if (!EQ(")"))
            err_proc(1012);
    }
    else if (EQ("~")) {
        UNARY();
    }
    gl();
}

void Parser::NUMBER() {
    if (!NUM())
        err_proc(1017);
    gl();
}

void Parser::RATIO() { // NE | EQ | LT | LE | GT | GE
    if (!(EQ("NE") || EQ("EQ") || EQ("LT") || EQ("LE") || EQ("GT") || EQ("GE")))
        err_proc(1018);
    gl();
}

void Parser::ADDITION() { // plus | min | or
    if (!(EQ("plus") || EQ("min") || EQ("or")))
        err_proc(1019);
    gl();
}

void Parser::MULTIPLICATION() { // mult | div | and
    if (!(EQ("mult") || EQ("dev") || EQ("and")))
        err_proc(1020);
    gl();
}

void Parser::UNARY() {  //  ~
    if (!EQ("~"))
        err_proc(1021);
    gl();
}