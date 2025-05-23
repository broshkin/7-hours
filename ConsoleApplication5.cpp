#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>
#include <algorithm>
#include <chrono>
#include <thread>
#include <ctime>
#include <fstream>
#include <codecvt>
#include <locale>
#include <unordered_map>

using namespace std;

// Глобальные переменные
vector<class Interaction*> interactions;
unordered_map<int, int> childToParent;

string interactions_path = "interactions_7hours.txt";
string locations_path = "locations_7hours.txt";
string items_path = "items_7hours.txt";
string enemies_path = "enemies_7hours.txt";
string gameState_path = "gameState_7hours.txt";
string actions_path = "actions_7hours.txt";

// ==================== Классы данных ====================
struct Item {
    int id;
    wstring name;
    int damage;
    int damageDistance;
    bool isKeyItem;
};

class Interaction {
private:
    int id;
    wstring name;
    int act;
    int locationId;
    int actionId;
    wstring resultText;
    vector<int> buff;
    vector<int> branchIds;
    int time;
    vector<int> conditionItemIds;
    int locationToMove;
    vector<int> rewardItemIds;
public:
    Interaction(const int& _id, const wstring& _name, const int& _act, const int& _locationId, const int& _actionId, const wstring& _resultText, const vector<int>& _buff, const vector<int>& _branchIds, const int& _time, const vector<int>& _conditionItemIds, const int& _locationToMove, const vector<int>& _rewardItemIds)
    {
        id = _id;
        name = _name;
        act = _act;
        locationId = _locationId;
        actionId = _actionId;
        resultText = _resultText;
        buff = _buff;
        branchIds = _branchIds;
        time = _time;
        conditionItemIds = _conditionItemIds;
        locationToMove = _locationToMove;
        rewardItemIds = _rewardItemIds;
    }
    void ShowResultText()
    {
        wcout << resultText << endl;
    }
    wstring GetName() const { return name; }
    int GetLocationId() const { return locationId; }
    vector<int> GetBuff() const { return buff; }
    vector<int> GetBranchIds() const { return branchIds; }
    int GetTime() const { return time; }
    vector<int> GetConditionItemIds() const { return conditionItemIds; }
    int GetLocationToMove() const { return locationToMove; }
    int GetId() const { return id; }
    vector<int> GetRewardItemIds() const { return rewardItemIds; }
};

struct Location {
    int id;
    wstring name;
    int act;
    wstring onEnterText;
    wstring onExitText;
    vector<int> branchIds;
};

class Enemy {
private:
    int id;
    wstring name;
    int health;
    int damage;
    int dropItemId;
    int damageDistance;
public:
    Enemy(const int& _id, const wstring& _name, const int& _health, const int& _damage, const int& _dropItemId, const int& _damageDistance)
    {
        id = _id;
        name = _name;
        health = _health;
        damage = _damage;
        dropItemId = _dropItemId;
        damageDistance = _damageDistance;
    }
    void Attack() {}
    void Die() {}
};

// ==================== Игровое состояние ====================
class GameState {
private:
    int currentLocationId;
    vector<Item*> inventory;
    int health;
    int morale;
    int damage;
    bool inFight;
    vector<int> completedInteractionIds;
public:
    void ShowInventory()
    {
        for (size_t i = 0; i < inventory.size(); i++)
        {
            wcout << i + 1 << L". " << inventory[i]->name << endl;
        }
    }
    void AddItemInInventory(Item* item)
    {
        inventory.push_back(item);
    }
    void RemoveItemFromInventoryById(int id)
    {
        for (size_t i = 0; i < inventory.size(); i++)
        {
            if (inventory[i]->id == id)
            {
                inventory.erase(inventory.cbegin() + i);
                break;
            }
        }
    }
    vector<Item*> GetInventory() { return inventory; }
    void SetHealth(int x) { health = x; }
    int GetHealth() { return health; }
    void SetMorale(int x) { morale = x; }
    int GetMorale() { return morale; }
    void SetDamage(int x) { damage = x; }
    int GetDamage() { return damage; }
    void SetInFight(bool x) { inFight = x; }
    bool GetInFight() { return inFight; }
    void SetLocationId(int x) { currentLocationId = x; }
    int GetLocationId() { return currentLocationId; }
    void ChangeMorale(int value) { morale += value; }
    void ChangeHealth(int value) { health += value; }
    void ChangeDamage(int value) { damage += value; }
    void ApplyBuff(int statId, int value) {
        switch (statId) {
        case 0: ChangeMorale(value); wcout << L"Мораль " << (value > 0 ? L"+" : L"") << value << L"!" << endl; break;
        case 1: ChangeHealth(value); wcout << L"Здоровье " << (value > 0 ? L"+" : L"") << value << L"!" << endl; break;
        case 2: ChangeDamage(value); wcout << L"Урон " << (value > 0 ? L"+" : L"") << value << L"!" << endl; break;
        }
    }
    void MarkInteractionCompleted(int interactionId) {
        if (find(completedInteractionIds.begin(), completedInteractionIds.end(), interactionId) == completedInteractionIds.end())
            completedInteractionIds.push_back(interactionId);
    }
    bool IsInteractionCompleted(int interactionId) const {
        return find(completedInteractionIds.begin(), completedInteractionIds.end(), interactionId) != completedInteractionIds.end();
    }
    void MarkBranchCompleted(int parentId, int chosenChildId) {
        MarkInteractionCompleted(chosenChildId);
        for (size_t i = 0; i < ::interactions.size(); i++) {
            Interaction* interaction = ::interactions[i];
            auto it = childToParent.find(interaction->GetId());
            if (it != childToParent.end() && it->second == parentId) {
                if (interaction->GetId() != chosenChildId) {
                    MarkInteractionCompleted(interaction->GetId());
                }
            }
        }
    }
};

vector<Location*> locations;
vector<Item*> items;
vector<Enemy*> enemies;
GameState* GS;

string to_utf8(const wchar_t* buffer, int len)
{
    int nChars = ::WideCharToMultiByte(CP_UTF8, 0, buffer, len, NULL, 0, NULL, NULL);
    if (nChars == 0) return "";
    string newbuffer;
    newbuffer.resize(nChars);
    ::WideCharToMultiByte(CP_UTF8, 0, buffer, len, &newbuffer[0], nChars, NULL, NULL);
    return newbuffer;
}

string to_utf8(wstring str)
{
    return to_utf8(str.c_str(), (int)str.size());
}

vector<wstring> split(const wstring& word, const wchar_t& delim)
{
    wstring a;
    vector<wstring> result;
    for (wchar_t i : word)
    {
        if (i == delim)
        {
            result.push_back(a);
            a.erase();
        }
        else if (i != '[' && i != ']')
        {
            a += i;
        }
    }
    result.push_back(a);
    return result;
}

vector<int> wstovi(wstring x)
{
    auto a = split(x, ',');
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

void buildParentMap() {
    childToParent.clear();
    for (size_t i = 0; i < interactions.size(); i++) {
        Interaction* interaction = interactions[i];
        vector<int> branchIds = interaction->GetBranchIds();
        for (size_t j = 0; j < branchIds.size(); j++) {
            int childId = branchIds[j];
            if (childId != -1) {
                childToParent[childId] = interaction->GetId();
            }
        }
    }
}

void parse()
{
    ifstream in;
    in.open(locations_path, ios::binary | ios::in);
    wstring_convert<codecvt_utf8<wchar_t>> converter;
    string get_line;
    wstring line;
    vector<wstring> str;

    if (in.is_open())
    {
        while (getline(in, get_line))
        {
            line = converter.from_bytes(get_line);
            str = split(line, ';');
            Location* location = new Location;
            location->id = stoi(str[0]);
            location->name = str[1];
            location->act = stoi(str[2]);
            location->onEnterText = str[3];
            location->onExitText = str[4];
            location->branchIds = wstovi(str[5]);
            locations.push_back(location);
        }
    }
    in.close();

    in.open(interactions_path, ios::binary | ios::in);

    if (in.is_open())
    {
        while (getline(in, get_line))
        {
            line = converter.from_bytes(get_line);
            str = split(line, ';');
            vector<int> rewardItemIds = (str.size() > 11) ? wstovi(str[11]) : vector<int>();
            Interaction* interaction = new Interaction(
                stoi(str[0]), str[1], stoi(str[2]), stoi(str[3]), stoi(str[4]), str[5],
                wstovi(str[6]), wstovi(str[7]), stoi(str[8]), wstovi(str[9]), stoi(str[10]), rewardItemIds
            );
            interactions.push_back(interaction);
        }
    }
    in.close();

    in.open(items_path, ios::binary | ios::in);

    if (in.is_open())
    {
        while (getline(in, get_line))
        {
            line = converter.from_bytes(get_line);
            str = split(line, ';');
            Item* item = new Item;
            item->id = stoi(str[0]);
            item->name = str[1];
            item->damage = stoi(str[2]);
            item->damageDistance = stoi(str[3]);
            items.push_back(item);
        }
    }
    in.close();

    in.open(enemies_path, ios::binary | ios::in);

    if (in.is_open())
    {
        while (getline(in, get_line))
        {
            line = converter.from_bytes(get_line);
            str = split(line, ';');
            Enemy* enemy = new Enemy(stoi(str[0]), str[1], stoi(str[2]), stoi(str[3]), stoi(str[4]), stoi(str[5]));
            enemies.push_back(enemy);
        }
    }
    in.close();

    in.open(gameState_path, ios::binary | ios::in);

    if (in.is_open())
    {
        GameState* gameState = new GameState;
        while (getline(in, get_line))
        {
            line = converter.from_bytes(get_line);
            str = split(line, ';');
            switch (stoi(str[0]))
            {
            case 0: gameState->SetMorale(stoi(str[2])); break;
            case 1: gameState->SetHealth(stoi(str[2])); break;
            case 2: gameState->SetDamage(stoi(str[2])); break;
            }
        }
        gameState->SetInFight(false);
        gameState->SetLocationId(0);
        GS = gameState;
    }
    in.close();

    buildParentMap();
}

// ==================== Главный игровой цикл ====================
void GameLoop() {
    bool gameRunning = true;
    int currentAct = 1;

    for (size_t i = 0; i < locations.size(); i++) {
        if (locations[i]->act == currentAct) {
            GS->SetLocationId(locations[i]->id);
            wcout << L"=== " << locations[i]->name << L" ===" << endl;
            wcout << locations[i]->onEnterText << endl;
            break;
        }
    }

    while (gameRunning) {
        system("pause");
        system("cls");
        int currentLocationId = GS->GetLocationId();
        Location* currentLocation = nullptr;

        for (size_t i = 0; i < locations.size(); i++) {
            if (locations[i]->id == currentLocationId) {
                currentLocation = locations[i];
                break;
            }
        }

        if (!currentLocation) {
            wcout << L"Ошибка: текущая локация не найдена!" << endl;
            break;
        }

        wcout << L"\nЧто вы хотите сделать?" << endl;
        wcout << L"1. Осмотреться" << endl;
        wcout << L"2. Взаимодействовать с объектами" << endl;
        wcout << L"3. Посмотреть состояние" << endl;
        wcout << L"4. Проверить инвентарь" << endl;
        wcout << L"5. Выйти из игры" << endl;

        wcout << L"Ввод: ";
        int choice;
        wcin >> choice;

        switch (choice) {
        case 1: {
            wcout << currentLocation->onEnterText << endl;
            break;
        }
        case 2: {
            wcout << L"\nС чем вы хотите взаимодействовать?" << endl;
            vector<Interaction*> availableInteractions;
            for (size_t i = 0; i < interactions.size(); i++) {
                Interaction* interaction = interactions[i];
                if (interaction->GetLocationId() == currentLocationId) {
                    if (GS->IsInteractionCompleted(interaction->GetId()))
                        continue;
                    bool conditionsMet = true;
                    vector<int> condItems = interaction->GetConditionItemIds();
                    for (size_t j = 0; j < condItems.size(); j++) {
                        int itemId = condItems[j];
                        if (itemId != -1) {
                            bool hasItem = false;
                            vector<Item*> inv = GS->GetInventory();
                            for (size_t k = 0; k < inv.size(); k++) {
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
                    auto it = childToParent.find(interaction->GetId());
                    if (it != childToParent.end()) {
                        int parentId = it->second;
                        if (!GS->IsInteractionCompleted(parentId)) {
                            continue;
                        }
                    }
                    availableInteractions.push_back(interaction);
                }
            }
            if (availableInteractions.empty()) {
                wcout << L"Здесь не с чем взаимодействовать или не выполнены условия." << endl;
                break;
            }
            for (size_t i = 0; i < availableInteractions.size(); i++) {
                wcout << i + 1 << L". " << availableInteractions[i]->GetName() << endl;
            }
            wcout << L"Ввод: ";
            int intChoice;
            wcin >> intChoice;
            if (intChoice > 0 && intChoice <= (int)availableInteractions.size()) {
                Interaction* chosenInteraction = availableInteractions[intChoice - 1];
                chosenInteraction->ShowResultText();
                auto it = childToParent.find(chosenInteraction->GetId());
                if (it != childToParent.end()) {
                    int parentId = it->second;
                    GS->MarkBranchCompleted(parentId, chosenInteraction->GetId());
                }
                else {
                    GS->MarkInteractionCompleted(chosenInteraction->GetId());
                }
                vector<int> buff = chosenInteraction->GetBuff();
                if (buff.size() == 2 && buff[0] != -1) {
                    GS->ApplyBuff(buff[0], buff[1]);
                }
                // --- ВЫДАЧА ПРЕДМЕТОВ ---
                vector<int> rewardItemIds = chosenInteraction->GetRewardItemIds();
                for (int itemId : rewardItemIds) {
                    if (itemId == -1) continue;
                    auto itItem = find_if(items.begin(), items.end(),
                        [itemId](Item* item) { return item->id == itemId; });
                    if (itItem != items.end()) {
                        GS->AddItemInInventory(*itItem);
                        wcout << L"Получен предмет: " << (*itItem)->name << endl;
                    }
                }
                if (chosenInteraction->GetTime() > 0) {
                    wcout << L"Прошло " << chosenInteraction->GetTime() << L" минут." << endl;
                }
                int moveToLocationId = chosenInteraction->GetLocationToMove();
                if (moveToLocationId != -1) {
                    Location* newLoc = nullptr;
                    for (size_t i = 0; i < locations.size(); i++) {
                        if (locations[i]->id == moveToLocationId) {
                            newLoc = locations[i];
                            break;
                        }
                    }
                    if (newLoc != nullptr) {
                        wcout << currentLocation->onExitText << endl;
                        GS->SetLocationId(moveToLocationId);
                        wcout << L"\n=== " << newLoc->name << L" ===" << endl;
                        wcout << newLoc->onEnterText << endl;
                    }
                }
            }
            break;
        }
        case 3: {
            wcout << L"\n=== СОСТОЯНИЕ ===" << endl;
            wcout << L"Здоровье: " << GS->GetHealth() << endl;
            wcout << L"Мораль: " << GS->GetMorale() << endl;
            wcout << L"Урон: " << GS->GetDamage() << endl;
            wcout << L"Текущая локация: ";
            for (size_t i = 0; i < locations.size(); i++) {
                if (locations[i]->id == currentLocationId) {
                    wcout << locations[i]->name << endl;
                    break;
                }
            }
            break;
        }
        case 4: {
            wcout << L"\n=== ИНВЕНТАРЬ ===" << endl;
            GS->ShowInventory();
            wcout << endl;
            break;
        }
        case 5: {
            gameRunning = false;
            wcout << L"Игра завершена." << endl;
            break;
        }
        default: {
            wcout << L"Неверный выбор! Попробуйте снова." << endl;
            break;
        }
        }
    }
}

// ==================== Главная функция ====================
int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    locale::global(locale("Russian"));
    setlocale(LC_ALL, "rus");
    parse();
    GameLoop();
    for (size_t i = 0; i < items.size(); i++) delete items[i];
    for (size_t i = 0; i < locations.size(); i++) delete locations[i];
    for (size_t i = 0; i < interactions.size(); i++) delete interactions[i];
    for (size_t i = 0; i < enemies.size(); i++) delete enemies[i];
    delete GS;
    return 0;
}
