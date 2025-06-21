#ifndef ENEMY_H
#define ENEMY_H

#include <string>
#include <iostream>

class Enemy {
private:
    int id_;
    std::wstring name_;
    int health_;
    int damage_;
    int drop_item_id_;
    int damage_distance_;
    int distance_;
    int interact_id_;

public:
    Enemy(const int& id_, const std::wstring& name_, const int& damage_,
        const int& drop_item_id_, const int& damage_distance_,
        const int& health, const int& interact_id_);

    int id();
    const std::wstring& name();
    int health();
    int damage();
    int drop_item_id();
    int damage_distance();
    int distance();
    int interact_id();
    void set_distance_(int dist);
    void MoveCloser();
    void MoveAway();
    void TakeDamage(int num);
};

#endif // ENEMY_H