#ifndef SEARCH_H
#define SEARCH_H

#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>

struct PersonRecord {
    std::wstring fio;
    std::wstring passport;
    std::wstring address;
};

//Функция для разбора строки с записью.
PersonRecord parseLine(const std::wstring& line);


int directSearch(const std::wstring& text, const std::wstring& pattern);


std::unordered_map<wchar_t, int> buildBadCharTable(const std::wstring& pattern);

int boyerMooreSearch(const std::wstring& text, const std::wstring& pattern);

#endif // SEARCH_H
