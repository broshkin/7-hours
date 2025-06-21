#include "Interaction.h"
#include <iostream>
#include <vector>

using namespace std;


    Interaction::Interaction(const int& _id, const wstring& _name, const int& _act, const int& _location_id, const int& _action_id,
        const wstring& _result_text, const vector<int>& _buff, const vector<int>& _branch_ids,
        const int& _time, const vector<int>& _condition_item_ids, const int& _location_to_move,
        const int& _reward_item_id, const bool& _starts_combat)
        : id_(_id), name_(_name), act_(_act), location_id_(_location_id), action_id_(_action_id),
        result_text_(_result_text), buff_(_buff), branch_ids_(_branch_ids), time_(_time),
        condition_item_ids_(_condition_item_ids), location_to_move_(_location_to_move), reward_item_id_(_reward_item_id), starts_combat_(_starts_combat)
    {
    }
    void Interaction::ShowResultText(){
        wcout << endl;
        wcout << result_text_ << endl;
    }
    wstring Interaction::name() { return name_; }
    int Interaction::location_id() { return location_id_; }
    vector<int> Interaction::buff() { return buff_; }
    vector<int> Interaction::branch_ids() { return branch_ids_; }
    int Interaction::time() { return time_; }
    vector<int>Interaction::condition_item_ids() { return condition_item_ids_; }
    int Interaction::location_to_move() { return location_to_move_; }
    int Interaction::id() { return id_; }
    int Interaction::reward_item_id() { return reward_item_id_; }
    bool Interaction::is_completed() { return is_completed_; }
    bool Interaction::starts_combat() { return starts_combat_; }
    void Interaction::set_is_completed_(bool x) { is_completed_ = x; }