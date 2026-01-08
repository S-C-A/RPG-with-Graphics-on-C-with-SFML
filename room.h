#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>  
#include <sstream>  
using namespace std;

struct Room {
    int id;                 
    std::string info;       
    
    int n, s, e, w; // Directions        
    int itemID;     // Items (-1 for none)
    int monsterID;  // Monsters (-1 for none)

    Room(int _id = 0, std::string _info = "", int _n = -1, int _s = -1, int _e = -1, int _w = -1, int _item = -1, int _mon = -1) {
        id = _id;
        info = _info;
        n = _n; s = _s; e = _e; w = _w;
        itemID = _item;
        monsterID = _mon;
    }
};

class MapManager {
private:
    std::map<int, Room> rooms; 

public:
    void loadMap(string filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "HATA: " << filename << " dosyasi acilamadi!" << endl;
            return;
        }
    
        string line;

        while(getline(file,line))
        {
            if (line.empty()) continue;

            stringstream ss(line);
            string segment;

            int id, n, s, e, w;
            int itemID = -1;   
            int monsterID = -1; 
            string info;

            // --- PARSING ---
            if (getline(ss, segment, ',')) id = stoi(segment);
            if (getline(ss, segment, ',')) info = segment;
            if (getline(ss, segment, ',')) n = stoi(segment); 
            if (getline(ss, segment, ',')) s = stoi(segment); 
            if (getline(ss, segment, ',')) e = stoi(segment); 
            if (getline(ss, segment, ',')) w = stoi(segment); 

            if (getline(ss, segment, ',')) {
                if (!segment.empty()) monsterID = stoi(segment);
            }
            if (getline(ss, segment, ',')) {
                if (!segment.empty()) itemID = stoi(segment);
            }

            Room newRoom(id, info, n, s, e, w, itemID, monsterID);
            rooms[id] = newRoom;
        }
        
        file.close();
        cout << "Harita yuklendi. Toplam oda: " << rooms.size() << endl;
    }

    Room* getRoom(int id) {
        if (rooms.find(id) != rooms.end()) {
            return &rooms[id];
        }
        return nullptr;
    }
};