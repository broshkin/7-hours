#include "Enemy.h"
#include <algorithm>

Enemy::Enemy(const int& _id, const std::wstring& _name, const int& _damage,
    const int& _drop_item_id, const int& _damage_distance, const int& _health,
    const int& _interact_id)
    : id_(_id), name_(_name), health_(_health), damage_(_damage),
    drop_item_id_(_drop_item_id), damage_distance_(_damage_distance),
    interact_id_(_interact_id), distance_(rand() % 5 + 2) {
}

// Реализация всех методов
int Enemy::id() { return id_; }
const std::wstring& Enemy::name() { return name_; }
int Enemy::health() { return health_; }
int Enemy::damage() { return damage_; }
int Enemy::drop_item_id() { return drop_item_id_; }
int Enemy::damage_distance() { return damage_distance_; }
int Enemy::distance() { return distance_; }
int Enemy::interact_id() { return interact_id_; }
void Enemy::set_distance_(int dist) { distance_ = dist; }

void Enemy::MoveCloser() {
    distance_ = std::max(1, distance_ - 2);
}

void Enemy::MoveAway() {
    distance_ = std::min(7, distance_ + 2);
}

void Enemy::TakeDamage(int num) {
    health_ -= num;
    if (health_ <= 0) {
        std::wcout << name_ << L" повержен!" << std::endl;
    }
}