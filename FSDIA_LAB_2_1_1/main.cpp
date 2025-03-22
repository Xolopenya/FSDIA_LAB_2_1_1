#include "search.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <cstdlib>  // Для system()

using namespace std;

int main() {
    // Настройка локали для корректной работы с UTF-8
    setlocale(LC_ALL, "ru_RU.UTF-8");
#ifdef _WIN32
    system("chcp 65001");
#endif

    wifstream fin("input.txt");
    if (!fin.is_open()) {
        wcerr << L"Не удалось открыть файл input.txt" << endl;
        return 1;
    }
    fin.imbue(locale(fin.getloc(), new codecvt_utf8<wchar_t, 0x10ffff, codecvt_mode::consume_header>));

    // Считываем и парсим все записи
    vector<PersonRecord> records;
    wstring line;
    while (getline(fin, line)) {
        if (!line.empty()) {
            records.push_back(parseLine(line));
        }
    }
    fin.close();

    wcout << L"Выберите поле для поиска:" << endl;
    wcout << L"1. ФИО" << endl;
    wcout << L"2. Паспорт (серия и номер)" << endl;
    wcout << L"3. Адрес (улица и номер дома)" << endl;
    wcout << L"Ваш выбор: ";
    int choice = 0;
    wcin >> choice;
    wcin.ignore(); 

    wcout << L"Введите шаблон для поиска: ";
    wstring pattern;
    getline(wcin, pattern);

    // Открытие файлов вывода в бинарном режиме с BOM для записи UTF-8
    wofstream foutDirect("output_direct.txt", ios::binary);
    if (!foutDirect.is_open()) {
        wcerr << L"Не удалось открыть файл output_direct.txt" << endl;
        return 1;
    }
    foutDirect.imbue(locale(foutDirect.getloc(), new codecvt_utf8<wchar_t, 0x10ffff, codecvt_mode::generate_header>));

    wofstream foutBM("output_bm.txt", ios::binary);
    if (!foutBM.is_open()) {
        wcerr << L"Не удалось открыть файл output_bm.txt" << endl;
        return 1;
    }
    foutBM.imbue(locale(foutBM.getloc(), new codecvt_utf8<wchar_t, 0x10ffff, codecvt_mode::generate_header>));

    // Прямой поиск
    auto startDirect = chrono::high_resolution_clock::now();
    for (size_t i = 0; i < records.size(); i++) {
        wstring field;
        switch (choice) {
        case 1: field = records[i].fio; break;
        case 2: field = records[i].passport; break;
        case 3: field = records[i].address; break;
        default: field = L""; break;
        }
        int pos = directSearch(field, pattern);
        if (pos != -1) {
            foutDirect << L"Запись " << (i + 1)
                << L", позиция в поле = " << pos << L"\n";
        }
    }
    auto endDirect = chrono::high_resolution_clock::now();
    auto durationDirect = chrono::duration_cast<chrono::microseconds>(endDirect - startDirect).count();
    foutDirect << L"Время прямого поиска (мкс): " << durationDirect << L"\n";
    foutDirect.close();

    // Поиск Бойера–Мура
    auto startBM = chrono::high_resolution_clock::now();
    for (size_t i = 0; i < records.size(); i++) {
        wstring field;
        switch (choice) {
        case 1: field = records[i].fio; break;
        case 2: field = records[i].passport; break;
        case 3: field = records[i].address; break;
        default: field = L""; break;
        }
        int pos = boyerMooreSearch(field, pattern);
        if (pos != -1) {
            foutBM << L"Запись " << (i + 1)
                << L", позиция в поле = " << pos << L"\n";
        }
    }
    auto endBM = chrono::high_resolution_clock::now();
    auto durationBM = chrono::duration_cast<chrono::microseconds>(endBM - startBM).count();
    foutBM << L"Время поиска Бойера–Мура (мкс): " << durationBM << L"\n";
    foutBM.close();

    wcout << L"Результаты записаны в output_direct.txt и output_bm.txt." << endl;
    return 0;
}
