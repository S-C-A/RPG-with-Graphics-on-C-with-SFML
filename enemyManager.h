#pragma once
#include "monster.h"
#include <map>
#include "enemies.h"

class EnemyManager {
private:
    std::map<int, Monster*> enemyDB;

public:
    EnemyManager(){
        initEnemies();
    }

    ~EnemyManager() {
        for (auto const& [id, Mob] : enemyDB) {
            delete Mob;
        }
        enemyDB.clear();
    }

    void initEnemies(){
        enemyDB[10] = new BanditSlasher(10);
    }

    Monster* getEnemy(int id){
        if (enemyDB.find(id) != enemyDB.end()) return enemyDB[id]->clone();
        return nullptr;
    }
};