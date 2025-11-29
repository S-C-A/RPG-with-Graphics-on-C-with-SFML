#pragma once
#include <string>
#include <iostream>

enum StatusType {
    NONE,       
    POISON,     
    BLEED,     
    BURN,
    STUN,       
    MARK,
    STR_BUFF,  
    DEF_BUFF    
};

struct StatusEffect {
    StatusType type; 
    int power;       
    int duration;    

    StatusEffect(StatusType _type, int _power, int _duration) {
        type = _type;
        power = _power;
        duration = _duration;
    }
};

