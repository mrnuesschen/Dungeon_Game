#pragma once

#include <array>
#include <memory>
#include <string>
#include <vector>

#include "GameTypes.h"
#include "combat/SkillSystem.h"
#include "entities/Enemy.h"
#include "entities/Player.h"
#include "items/ItemSystem.h"
#include "raylib.h"
#include "ui/Button.h"

class BattleScene {
public:
    BattleScene();
    ~BattleScene();

    void ConfigurePlayer(const CharacterSetupData& setupData);
    PlayerClass GetPlayerClass() const;
    void ConfigureEncounterDifficulty(int minEnemies, int maxEnemies, int enemyLevelBonus);
    void SetEncounterFloor(int floor);
    void ResetEncounterDifficulty();
    int GetPlayerGold() const;
    bool TryPurchaseShopItem(items::ItemId id, std::string& outMessage);
    bool TryEquipPlayerItem(items::ItemId id, items::EquipmentSlot slot, std::string& outMessage);
    bool TryUnequipPlayerItem(items::EquipmentSlot slot, std::string& outMessage);
    const items::EquipmentLoadout& GetPlayerEquipmentLoadout() const;
    void RestAtInn();

    void StartNew();
    void StartFromSave(const BattleSaveData& saveData);

    void Update();
    void Draw();

    BattleSaveData GetSaveData() const;
    BattlePhase GetPhase() const;
    std::vector<EnemyArchetype> ConsumeLastVictoryEnemyArchetypes();

    bool ConsumeReturnToMenu();
    bool ConsumeWinEvent();

private:
    enum class ActionMenuState {
        Root,
        Fight,
        Skills,
        TargetSelect
    };

    enum class PendingTargetAction {
        None,
        SimpleAttack,
        ClassSkill
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
    PendingTargetAction pendingTargetAction;
    size_t pendingSkillIndex;
    int playerDamageReductionAmount;
    int playerDamageReductionTurns;
    int minEncounterEnemies;
    int maxEncounterEnemies;
    int encounterEnemyLevelBonus;
    int encounterFloor;
    std::vector<EnemyArchetype> lastVictoryEnemyArchetypes;

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
    int RollEnemyLevel() const;
    int CalculateBattleExpReward() const;
    int CalculateBattleGoldReward() const;
    std::string AwardLootForVictory();
    std::string BuildVictoryText(int expAward, int levelUps) const;
    void AddEnemyToGroup(EnemyArchetype type, int level, int hp);
    int FindFirstAliveEnemyIndex() const;
    bool AreAllEnemiesDefeated() const;
    void ResetBattleEffects();
    void ApplyEnemyDotEffects();
    bool SkillRequiresEnemyTarget(const combat::SkillDefinition& skill) const;
    void BeginSimpleAttackTargetSelection();
    void BeginSkillTargetSelection(size_t skillIndex);
    void CancelTargetSelection();
    int GetEnemyIndexAtMouse(float sw, float sh, float sx, float sy, float sf) const;
    void ResolvePendingTargetAction(int targetIndex);
    void ExecuteSimpleAttack(int targetIndex);
    void ExecuteClassSkill(size_t skillIndex, int targetIndex);
    std::string BuildClassSkillMenuText() const;
    const std::vector<combat::SkillDefinition>& GetClassSkills() const;

    void DrawHpBar(float x, float y, float w, float h, int hp, int maxHp) const;
};
