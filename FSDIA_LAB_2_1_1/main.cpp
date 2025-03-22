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
#include <cstdlib>  // ��� system()

using namespace std;

int main() {
    // ��������� ������ ��� ���������� ������ � UTF-8
    setlocale(LC_ALL, "ru_RU.UTF-8");
#ifdef _WIN32
    system("chcp 65001");
#endif

    wifstream fin("input.txt");
    if (!fin.is_open()) {
        wcerr << L"�� ������� ������� ���� input.txt" << endl;
        return 1;
    }
    fin.imbue(locale(fin.getloc(), new codecvt_utf8<wchar_t, 0x10ffff, codecvt_mode::consume_header>));

    // ��������� � ������ ��� ������
    vector<PersonRecord> records;
    wstring line;
    while (getline(fin, line)) {
        if (!line.empty()) {
            records.push_back(parseLine(line));
        }
    }
    fin.close();

    wcout << L"�������� ���� ��� ������:" << endl;
    wcout << L"1. ���" << endl;
    wcout << L"2. ������� (����� � �����)" << endl;
    wcout << L"3. ����� (����� � ����� ����)" << endl;
    wcout << L"��� �����: ";
    int choice = 0;
    wcin >> choice;
    wcin.ignore(); 

    wcout << L"������� ������ ��� ������: ";
    wstring pattern;
    getline(wcin, pattern);

    // �������� ������ ������ � �������� ������ � BOM ��� ������ UTF-8
    wofstream foutDirect("output_direct.txt", ios::binary);
    if (!foutDirect.is_open()) {
        wcerr << L"�� ������� ������� ���� output_direct.txt" << endl;
        return 1;
    }
    foutDirect.imbue(locale(foutDirect.getloc(), new codecvt_utf8<wchar_t, 0x10ffff, codecvt_mode::generate_header>));

    wofstream foutBM("output_bm.txt", ios::binary);
    if (!foutBM.is_open()) {
        wcerr << L"�� ������� ������� ���� output_bm.txt" << endl;
        return 1;
    }
    foutBM.imbue(locale(foutBM.getloc(), new codecvt_utf8<wchar_t, 0x10ffff, codecvt_mode::generate_header>));

    // ������ �����
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
            foutDirect << L"������ " << (i + 1)
                << L", ������� � ���� = " << pos << L"\n";
        }
    }
    auto endDirect = chrono::high_resolution_clock::now();
    auto durationDirect = chrono::duration_cast<chrono::microseconds>(endDirect - startDirect).count();
    foutDirect << L"����� ������� ������ (���): " << durationDirect << L"\n";
    foutDirect.close();

    // ����� �����������
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
            foutBM << L"������ " << (i + 1)
                << L", ������� � ���� = " << pos << L"\n";
        }
    }
    auto endBM = chrono::high_resolution_clock::now();
    auto durationBM = chrono::duration_cast<chrono::microseconds>(endBM - startBM).count();
    foutBM << L"����� ������ ����������� (���): " << durationBM << L"\n";
    foutBM.close();

    wcout << L"���������� �������� � output_direct.txt � output_bm.txt." << endl;
    return 0;
}
