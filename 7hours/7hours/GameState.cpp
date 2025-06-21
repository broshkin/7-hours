#include "GameState.h"
#include "Enemy.h"
#include "Interaction.h"
#include "Item.h"
#include <iostream>

struct Item;

using std::endl;
using std::wcout;
using std::wcin;

Enemy* GameState::GetClosestEnemy(std::vector<Enemy*>& enemies) {
    Enemy* closest = nullptr;
    int minDist = INT_MAX;
    for (auto enemy : enemies) {
        if (enemy->distance() < minDist) {
            minDist = enemy->distance();
            closest = enemy;
        }
    }
    return closest;
}

GameState::GameState(const int& _current_location_id, const std::vector<Item*>& _inventory, const int& _health, const int& _morale, const int& _damage, const bool& _in_fight, const std::vector<int>& _completed_interaction_ids) :
    current_location_id_(_current_location_id), inventory_(_inventory), health_(_health), morale_(_morale), damage_(_damage), in_fight_(_in_fight), completed_interaction_ids_(_completed_interaction_ids) { }

std::vector<Item*> GameState::inventory() { return inventory_; }
void GameState::set_health_(int x) { health_ = x; }
int GameState::health() { return health_; }
void GameState::set_morale_(int x) { morale_ = x; }
int GameState::morale() { return morale_; }
void GameState::set_damage_(int x) { damage_ = x; }
int GameState::damage() { return damage_; }
void GameState::set_in_fight_(bool x) { in_fight_ = x; }
bool GameState::in_fight() { return in_fight_; }
void GameState::set_location_id_(int x) { current_location_id_ = x; }
int GameState::location_id() { return current_location_id_; }

void GameState::ChangeMorale(int value) { morale_ += value; }
void GameState::ChangeHealth(int value) { health_ += value; }
void GameState::ChangeDamage(int value) { damage_ += value; }

void GameState::ShowInventory() {
    for (size_t i = 0; i < inventory_.size(); i++) {
        std::wcout << i + 1 << L". " << inventory_[i]->name << L" (���. ���� "
            << inventory_[i]->damage << L", ��������� " << inventory_[i]->damage_distance << L")" << std::endl;
    }
}

void GameState::AddItemToInventory(Item* item) {
    inventory_.push_back(item);
}

void GameState::RemoveItemFromInventoryById(int id) {
    for (size_t i = 0; i < inventory_.size(); i++) {
        if (inventory_[i]->id == id) {
            inventory_.erase(inventory_.cbegin() + i);
            break;
        }
    }
}

void GameState::StartCombat(std::vector<Enemy*>& enemies) {
    set_in_fight_(true);
    std::wcout << L"\n�� ��� ������! ������� ���!" << std::endl << std::endl;

    while (in_fight() && !enemies.empty() && health() > 0) {
        PlayerTurn(enemies);
        if (enemies.empty() || health() <= 0) break;
        EnemyTurn(enemies);
    }
    set_in_fight_(false);

    if (health() <= 0) {
        std::wcout << L"\n���� ��������! �� ���������!" << std::endl;
        exit(0);
    }
}

void GameState::PlayerTurn(std::vector<Enemy*>& enemies) {
    system("pause");
    system("cls");

    int time_todo = 0;
    std::wcout << L"\n=== ��� ��� ===" << std::endl;
    std::wcout << L"\n��������: " << health() << std::endl;
    std::wcout << L"����: " << damage() << std::endl;

    ShowEnemyInfos(enemies);
    std::wcout << L"1. ���������" << std::endl;
    std::wcout << L"2. ������� ����� �� 2 �.�. (������ ���)" << std::endl;
    std::wcout << L"3. ������ ����� �� 2 �.�. (������ ���)" << std::endl;

    int choice;
    std::wcin >> choice;

    switch (choice) {
    case 1: {
        PlayerAttack(enemies);
        time_todo = rand() % 4 + 3;
        break;
    }
    case 2: {
        for (auto enemy : enemies) enemy->MoveCloser();
        std::wcout << L"�� ������������ �� 2 �.�." << std::endl;
        time_todo = rand() % 3 + 2;
        ShowEnemyInfos(enemies);
        break;
    }
    case 3: {
        for (auto enemy : enemies) enemy->MoveAway();
        std::wcout << L"�� ��������� �� 2 �.�." << std::endl;
        time_todo = rand() % 3 + 2;
        ShowEnemyInfos(enemies);
        break;
    }
    default:
        std::wcout << L"�������� �����! ��� ��������." << std::endl;
    }

    total_time -= time_todo;

    if (total_time <= 0) {
        std::wcout << L"\n����� �����! �� ���������!" << std::endl;
        exit(0);
    }

    std::wcout << L"\n������ " << time_todo << L" �����." << std::endl;
    std::wcout << L"�������� " << total_time / 60 << L" ����� � " << total_time % 60 << L" �����." << std::endl;
}

    void GameState::PlayerAttack(std::vector<Enemy*>& enemies) {
        // ����� ������
        std::wcout << L"\n�������� ������:" << std::endl;
        ShowInventory();
        std::wcout << inventory().size() + 1 << L". ���������� ����� (���. ���� 0, ��������� 1)" << std::endl;

        int weaponChoice;
        std::wcin >> weaponChoice;

        int weaponDamage = 0;
        int weaponRange = 1;
        Item* weapon = nullptr;

        if (weaponChoice > 0 && weaponChoice <= inventory().size()) {
            weapon = inventory()[weaponChoice - 1];
            weaponDamage = weapon->damage;
            weaponRange = weapon->damage_distance;
        }

        // ����� ����
        std::wcout << L"��������� ����:" << std::endl;
        std::vector<Enemy*> availableTargets;
        for (size_t i = 0; i < enemies.size(); ++i) {
            if (enemies[i]->distance() <= weaponRange) {
                wcout << availableTargets.size() + 1 << L". " << enemies[i]->name()
                    << L" (���������: " << enemies[i]->distance()
                    << L", HP: " << enemies[i]->health() << L")" << endl;
                availableTargets.push_back(enemies[i]);
            }
        }

        if (availableTargets.empty()) {
            wcout << L"��� ����� � ������� �����!" << endl;
            return;
        }

        int targetChoice;
        wcin >> targetChoice;

        if (targetChoice < 1 || targetChoice > availableTargets.size()) {
            wcout << L"�������� ����� ����!" << endl;
            return;
        }

        Enemy* target = availableTargets[targetChoice - 1];

        // ������ �����
        int totalDamage = damage() + weaponDamage + (rand() % 3 + 1);

        if (rand() % 2 == 0) { // 50% ���� �� ������
            wcout << L"�� ������������!" << endl;
            return;
        }

        wcout << L"�� �������� " << totalDamage << L" �����!" << endl;
        target->TakeDamage(totalDamage);

        enemies.erase(std::remove_if(enemies.begin(), enemies.end(), [](Enemy* enemy) {
            if (enemy->health() <= 0) {
                delete enemy;
                return true;
            }
            return false; }), enemies.end()
                );
    }

    void GameState::ShowEnemyInfos(std::vector<Enemy*>& enemies) {
        wcout << L"\n������� �����:" << endl;
        for (size_t i = 0; i < enemies.size(); ++i) {
            wcout << i + 1 << L". " << enemies[i]->name()
                << L": " << enemies[i]->health() << L" HP, " << enemies[i]->distance() << L" �.�." << endl;
        }
        wcout << endl;
    }

    void GameState::EnemyTurn(std::vector<Enemy*>& enemies) {
        wcout << L"\n=== ��� ������ ===" << endl;
        for (auto enemy : enemies) {
            if (health() <= 0) break;

            if (rand() % 2 == 0) { // 50% ���� �����
                int damage_ = enemy->damage();
                ChangeHealth(-damage_);
                wcout << enemy->name() << L" ������� � ������� "
                    << damage_ << L" �����!" << endl;
            }
            else {
                wcout << enemy->name() << L" �����������" << endl;
            }
        }
    }

    void GameState::ApplyBuff(int stat_id, int value)
    {
        switch (stat_id)
        {
        case 0: ChangeMorale(value); wcout << L"\n������ " << (value > 0 ? L"+" : L"") << value << L"!" << endl; break;
        case 1: ChangeHealth(value); wcout << L"\n�������� " << (value > 0 ? L"+" : L"") << value << L"!" << endl; break;
        case 2: ChangeDamage(value); wcout << L"\n���� " << (value > 0 ? L"+" : L"") << value << L"!" << endl; break;
        }
    }

    void GameState::MarkBranchCompleted(int parent_id, int chosen_child_id)
    {
        interactions[chosen_child_id]->set_is_completed_(true);

        for (size_t i = 0; i < ::interactions.size(); i++)
        {
            Interaction* interaction = ::interactions[i];
            auto it = child_to_parent.find(interaction->id());

            if (it != child_to_parent.end() && it->second == parent_id)
            {
                if (interaction->id() != chosen_child_id)
                {
                    interaction->set_is_completed_(true);
                }
            }
        }
    }
