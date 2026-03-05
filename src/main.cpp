#include "GameTypes.h"
#include "SaveSystem.h"
#include "raylib.h"
#include "scenes/BattleScene.h"
#include "scenes/CharacterCreationScene.h"
#include "scenes/GalleryScene.h"
#include "scenes/MainMenuScene.h"

static int RunGame() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(960, 540, "Dungeon Game");
    SetWindowMinSize(960, 540);
    SetTargetFPS(60);

    AppScene currentScene = AppScene::MainMenu;
    MainMenuScene mainMenu;
    CharacterCreationScene characterCreation;
    BattleScene battleScene;
    GalleryScene galleryScene;
    GalleryData gallery = save_system::LoadGallery();

    bool running = true;
    while (running && !WindowShouldClose()) {
        if (currentScene == AppScene::MainMenu) {
            const MainMenuAction action = mainMenu.Update(save_system::BattleSaveExists());
            if (action == MainMenuAction::NewGame) {
                characterCreation.Enter();
                currentScene = AppScene::CharacterCreation;
            } else if (action == MainMenuAction::LoadGame) {
                BattleSaveData saveData;
                if (save_system::LoadBattle(saveData)) {
                    battleScene.StartFromSave(saveData);
                    currentScene = AppScene::Battle;
                }
            } else if (action == MainMenuAction::OpenGallery) {
                currentScene = AppScene::Gallery;
            } else if (action == MainMenuAction::QuitGame) {
                running = false;
            }
        } else if (currentScene == AppScene::CharacterCreation) {
            const CharacterCreationAction action = characterCreation.Update();
            if (action == CharacterCreationAction::Back) {
                currentScene = AppScene::MainMenu;
            } else if (action == CharacterCreationAction::Start) {
                battleScene.ConfigurePlayer(characterCreation.GetSetupData());
                battleScene.StartNew();
                save_system::SaveBattle(battleScene.GetSaveData());
                currentScene = AppScene::Battle;
            }
        } else if (currentScene == AppScene::Battle) {
            battleScene.Update();

            const BattlePhase phase = battleScene.GetPhase();
            if (phase == BattlePhase::PlayerTurn || phase == BattlePhase::EnemyTurn) {
                save_system::SaveBattle(battleScene.GetSaveData());
            } else {
                save_system::DeleteBattleSave();
            }

            if (battleScene.ConsumeWinEvent()) {
                for (bool& unlocked : gallery.unlocked) {
                    if (!unlocked) {
                        unlocked = true;
                        break;
                    }
                }
                save_system::SaveGallery(gallery);
            }

            if (battleScene.ConsumeReturnToMenu()) {
                currentScene = AppScene::MainMenu;
            }
        } else if (currentScene == AppScene::Gallery) {
            if (galleryScene.Update()) {
                currentScene = AppScene::MainMenu;
            }
        }

        BeginDrawing();
        ClearBackground(Color{22, 26, 36, 255});

        if (currentScene == AppScene::MainMenu) {
            mainMenu.Draw(save_system::BattleSaveExists());
        } else if (currentScene == AppScene::CharacterCreation) {
            characterCreation.Draw();
        } else if (currentScene == AppScene::Battle) {
            battleScene.Draw();
        } else if (currentScene == AppScene::Gallery) {
            galleryScene.Draw(gallery);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

#if defined(_WIN32)
#ifndef WINAPI
#define WINAPI __stdcall
#endif

int WINAPI WinMain(void*, void*, char*, int) {
    return RunGame();
}
#else
int main() {
    return RunGame();
}
#endif
