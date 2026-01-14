#pragma once
#include <map>
#include <fstream>  
#include <sstream>  
#include <iostream>
#include <string>
#include "NPC.h"

using namespace std;

class NPCManager {
private:
    std::map<int, NPC*> npcDB;

public:
    NPCManager() {
        loadNPCs("Dialogues.txt");
    }

    ~NPCManager() {
        for (auto const& [id, npc] : npcDB) {
            delete npc;
        }
        npcDB.clear();
    }

    // --- ANA YUKLEME FONKSIYONU ---
    void loadNPCs(string filename) {
        ifstream file(filename);
        
        // 1. Dosya var mi kontrol et
        if (!file.is_open()) {
            cout << "HATA: " << filename << " dosyasi bulunamadi! (NPC'ler yuklenemedi)" << endl;
            return;
        }

        string line;
        
        // --- (STATE TRACKING) ---
        NPC* currentNPC = nullptr;       
        int currentNodeID = -1;          

        cout << "--- NPC and DIALOGUES LOADING ---" << endl;

        // 2. Dosya bitene kadar satir satir oku
        while (getline(file, line)) {
            
            // Bos satirlari veya yorum satirlarini (//) atla
            if (line.empty() || line.size() < 2 || line.substr(0, 2) == "//") continue;

            stringstream ss(line);
            string tag;
            ss >> tag; // Satirin ilk kelimesini (Etiketi) al: NPC, NODE veya CHOICE

            // ==========================================
            // DURUM 1: YENI BIR NPC TANIMI (Etiket: NPC)
            // Ornek Satir: NPC 50 Yasli_Bilge
            // ==========================================
            if (tag == "NPC") {
                int id;
                string name;
                ss >> id >> name;
                
                // Puf Nokta: Dosyada bosluk kullanamadigimiz icin alt tire (_) kullandik.
                // Burada onlari tekrar bosluga ceviriyoruz. (Yasli_Bilge -> Yasli Bilge)
                for(char &c : name) { 
                    if(c == '_') c = ' '; 
                }

                // Hafizada yeni bir NPC yarat
                currentNPC = new NPC(id, name);
                
                // Depoya kaydet
                npcDB[id] = currentNPC;
                
                // Yeni bir NPC'ye gectik, eski cumle ID'sini sifirla
                currentNodeID = -1; 
            }
            
            // ==========================================
            // DURUM 2: YENI BIR KONUSMA (Etiket: NODE)
            // Ornek Satir: NODE 0 Merhaba yolcu.
            // ==========================================
            else if (tag == "NODE") {
                // Guvenlik: Dosyada NPC tanimlamadan NODE yazildiysa hata verme, atla.
                if (currentNPC == nullptr) continue;

                int id;
                ss >> id; // Dugum ID'sini oku (0)

                // Satirin geri kalanini (Metni) oku
                string text;
                getline(ss, text); 
                
                // Ufak temizlik: getline bazen bastaki boslugu da alir, onu silelim.
                if (!text.empty() && text[0] == ' ') text.erase(0, 1);

                // NPC'nin hafizasina bu cumleyi ekle
                currentNPC->addDialogueNode(id, text);
                
                // "Su an bu cumledeyiz" diye isaretle (Choice eklemek icin lazim)
                currentNodeID = id; 
            }
            
            // ==========================================
            // DURUM 3: SECENEK (Etiket: CHOICE)
            // Ornek Satir: CHOICE 1 : Evet kayboldum.
            // ==========================================
            else if (tag == "CHOICE") {
                // Guvenlik: Hangi NPC'ye ve hangi cumleye ekleyecegimizi biliyor muyuz?
                if (currentNPC == nullptr || currentNodeID == -1) continue;

                int targetID;
                ss >> targetID; // Hedef ID'yi oku (1)

                // Ayirici karakteri (:) okuyup yutmamiz lazim
                char separator;
                ss >> separator; 

                // Secenek metnini oku
                string optionText;
                getline(ss, optionText);
                
                // Bastaki boslugu temizle
                if (!optionText.empty() && optionText[0] == ' ') optionText.erase(0, 1);

                // NPC'nin su anki cumlesine (currentNodeID) bu secenegi ekle
                currentNPC->addOption(currentNodeID, optionText, targetID);
            
            }
            // ==========================================
            // DURUM 4: AKSIYONLAR (ACTION)
            // ==========================================
            else if (tag == "ACTION") {
                if (currentNPC == nullptr || currentNodeID == -1) continue;

                string actionName;
                ss >> actionName; // Ornek: "GIVE_ITEM" veya "SET_ROOT"

                int param;
                ss >> param; // Degeri oku (10, 1 vs.)

                // Varsayilan degerler
                int rootChange = -1;
                EventType type = EVENT_NONE;
                int val = 0;

                // 1. AKIS DEGISTIRME
                if (actionName == "SET_ROOT") {
                    rootChange = param;
                }
                // 2. ESYA VERME
                else if (actionName == "GIVE_ITEM") {
                    type = EVENT_GIVE_ITEM;
                    val = param;
                }
                // 3. IYILESTIRME
                else if (actionName == "HEAL") {
                    type = EVENT_HEAL;
                    val = param;
                }
                // 4. SAVAS BASLATMA
                else if (actionName == "START_COMBAT") {
                    type = EVENT_START_COMBAT;
                    val = param;
                }
                // 5. SHOP ACMA
                else if (actionName == "OPEN_SHOP") {
                    type = EVENT_OPEN_SHOP;
                    // Parametreye gerek yok ama format bozulmasin diye 0 okunabilir
                }
                

                // Node'u guncelle
                currentNPC->setNodeAction(currentNodeID, rootChange, type, val);
            }
            // ==========================================
            // DURUM 5: DUKKAN STOKLARI (Etiket: SHOP_ITEM)
            // Ornek Satir: SHOP_ITEM 1 50 (ID 1'i 50 altına sat)
            // ==========================================
            else if (tag == "SHOP_ITEM") {
                if (currentNPC == nullptr) continue;

                int itemID, price;
                ss >> itemID >> price;

                currentNPC->addShopItem(itemID, price);
            }
        }

        file.close();
        cout << ">>> NPC Yukleme Tamamlandi. Toplam: " << npcDB.size() << " NPC hazir." << endl;
    }

    // --- SERVIS SAGLAYICI ---
    // Oyun motoru ID verip NPC isterse buradan doneriz.
    NPC* getNPC(int id) {
        if (npcDB.find(id) != npcDB.end()) {
            return npcDB[id];
        }
        return nullptr; // Bulamazsa bos doner
    }
};