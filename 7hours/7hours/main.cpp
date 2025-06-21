#include <Windows.h>
#include <locale>
#include <fstream>
#include <codecvt>
#include <algorithm>
#include <iostream>

#include "GameState.h"
#include "Enemy.h"
#include "Interaction.h"
#include "Item.h"
#include "Location.h"
#include "helpers.h"

using namespace std;

vector<Interaction*> interactions;
unordered_map<int, int> child_to_parent;
int total_time = 420;

const wchar_t kDelim = ';';

const string kInteractionsPath = "data/interactions_7hours.txt";
const string kLocationsPath = "data/locations_7hours.txt";
const string kItemsPath = "data/items_7hours.txt";
const string kEnemiesPath = "data/enemies_7hours.txt";
const string kGameStatePath = "data/gameState_7hours.txt";
const string kActionsPath = "data/actions_7hours.txt";

vector<Location*> locations;
vector<Item*> items;
vector<Enemy*> enemies;

GameState* game_state;

void BuildParentMap() {

    child_to_parent.clear();
    for (size_t i = 0; i < interactions.size(); i++) {

        Interaction* interaction = interactions[i];
        vector<int> branch_ids_ = interaction->branch_ids();

        for (size_t j = 0; j < branch_ids_.size(); j++) {

            int child_id = branch_ids_[j];

            if (child_id != -1) {
                child_to_parent[child_id] = interaction->id();
            }
        }
    }
}

void Parse()
{
    ifstream in;
    in.open(kLocationsPath, ios::binary | ios::in);
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    string get_line;
    wstring line;
    vector<wstring> str;

    if (in.is_open()) {
        while (getline(in, get_line)) {

            line = converter.from_bytes(get_line);
            str = Split(line, kDelim);
            Location* location = new Location;
            location->id = stoi(str[0]);
            location->name = str[1];
            location->act = stoi(str[2]);
            location->on_enter_text = str[3];
            location->on_exit_text = str[4];
            location->branch_ids = WsToVI(str[5]);
            locations.push_back(location);
        }
    }
    in.close();

    in.open(kInteractionsPath, ios::binary | ios::in);

    if (in.is_open()) {
        while (getline(in, get_line)) {

            line = converter.from_bytes(get_line);
            str = Split(line, kDelim);
            Interaction* interaction = new Interaction(
                stoi(str[0]), str[1], stoi(str[2]), stoi(str[3]), stoi(str[4]), str[5],
                WsToVI(str[6]), WsToVI(str[7]), stoi(str[8]), WsToVI(str[9]), stoi(str[10]), stoi(str[11]), stoi(str[12]));
            interactions.push_back(interaction);
        }
    }
    in.close();

    in.open(kItemsPath, ios::binary | ios::in);

    if (in.is_open()) {
        while (getline(in, get_line))  {

            line = converter.from_bytes(get_line);
            str = Split(line, kDelim);
            Item* item = new Item;
            item->id = stoi(str[0]);
            item->name = str[1];
            item->damage = stoi(str[2]);
            item->damage_distance = stoi(str[3]);
            items.push_back(item);
        }
    }
    in.close();

    in.open(kEnemiesPath, ios::binary | ios::in);

    if (in.is_open()) {
        while (getline(in, get_line)){

            line = converter.from_bytes(get_line);
            str = Split(line, kDelim);
            Enemy* enemy = new Enemy(stoi(str[0]), str[1], stoi(str[2]), stoi(str[3]), stoi(str[4]), stoi(str[5]), stoi(str[6]));
            enemies.push_back(enemy);
        }
    }
    in.close();

    in.open(kGameStatePath, ios::binary | ios::in);

    if (in.is_open()) {

        game_state = new GameState(0, {}, 0, 0, 0, false, {});

        while (getline(in, get_line)) {

            line = converter.from_bytes(get_line);

            str = Split(line, kDelim);
            switch (stoi(str[0])) {
            case 0: game_state->set_morale_(stoi(str[2])); break;
            case 1: game_state->set_health_(stoi(str[2])); break;
            case 2: game_state->set_damage_(stoi(str[2])); break;
            }
        }
        game_state->set_in_fight_(false);
        game_state->set_location_id_(0);
        game_state = game_state;
    }
    in.close();

    BuildParentMap();
}

void ReleaseInteraction(vector<Interaction*> available_interactions) {
    if (available_interactions.empty()) {
        wcout << L"\nЗдесь не с чем/кем взаимодействовать или не выполнены условия." << endl;
    }
    else {
        for (size_t i = 0; i < available_interactions.size(); i++) {
            wcout << i + 1 << L". " << available_interactions[i]->name() << endl;
        }

        wcout << L"\nВвод: ";
        int int_choice;
        wcin >> int_choice;

        if (int_choice > 0 && int_choice <= available_interactions.size()) {

            Interaction* chosen_interaction = available_interactions[int_choice - 1];

            vector<Enemy*> combat_enemies;
            for (auto enemy : enemies) {
                if (enemy->interact_id() == chosen_interaction->id()) {
                    enemy->set_distance_(rand() % 5 + 2); // Устанавливаем случайную дистанцию
                    combat_enemies.push_back(enemy);
                }
            }

            if (!combat_enemies.empty()) {
                game_state->StartCombat(combat_enemies);
            }

            chosen_interaction->ShowResultText();
            auto it = child_to_parent.find(chosen_interaction->id());
            if (it != child_to_parent.end()) {
                int parent_id = it->second;
                game_state->MarkBranchCompleted(parent_id, chosen_interaction->id());
            }
            else {
                chosen_interaction->set_is_completed_(true);
            }

            if (chosen_interaction->branch_ids().size() > 0 && chosen_interaction->branch_ids()[0] != -1) {
                vector<Interaction*> children;
                for (int i = 0; i < chosen_interaction->branch_ids().size(); i++) {
                    children.push_back(interactions[chosen_interaction->branch_ids()[i]]);
                }

                wcout << endl;
                ReleaseInteraction(children);
            }

            vector<int> buff = chosen_interaction->buff();

            if (buff.size() >= 2 && buff[0] != -1) {
                for (int i = 0; i < buff.size(); i += 2) {
                    game_state->ApplyBuff(buff[i], buff[i + 1]);
                }
            }
            // --- ВЫДАЧА ПРЕДМЕТОВ ---
            int reward_item_id = chosen_interaction->reward_item_id();

            if (reward_item_id != -1) {
                game_state->AddItemToInventory(items[reward_item_id]);
                wcout << L"\nПолучен предмет: " << items[reward_item_id]->name << endl;
            }


            if (chosen_interaction->time() > 0) {
                total_time -= chosen_interaction->time();
                wcout << L"\nПрошло " << chosen_interaction->time() << L" минут." << endl;
                wcout << L"Осталось " << total_time / 60 << L" часов и " << total_time % 60 << L" минут." << endl;
            }

            int move_to_location_id = chosen_interaction->location_to_move();

            if (move_to_location_id != -1) {
                Location* new_location = locations[move_to_location_id];

                if (new_location != nullptr) {
                    wcout << endl;
                    wcout << locations[game_state->location_id()]->on_exit_text << endl;

                    game_state->set_location_id_(move_to_location_id);

                    wcout << L"\n=== " << new_location->name << L" ===" << endl;
                    wcout << endl;
                    wcout << new_location->on_enter_text << endl;
                }
            }
        }
    }
}

// ==================== Главный игровой цикл ====================
void GameLoop() {
    bool game_running = true;
    int current_act = 1;

    for (size_t i = 0; i < locations.size(); i++) {
        if (locations[i]->act == current_act) {
            game_state->set_location_id_(locations[i]->id);
           
            wcout << L"=== " << locations[i]->name << L" ===" << endl;
            wcout << locations[i]->on_enter_text << endl;
            wcout << endl;
            break;
        }
    }

    while (game_running) {
        system("pause");
        system("cls");

        if (total_time <= 0) {
            wcout << L"\nВремя вышло! Вы проиграли!" << endl;
            exit(0);
        }

        int current_location_id = game_state->location_id();
        Location* current_location = nullptr;

        for (size_t i = 0; i < locations.size(); i++) {
            if (locations[i]->id == current_location_id) {
                current_location = locations[i];
                break;
            }
        }

        if (!current_location) {
            wcout << L"Ошибка: текущая локация не найдена!" << endl;
            break;
        }

        wcout << L"Что вы хотите сделать?" << endl;
        wcout << L"1. Осмотреться" << endl;
        wcout << L"2. Взаимодействовать с объектами" << endl;
        wcout << L"3. Посмотреть состояние" << endl;
        wcout << L"4. Проверить инвентарь" << endl;
        wcout << L"5. Выйти из игры" << endl;

        wcout << L"\nВвод: ";
        int choice;
        wcin >> choice;

        switch (choice) {
        case 1: {
            wcout << current_location->on_enter_text << endl;
            break;
        }
        case 2: {
            wcout << L"\nС чем/кем вы хотите взаимодействовать?" << endl;
            vector<Interaction*> availableInteractions;
            for (int i = 0; i < interactions.size(); i++) {

                Interaction* interaction = interactions[i];

                if (interaction->location_id() == game_state->location_id()) {

                    if (interaction->is_completed())
                        continue;

                    bool conditionsMet = true;
                    vector<int> condItems = interaction->condition_item_ids();

                    for (int j = 0; j < condItems.size(); j++) {

                        int itemId = condItems[j];

                        if (itemId != -1) {
                            bool hasItem = false;
                            vector<Item*> inv = game_state->inventory();

                            for (int k = 0; k < inv.size(); k++) {

                                if (inv[k]->id == itemId) {
                                    hasItem = true;
                                    break;
                                }
                            }

                            if (!hasItem) {
                                conditionsMet = false;
                                break;
                            }
                        }
                    }

                    if (!conditionsMet)
                        continue;

                    auto it = child_to_parent.find(interaction->id());
                    if (it != child_to_parent.end()) {
                        continue;
                    }

                    availableInteractions.push_back(interaction);
                }
            }

            ReleaseInteraction(availableInteractions);
            break;
        }
        case 3: {
            wcout << L"\n=== СОСТОЯНИЕ ===" << endl;
            wcout << L"Здоровье: " << game_state->health() << endl;
            wcout << L"Мораль: " << game_state->morale() << endl;
            wcout << L"Урон: " << game_state->damage() << endl;
            wcout << L"Текущая локация: ";
            for (size_t i = 0; i < locations.size(); i++) {
                if (locations[i]->id == current_location_id) {
                    wcout << locations[i]->name << endl;
                    break;
                }
            }
            break;
        }
        case 4: {
            wcout << L"\n=== ИНВЕНТАРЬ ===" << endl;
            game_state->ShowInventory();
            wcout << endl;
            break;
        }
        case 5: {
            game_running = false;
            wcout << L"Игра завершена." << endl;
            break;
        }
        default: {
            wcout << L"Неверный выбор! Попробуйте снова." << endl;
            break;
        }
        }
        wcout << endl;
    }

}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    locale::global(locale("Russian"));
    setlocale(LC_ALL, "rus");

    Parse();
    GameLoop();

    for (size_t i = 0; i < items.size(); i++) delete items[i];
    for (size_t i = 0; i < locations.size(); i++) delete locations[i];
    for (size_t i = 0; i < interactions.size(); i++) delete interactions[i];
    for (size_t i = 0; i < enemies.size(); i++) delete enemies[i];

    delete game_state;

    return 0;
}
