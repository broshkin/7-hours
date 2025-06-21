#ifndef INTERACTION_H
#define INTERACTION_H

#include <string>
#include <vector>

class Interaction {
private:
    int id_;
    std::wstring name_;
    int act_;
    int location_id_;
    int action_id_;
    std::wstring result_text_;
    std::vector<int> buff_;
    std::vector<int> branch_ids_;
    int time_;
    std::vector<int> condition_item_ids_;
    int location_to_move_;
    int reward_item_id_;
    bool starts_combat_;
    bool is_completed_ = false;

public:
    Interaction(const int& id_, const std::wstring& name_, const int& act_,
        const int& location_id, const int& action_id_,
        const std::wstring& result_text_, const std::vector<int>& buff_,
        const std::vector<int>& branch_ids_, const int& time_,
        const std::vector<int>& condition_item_ids_, const int& location_to_move_,
        const int& reward_item_id_, const bool& starts_combat_);

    void ShowResultText();
    std::wstring name();
    int location_id();
    std::vector<int> buff();
    std::vector<int> branch_ids();
    int time();
    std::vector<int> condition_item_ids();
    int location_to_move();
    int id();
    int reward_item_id();
    bool is_completed();
    bool starts_combat();
    void set_is_completed_(bool x);
};

#endif // INTERACTION_H