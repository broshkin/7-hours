#ifndef HELPERS_H
#define HELPERS_H

#include <string>
#include <vector>
#include <codecvt>
#include <Windows.h>

using namespace std;

string ToUtf8(const wstring& str);
string ToUtf8(wstring str);
vector<wstring> Split(const wstring& word, const wchar_t& kDelim);
vector<int> WsToVI(wstring value);

#endif // HELPERS_H