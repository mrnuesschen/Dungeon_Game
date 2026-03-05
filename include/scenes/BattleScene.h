#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "GameTypes.h"
#include "combat/SkillSystem.h"
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
    enum class ActionMenuState {
        Root,
        Fight,
        Skills
    };

    struct EnemyUnit {
        std::unique_ptr<Enemy> enemy;
        Texture2D sprite;
        bool spriteLoaded;
        int dotDamage;
        int dotTurns;
        int attackDebuffAmount;
        int attackDebuffTurns;
    };

    Player player;
    std::vector<EnemyUnit> enemies;
    BattlePhase phase;
    std::string combatLog;
    float enemyActionDelay;

    bool returnToMenuRequested;
    bool winEventPending;
    ActionMenuState actionMenuState;
    int playerDamageReductionAmount;
    int playerDamageReductionTurns;

    ui::Button fightButton;
    ui::Button defendButton;
    ui::Button healButton;
    ui::Button menuButton;
    ui::Button restartButton;
    ui::Button simpleAttackButton;
    ui::Button skillsButton;
    ui::Button backButton;
    ui::Button classSkillButtonOne;
    ui::Button classSkillButtonTwo;
    ui::Button skillsBackButton;

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
    void ResetBattleEffects();
    void ApplyEnemyDotEffects();
    void ExecuteSimpleAttack();
    void ExecuteClassSkill(size_t skillIndex);
    std::string BuildClassSkillMenuText() const;
    const std::vector<combat::SkillDefinition>& GetClassSkills() const;

    void DrawHpBar(float x, float y, float w, float h, int hp, int maxHp) const;
};
