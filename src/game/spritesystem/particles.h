//
// Created by GiaKhanhVN on 3/31/2025.
//

#ifndef TETISENGINE_PARTICLES_H
#define TETISENGINE_PARTICLES_H
#include "sprite.h"
#include <vector>
#include <cstdlib>
#include <ctime>

// random float generator
static double randomFloat(double min, double max) {
    return min + static_cast<double>(rand()) / RAND_MAX * (max - min);
}

class Particle : public Sprite {
private:
    double px, py; // floating-point position
    double vx, vy; // velocity
    int lifetime;  //frames until particle disappears
    double gravity; // downward acceleration
    int tint = 0xFFFFFF; // tint

public:
    /**
     * @param texture texture to use
     * @param width particle width (sprite's)
     * @param height particle height (sprite's)
     * @param initialX the first position X
     * @param initialY the first pos Y
     * @param initialVx initial Y velocity (moves down with gravity)
     * @param initialVy initial X velocity (moves down with gravity)
     * @param lifetime frames that this shit would survive for (-1 if no despawn (MANUAL DESPAWN NEEDED))
     * @param gravity downwards momentum
     */
    Particle(SpriteTexture texture, int width, int height, double initialX, double initialY,
             double initialVx, double initialVy, int lifetime, double gravity = 0.5)
            : Sprite(texture, width, height),
              px(initialX), py(initialY),
              vx(initialVx), vy(initialVy),
              lifetime(lifetime), gravity(gravity) {
        x = static_cast<int>(px);
        y = static_cast<int>(py);
    }

    void setTint(const int r, const int g, const int b) {
        tint |= (r << 16) | (g << 8) | b;
    }

    void setTint(const int tintParam) {
        this->tint = tintParam;
    }

    void onBeforeTextureDraw(SDL_Texture *texture) override {
        SDL_SetTextureColorMod(texture, (tint >> 16) & 0xFF, (tint >> 8) & 0xFF, tint & 0xFF);
    }

    void onDrawCall() override {
        // update position with velocity
        px += vx;
        py += vy;
        // apply gravity (downward, so vy increases)
        vy += gravity;
        // teleport
        this->teleport(static_cast<int>(px), static_cast<int>(py));
        // time
        if (lifetime != -1) {
            if (lifetime <= 0) {
                this->discard(); // remove from garbage and clean so no mem leaks kek
            }
            lifetime--;
        }
    }
};

class ParticleSystem : public Sprite {
private:
    SpriteTexture particleTexture; // texture for particle (sprite.h)
    int particleWidth, particleHeight; // bounding size
    int emitInterval; // interval of busting
    int emitCounter;  // for counting the intervals
    int numParticlesPerEmit; // amount per busting
    double initialVxMin, initialVxMax; // horizontal velocity range
    double initialVyMin, initialVyMax; // vertical velocity range (negative = up, positive = down [y major])
    int lifetime; // life of each particle
    double gravity; // Gravity value

public:
    bool once; // emit once and die

    ParticleSystem(SpriteTexture particleTexture, int particleWidth, int particleHeight,
                   int emitInterval, int numParticlesPerEmit,
                   double initialVxMin, double initialVxMax,
                   double initialVyMin, double initialVyMax,
                   int lifetime, double gravity)
            : Sprite({0, 0, 0, 0}, 0, 0), // Zero size so it doesn’t render
              particleTexture(particleTexture),
              particleWidth(particleWidth), particleHeight(particleHeight),
              emitInterval(emitInterval), emitCounter(0),
              numParticlesPerEmit(numParticlesPerEmit),
              initialVxMin(initialVxMin), initialVxMax(initialVxMax),
              initialVyMin(initialVyMin), initialVyMax(initialVyMax),
              lifetime(lifetime), gravity(gravity) {}

    // this gets called every single game tick
    void onDrawCall() override {
        emitCounter++; // only emit in certain frames (+ % mod)
        if (emitCounter >= emitInterval) {
            emitCounter = 0;
            for (int i = 0; i < numParticlesPerEmit; i++) {
                double vx = randomFloat(initialVxMin, initialVxMax); // choose a random velocity to bust
                double vy = randomFloat(initialVyMin, initialVyMax);
                // new particle eh
                Particle* p = new Particle(particleTexture, particleWidth, particleHeight,
                                           x, y, vx, vy, lifetime, gravity);
                p->spawn(); // begin render (Each frame)
            }
            if (once) { // kill the entity because its once
                this->discard();
            }
        }
    }

    ~ParticleSystem() {
        // what the fuck
        // PSA: Removed the garbage collector entry because shit lag as fuck
    }
};

#endif //TETISENGINE_PARTICLES_H
