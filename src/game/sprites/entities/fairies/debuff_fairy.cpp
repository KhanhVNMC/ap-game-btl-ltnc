//
// Created by GiaKhanhVN on 4/7/2025.
//
#include "debuff_fairy.h"
#include "../../../tetris_player.h"

static double randomFloat(double min, double max) {
    return min + static_cast<double>(rand()) / RAND_MAX * (max - min);
}

void DebuffFairy::attackPlayer() {
    // prerequisite
    if (this->isAttacking) return;
    TetrisPlayer* target = this->pTetrisPlayer;

    auto curLoc = target->getLocation();
    auto toReturn = this->getLocation();
    auto lastLane = target->currentLane;

    this->isAttacking = true;

    // warn the player that the entity is attacking
    scheduleAnimation(ENTITY_APPROACH, [&, target, curLoc, toReturn, lastLane]() {
        // move there and attack
        moveSmooth(curLoc.x, curLoc.y, [&, target, toReturn, lastLane]() {
            // inflict debuff
            target->inflictDebuff(availableDebuffs[rand() % availableDebuffs.size()], static_cast<int>(randomFloat(this->damageBounds[0], this->damageBounds[1] + 1)), lastLane);

            // attack "animation"
            // return to the spawn point
            moveSmooth(toReturn.x, toReturn.y, [&]() {
                this->isAttacking = false;
            }, 10);
        }, 15);
    }, 40);
}
