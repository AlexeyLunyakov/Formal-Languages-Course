//#include "lexer_part.hpp"
//#include "parser_part.hpp"
//#include "kr_main_head.h"
#include "rec_part.hpp"

int main() {
    setlocale(LC_ALL, "Russian");
    Recognizer rec("testing_kr.txt");
    rec.exec();
}