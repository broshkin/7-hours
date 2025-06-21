#ifndef GAMESTATE_H
#define GAMESTATE_H
         
#include <unordered_map>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <ctime>

class Enemy;
class Interaction;
struct Item;

class GameState {
private:
    int current_location_id_;
    std::vector<Item*> inventory_;
    int health_;
    int morale_;
    int damage_;
    bool in_fight_;
    std::vector<int> completed_interaction_ids_;

    Enemy* GetClosestEnemy(std::vector<Enemy*>& enemies);

public:
    GameState(const int& _current_location_id,
        const std::vector<Item*>& _inventory,
        const int& _health,
        const int& _morale,
        const int& _damage,
        const bool& _in_fight,
        const std::vector<int>& _completed_interaction_ids); 

    void ShowInventory();
    void AddItemToInventory(Item* item);
    void RemoveItemFromInventoryById(int id_);
    void StartCombat(std::vector<Enemy*>& enemies);
    void PlayerTurn(std::vector<Enemy*>& enemies);
    void PlayerAttack(std::vector<Enemy*>& enemies);
    void ShowEnemyInfos(std::vector<Enemy*>& enemies);
    void EnemyTurn(std::vector<Enemy*>& enemies);

    std::vector<Item*> inventory();
    void set_health_(int x);
    int health();
    void set_morale_(int x);
    int morale();
    void set_damage_(int x);
    int damage();
    void set_in_fight_(bool x);
    bool in_fight();
    void set_location_id_(int x);
    int location_id();
    void ChangeMorale(int value);
    void ChangeHealth(int value);
    void ChangeDamage(int value);
    void ApplyBuff(int stat_id, int value);
    void MarkBranchCompleted(int parent_id, int chosen_child_id);
};

// Глобальные переменные
extern int total_time;
extern std::vector<Interaction*> interactions;
extern std::unordered_map<int, int> child_to_parent;

#endif // GAMESTATE_H