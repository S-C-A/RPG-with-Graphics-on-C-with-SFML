#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>  
#include <sstream>
#include <algorithm> // Temizleme islemleri icin gerekli

using namespace std;

struct Room {
    int id;                 
    std::string info;       
    int n, s, e, w;        
    int itemID;     
    std::vector<int> monsterID; 

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

    // YARDIMCI: String icindeki bosluklari ve satir sonu karakterlerini temizler
    string cleanString(string input) {
        // Gereksiz karakterleri sil
        input.erase(remove(input.begin(), input.end(), '\r'), input.end()); // Windows satir sonu
        input.erase(remove(input.begin(), input.end(), '\n'), input.end()); // Linux satir sonu
        input.erase(remove(input.begin(), input.end(), ' '), input.end());  // Bosluklar
        return input;
    }

public:
    void loadMap(string filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "!!! HATA: " << filename << " dosyasi bulunamadi!" << endl;
            return;
        }
    
        string line;
        cout << "\n--- HARITA OKUNUYOR ---" << endl;

        while(getline(file,line))
        {
            if (line.empty() || line[0] == '/') continue;

            stringstream ss(line);
            string segment;

            int id = 0, n = -1, s = -1, e = -1, w = -1;
            int itemID = -1;   
            string info = "Bilinmiyor";

            try {
                // 1. Standart Veriler
                if (getline(ss, segment, ',')) id = stoi(cleanString(segment));
                else continue;

                if (getline(ss, segment, ',')) info = segment; // Info'yu temizlemiyoruz, bosluklu olabilir

                if (getline(ss, segment, ',')) n = stoi(cleanString(segment)); 
                if (getline(ss, segment, ',')) s = stoi(cleanString(segment)); 
                if (getline(ss, segment, ',')) e = stoi(cleanString(segment)); 
                if (getline(ss, segment, ',')) w = stoi(cleanString(segment)); 
                
                if (getline(ss, segment, ',')) {
                    string cleanSeg = cleanString(segment);
                    if (!cleanSeg.empty()) itemID = stoi(cleanSeg);
                }

                Room newRoom(id, info, n, s, e, w, itemID);

                // 2. Canavarlar (HATA DUZELTICI MOD)
                // Satirin geri kalanini komple oku
                while (getline(ss, segment, ',')) {
                    string cleanSeg = cleanString(segment);
                    
                    if (cleanSeg.empty()) continue;
                    
                    try {
                        int mID = stoi(cleanSeg);
                        if (mID != -1) {
                            newRoom.monsterID.push_back(mID);
                        }
                    } catch (...) { 
                        // Sayi degilse atla
                    }
                }
                
                // KONTROL MESAJI (Canavarlar yüklendi mi?)
                cout << "Oda " << id << " yuklendi. Canavar Sayisi: " << newRoom.monsterID.size() << endl;

                rooms[id] = newRoom;

            } catch (...) {
                cout << "!!! Satir okuma hatasi: " << line << endl;
            }
        }
        
        file.close();
        cout << "--- YUKLEME TAMAM ---\n" << endl;
    }

    Room* getRoom(int id) {
        if (rooms.find(id) != rooms.end()) {
            return &rooms[id];
        }
        return nullptr;
    }
};