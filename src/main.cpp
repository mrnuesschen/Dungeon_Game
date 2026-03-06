#include "GameTypes.h"
#include "SaveSystem.h"
#include "raylib.h"
#include "scenes/BattleScene.h"
#include "scenes/CityMapScene.h"
#include "scenes/CharacterCreationScene.h"
#include "scenes/DungeonScene.h"
#include "scenes/GalleryScene.h"
#include "scenes/MainMenuScene.h"
#include "scenes/WorldMapScene.h"

static int RunGame() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(960, 540, "Dungeon Game");
    SetWindowMinSize(960, 540);
    SetTargetFPS(60);

    AppScene currentScene = AppScene::MainMenu;
    MainMenuScene mainMenu;
    CharacterCreationScene characterCreation;
    WorldMapScene worldMapScene;
    CityMapScene cityMapScene;
    DungeonScene dungeonScene;
    BattleScene battleScene;
    GalleryScene galleryScene;
    GalleryData gallery = save_system::LoadGallery();
    bool battleFromDungeon = false;
    int activeEncounterTile = -1;
    std::string activePlayerName = "Hero";

    bool running = true;
    while (running && !WindowShouldClose()) {
        if (currentScene == AppScene::MainMenu) {
            const std::vector<SaveSlotPreview> saveSlots = save_system::ListGameSaves();
            const MainMenuAction action = mainMenu.Update(saveSlots);
            if (action == MainMenuAction::NewGame) {
                characterCreation.Enter();
                currentScene = AppScene::CharacterCreation;
            } else if (action == MainMenuAction::LoadGame) {
                const int selectedSlot = mainMenu.ConsumeSelectedSaveSlot();
                BattleSaveData saveData;
                if (save_system::LoadGameSlot(selectedSlot, saveData)) {
                    battleScene.StartFromSave(saveData);
                    activePlayerName = saveData.playerName;
                    worldMapScene.Enter(activePlayerName);
                    battleFromDungeon = false;
                    activeEncounterTile = -1;
                    battleScene.ResetEncounterDifficulty();
                    cityMapScene.SetShopFeedback("Loaded save slot " + std::to_string(selectedSlot) + ".");
                    currentScene = AppScene::CityMap;
                }
            } else if (action == MainMenuAction::DeleteSave) {
                const int selectedSlot = mainMenu.ConsumeSelectedSaveSlot();
                save_system::DeleteGameSlot(selectedSlot);
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
                const CharacterSetupData setupData = characterCreation.GetSetupData();
                battleScene.ConfigurePlayer(setupData);
                activePlayerName = setupData.playerName;
                worldMapScene.Enter(activePlayerName);
                currentScene = AppScene::WorldMap;
            }
        } else if (currentScene == AppScene::WorldMap) {
            const WorldMapAction action = worldMapScene.Update();
            if (action == WorldMapAction::EnterDungeon) {
                dungeonScene.EnterNewDungeon(activePlayerName);
                currentScene = AppScene::Dungeon;
            } else if (action == WorldMapAction::EnterCity) {
                currentScene = AppScene::CityMap;
            } else if (action == WorldMapAction::BackToMenu) {
                currentScene = AppScene::MainMenu;
            }
        } else if (currentScene == AppScene::CityMap) {
            cityMapScene.SetPlayerGold(battleScene.GetPlayerGold());
            cityMapScene.SetInnSnapshot(battleScene.GetSaveData());
            cityMapScene.SetInnEquipmentSnapshot(battleScene.GetPlayerEquipmentLoadout());
            const CityMapAction action = cityMapScene.Update();
            if (action == CityMapAction::BuyItem) {
                const std::optional<items::ItemId> requestedItem = cityMapScene.ConsumeRequestedShopItem();
                if (requestedItem.has_value()) {
                    std::string shopMessage;
                    battleScene.TryPurchaseShopItem(*requestedItem, shopMessage);
                    cityMapScene.SetShopFeedback(shopMessage);
                }
            } else if (action == CityMapAction::EquipItemAtInn) {
                const std::optional<CityMapEquipRequest> requestedEquip = cityMapScene.ConsumeRequestedEquipItem();
                if (requestedEquip.has_value()) {
                    std::string equipMessage;
                    battleScene.TryEquipPlayerItem(requestedEquip->itemId, requestedEquip->slot, equipMessage);
                    cityMapScene.SetShopFeedback(equipMessage);
                }
            } else if (action == CityMapAction::UnequipItemAtInn) {
                const std::optional<items::EquipmentSlot> requestedSlot = cityMapScene.ConsumeRequestedUnequipSlot();
                if (requestedSlot.has_value()) {
                    std::string unequipMessage;
                    battleScene.TryUnequipPlayerItem(*requestedSlot, unequipMessage);
                    cityMapScene.SetShopFeedback(unequipMessage);
                }
            } else if (action == CityMapAction::SaveAtInn) {
                battleScene.RestAtInn();
                const int savedSlot = save_system::CreateInnSave(battleScene.GetSaveData());
                if (savedSlot > 0) {
                    cityMapScene.SetShopFeedback("Saved at inn in slot " + std::to_string(savedSlot) + ".");
                } else {
                    cityMapScene.SetShopFeedback("All save slots are full. Delete one from Load Game.");
                }
            } else if (action == CityMapAction::BackToWorld) {
                currentScene = AppScene::WorldMap;
            }
        } else if (currentScene == AppScene::Dungeon) {
            dungeonScene.SetQuestItemSpawnsForCurrentFloor(cityMapScene.GetPendingQuestItemSpawnsForFloor(dungeonScene.GetCurrentFloor()));
            const DungeonSceneAction action = dungeonScene.Update();
            if (action == DungeonSceneAction::EncounterTriggered) {
                activeEncounterTile = dungeonScene.GetTriggeredEncounterTile();
                battleScene.ConfigureEncounterDifficulty(
                    dungeonScene.GetMinEncounterEnemies(),
                    dungeonScene.GetMaxEncounterEnemies(),
                    dungeonScene.GetEncounterEnemyLevelBonus());
                battleScene.SetEncounterFloor(dungeonScene.GetCurrentFloor());
                battleScene.StartNew();
                battleFromDungeon = true;
                currentScene = AppScene::Battle;
            } else if (action == DungeonSceneAction::QuestItemCollected) {
                const std::optional<DungeonQuestItemCollection> collected = dungeonScene.ConsumeCollectedQuestItem();
                if (collected.has_value()) {
                    cityMapScene.RegisterQuestItemCollected(collected->questId, collected->itemId, collected->floor);
                    cityMapScene.SetShopFeedback(std::string("Quest item found: ") + items::GetItemDefinition(collected->itemId).name + ".");
                }
            } else if (action == DungeonSceneAction::ReachedExit) {
                dungeonScene.AdvanceToNextFloor();
            } else if (action == DungeonSceneAction::ReturnToWorld) {
                currentScene = AppScene::WorldMap;
            }
        } else if (currentScene == AppScene::Battle) {
            battleScene.Update();

            const BattlePhase phase = battleScene.GetPhase();
            if (battleScene.ConsumeWinEvent()) {
                const std::vector<EnemyArchetype> defeatedEnemies = battleScene.ConsumeLastVictoryEnemyArchetypes();
                if (battleFromDungeon && !defeatedEnemies.empty()) {
                    cityMapScene.RegisterBattleVictoryForQuests(dungeonScene.GetCurrentFloor(), defeatedEnemies);
                }

                for (bool& unlocked : gallery.unlocked) {
                    if (!unlocked) {
                        unlocked = true;
                        break;
                    }
                }
                save_system::SaveGallery(gallery);

                if (battleFromDungeon) {
                    dungeonScene.ClearEncounter(activeEncounterTile);
                    activeEncounterTile = -1;
                    battleFromDungeon = false;
                    battleScene.ResetEncounterDifficulty();
                    currentScene = AppScene::Dungeon;
                    continue;
                }
            }

            if (battleFromDungeon && phase == BattlePhase::Lost) {
                activeEncounterTile = -1;
                battleFromDungeon = false;
                battleScene.ResetEncounterDifficulty();
                currentScene = AppScene::WorldMap;
                continue;
            }

            if (battleScene.ConsumeReturnToMenu()) {
                if (battleFromDungeon) {
                    activeEncounterTile = -1;
                    battleFromDungeon = false;
                    battleScene.ResetEncounterDifficulty();
                    currentScene = AppScene::WorldMap;
                } else {
                    currentScene = AppScene::MainMenu;
                }
            }
        } else if (currentScene == AppScene::Gallery) {
            if (galleryScene.Update()) {
                currentScene = AppScene::MainMenu;
            }
        }

        BeginDrawing();
        ClearBackground(Color{22, 26, 36, 255});

        if (currentScene == AppScene::MainMenu) {
            mainMenu.Draw(save_system::ListGameSaves());
        } else if (currentScene == AppScene::CharacterCreation) {
            characterCreation.Draw();
        } else if (currentScene == AppScene::WorldMap) {
            worldMapScene.Draw();
        } else if (currentScene == AppScene::CityMap) {
            cityMapScene.Draw();
        } else if (currentScene == AppScene::Dungeon) {
            dungeonScene.Draw();
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
