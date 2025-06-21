#ifndef LOCATION_H
#define LOCATION_H

#include <string>
#include <vector>

struct Location {
    int id;
    std::wstring name;
    int act;
    std::wstring on_enter_text;
    std::wstring on_exit_text;
    std::vector<int> branch_ids;
};

#endif // LOCATION_H