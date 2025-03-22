#include "search.h"
#include <algorithm>
#include <sstream>

// Реализация функции parseLine.
PersonRecord parseLine(const std::wstring& line) {
    PersonRecord record;
    std::wstringstream wss(line);

    // Считываем ФИО (3 слова)
    std::wstring token;
    std::vector<std::wstring> fioTokens;
    for (int i = 0; i < 3 && (wss >> token); i++) {
        fioTokens.push_back(token);
    }
    {
        std::wstringstream ss;
        for (size_t i = 0; i < fioTokens.size(); ++i) {
            ss << fioTokens[i];
            if (i + 1 < fioTokens.size())
                ss << L" ";
        }
        record.fio = ss.str();
    }

    // Считываем паспорт (2 слова)
    std::vector<std::wstring> passportTokens;
    for (int i = 0; i < 2 && (wss >> token); i++) {
        passportTokens.push_back(token);
    }
    {
        std::wstringstream ss;
        for (size_t i = 0; i < passportTokens.size(); ++i) {
            ss << passportTokens[i];
            if (i + 1 < passportTokens.size())
                ss << L" ";
        }
        record.passport = ss.str();
    }

    // Остальные токены – это адрес
    std::vector<std::wstring> addressTokens;
    while (wss >> token) {
        addressTokens.push_back(token);
    }
    {
        std::wstringstream ss;
        for (size_t i = 0; i < addressTokens.size(); ++i) {
            ss << addressTokens[i];
            if (i + 1 < addressTokens.size())
                ss << L" ";
        }
        record.address = ss.str();
    }

    return record;
}

// Реализация прямого поиска шаблона в строке.
int directSearch(const std::wstring& text, const std::wstring& pattern) {
    if (pattern.empty()) return 0;
    if (pattern.size() > text.size()) return -1;
    for (size_t i = 0; i <= text.size() - pattern.size(); i++) {
        size_t j = 0;
        while (j < pattern.size() && text[i + j] == pattern[j])
            j++;
        if (j == pattern.size())
            return static_cast<int>(i);
    }
    return -1;
}

// Построение таблицы смещений для алгоритма Бойера–Мура.
std::unordered_map<wchar_t, int> buildBadCharTable(const std::wstring& pattern) {
    std::unordered_map<wchar_t, int> badChar;
    for (size_t i = 0; i < pattern.size(); i++) {
        badChar[pattern[i]] = static_cast<int>(i);
    }
    return badChar;
}

// Реализация поиска методом Бойера–Мура.
int boyerMooreSearch(const std::wstring& text, const std::wstring& pattern) {
    if (pattern.empty()) return 0;
    if (pattern.size() > text.size()) return -1;
    std::unordered_map<wchar_t, int> badChar = buildBadCharTable(pattern);
    int n = static_cast<int>(text.size());
    int m = static_cast<int>(pattern.size());
    int shift = 0;
    while (shift <= n - m) {
        int j = m - 1;
        while (j >= 0 && pattern[j] == text[shift + j])
            j--;
        if (j < 0) {
            return shift;
        }
        else {
            wchar_t c = text[shift + j];
            int lastOccur = -1;
            if (badChar.find(c) != badChar.end())
                lastOccur = badChar[c];
            shift += std::max(1, j - lastOccur);
        }
    }
    return -1;
}
