#include "helpers.h"
#include <algorithm>

using namespace std;

string ToUtf8(const wchar_t* buffer, int len)
{
    int n_chars = ::WideCharToMultiByte(CP_UTF8, 0, buffer, len, NULL, 0, NULL, NULL);
    if (n_chars == 0) return "";
    string new_buffer;
    new_buffer.resize(n_chars);
    ::WideCharToMultiByte(CP_UTF8, 0, buffer, len, &new_buffer[0], n_chars, NULL, NULL);
    return new_buffer;
}

string ToUtf8(std::wstring str)
{
    return ToUtf8(str.c_str(), (int)str.size());
}

vector<wstring> Split(const wstring& word, const wchar_t& kDelim) {
    wstring a;
    vector<wstring> result;
    for (wchar_t i : word) {
        if (i == kDelim) {
            result.push_back(a);
            a.erase();
        }
        else if (i != '[' && i != ']') {
            a += i;
        }
    }
    result.push_back(a);
    return result;
}

vector<int> WsToVI(wstring value)
{
    auto a = Split(value, ',');
    vector<int> b = {};
    if (a.size() > 1)
    {
        transform(a.begin(), a.end(), back_inserter(b), [](const wstring& ws) {
            return stoi(ws);
            });
    }
    else
    {
        b = { -1 };
    }
    return b;
}