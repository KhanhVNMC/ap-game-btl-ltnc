//
// Created by GiaKhanhVN on 4/1/2025.
//

#ifndef TETISENGINE_PLAYERENTITY_H
#define TETISENGINE_PLAYERENTITY_H

#include "../../spritesystem/sprite.h"
#include "../entity_prop.h"
#include <functional>

using namespace std;
class FlandreScarlet final : public Sprite {
public:
    SDL_RendererFlip initialFlip;
    explicit FlandreScarlet(const SDL_RendererFlip flip = SDL_FLIP_HORIZONTAL, const int width = 50, const int height = 50, const int initialRotation = 0)
            : Sprite({ 0, 41, DEFAULT_SPRITE_W, DEFAULT_SPRITE_H }, width, height, initialRotation) {
        this->setTextureFile(PLAYER_SHEET);
        this->scale(4);
        this->flipSprite(this->initialFlip = flip);
    }

    /**
     * The ACTUAL X AND Y POSITIONS, THE PROVIDED SPRITE::X AND ::Y IS FOR
     * ANIMATION PURPOSES!!! (INDEPENDENT)
     */
    int strictX{};
    int strictY{};

    /**
     * Teleport and shit (instantly)
     * @param sx target x
     * @param sy target y
     */
    void teleportStrict(const int sx, const int sy) {
        this->strictX = sx;
        this->strictY = sy;
    }

    /**
     * Target position for smooth movement
     * -1 means no movement is in progress
     */
    int targetMoveX = -1;
    int targetMoveY = -1;
    /**
     * Movement speed in pixels per frame (when you call moveSmooth you can fiddle with this)
     */
    int speed = 1;
    /**
     * Ease smoothly from one point to another
     * @param targetX
     * @param targetY
     * @param onComplete
     * @param speed_ pixel per frame
     */
    void moveSmooth(const int targetX, const int targetY, const function<void()> onComplete = nullptr, const int speed_ = 5, const bool ignoreAnimation = false);

    function<void()> animationAfterAttackAnimation = nullptr;
    void scheduleAnimation(int animation, function<void()> toRunLater, int frameSpeed);

    int glowRedUntil = 0;
    void damagedAnimation(const bool blood = false);
    void deathAnimation();

    int frameSpeed{};
    int maxOffset{};
    void setAnimation(const int animation);

    int textureOffset{};
    function<void()> onMovedComplete;

    int internalClock = 0;
    void onDrawCall() override;

    void onBeforeTextureDraw(SDL_Texture* texture) override;
private:
    void processMove();

public:
    ~FlandreScarlet() override = default;
};

#endif //TETISENGINE_PLAYERENTITY_H
