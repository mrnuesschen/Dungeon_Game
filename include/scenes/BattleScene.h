#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "GameTypes.h"
#include "entities/Enemy.h"
#include "entities/Player.h"
#include "raylib.h"
#include "ui/Button.h"

class BattleScene {
public:
    BattleScene();
    ~BattleScene();

    void ConfigurePlayer(const CharacterSetupData& setupData);
    PlayerClass GetPlayerClass() const;

    void StartNew();
    void StartFromSave(const BattleSaveData& saveData);

    void Update();
    void Draw();

    BattleSaveData GetSaveData() const;
    BattlePhase GetPhase() const;

    bool ConsumeReturnToMenu();
    bool ConsumeWinEvent();

private:
    struct EnemyUnit {
        std::unique_ptr<Enemy> enemy;
        Texture2D sprite;
        bool spriteLoaded;
    };

    Player player;
    std::vector<EnemyUnit> enemies;
    BattlePhase phase;
    std::string combatLog;
    float enemyActionDelay;

    bool returnToMenuRequested;
    bool winEventPending;

    ui::Button attackButton;
    ui::Button defendButton;
    ui::Button healButton;
    ui::Button menuButton;
    ui::Button restartButton;

    Texture2D playerAvatarTexture;
    bool playerAvatarLoaded;
    Texture2D battleBackgroundTexture;
    bool battleBackgroundLoaded;

    void ReloadPlayerAvatar();
    void ReloadRandomBattleBackground();
    void UnloadBattleBackground();
    std::string ResolveAssetPath(const std::string& relativePath) const;
    void ClearEnemies();
    void CreateEnemyGroupRandom();
    void CreateEnemyGroupFromSave(const BattleSaveData& saveData);
    void AddEnemyToGroup(EnemyArchetype type, int hp);
    int FindFirstAliveEnemyIndex() const;
    bool AreAllEnemiesDefeated() const;

    void DrawHpBar(float x, float y, float w, float h, int hp, int maxHp) const;
};
