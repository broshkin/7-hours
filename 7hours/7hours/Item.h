#ifndef ITEM_H
#define ITEM_H

#include <string>

struct Item {
    int id;
    std::wstring name;
    int damage;
    int damage_distance;
    bool is_key_item;
};

#endif // ITEM_H