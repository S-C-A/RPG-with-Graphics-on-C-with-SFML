#pragma once
#include <string>
#include <iostream>
using namespace std;
#include "player.h"

struct ItemStats {
    int healVal = 0;
    int dmgVal = 0;
    int buffVal = 0;
};

class Item {

    int ID;
    string name;
    string info;
    int value;

public:
    Item(int a, string b, string c, int val = 0) : ID(a), name(b), info(c), value(val){}
    Item() : ID(0), name(""), info(""), value(0){}

    virtual ~Item() {}

    virtual Item* clone() const = 0;

    int getID() { return ID; }
    string getName() { return name; }
    string getInfo() { return info; }
    int getPrice() const { return value; }

    virtual string use(Player* user) = 0;
    
    virtual bool canDrop() { 
        return true; 
    }
};

class Consumable : public Item{
private:
    ItemStats stats;

public:
    Consumable() : Item() {}    
    Consumable(int a, string b, string c, int _val, ItemStats _stats) : Item(a, b, c, _val){
        stats = _stats;
    }

    Item* clone() const override {
        return new Consumable(*this); 
    }

    string use(Player* user) override {
        string report = "Used " + getName() + ". ";

        if (stats.healVal > 0)
        {
            user->heal(stats.healVal);
            report += "Healed " + to_string(stats.healVal) + " HP. ";
        }
        if (stats.dmgVal > 0)
        {
            user->hurt(stats.dmgVal);
            report += "Took " + to_string(stats.dmgVal) + " dmg. ";
        }
        if (stats.buffVal > 0)
        {
            user->buff(stats.buffVal);
            report += "Power increased by " + to_string(stats.buffVal) + ". ";
        }

        return report;
    }
};

class Armor : public Item{
private:
    int defense;

public:
    Armor(int ID, string name, string info, int val, int def) : Item(ID, name, info, val){
        defense = def;
    }

    int getDefense() { return defense; }

    Item* clone() const override {
        return new Armor(*this); 
    }

    string use(Player* user) override{
        // Player.h'da bu fonksiyonu guncelleyecegiz
        return user->equipArmor(this); 
    }
};

class Weapon : public Item{
private:
    int power;

public:
    Weapon(int ID, string name, string info, int val, int dmg) : Item(ID, name, info, val){
        power = dmg;
    }

    int getPower() { return power; }

    Item* clone() const override {
        return new Weapon(*this); 
    }

    string use(Player* user) override{
        // Player.h'da bu fonksiyonu guncelleyecegiz
        return user->equipWeapon(this);
    }
};

class KeyItem : public Item {
public:
    
    KeyItem(int id, string name, string desc, int val = 0) : Item(id, name, desc, val) {}

    Item* clone() const override {
        return new KeyItem(*this); 
    }

    bool canDrop() override {
        return false; 
    }

    string use(Player* user) override {
        return "You cannot use [" + getName() + "] directly.";
    }
};