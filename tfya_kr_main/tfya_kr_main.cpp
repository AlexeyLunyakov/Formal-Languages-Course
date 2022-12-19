#include "lexer_part.hpp"
#include "parser_part.hpp"

void local_menu() {
    cout << "Разработка распознавателя модельного языка программирования.\n" 
                "Вариант 21. ИКБО-04-21. Луняков Алексей Александрович\n";
    cout << "Список команд для выполнения: \n"
        "1. Ввод названия текстового файла с клавиатуры\n"
        "2. Работа с существующим текстовым файлом\n"
        "3. Повторить главное меню. \n"
        "Любое другое число завершает работу программы.\n";
}

int main() {
    setlocale(LC_ALL, "Russian");
    string file_name;
    bool main = true;
    int mnum = 0;
    local_menu();
    while (main) {
        int val;
        cout << "\nВведите вашу команду меню: \n";
        cin >> mnum;
        switch (mnum) {
            case 1: {
                cout << "Введите название файла: \n"; 
                cin >> file_name;
                Lexer lexer(file_name);
                lexer.scanner();
                Parser parcer(file_name);
                parcer.scanner();
                break;
            }
            case 2: {
                file_name = "testing_kr.txt";
                cout << "Название файла: " << file_name << endl;
                Lexer lexer(file_name);
                lexer.scanner();
                Parser parcer(file_name);
                parcer.scanner();
                break;
            }
            case 3: {
                local_menu();
                break;
            }
            default: {
                main = false;
                break;
            }
        }
    }
    return(0);
}