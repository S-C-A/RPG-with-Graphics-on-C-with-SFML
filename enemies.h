#pragma once
#include "monster.h"
#include "player.h"  
#include "condition.h"  
#include <iostream>
#include <cstdlib>   

class BanditSlasher : public Monster {
public:
    BanditSlasher(int id) 
        : Monster(id, "Bandit Slasher", "Tehlikeli bir haydut.", 24, 5, 3, 15) 
    {loot.push_back(1);}

    Monster* clone() const override {
        return new BanditSlasher(*this);
    }

    void makeMove(Player* target) override {
        
        if (target->hasStatus(MARK)) {
            std::cout << ">> Bandit Slasher isaretli hedefini goruyor ve cilginca saldiriyor!" << std::endl;
            
            target->hurt(atk);
            target->addStatus(StatusEffect(BLEED, 3, 3));
            
            std::cout << "KRITIK! " << atk << " hasar aldin ve derin bir yara aldin!" << std::endl;
        }
        else {
            srand(time(0));
            int choice = rand() % 3;

            switch (choice) {
            case 0:
                std::cout << ">> Bandit Slasher kilicini savurdu." << std::endl;
                target->hurt(atk); 
                std::cout << atk << " hasar aldin." << std::endl;
                break;

            case 1:
                std::cout << ">> Bandit Slasher sinsi bir hamle yapti!" << std::endl;
                target->hurt(2);
                target->addStatus(StatusEffect(BLEED, 2, 3)); 
                std::cout << "2 hasar aldin ve hafifce kaniyorsun." << std::endl;
                break;

            case 2:
                std::cout << ">> Bandit Slasher gozlerini sana dikti..." << std::endl;
                target->addStatus(StatusEffect(MARK, 0, 3)); 
                std::cout << "ISARETLENDIN! (Bir sonraki saldiriya dikkat et)" << std::endl;
                break;
            }
        }
    }
};
