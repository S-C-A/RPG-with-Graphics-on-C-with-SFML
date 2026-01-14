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
    int n, s, e, w;        
    int itemID;     
    std::vector<int> monsterID; // Coklu dusman destegi
    std::vector<int> npcID;

    Room(int _id = 0, std::string _info = "", int _n = -1, int _s = -1, int _e = -1, int _w = -1, int _item = -1) {
        id = _id;
        info = _info;
        n = _n; s = _s; e = _e; w = _w;
        itemID = _item;
    }
};

class MapManager {
private:
    std::map<int, Room> rooms; 

public:
    void loadMap(string filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "HATA: " << filename << " acilamadi." << endl;
            return;
        }
    
        string line;
        while(getline(file,line))
        {
            // Yorum satirlarini ve bos satirlari atla
            if (line.empty() || line[0] == '/') continue;

            stringstream ss(line);
            string segment;

            int id = 0, n = -1, s = -1, e = -1, w = -1;
            int itemID = -1;   
            string info;

            // --- 1. SABIT VERILERI OKU ---
            // Format: ID, Info, N, S, E, W, ItemID
            
            if (getline(ss, segment, ',')) id = stoi(segment);
            else continue; 

            if (getline(ss, segment, ',')) info = segment;
            
            if (getline(ss, segment, ',')) n = stoi(segment); 
            if (getline(ss, segment, ',')) s = stoi(segment); 
            if (getline(ss, segment, ',')) e = stoi(segment); 
            if (getline(ss, segment, ',')) w = stoi(segment); 
            
            if (getline(ss, segment, ',')) {
                 if (!segment.empty()) itemID = stoi(segment);
            }

            Room newRoom(id, info, n, s, e, w, itemID);

            // --- 2. DINAMIK CANAVAR OKUMA ---
            // Satirin geri kalanini oku ve listeye ekle
            while (getline(ss, segment, ',')) {
                if (segment.empty()) continue;
                try {
                    int mID = stoi(segment);
                    if (mID != -1) newRoom.monsterID.push_back(mID);
                } catch (...) { continue; }
            }

            rooms[id] = newRoom;
        }
        
        file.close();
        cout << "Harita yuklendi. Oda sayisi: " << rooms.size() << endl;
    }

    Room* getRoom(int id) {
        if (rooms.find(id) != rooms.end()) {
            return &rooms[id];
        }
        return nullptr;
    }
};