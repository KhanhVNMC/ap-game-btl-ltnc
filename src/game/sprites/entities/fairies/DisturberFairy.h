//
// Created by GiaKhanhVN on 4/7/2025.
//


#ifndef DISTURBERFAIRY_H
#define DISTURBERFAIRY_H
#include "debuff_fairy.h"

class DisturberFairy : public DebuffFairy {
public:
    DisturberFairy(TetrisPlayer* tetrisPlayer) : DebuffFairy(tetrisPlayer) {
        this->setTextureFile(FAIRY_DISTURBER_SHEET); // blue nigga
        this->setDamageThresholds(15, 30); // 13-25s debuff

        this->setAttackSpeed(30); // 30s between attacks
        this->setDifficulty(EASY); // this is a hard mini boss
        this->setMaxHealth(18); // 14HP

        this->availableDebuffs = { NO_HOLD };
    }
};

#endif //DISTURBERFAIRY_H