#pragma once
#include <string>
#include <iostream>
#include <vector>

class Item;
class Weapon;
class Armor;

class Player {
private:
    int max_hp;
    int hp;
    int atk;
    int exp;
    int lvl;
    int gold;
    int current_room;
    Weapon* equippedWeapon;
    Armor* equippedArmor;

    std::vector<Item*> inventory;

    const int MAX_LVL = 5;

    const int lvl_need[5] = {20, 45, 75, 120, 180};
    const int hp_table[5] = {40, 65, 100, 150, 230};
    const int base_atk[5] = {5, 9, 15, 22, 30};

    void lvlUp() {
        if (lvl < MAX_LVL) {
            lvl++;
            max_hp = hp_table[lvl - 1];
            atk = base_atk[lvl - 1];
            std::cout << ">>> TEBRIKLER! SEVIYE " << lvl << " OLDUNUZ! <<<" << std::endl;
        }
    }

public:
    Player() {
        lvl = 1;
        exp = 0;
        gold = 17;
        max_hp = hp_table[0];
        hp = max_hp;
        atk = base_atk[0];
        current_room = 0;
        equippedWeapon = nullptr;
        equippedArmor = nullptr;
    }

    int getHp() { return hp; }
    int getMaxHp() { return max_hp; }
    int getAtk() { return atk; }
    int getLvl() { return lvl; }
    int getExp() { return exp; }
    int getGold() { return gold; }

    bool addItem(Item* item);

    void equipArmor(Armor* newArmor);

    void equipWeapon(Weapon* newWeapon);

    void useItem(int index);

    void addExp(int amount) {
        if (lvl >= MAX_LVL) return;

        exp += amount;

        while (lvl < MAX_LVL && exp >= lvl_need[lvl - 1]) {
            exp -= lvl_need[lvl - 1];
            lvlUp();
        }
    }

    void roomChange(int room){
        current_room = room;
    }
   
    bool isAlive() const {
        return hp <= 0;
    }

    void goldChange(int amount){
        gold = gold + amount;
        if (gold < 0)
        {
            gold = 0;
        }
    }

    void heal(int amount){
        hp = hp + amount;
        if (hp > max_hp)
        {
            hp = max_hp;
        }
    }

    void hurt(int amount){
        hp = hp - amount;
        if (hp < 0)
        {
            hp = 0;
        }
    }

    void buff(int amount){
        atk = atk + amount;
    }

    void printInventory();
    void printEquipment();

    void printStats() {
        std::cout << "--- Status ---" << std::endl;
        std::cout << "Hp: " << hp << "/" << max_hp << std::endl;
        std::cout << "Strength: " << atk << std::endl;
        std::cout << "Level: " << lvl << (lvl == MAX_LVL ? " (MAX)" : "") << std::endl;
        std::cout << "Gold: " << gold << std::endl;

        if (lvl < MAX_LVL)
            std::cout << "XP: " << exp << " / " << lvl_need[lvl - 1] << std::endl;
        else
            std::cout << "XP: -" << std::endl;

        std::cout << "-------------" << std::endl;
    }
};

#include "item.h"

inline bool Player::addItem(Item* item){
    if (inventory.size() < 10)
    {
        inventory.push_back(item);
        std::cout <<"Item added to inventory" << std::endl;
        return true;
    } else {
        std::cout <<"Item couldn't be added to inventory" <<std::endl;
        return false;
    }
}

inline void Player::useItem(int index){
    if (index >= inventory.size()){
        std::cout << "Gecersiz esya secimi!" << std::endl;
        return;
    }
    Item* itemToUse = inventory[index];
    bool isConsumed = itemToUse->use(this);
    if (isConsumed == true) {
        delete itemToUse;
        inventory.erase(inventory.begin() + index);
    }
}

inline void Player::equipWeapon(Weapon* newWeapon){
    if (newWeapon == nullptr) return;

    for (size_t i = 0; i < inventory.size(); i++) {
        if (inventory[i] == newWeapon) {
            inventory.erase(inventory.begin() + i);
            break;
        }
    }
    
    Weapon* oldWeapon = equippedWeapon;
    equippedWeapon = newWeapon;

    if (oldWeapon != nullptr)
    {
        inventory.push_back(oldWeapon);
        std::cout << oldWeapon->getName() << " cantaya geri kondu." << std::endl;
    }

}

inline void Player::equipArmor(Armor* newArmor){
    if (newArmor == nullptr) return;

    for (size_t i = 0; i < inventory.size(); i++) {
        if (inventory[i] == newArmor) {
            inventory.erase(inventory.begin() + i);
            break;
        }
    }
    
    Armor* oldArmor = equippedArmor;
    equippedArmor = newArmor;

    if (oldArmor != nullptr)
    {
        inventory.push_back(oldArmor);
        std::cout << oldArmor->getName() << " cantaya geri kondu." << std::endl;
    }
}

inline void Player::printInventory() {
    std::cout << "--- BACKPACK (" << inventory.size() << "/10) ---" << std::endl;
    
    if (inventory.empty()) {
        std::cout << "Cantan bos." << std::endl;
        return;
    }

    for (size_t i = 0; i < inventory.size(); i++) {
        std::cout << "[" << i << "] " << inventory[i]->getName() << std::endl;
    }
    std::cout << "----------------------" << std::endl;
}

inline void Player::printEquipment() {

        std::cout << "-------------" << std::endl;
        std::cout << "Weapon: " << (equippedWeapon == nullptr ? "No Weapon" : equippedWeapon->getName()) << std::endl;
        std::cout << "Armor: " << (equippedArmor == nullptr ? "No Armor" : equippedArmor->getName()) << std::endl;
        std::cout << "-------------" << std::endl;
}

