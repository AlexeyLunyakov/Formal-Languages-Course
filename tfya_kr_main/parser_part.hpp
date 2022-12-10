#include "kr_main_head.h"

//�������
struct lexem {
    enum class Tbl_index tbl;   //�������������� ������� 
    int id;						//���������� ����� � �������
    std::string value;			//�������� �������
    int number;					//����� ������� �� �������
};

class Parser {
    vector<lexem> declareted_identificators;	//������ ���������� ���������������
    string filename;							//�������� �����
    fstream lexeme_file;						//���� � ���������
    lexem LEX;									//�������

    bool gl();
    bool EQ(std::string);
    bool ID();
    bool NUM();
    int err_proc(int);							//����� ������
    int check_lex();
    void add();									//�������� ������������� � ������ �����������

public:
    Parser(std::string);
    void scanner();
private:
    /*--------------------------��������� �������� ���������� ������, ����������� ������� ����--------------------------*/
    //<���������>::=        program var <��������> begin <��������>{ ; <��������> } end.
    void PROG();

    //<��������>::= 		{<�������������> {, <�������������> } : <���> ;}
    void DESCR();

    //<��������>::= 	<���������> | <������������> | <��������> |
    //		<��������������_�����> | <���������_�����> | <�����> |
    //		<������>
    void OPERATOR();

    //<���>::= 		% | ! | $
    void TYPE();

    /*--------------------------�������, ������������ �������� ���������--------------------------*/
    //<���������>::=        �[� <��������> { ( : | ������� ������) <��������> } �]�
    void COMP_OP();

    //<������������>::=     <�������������> as <���������>
    void ASSIGN_OP();

    //<��������>::=         if <���������> then <��������> [ else <��������>]
    void IF_OP();

    //<��������������_�����>::=     for <������������> to <���������> do<��������>
    void FOR_CICLE();

    //<���������_�����>::=      while <���������> do <��������>
    //<���������_�����>:: =     do while <���������> <��������> loop
    void WHILE_CICLE();


    //<�����>::=        read �(�<�������������> {, <�������������> } �)�
    void READ();

    //<������>::=       write �(�<���������> {, <���������> } �)�
    void WRITE();

    /*--------------------------��������� �����--------------------------*/

    //<���������>:: =	<�������>{ <��������_������_���������> <�������> }
    void EXPRESSION();

    //<�������>::= 		<���������> {<��������_������_��������> <���������>}
    void OPERAND();

    //<���������>::= 	<���������> {<��������_������_���������> <���������>}
    void SUMMAND();

    //<���������>::= 	<�������������> | <�����> | <����������_���������> |
    //					<�������_��������> <���������> | �(�<���������>�)�
    void MULTIPLIER();

    //<�����>:: =		<�����> | <��������������>
    void NUMBER();

    /*--------------------------�������� �����--------------------------*/
    //<��������_������_���������>::=    NE | EQ | LT | LE | GT | GE            
    void RATIO();

    //<��������_������_��������>::=     plus | min | or
    void ADDITION();

    //<��������_������_���������>::=    mult | div | and
    void MULTIPLICATION();

    //<�������_��������>::=         ~
    void UNARY();
};

bool Parser::gl() {
    static int number = 1;
    if (lexeme_file.eof())
        return true;
    string lex;
    lexeme_file >> lex;
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
        throw "������������ �������";
    }
    LEX.number = number;
    ++number;
    return lexeme_file.eof();

}

bool Parser::EQ(std::string Str_buff)
{
    switch (LEX.tbl)
    {
    case(Tbl_index::T_kw_index):
        return Tbl_kw[Str_buff] == LEX.id;
    case(Tbl_index::T_lim_index):
        return Tbl_lim[Str_buff] == LEX.id;
    case(Tbl_index::T_idnt_index):
        return Tbl_idnt[Str_buff] == LEX.id;
    case(Tbl_index::T_num_index):
        return Tbl_num[Str_buff] == LEX.id;
    default:
        return false;
    }
}

bool Parser::ID()
{
    return (LEX.tbl == Tbl_index::T_idnt_index);
}

bool Parser::NUM()
{
    return LEX.tbl == Tbl_index::T_num_index;
}

int Parser::err_proc(int error)
{
    cout << "������: " << error << ": ";
    switch (error)
    {
    case  201:
        cout << "�������� �������������" << endl;
        break;
    case  202:
        cout << "�������� ':'" << endl;
        break;
    case  203:
        cout << "�������� ��������" << endl;
        break;
    case  204:
        cout << "�������� ���" << endl;
        break;
    case  205:
        cout << "�������� ']'" << endl;
        break;
    case  206:
        cout << "�������� '['" << endl;
        break;
    case  207:
        cout << "�������� 'as'" << endl;
        break;
    case  208:
        cout << "�������� 'then'" << endl;
        break;
    case  209:
        cout << "�������� 'end.'" << endl;
        break;
    case  210:
        cout << "�������� '('" << endl;
        break;
    case  211:
        cout << "�������� ';'" << endl;
        break;
    case  212:
        cout << "�������� ')'" << endl;
        break;
    case  213:
        cout << "�������� 'loop' " << endl;
        break;
    case  214:
        cout << "�������� ������� " << endl;
        break;
    case  215:
        cout << "��������� ���������" << endl;
        break;
    case  216:
        cout << "�������� ���������" << endl;
        break;
    case  217:
        cout << "��������� �����" << endl;
        break;
    case  218:
        cout << "�������� �������� ������ ���������" << endl;
        break;
    case  219:
        cout << "�������� �������� ������ ��������" << endl;
        break;
    case  220:
        cout << "�������� �������� ������ ���������" << endl;
        break;
    case  221:
        cout << "�������� ������� ��������" << endl;
        break;
    case 301:
        cout << "���������� ��� ���� ���������" << endl;
        break;
    case 302:
        cout << "���������� �� ���� ��������� �����" << endl;
        break;
    default:
        cout << "����������� ������" << endl;
        break;
    }
    cout << "�������� " << LEX.value << ", ����� �������: " << LEX.number;
    throw error;

}

Parser::Parser(std::string p_filename) :
    filename("lexems " + p_filename),
    declareted_identificators(0),
    LEX({ Tbl_index::T_lim_index, 1 })
{}

void Parser::scanner() {
    lexeme_file.open(filename, ios::in);
    PROG();
    cout << "�������������� � ������������� ������� ������� ���������";
    lexeme_file.close();
}

//<���������>::=        program var <��������> begin <��������>{ ;  <��������> } end. 
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
                err_proc(203);
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
                    err_proc(203);
                if (EQ("end."))
                    break;
                if (!EQ(";"))
                    err_proc(211);
            } while (EQ(";"));
        }
        if (!(EQ("end.")))
            err_proc(209);
    }
    else err_proc(2);
}

void Parser::DESCR() {
    if (check_lex())
        err_proc(301);
    add();
    gl();
    while (EQ(",")) {
        gl();
        if (!ID())
            err_proc(201);
        if (check_lex())
            err_proc(301);
        add();
        gl();
    }
    if (EQ(":")) {
        gl();
        TYPE();
    }
    else  err_proc(202);
    if (EQ(";")) {
        gl();
    }
    else  err_proc(211);
    
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
        err_proc(204);
    gl();
}

//<���������>::=        �[� <��������> { ( : | ������� ������) <��������> } �]�
void Parser::COMP_OP() {
    do {
        gl();
        if (EQ("]")) break;
        OPERATOR();
        
    } while (EQ(":") || EQ("\n"));
    if (!EQ("]"))
        err_proc(205);
    gl();
}

//<������������>::=     <�������������> as <���������>
void Parser::ASSIGN_OP() {
    if (!ID())
        err_proc(201);
    gl();
    if (!EQ("as"))
        err_proc(207);
    gl();
    EXPRESSION();
}

//<��������>::=        if <���������> then <��������> [ else <��������>]
void Parser::IF_OP() {
    gl();
    EXPRESSION();
    if (!EQ("then"))
        err_proc(208);
    gl();
    OPERATOR();
    if (EQ("else")) {
        gl();
        OPERATOR();
    }
}

//<��������������_�����>::=     for <������������> to <���������> do <��������>
void Parser::FOR_CICLE() {
    gl();
    ASSIGN_OP();
    if (!EQ("to"))
        err_proc(211);
    gl();
    if (ID() || NUM() || EQ("true") || EQ("false") || EQ("~") || EQ("(")) {
        if (ID() && !check_lex())
            err_proc(302);
        EXPRESSION();
    }
    if (!EQ("do"))
        err_proc(212);
    gl();
    OPERATOR();
}

//<���������_�����>::=      while <���������> do <��������>
void Parser::WHILE_CICLE() {
    gl();
    if (ID() || NUM() || EQ("true") || EQ("false") || EQ("~") || EQ("(")) {
        EXPRESSION();
    }
    else
        err_proc(228);
    if (!EQ("do"))
        err_proc(213);
    gl();
    if (EQ("if") || EQ("while") || EQ("for") || EQ("input") || EQ("output") || EQ("[")) {
        OPERATOR();
    }
    else
        err_proc(203);
}

//<�����>::=        read �(�<�������������> {, <�������������> } �)�
//<�����>::= 		input �(�<�������������> {������ <�������������>} �)�
void Parser::READ() {
    gl();
    if (!EQ("("))
        err_proc(210);
    gl();
    while (ID()) {
        if (!EQ(","))
            break;
        gl();
    }
    gl();
    if (!EQ(")"))
        err_proc(212);
    gl();
}

//<������>::=       write �(�<���������> {, <���������> } �)�
//<������>::= 		output �(�<���������> { ������ <���������> } �)�
void Parser::WRITE() {
    gl();
    if (!EQ("("))
        err_proc(210);
    gl();
    while (ID() || NUM() || EQ("true") || EQ("false") || EQ("~") || EQ("(")) {
        if (ID() && !check_lex())
            err_proc(302);
        if (!EQ(",")) break;
        gl();
        EXPRESSION();
    }
    gl();
    if (!EQ(")"))
        err_proc(212);
    gl();
}

void Parser::EXPRESSION() {
    if (ID() || NUM() || EQ("true") || EQ("false") || EQ("~") || EQ("(")) {
        if (ID() && !check_lex())
            err_proc(302);
        OPERAND();
    }
    else err_proc(214);
    // NE | EQ | LT | LE | GT | GE
    if (EQ("NE") || EQ("EQ") || EQ("LT") || EQ("LE") || EQ("GT") || EQ("GE")) {
        RATIO();
        if (ID() || NUM() || EQ("true") || EQ("false") || EQ("~") || EQ("(")) {
            if (ID() && !check_lex())
                err_proc(302);
            OPERAND();
        }
        else err_proc(214);
    }
}

void Parser::OPERAND() {
    if (ID() || NUM() || EQ("true") || EQ("false") || EQ("~") || EQ("(")) {
        if (ID() && !check_lex())
            err_proc(302);
        SUMMAND();
    }
    else err_proc(215);
    // plus | min | or
    if (EQ("plus") || EQ("min") || EQ("or"))
    {
        ADDITION();
        if (ID() || NUM() || EQ("true") || EQ("false") || EQ("~") || EQ("(")) {
            if (ID() && !check_lex())
                err_proc(302);
            SUMMAND();
        }
        else err_proc(215);
    }
}

void Parser::SUMMAND() {
    if (ID() || NUM() || EQ("true") || EQ("false") || EQ("~") || EQ("(")) {
        if (ID() && !check_lex())
            err_proc(302);
        MULTIPLIER();
    }
    else err_proc(216);
    // mult | div | and
    if (EQ("mult") || EQ("div") || EQ("and"))
    {
        MULTIPLICATION();
        if (ID() || NUM() || EQ("true") || EQ("false") || EQ("~") || EQ("(")) {
            if (ID() && !check_lex())
                err_proc(302);
            MULTIPLIER();
        }
        else err_proc(216);
    }
}

void Parser::MULTIPLIER() {
    if (EQ("(")) {
        gl();
        EXPRESSION();
        if (!EQ(")"))
            err_proc(212);
    }
    else if (EQ("~")) {
        UNARY();
    }
    gl();
}

void Parser::NUMBER() {
    if (!NUM())
        err_proc(217);
    gl();
}

void Parser::RATIO() { // NE | EQ | LT | LE | GT | GE
    if (!(EQ("NE") || EQ("EQ") || EQ("LT") || EQ("LE") || EQ("GT") || EQ("GE")))
        err_proc(218);
    gl();
}

void Parser::ADDITION() { // plus | min | or
    if (!(EQ("plus") || EQ("min") || EQ("or")))
        err_proc(219);
    gl();
}

void Parser::MULTIPLICATION() { // mult | div | and
    if (!(EQ("mult") || EQ("dev") || EQ("and")))
        err_proc(220);
    gl();
}

void Parser::UNARY() {  //  ~
    if (!EQ("~"))
        err_proc(221);
    gl();
}

int Parser::check_lex()
{
    for (lexem lex : declareted_identificators)
        if (lex.id == LEX.id and lex.tbl == LEX.tbl)
            return true;
    return false;
}

void Parser::add()
{
    declareted_identificators.push_back(LEX);
}
