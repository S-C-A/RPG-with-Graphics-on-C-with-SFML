#pragma once
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Player; 

class Monster {
protected: 
    int ID;
    string name;
    string info;
    
    int max_hp;
    int hp;
    int atk;
    int def;
    int gold; 
    vector<int> loot; 

public:
    // --- CONSTRUCTORS ---
    Monster() : ID(0), name("Bilinmiyor"), info(""), max_hp(1), hp(1), atk(0), def(0), gold(0) {}
    
    Monster(int _id, string _name, string _info, int _max_hp, int _atk, int _def, int _gold) 
        : ID(_id), name(_name), info(_info), max_hp(_max_hp), hp(_max_hp), atk(_atk), def(_def), gold(_gold) {}

    virtual ~Monster() {}

    virtual Monster* clone() const {
        return new Monster(*this);
    }

    virtual void makeMove(Player* target);

    string getName() const { return name; }
    int getAtk() const { return atk; }
    int getDef() const { return def; }
    int getGold() const { return gold; }
    int getHp() const { return hp; }
    int getMaxHp() const { return max_hp; }

    const vector<int>& getLootList() const {
        return loot;
    }

    void addLoot(int itemID) {
        loot.push_back(itemID);
    }

    void takeDamage(int amount){
        int netDamage = amount - def;
        if (netDamage < 0) netDamage = 0;

        hp -= netDamage;
        if (hp < 0) hp = 0;

        cout << name << " " << netDamage << " hasar aldi! (Zirh: " << def << ")" << endl;
    }

    bool isDead() const {
        return hp <= 0;
    }
};

// ========================================================
// KRİTİK BÖLGE: Class bitti, şimdi Player'ı dahil ediyoruz
// ========================================================
#include "player.h"

inline void Monster::makeMove(Player* target) {
    if (target != nullptr) {
        cout << name << " sana saldiriyor!" << endl;
        target->hurt(atk);
    }
}