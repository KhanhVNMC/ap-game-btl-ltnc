//
// Created by GiaKhanhVN on 4/8/2025.
//

#ifndef TETISENGINE_MAIN_MENU_H
#define TETISENGINE_MAIN_MENU_H

#include <SDL_render.h>
#include "../gamescene.h"
#include "../hooker.h"
#include "../../engine/javalibs/jsystemstd.h"
#include "../bag_generator.h"
#include "loading_screen.h"
#include "../../game/tetris_player.h"

class MainMenu : public GameScene {
    ExecutionContext* context;
    SDL_Renderer* renderer;

    int hookId;

    public: MainMenu(ExecutionContext* executionContext, SDL_Renderer* sdlRenderer) {
        this->context = executionContext;
        this->renderer = sdlRenderer;
    }

    #define MENU_X_POS 550
    #define MENU_Y_POS 440
    SDL_Texture* cachedTexture = nullptr;
    void menuLoop() {
        // render the logo
        auto cached = disk_cache::bmp_load_and_cache(renderer, GAME_LOGO_SHEET);
        const struct_render_component component = {
                0, 0, 100, 69,
                MENU_X_POS + 150, MENU_Y_POS - 320, static_cast<int>(100 * 3), static_cast<int>(69 * 3)
        };
        render_component(renderer, cached, component, 1);
        // render copyright text
        Button::renderStringReverse(renderer, 1670, 780, "2025 giakhanhvn", 2, 1, 29);
        Button::renderStringReverse(renderer, 1670, 820, VERSION_IDF, 2, 1, 29);
    }

    function<void()> onNavigation = nullptr;

    void stopScene() override {
        context->unhook(this->hookId, onNavigation);
    }

    void startTetrisGame(GameMode mode) {
        onNavigation = [&, this]() {
            openLoadingScreenToTetris(mode);
            // remove this memory region
            delete this;
        };
        // destroy this scene since there's a new takeover already
        this->stopScene();
    }

    void openLoadingScreenToTetris(GameMode mode) {
        // show loading screen (obv fake)
        auto* load = new LoadingScreen(context, renderer);
        load->fakeLoadFor = 40 + (rand() % 40); // fake load for 40 -> 80frames

        // execute the code with the inner scope pointing to the
        // loading screen's references
        load->onLoadingScreenComplete = [load, mode](ExecutionContext* icontext, SDL_Renderer* irenderer) {
            // stop any potential audio overflow
            SysAudio::stopAudio();

            // initialize 7 bag gen with seed = current time
            TetrominoGenerator* generator = new SevenBagGenerator(System::currentTimeMillis());
            TetrisConfig* config = TetrisConfig::builder();
            config->setLineClearsDelay(0.35);

            auto* engine = new TetrisEngine(config, generator);
            auto* player = new TetrisPlayer(icontext, irenderer, engine, mode);

            // start scene first
            player->startScene();
        };

        // begin the "loading" process
        load->startScene();
    }

    void startScene() override {
        // clean current rendering context to begin a new life
        SpritesRenderingPipeline::stopAndCleanCurrentContext();
        this->hookId = context->hook([&]() { onContextTick(); }, "Main Menu");

        // The PLAY CAMPAIGN BUTTON
        Button* campaignButton = (new Button(600, 50, "play campaign", 50, -5));
        campaignButton->onButtonClick([&](int m) {
            startTetrisGame(GameMode::CAMPAIGN);
        });
        campaignButton->teleport(MENU_X_POS, MENU_Y_POS);
        campaignButton->spawn();

        // PLAY ENDLESS BUTTON
        Button* endlessButton = (new Button(600, 50, "play endless", 60, -5));
        endlessButton->onButtonClick([&](int m) {
            startTetrisGame(GameMode::ENDLESS);
        });
        endlessButton->teleport(MENU_X_POS, MENU_Y_POS + 60);
        endlessButton->spawn();

        // SETTINGS BUTTON
        Button* settingsButton = (new Button(300, 50, "settings", 7, -5));
        settingsButton->onButtonClick([](int m) {
            SDL_ShowSimpleMessageBox(
                    SDL_MESSAGEBOX_INFORMATION,
                    "Tetris VS: Not Implemented",
                    "Settings: This feature is not yet implemented!",
                    nullptr
            );
        });
        settingsButton->teleport(MENU_X_POS + 10, MENU_Y_POS + 150);
        settingsButton->spawn();

        // QUIT BUTTON
        Button* quitButton = (new Button(280, 50, "quit", 40, -5));
        quitButton->onButtonClick([](int m) {
            // quit the game, that's it
            exit(0);
        });
        quitButton->teleport(MENU_X_POS + 10 + 300, MENU_Y_POS + 150);
        quitButton->spawn();

        // boilerplate
        campaignButton->onButtonHover([]() {});
        endlessButton->onButtonHover([]() {});
        settingsButton->onButtonHover(  []() {});
        quitButton->onButtonHover([]() {});
    }

    void onContextTick() {
        // begin render the scene
        SDL_RenderClear(renderer);

        // render background
        if (cachedTexture == nullptr) cachedTexture = disk_cache::bmp_load_and_cache(renderer, BACKGROUND_SHEET);
        const struct_render_component bkgComponent = {
                0, 0, 1720, 860,
                0, 0, 1720, 860
        };
        render_component(renderer, cachedTexture, bkgComponent, 0.25);

        // render low priority sprites first
        SpritesRenderingPipeline::renderNormal(renderer);

        // scene loop
        this->menuLoop();

        Thread::sleep(16); // 60.0FPS, std of the thing
        SDL_RenderPresent(renderer); // Show updated frame
    }
};


#endif //TETISENGINE_MAIN_MENU_H
