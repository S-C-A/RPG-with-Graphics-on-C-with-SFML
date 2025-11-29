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

public:
    Item() : ID(0), name(""), info(""){}
    Item(int a, string b, string c) : ID(a), name(b), info(c){}

    virtual ~Item() {}

    virtual Item* clone() const = 0;

    int getID() { return ID; }
    string getName() { return name; }
    string getInfo() { return info; }

    virtual bool use(Player* user) = 0;
    
    virtual bool canDrop() { 
        return true; 
    }
};

class Consumable : public Item{
private:
    ItemStats stats;

public:
    Consumable() : Item() {}    
    Consumable(int a, string b, string c, ItemStats _stats) : Item(a, b, c){
        stats = _stats;
    }

    Item* clone() const override {
        return new Consumable(*this); 
    }

    bool use(Player* user) override {
        if (stats.healVal > 0)
        {
            user->heal(stats.healVal);
            cout << getName() << " canini " << stats.healVal << " yeniledi." << endl;
        }
        if (stats.dmgVal > 0)
        {
            user->hurt(stats.dmgVal);
            cout << getName() << " canini " << stats.dmgVal << " aldi." << endl;
        }
        if (stats.buffVal > 0)
        {
            user->buff(stats.buffVal);
            cout << getName() << " gucunu " << stats.buffVal << " arttirdi." << endl;
        }

        return true;
    }
};

class Armor : public Item{
private:
    int defense;

public:
    Armor(int ID, string name, string info, int def) : Item(ID, name, info){
        defense = def;
    }

    int getDefense() { return defense; }

    Item* clone() const override {
        return new Armor(*this); 
    }

    bool use(Player* user) override{

        user->equipArmor(this); 
        cout << getName() << " kusandin!" << endl;
        return false;
    }
};

class Weapon : public Item{
private:
    int power;

public:
    Weapon(int ID, string name, string info, int dmg) : Item(ID, name, info){
        power = dmg;
    }

    int getPower() { return power; }

    Item* clone() const override {
        return new Weapon(*this); 
    }

    bool use(Player* user) override{

        user->equipWeapon(this);
        cout << getName() << " kusandin!" << endl;
        return false;
    }
};

class KeyItem : public Item {
public:
    
    KeyItem(int id, string name, string desc) : Item(id, name, desc) {}

    Item* clone() const override {
        return new KeyItem(*this); 
    }

    bool canDrop() override {
        return false; 
    }

    bool use(Player* user) override {
        cout << "Bu esya (" << getName() << ") dogrudan kullanilamaz." << endl;
        cout << "Dogru yere gittiginde kendiliginden ise yarayacaktir." << endl;
        
        return false;
    }
};