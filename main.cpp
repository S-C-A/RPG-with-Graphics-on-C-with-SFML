#include <iostream>
#include <ctime>
#include "Game.h"

using namespace std;

int main() {
    // Rastgelelik icin
    srand(time(NULL));

    // Oyun Motorunu Olustur
    Game rpg; 

    // --- TEST ASAMASI ---
    // Oyuncuya Master Sword ve Iksirleri ver
    rpg.setupCheats(); 
    // --------------------

    // Oyunu Baslat
    rpg.run();

    system("pause"); 
    return 0;
}