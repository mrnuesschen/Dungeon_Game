#include "scenes/BattleScene.h"

#include <algorithm>
#include <cctype>
#include <filesystem>

#include "entities/EnemyFactory.h"
#include "raylib.h"

BattleScene::BattleScene()
    : player(),
      enemies(),
      phase(BattlePhase::PlayerTurn),
    combatLog("Your turn: Fight, Defend, or Heal."),
      enemyActionDelay(0.0f),
      returnToMenuRequested(false),
      winEventPending(false),
    actionMenuState(ActionMenuState::Root),
        pendingTargetAction(PendingTargetAction::None),
        pendingSkillIndex(0),
    playerDamageReductionAmount(0),
    playerDamageReductionTurns(0),
    minEncounterEnemies(1),
    maxEncounterEnemies(kMaxBattleEnemies),
    encounterEnemyLevelBonus(0),
    encounterFloor(0),
    fightButton{Rectangle{620.0f, 345.0f, 140.0f, 52.0f}, "Fight", true},
    defendButton{Rectangle{770.0f, 345.0f, 140.0f, 52.0f}, "Defend", true},
    healButton{Rectangle{620.0f, 407.0f, 140.0f, 52.0f}, "Heal", true},
    menuButton{Rectangle{770.0f, 407.0f, 140.0f, 52.0f}, "Menu", true},
    restartButton{Rectangle{620.0f, 469.0f, 290.0f, 52.0f}, "New Battle", true},
    simpleAttackButton{Rectangle{620.0f, 345.0f, 140.0f, 52.0f}, "Simple Attack", true},
    skillsButton{Rectangle{770.0f, 345.0f, 140.0f, 52.0f}, "Skills", true},
    backButton{Rectangle{620.0f, 407.0f, 290.0f, 52.0f}, "Back", true},
    classSkillButtonOne{Rectangle{620.0f, 345.0f, 140.0f, 52.0f}, "Skill 1", true},
    classSkillButtonTwo{Rectangle{770.0f, 345.0f, 140.0f, 52.0f}, "Skill 2", true},
    skillsBackButton{Rectangle{620.0f, 407.0f, 290.0f, 52.0f}, "Back", true},
      playerAvatarTexture{},
    playerAvatarLoaded(false),
    battleBackgroundTexture{},
    battleBackgroundLoaded(false) {
    CreateEnemyGroupRandom();
    ReloadRandomBattleBackground();
}

BattleScene::~BattleScene() {
    ClearEnemies();
    UnloadBattleBackground();

    if (playerAvatarLoaded) {
        UnloadTexture(playerAvatarTexture);
        playerAvatarLoaded = false;
    }
}

void BattleScene::ConfigurePlayer(const CharacterSetupData& setupData) {
    player.SetClass(setupData.playerClass);
    player.SetProgress(1, 0);
    player.SetGold(0);
    player.SetCharacterName(setupData.playerName);
    player.SetAvatarPath(setupData.avatarPath);
    ReloadPlayerAvatar();
}

void BattleScene::ConfigureEncounterDifficulty(int minEnemies, int maxEnemies, int enemyLevelBonus) {
    minEncounterEnemies = std::clamp(minEnemies, 1, kMaxBattleEnemies);
    maxEncounterEnemies = std::clamp(maxEnemies, minEncounterEnemies, kMaxBattleEnemies);
    encounterEnemyLevelBonus = std::max(0, enemyLevelBonus);
}

void BattleScene::SetEncounterFloor(int floor) {
    encounterFloor = std::max(0, floor);
}

void BattleScene::ResetEncounterDifficulty() {
    minEncounterEnemies = 1;
    maxEncounterEnemies = kMaxBattleEnemies;
    encounterEnemyLevelBonus = 0;
    encounterFloor = 0;
}

PlayerClass BattleScene::GetPlayerClass() const {
    return player.GetClass();
}

int BattleScene::GetPlayerGold() const {
    return player.GetGold();
}

bool BattleScene::TryPurchaseShopItem(items::ItemId id, std::string& outMessage) {
    const items::ItemDefinition& definition = items::GetItemDefinition(id);
    if (definition.buyPrice <= 0) {
        outMessage = "This item cannot be bought.";
        return false;
    }

    if (!player.CanAddItemToInventory(id, 1)) {
        outMessage = std::string("Cannot carry more ") + definition.name + ".";
        return false;
    }

    if (!player.SpendGold(definition.buyPrice)) {
        outMessage = "Not enough gold.";
        return false;
    }

    player.AddItemToInventory(id, 1);
    outMessage = std::string("Bought ") + definition.name + " for " + std::to_string(definition.buyPrice) + " gold.";
    return true;
}

void BattleScene::RestAtInn() {
    player.ResetForBattle();
}

void BattleScene::StartNew() {
    player.ResetForBattle();
    CreateEnemyGroupRandom();
    ReloadRandomBattleBackground();
    phase = BattlePhase::PlayerTurn;
    combatLog = "New battle started. Your turn.";
    enemyActionDelay = 0.0f;
    returnToMenuRequested = false;
    winEventPending = false;
    actionMenuState = ActionMenuState::Root;
    pendingTargetAction = PendingTargetAction::None;
    pendingSkillIndex = 0;
    ResetBattleEffects();
}

void BattleScene::StartFromSave(const BattleSaveData& saveData) {
    ResetEncounterDifficulty();
    player.SetClass(saveData.playerClass);
    player.SetProgress(saveData.playerLevel, saveData.playerExp);
    player.SetGold(saveData.playerGold);
    player.SetCharacterName(saveData.playerName);
    player.SetAvatarPath(saveData.avatarPath);
    player.SetHp(saveData.playerHp);
    player.SetDefending(saveData.playerDefending);

    std::vector<items::InventoryEntry> inventoryEntries;
    const int entryCount = std::clamp(saveData.inventoryEntryCount, 0, kMaxInventorySaveEntries);
    inventoryEntries.reserve(static_cast<size_t>(entryCount));
    for (int i = 0; i < entryCount; ++i) {
        const int rawId = saveData.inventoryItemId[static_cast<size_t>(i)];
        const int quantity = saveData.inventoryQuantity[static_cast<size_t>(i)];
        if (quantity <= 0) {
            continue;
        }

        const items::ItemId itemId = static_cast<items::ItemId>(std::clamp(rawId, 0, 8));
        inventoryEntries.push_back(items::InventoryEntry{itemId, quantity});
    }
    player.SetInventoryEntries(inventoryEntries);

    ReloadPlayerAvatar();

    CreateEnemyGroupFromSave(saveData);
    ReloadRandomBattleBackground();

    phase = saveData.phase;
    combatLog = "Save game loaded.";
    enemyActionDelay = 0.4f;
    returnToMenuRequested = false;
    winEventPending = false;
    actionMenuState = ActionMenuState::Root;
    pendingTargetAction = PendingTargetAction::None;
    pendingSkillIndex = 0;
    ResetBattleEffects();
}

void BattleScene::Update() {
    const float sw = static_cast<float>(GetScreenWidth());
    const float sh = static_cast<float>(GetScreenHeight());
    const float sx = sw / 960.0f;
    const float sy = sh / 540.0f;
    const float sf = std::min(sx, sy);

    const float enemyAreaHeight = sh * (2.0f / 3.0f);
    const float playerAreaTop = enemyAreaHeight;
    const float playerAreaHeight = sh - playerAreaTop;
    const float pad = 12.0f * sf;

    const float actionStartX = sw * 0.64f;
    const float actionWidth = sw - actionStartX - pad;
    const float buttonWidth = (actionWidth - pad) * 0.5f;
    const float buttonHeight = std::max(34.0f * sf, (playerAreaHeight - pad * 4.0f) / 3.0f);
    const float row1Y = playerAreaTop + pad;
    const float row2Y = row1Y + buttonHeight + pad;
    const float row3Y = row2Y + buttonHeight + pad;

    fightButton.bounds = Rectangle{actionStartX, row1Y, buttonWidth, buttonHeight};
    defendButton.bounds = Rectangle{actionStartX + buttonWidth + pad, row1Y, buttonWidth, buttonHeight};
    healButton.bounds = Rectangle{actionStartX, row2Y, buttonWidth, buttonHeight};
    menuButton.bounds = Rectangle{actionStartX + buttonWidth + pad, row2Y, buttonWidth, buttonHeight};
    restartButton.bounds = Rectangle{actionStartX, row3Y, actionWidth, buttonHeight};
    simpleAttackButton.bounds = Rectangle{actionStartX, row1Y, buttonWidth, buttonHeight};
    skillsButton.bounds = Rectangle{actionStartX + buttonWidth + pad, row1Y, buttonWidth, buttonHeight};
    backButton.bounds = Rectangle{actionStartX, row2Y, actionWidth, buttonHeight};
    classSkillButtonOne.bounds = Rectangle{actionStartX, row1Y, buttonWidth, buttonHeight};
    classSkillButtonTwo.bounds = Rectangle{actionStartX + buttonWidth + pad, row1Y, buttonWidth, buttonHeight};
    skillsBackButton.bounds = Rectangle{actionStartX, row2Y, actionWidth, buttonHeight};

    const auto& classSkills = GetClassSkills();
    if (!classSkills.empty()) {
        classSkillButtonOne.text = classSkills[0].name;
    } else {
        classSkillButtonOne.text = "Skill";
    }

    if (classSkills.size() > 1) {
        classSkillButtonTwo.text = classSkills[1].name;
    } else {
        classSkillButtonTwo.text = "-";
    }

    if (actionMenuState == ActionMenuState::Root && ui::IsPressed(menuButton)) {
        returnToMenuRequested = true;
    }

    if ((phase == BattlePhase::Won || phase == BattlePhase::Lost) && ui::IsPressed(restartButton)) {
        StartNew();
    }

    if (phase == BattlePhase::EnemyTurn) {
        enemyActionDelay -= GetFrameTime();
        if (enemyActionDelay <= 0.0f) {
            ApplyEnemyDotEffects();

            int totalDamage = 0;
            int attackers = 0;
            bool defenseAvailable = player.IsDefending();

            if (!AreAllEnemiesDefeated()) {
                for (EnemyUnit& unit : enemies) {
                    if (!unit.enemy->IsAlive()) {
                        continue;
                    }

                    int damage = unit.enemy->RollAttack();
                    if (unit.attackDebuffTurns > 0) {
                        damage = std::max(1, damage - unit.attackDebuffAmount);
                        unit.attackDebuffTurns--;
                        if (unit.attackDebuffTurns <= 0) {
                            unit.attackDebuffAmount = 0;
                        }
                    }

                    if (defenseAvailable) {
                        damage = std::max(1, damage / 2);
                        defenseAvailable = false;
                    }

                    if (playerDamageReductionTurns > 0) {
                        damage = std::max(1, damage - playerDamageReductionAmount);
                    }

                    player.ApplyDamage(damage);
                    totalDamage += damage;
                    attackers++;

                    if (!player.IsAlive()) {
                        break;
                    }
                }
            }

            player.SetDefending(false);
            if (playerDamageReductionTurns > 0) {
                playerDamageReductionTurns--;
                if (playerDamageReductionTurns <= 0) {
                    playerDamageReductionAmount = 0;
                }
            }

            if (attackers > 0) {
                combatLog = "Enemies hit you for " + std::to_string(totalDamage) + " damage.";
            } else {
                combatLog = "No enemies can attack.";
            }

            if (!player.IsAlive()) {
                phase = BattlePhase::Lost;
                actionMenuState = ActionMenuState::Root;
                pendingTargetAction = PendingTargetAction::None;
                combatLog += " You were defeated.";
            } else if (AreAllEnemiesDefeated()) {
                const int expAward = CalculateBattleExpReward();
                const int levelUps = player.AddExperience(expAward);
                phase = BattlePhase::Won;
                winEventPending = true;
                actionMenuState = ActionMenuState::Root;
                pendingTargetAction = PendingTargetAction::None;
                combatLog = "Damage over time defeated all enemies. " + BuildVictoryText(expAward, levelUps) + AwardLootForVictory();
            } else {
                phase = BattlePhase::PlayerTurn;
                actionMenuState = ActionMenuState::Root;
                pendingTargetAction = PendingTargetAction::None;
                combatLog = "Your turn: Fight, Defend, or Heal.";
            }
        }
    }

    if (phase != BattlePhase::PlayerTurn) {
        return;
    }

    if (actionMenuState == ActionMenuState::Root) {
        if (ui::IsPressed(fightButton)) {
            actionMenuState = ActionMenuState::Fight;
            combatLog = "Choose: Simple Attack or Skills.";
        } else if (ui::IsPressed(defendButton)) {
            player.SetDefending(true);
            combatLog = "You defend. The next hit is halved.";
            phase = BattlePhase::EnemyTurn;
            enemyActionDelay = 0.6f;
        } else if (ui::IsPressed(healButton)) {
            const int healAmount = player.RollHeal();
            const int oldHp = player.GetHp();
            player.Heal(healAmount);
            combatLog = "You heal " + std::to_string(player.GetHp() - oldHp) + " HP.";
            phase = BattlePhase::EnemyTurn;
            enemyActionDelay = 0.6f;
        }
    } else if (actionMenuState == ActionMenuState::Fight) {
        if (ui::IsPressed(backButton)) {
            actionMenuState = ActionMenuState::Root;
            combatLog = "Your turn: Fight, Defend, or Heal.";
        } else if (ui::IsPressed(simpleAttackButton)) {
            BeginSimpleAttackTargetSelection();
        } else if (ui::IsPressed(skillsButton)) {
            actionMenuState = ActionMenuState::Skills;
            combatLog = BuildClassSkillMenuText();
        }
    } else if (actionMenuState == ActionMenuState::Skills) {
        if (ui::IsPressed(skillsBackButton)) {
            actionMenuState = ActionMenuState::Fight;
            combatLog = "Choose: Simple Attack or Skills.";
            return;
        }

        if (ui::IsPressed(classSkillButtonOne) && !classSkills.empty()) {
            BeginSkillTargetSelection(0);
        } else if (ui::IsPressed(classSkillButtonTwo) && classSkills.size() > 1) {
            BeginSkillTargetSelection(1);
        }
    } else if (actionMenuState == ActionMenuState::TargetSelect) {
        if (ui::IsPressed(backButton) || IsKeyPressed(KEY_ESCAPE)) {
            CancelTargetSelection();
            return;
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            const int targetIndex = GetEnemyIndexAtMouse(sw, sh, sx, sy, sf);
            if (targetIndex >= 0) {
                ResolvePendingTargetAction(targetIndex);
            }
        }
    }
}

void BattleScene::Draw() {
    const int sw = GetScreenWidth();
    const int sh = GetScreenHeight();
    const float sx = static_cast<float>(sw) / 960.0f;
    const float sy = static_cast<float>(sh) / 540.0f;
    const float sf = std::min(sx, sy);

    if (battleBackgroundLoaded) {
        const float scaleX = static_cast<float>(sw) / static_cast<float>(battleBackgroundTexture.width);
        const float scaleY = static_cast<float>(sh) / static_cast<float>(battleBackgroundTexture.height);
        const float scale = std::max(scaleX, scaleY);
        const float drawW = static_cast<float>(battleBackgroundTexture.width) * scale;
        const float drawH = static_cast<float>(battleBackgroundTexture.height) * scale;
        const float drawX = (static_cast<float>(sw) - drawW) * 0.5f;
        const float drawY = (static_cast<float>(sh) - drawH) * 0.5f;
        DrawTextureEx(battleBackgroundTexture, Vector2{drawX, drawY}, 0.0f, scale, WHITE);
    }

    const float enemyAreaHeight = static_cast<float>(sh) * (2.0f / 3.0f);
    const float playerAreaTop = enemyAreaHeight;
    const float playerAreaHeight = static_cast<float>(sh) - playerAreaTop;
    const float pad = 12.0f * sf;

    DrawRectangle(0, 0, sw, static_cast<int>(enemyAreaHeight), Color{12, 16, 26, 140});
    DrawRectangle(0, static_cast<int>(playerAreaTop), sw, static_cast<int>(playerAreaHeight), Color{10, 13, 22, 170});
    DrawLine(0, static_cast<int>(playerAreaTop), sw, static_cast<int>(playerAreaTop), Color{90, 100, 130, 255});

    DrawText("Battle - Enemies", static_cast<int>(pad), static_cast<int>(14.0f * sy), static_cast<int>(30.0f * sf), RAYWHITE);
    DrawText("Player", static_cast<int>(pad), static_cast<int>(playerAreaTop + 8.0f * sy), static_cast<int>(20.0f * sf), RAYWHITE);

    std::vector<size_t> aliveEnemyIndices;
    aliveEnemyIndices.reserve(enemies.size());
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i].enemy->IsAlive()) {
            aliveEnemyIndices.push_back(i);
        }
    }

    const int enemyCount = static_cast<int>(aliveEnemyIndices.size());
    int cols = 1;
    if (enemyCount <= 2) {
        cols = std::max(1, enemyCount);
    } else if (enemyCount <= 4) {
        cols = 2;
    } else {
        cols = 4;
    }
    const int rows = std::max(1, (enemyCount + cols - 1) / cols);
    const float enemyTop = 56.0f * sy;
    const float enemyBottom = enemyAreaHeight - 10.0f * sy;
    const float enemyGapX = 20.0f * sx;
    const float enemyGapY = 14.0f * sy;
    const float slotWidth = (static_cast<float>(sw) - pad * 2.0f - enemyGapX * static_cast<float>(std::max(cols - 1, 0))) / static_cast<float>(std::max(cols, 1));
    const float slotHeight = (enemyBottom - enemyTop - enemyGapY * static_cast<float>(std::max(rows - 1, 0))) / static_cast<float>(std::max(rows, 1));

    for (int i = 0; i < enemyCount; ++i) {
        const int col = cols == 0 ? 0 : i % cols;
        const int row = cols == 0 ? 0 : i / cols;
        const float x = pad + static_cast<float>(col) * (slotWidth + enemyGapX);
        const float y = enemyTop + static_cast<float>(row) * (slotHeight + enemyGapY);

        EnemyUnit& unit = enemies[aliveEnemyIndices[static_cast<size_t>(i)]];
        DrawText(unit.enemy->GetName().c_str(), static_cast<int>(x + 8.0f * sx), static_cast<int>(y + 4.0f * sy), static_cast<int>(22.0f * sf), RAYWHITE);
        DrawText(TextFormat("Lv %d", unit.enemy->GetLevel()), static_cast<int>(x + 8.0f * sx), static_cast<int>(y + 26.0f * sy), static_cast<int>(16.0f * sf), Color{190, 210, 240, 255});

        const float nameHeight = 42.0f * sf;
        const float hpH = std::max(7.0f * sf, 6.0f);
        const float spriteAreaTop = y + nameHeight + hpH + 14.0f * sy;
        const float spriteAreaHeight = std::max(10.0f, slotHeight - (nameHeight + hpH + 20.0f * sy));
        const float spriteAreaWidth = slotWidth - 16.0f * sx;
        const float spriteAreaLeft = x + 8.0f * sx;

        float spriteDrawX = spriteAreaLeft;
        float spriteDrawY = spriteAreaTop;
        float spriteDrawW = spriteAreaWidth;
        float spriteDrawH = spriteAreaHeight;

        if (unit.spriteLoaded) {
            const float targetW = spriteAreaWidth;
            const float targetH = spriteAreaHeight;
            const float scaleX = targetW / static_cast<float>(unit.sprite.width);
            const float scaleY = targetH / static_cast<float>(unit.sprite.height);
            const float scale = std::min(scaleX, scaleY);
            spriteDrawW = unit.sprite.width * scale;
            spriteDrawH = unit.sprite.height * scale;
            spriteDrawX = spriteAreaLeft + (spriteAreaWidth - spriteDrawW) * 0.5f;
            spriteDrawY = spriteAreaTop + (spriteAreaHeight - spriteDrawH) * 0.5f;
            DrawTextureEx(unit.sprite, Vector2{spriteDrawX, spriteDrawY}, 0.0f, scale, WHITE);
        } else {
            DrawText("Sprite", static_cast<int>(spriteAreaLeft + 10.0f * sx), static_cast<int>(spriteAreaTop + spriteAreaHeight * 0.5f), static_cast<int>(18.0f * sf), LIGHTGRAY);
        }

        const float hpW = std::min(spriteDrawW, slotWidth * 0.5f);
        const float hpX = spriteDrawX + (spriteDrawW - hpW) * 0.5f;
        const float hpY = std::max(y + nameHeight + 4.0f * sy, spriteDrawY - hpH - 4.0f * sy);
        DrawHpBar(hpX, hpY, hpW, hpH, unit.enemy->GetHp(), unit.enemy->GetMaxHp());

    }

    const float avatarSize = std::min(190.0f * sf, playerAreaHeight - 26.0f * sy);
    const float avatarX = 20.0f * sx;
    const float avatarY = playerAreaTop + 22.0f * sy;

    if (playerAvatarLoaded) {
        const float scaleX = avatarSize / static_cast<float>(playerAvatarTexture.width);
        const float scaleY = avatarSize / static_cast<float>(playerAvatarTexture.height);
        const float scale = std::min(scaleX, scaleY);
        DrawTextureEx(playerAvatarTexture, Vector2{avatarX, avatarY}, 0.0f, scale, WHITE);
    } else {
        DrawRectangle(static_cast<int>(avatarX), static_cast<int>(avatarY), static_cast<int>(avatarSize), static_cast<int>(avatarSize), Color{60, 60, 75, 160});
        DrawText("Avatar", static_cast<int>(avatarX + 40.0f * sx), static_cast<int>(avatarY + avatarSize * 0.5f), static_cast<int>(22.0f * sf), LIGHTGRAY);
    }

    const float infoX = avatarX + avatarSize + 12.0f * sx;
    const float infoY = playerAreaTop + 28.0f * sy;
    DrawText(TextFormat("Class: %s", Player::ClassToString(player.GetClass())), static_cast<int>(infoX), static_cast<int>(infoY), static_cast<int>(15.0f * sf), LIGHTGRAY);
    DrawText(TextFormat("Lv %d", player.GetLevel()), static_cast<int>(infoX), static_cast<int>(infoY + 18.0f * sy), static_cast<int>(14.0f * sf), Color{180, 210, 255, 255});
    DrawHpBar(infoX, infoY + 38.0f * sy, 170.0f * sx, 11.0f * sy, player.GetHp(), player.GetMaxHp());

    const float bottomInfoY = playerAreaTop + playerAreaHeight - 34.0f * sy;
    const std::string bottomInfo = player.GetCharacterName() +
        "  |  HP " + std::to_string(player.GetHp()) + "/" + std::to_string(player.GetMaxHp()) +
        "  |  EXP " + std::to_string(player.GetExp()) + "/" + std::to_string(player.GetExpToNextLevel()) +
        "  |  Gold " + std::to_string(player.GetGold()) +
        "  |  INV " + std::to_string(player.GetInventory().GetTotalItemCount()) +
        " (E " + std::to_string(player.GetInventory().GetCountByCategory(items::ItemCategory::Equipment)) +
        ", C " + std::to_string(player.GetInventory().GetCountByCategory(items::ItemCategory::Consumable)) +
        ", Q " + std::to_string(player.GetInventory().GetCountByCategory(items::ItemCategory::Quest)) + ")";
    DrawText(bottomInfo.c_str(), static_cast<int>(20.0f * sx), static_cast<int>(bottomInfoY), static_cast<int>(18.0f * sf), RAYWHITE);

    const float logX = 20.0f * sx;
    const float logY = playerAreaTop + playerAreaHeight - 58.0f * sy;
    DrawText(combatLog.c_str(), static_cast<int>(logX), static_cast<int>(logY), static_cast<int>(16.0f * sf), RAYWHITE);

    const bool isPlayerTurn = phase == BattlePhase::PlayerTurn;
    const bool isRootMenu = actionMenuState == ActionMenuState::Root;
    const bool isFightMenu = actionMenuState == ActionMenuState::Fight;
    const bool isSkillsMenu = actionMenuState == ActionMenuState::Skills;
    const bool isTargetSelect = actionMenuState == ActionMenuState::TargetSelect;
    const auto& classSkills = GetClassSkills();

    fightButton.enabled = isPlayerTurn && isRootMenu;
    defendButton.enabled = isPlayerTurn && isRootMenu;
    healButton.enabled = isPlayerTurn && isRootMenu;
    menuButton.enabled = isRootMenu;

    simpleAttackButton.enabled = isPlayerTurn && isFightMenu;
    skillsButton.enabled = isPlayerTurn && isFightMenu;
    backButton.enabled = isPlayerTurn && isFightMenu;

    classSkillButtonOne.enabled = isPlayerTurn && isSkillsMenu && !classSkills.empty();
    classSkillButtonTwo.enabled = isPlayerTurn && isSkillsMenu && classSkills.size() > 1;
    skillsBackButton.enabled = isPlayerTurn && isSkillsMenu;
    backButton.enabled = isPlayerTurn && isTargetSelect;

    if (isSkillsMenu && isPlayerTurn) {
        ui::DrawButton(classSkillButtonOne);
        ui::DrawButton(classSkillButtonTwo);
        ui::DrawButton(skillsBackButton);
    } else if (isTargetSelect && isPlayerTurn) {
        ui::Button cancelButton = backButton;
        cancelButton.text = "Cancel";
        ui::DrawButton(cancelButton);
    } else if (isFightMenu && isPlayerTurn) {
        ui::DrawButton(simpleAttackButton);
        ui::DrawButton(skillsButton);
        ui::DrawButton(backButton);
    } else {
        ui::DrawButton(fightButton);
        ui::DrawButton(defendButton);
        ui::DrawButton(healButton);
        ui::DrawButton(menuButton);
    }

    if (phase == BattlePhase::Won || phase == BattlePhase::Lost) {
        ui::DrawButton(restartButton);
    }

    if (phase == BattlePhase::Won) {
        DrawText("Victory!", static_cast<int>(438.0f * sx), static_cast<int>(enemyAreaHeight - 28.0f * sy), static_cast<int>(34.0f * sf), Color{240, 220, 90, 255});
    } else if (phase == BattlePhase::Lost) {
        DrawText("Defeat", static_cast<int>(430.0f * sx), static_cast<int>(enemyAreaHeight - 28.0f * sy), static_cast<int>(34.0f * sf), Color{220, 90, 90, 255});
    } else if (phase == BattlePhase::EnemyTurn) {
        DrawText("Enemy turn...", static_cast<int>(420.0f * sx), static_cast<int>(enemyAreaHeight - 28.0f * sy), static_cast<int>(24.0f * sf), LIGHTGRAY);
    } else if (phase == BattlePhase::PlayerTurn && isTargetSelect) {
        DrawText("Choose target", static_cast<int>(420.0f * sx), static_cast<int>(enemyAreaHeight - 28.0f * sy), static_cast<int>(24.0f * sf), Color{245, 205, 90, 255});
    }
}

BattleSaveData BattleScene::GetSaveData() const {
    BattleSaveData data;
    data.playerHp = player.GetHp();
    data.playerLevel = player.GetLevel();
    data.playerExp = player.GetExp();
    data.playerGold = player.GetGold();
    data.enemyCount = static_cast<int>(std::min<size_t>(enemies.size(), static_cast<size_t>(kMaxBattleEnemies)));
    data.playerDefending = player.IsDefending();
    data.playerClass = player.GetClass();
    data.phase = phase;
    data.playerName = player.GetCharacterName();
    data.avatarPath = player.GetAvatarPath();

    for (int i = 0; i < data.enemyCount; ++i) {
        data.enemyType[static_cast<size_t>(i)] = enemies[static_cast<size_t>(i)].enemy->GetArchetype();
        data.enemyHp[static_cast<size_t>(i)] = enemies[static_cast<size_t>(i)].enemy->GetHp();
        data.enemyLevel[static_cast<size_t>(i)] = enemies[static_cast<size_t>(i)].enemy->GetLevel();
    }

    for (int i = data.enemyCount; i < kMaxBattleEnemies; ++i) {
        data.enemyType[static_cast<size_t>(i)] = EnemyArchetype::SmallSlime;
        data.enemyHp[static_cast<size_t>(i)] = 0;
        data.enemyLevel[static_cast<size_t>(i)] = 1;
    }

    const auto& entries = player.GetInventory().GetEntries();
    data.inventoryEntryCount = std::min(static_cast<int>(entries.size()), kMaxInventorySaveEntries);
    for (int i = 0; i < data.inventoryEntryCount; ++i) {
        data.inventoryItemId[static_cast<size_t>(i)] = static_cast<int>(entries[static_cast<size_t>(i)].id);
        data.inventoryQuantity[static_cast<size_t>(i)] = entries[static_cast<size_t>(i)].quantity;
    }
    for (int i = data.inventoryEntryCount; i < kMaxInventorySaveEntries; ++i) {
        data.inventoryItemId[static_cast<size_t>(i)] = 0;
        data.inventoryQuantity[static_cast<size_t>(i)] = 0;
    }

    return data;
}

BattlePhase BattleScene::GetPhase() const {
    return phase;
}

bool BattleScene::ConsumeReturnToMenu() {
    const bool value = returnToMenuRequested;
    returnToMenuRequested = false;
    return value;
}

bool BattleScene::ConsumeWinEvent() {
    const bool value = winEventPending;
    winEventPending = false;
    return value;
}

void BattleScene::ResetBattleEffects() {
    playerDamageReductionAmount = 0;
    playerDamageReductionTurns = 0;

    for (EnemyUnit& unit : enemies) {
        unit.dotDamage = 0;
        unit.dotTurns = 0;
        unit.attackDebuffAmount = 0;
        unit.attackDebuffTurns = 0;
    }
}

void BattleScene::ApplyEnemyDotEffects() {
    int totalDot = 0;
    int affectedEnemies = 0;
    for (EnemyUnit& unit : enemies) {
        if (!unit.enemy->IsAlive() || unit.dotTurns <= 0) {
            continue;
        }

        unit.enemy->ApplyDamage(unit.dotDamage);
        totalDot += unit.dotDamage;
        affectedEnemies++;

        unit.dotTurns--;
        if (unit.dotTurns <= 0) {
            unit.dotDamage = 0;
        }
    }

    if (affectedEnemies > 0) {
        combatLog = "Damage over time hits " + std::to_string(affectedEnemies) + " enemies for " + std::to_string(totalDot) + " total damage.";
    }
}

bool BattleScene::SkillRequiresEnemyTarget(const combat::SkillDefinition& skill) const {
    for (const combat::SkillEffectDefinition& effect : skill.effects) {
        switch (effect.type) {
        case combat::SkillEffectType::DirectDamage:
        case combat::SkillEffectType::AreaDamage:
        case combat::SkillEffectType::DamageOverTime:
        case combat::SkillEffectType::EnemyAttackDebuff:
            return true;
        case combat::SkillEffectType::PlayerDamageReductionBuff:
        case combat::SkillEffectType::SelfHeal:
            break;
        }
    }

    return false;
}

void BattleScene::BeginSimpleAttackTargetSelection() {
    const int targetIndex = FindFirstAliveEnemyIndex();
    if (targetIndex < 0) {
        phase = BattlePhase::Won;
        winEventPending = true;
        actionMenuState = ActionMenuState::Root;
        pendingTargetAction = PendingTargetAction::None;
        return;
    }

    pendingTargetAction = PendingTargetAction::SimpleAttack;
    pendingSkillIndex = 0;
    actionMenuState = ActionMenuState::TargetSelect;
    combatLog = "Select a target for Simple Attack.";
}

void BattleScene::BeginSkillTargetSelection(size_t skillIndex) {
    const auto& skills = GetClassSkills();
    if (skillIndex >= skills.size()) {
        combatLog = "No valid skill selected.";
        actionMenuState = ActionMenuState::Fight;
        return;
    }

    const combat::SkillDefinition& skill = skills[skillIndex];
    if (!SkillRequiresEnemyTarget(skill)) {
        ExecuteClassSkill(skillIndex, -1);
        return;
    }

    if (FindFirstAliveEnemyIndex() < 0) {
        phase = BattlePhase::Won;
        winEventPending = true;
        actionMenuState = ActionMenuState::Root;
        pendingTargetAction = PendingTargetAction::None;
        return;
    }

    pendingTargetAction = PendingTargetAction::ClassSkill;
    pendingSkillIndex = skillIndex;
    actionMenuState = ActionMenuState::TargetSelect;
    combatLog = "Select a target for " + skill.name + ".";
}

void BattleScene::CancelTargetSelection() {
    pendingTargetAction = PendingTargetAction::None;
    pendingSkillIndex = 0;
    actionMenuState = ActionMenuState::Fight;
    combatLog = "Target selection canceled. Choose: Simple Attack or Skills.";
}

int BattleScene::GetEnemyIndexAtMouse(float sw, float sh, float sx, float sy, float sf) const {
    std::vector<size_t> aliveEnemyIndices;
    aliveEnemyIndices.reserve(enemies.size());
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i].enemy->IsAlive()) {
            aliveEnemyIndices.push_back(i);
        }
    }

    const int enemyCount = static_cast<int>(aliveEnemyIndices.size());
    if (enemyCount <= 0) {
        return -1;
    }

    const float enemyAreaHeight = sh * (2.0f / 3.0f);
    const float pad = 12.0f * sf;
    int cols = 1;
    if (enemyCount <= 2) {
        cols = std::max(1, enemyCount);
    } else if (enemyCount <= 4) {
        cols = 2;
    } else {
        cols = 4;
    }
    const int rows = std::max(1, (enemyCount + cols - 1) / cols);
    const float enemyTop = 56.0f * sy;
    const float enemyBottom = enemyAreaHeight - 10.0f * sy;
    const float enemyGapX = 20.0f * sx;
    const float enemyGapY = 14.0f * sy;
    const float slotWidth = (sw - pad * 2.0f - enemyGapX * static_cast<float>(std::max(cols - 1, 0))) / static_cast<float>(std::max(cols, 1));
    const float slotHeight = (enemyBottom - enemyTop - enemyGapY * static_cast<float>(std::max(rows - 1, 0))) / static_cast<float>(std::max(rows, 1));

    const Vector2 mousePos = GetMousePosition();
    for (int i = 0; i < enemyCount; ++i) {
        const int col = cols == 0 ? 0 : i % cols;
        const int row = cols == 0 ? 0 : i / cols;
        const float x = pad + static_cast<float>(col) * (slotWidth + enemyGapX);
        const float y = enemyTop + static_cast<float>(row) * (slotHeight + enemyGapY);
        const Rectangle slotBounds{x, y, slotWidth, slotHeight};

        if (CheckCollisionPointRec(mousePos, slotBounds)) {
            return static_cast<int>(aliveEnemyIndices[static_cast<size_t>(i)]);
        }
    }

    return -1;
}

void BattleScene::ResolvePendingTargetAction(int targetIndex) {
    switch (pendingTargetAction) {
    case PendingTargetAction::SimpleAttack:
        ExecuteSimpleAttack(targetIndex);
        break;
    case PendingTargetAction::ClassSkill:
        ExecuteClassSkill(pendingSkillIndex, targetIndex);
        break;
    case PendingTargetAction::None:
        break;
    }
}

void BattleScene::ExecuteSimpleAttack(int targetIndex) {
    if (targetIndex < 0 || targetIndex >= static_cast<int>(enemies.size()) || !enemies[static_cast<size_t>(targetIndex)].enemy->IsAlive()) {
        combatLog = "Invalid target.";
        return;
    }

    Enemy& target = *enemies[static_cast<size_t>(targetIndex)].enemy;
    const int damage = player.RollAttack();
    target.ApplyDamage(damage);
    combatLog = "You strike " + target.GetName() + " for " + std::to_string(damage) + " damage.";

    if (AreAllEnemiesDefeated()) {
        const int expAward = CalculateBattleExpReward();
        const int levelUps = player.AddExperience(expAward);
        phase = BattlePhase::Won;
        combatLog += " " + BuildVictoryText(expAward, levelUps) + AwardLootForVictory();
        winEventPending = true;
    } else {
        phase = BattlePhase::EnemyTurn;
        enemyActionDelay = 0.6f;
    }

    pendingTargetAction = PendingTargetAction::None;
    pendingSkillIndex = 0;
    actionMenuState = ActionMenuState::Root;
}

void BattleScene::ExecuteClassSkill(size_t skillIndex, int targetIndex) {
    const auto& skills = GetClassSkills();
    if (skillIndex >= skills.size()) {
        combatLog = "No valid skill selected.";
        actionMenuState = ActionMenuState::Fight;
        return;
    }

    const combat::SkillDefinition& skill = skills[skillIndex];
    int totalImmediateDamage = 0;
    int directHits = 0;
    int aoeHits = 0;
    int dotApplied = 0;
    int debuffApplied = 0;
    int healed = 0;

    const bool validTarget = targetIndex >= 0 && targetIndex < static_cast<int>(enemies.size()) && enemies[static_cast<size_t>(targetIndex)].enemy->IsAlive();
    const bool hasAreaDamage = std::any_of(skill.effects.begin(), skill.effects.end(), [](const combat::SkillEffectDefinition& effect) {
        return effect.type == combat::SkillEffectType::AreaDamage;
    });

    for (const combat::SkillEffectDefinition& effect : skill.effects) {
        switch (effect.type) {
        case combat::SkillEffectType::DirectDamage: {
            if (validTarget) {
                EnemyUnit& target = enemies[static_cast<size_t>(targetIndex)];
                target.enemy->ApplyDamage(effect.magnitude);
                totalImmediateDamage += effect.magnitude;
                directHits++;
            }
            break;
        }
        case combat::SkillEffectType::AreaDamage: {
            for (EnemyUnit& unit : enemies) {
                if (!unit.enemy->IsAlive()) {
                    continue;
                }

                unit.enemy->ApplyDamage(effect.magnitude);
                totalImmediateDamage += effect.magnitude;
                aoeHits++;
            }
            break;
        }
        case combat::SkillEffectType::DamageOverTime: {
            if (hasAreaDamage) {
                for (EnemyUnit& unit : enemies) {
                    if (!unit.enemy->IsAlive()) {
                        continue;
                    }

                    unit.dotDamage = std::max(unit.dotDamage, effect.magnitude);
                    unit.dotTurns = std::max(unit.dotTurns, effect.durationTurns);
                    dotApplied++;
                }
            } else if (validTarget) {
                EnemyUnit& target = enemies[static_cast<size_t>(targetIndex)];
                if (target.enemy->IsAlive()) {
                    target.dotDamage = std::max(target.dotDamage, effect.magnitude);
                    target.dotTurns = std::max(target.dotTurns, effect.durationTurns);
                    dotApplied = 1;
                }
            }
            break;
        }
        case combat::SkillEffectType::PlayerDamageReductionBuff: {
            playerDamageReductionAmount = std::max(playerDamageReductionAmount, effect.magnitude);
            playerDamageReductionTurns = std::max(playerDamageReductionTurns, effect.durationTurns);
            break;
        }
        case combat::SkillEffectType::EnemyAttackDebuff: {
            if (hasAreaDamage) {
                for (EnemyUnit& unit : enemies) {
                    if (!unit.enemy->IsAlive()) {
                        continue;
                    }

                    unit.attackDebuffAmount = std::max(unit.attackDebuffAmount, effect.magnitude);
                    unit.attackDebuffTurns = std::max(unit.attackDebuffTurns, effect.durationTurns);
                    debuffApplied++;
                }
            } else if (validTarget) {
                EnemyUnit& target = enemies[static_cast<size_t>(targetIndex)];
                if (target.enemy->IsAlive()) {
                    target.attackDebuffAmount = std::max(target.attackDebuffAmount, effect.magnitude);
                    target.attackDebuffTurns = std::max(target.attackDebuffTurns, effect.durationTurns);
                    debuffApplied = 1;
                }
            }
            break;
        }
        case combat::SkillEffectType::SelfHeal: {
            const int oldHp = player.GetHp();
            player.Heal(effect.magnitude);
            healed += player.GetHp() - oldHp;
            break;
        }
        }
    }

    combatLog = skill.name + ": " + skill.description;
    if (totalImmediateDamage > 0) {
        combatLog += " Immediate damage " + std::to_string(totalImmediateDamage) + ".";
    }
    if (dotApplied > 0) {
        combatLog += " DoT applied to " + std::to_string(dotApplied) + " target(s).";
    }
    if (debuffApplied > 0) {
        combatLog += " Attack debuff on " + std::to_string(debuffApplied) + " target(s).";
    }
    if (playerDamageReductionTurns > 0) {
        combatLog += " Damage reduction active for " + std::to_string(playerDamageReductionTurns) + " turn(s).";
    }
    if (healed > 0) {
        combatLog += " Healed " + std::to_string(healed) + " HP.";
    }

    if (AreAllEnemiesDefeated()) {
        const int expAward = CalculateBattleExpReward();
        const int levelUps = player.AddExperience(expAward);
        phase = BattlePhase::Won;
        combatLog += " " + BuildVictoryText(expAward, levelUps) + AwardLootForVictory();
        winEventPending = true;
    } else {
        phase = BattlePhase::EnemyTurn;
        enemyActionDelay = 0.6f;
    }

    pendingTargetAction = PendingTargetAction::None;
    pendingSkillIndex = 0;
    actionMenuState = ActionMenuState::Root;
}

std::string BattleScene::BuildClassSkillMenuText() const {
    const auto& skills = GetClassSkills();
    if (skills.empty()) {
        return "No class skills available.";
    }

    std::string text = "Class Skills: " + skills[0].name + " - " + skills[0].description;
    if (skills.size() > 1) {
        text += " | " + skills[1].name + " - " + skills[1].description;
    }
    return text;
}

const std::vector<combat::SkillDefinition>& BattleScene::GetClassSkills() const {
    return player.GetSkills();
}

void BattleScene::DrawHpBar(float x, float y, float w, float h, int hp, int maxHp) const {
    const float ratio = maxHp > 0 ? static_cast<float>(hp) / static_cast<float>(maxHp) : 0.0f;
    DrawRectangle(static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h), DARKGRAY);
    DrawRectangle(static_cast<int>(x), static_cast<int>(y), static_cast<int>(w * std::clamp(ratio, 0.0f, 1.0f)), static_cast<int>(h), GREEN);
    DrawRectangleLines(static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h), RAYWHITE);
}

void BattleScene::ReloadPlayerAvatar() {
    if (playerAvatarLoaded) {
        UnloadTexture(playerAvatarTexture);
        playerAvatarLoaded = false;
    }

    const std::string& path = player.GetAvatarPath();
    if (!path.empty() && FileExists(path.c_str())) {
        playerAvatarTexture = LoadTexture(path.c_str());
        playerAvatarLoaded = playerAvatarTexture.id != 0;
    }
}

void BattleScene::ReloadRandomBattleBackground() {
    UnloadBattleBackground();

    const std::string backgroundRoot = ResolveAssetPath("assets/backgrounds");
    const std::filesystem::path root(backgroundRoot);
    if (!std::filesystem::exists(root) || !std::filesystem::is_directory(root)) {
        return;
    }

    std::vector<std::string> files;
    for (const auto& entry : std::filesystem::directory_iterator(root)) {
        if (!entry.is_regular_file()) {
            continue;
        }

        std::string extension = entry.path().extension().string();
        std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char c) {
            return static_cast<char>(std::tolower(c));
        });

        if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".bmp") {
            files.push_back(std::filesystem::absolute(entry.path()).generic_string());
        }
    }

    if (files.empty()) {
        return;
    }

    const int randomIndex = GetRandomValue(0, static_cast<int>(files.size()) - 1);
    const std::string& filePath = files[static_cast<size_t>(randomIndex)];
    if (FileExists(filePath.c_str())) {
        battleBackgroundTexture = LoadTexture(filePath.c_str());
        battleBackgroundLoaded = battleBackgroundTexture.id != 0;
    }
}

void BattleScene::UnloadBattleBackground() {
    if (battleBackgroundLoaded) {
        UnloadTexture(battleBackgroundTexture);
        battleBackgroundLoaded = false;
    }
}

std::string BattleScene::ResolveAssetPath(const std::string& relativePath) const {
    std::filesystem::path direct(relativePath);
    if (direct.is_absolute() && std::filesystem::exists(direct)) {
        return direct.generic_string();
    }

    std::filesystem::path current = std::filesystem::current_path();
    for (int i = 0; i < 8; ++i) {
        const std::filesystem::path candidate = current / relativePath;
        if (std::filesystem::exists(candidate)) {
            return std::filesystem::absolute(candidate).generic_string();
        }

        if (!current.has_parent_path()) {
            break;
        }

        const std::filesystem::path parent = current.parent_path();
        if (parent == current) {
            break;
        }
        current = parent;
    }

    return relativePath;
}

void BattleScene::ClearEnemies() {
    for (EnemyUnit& unit : enemies) {
        if (unit.spriteLoaded) {
            UnloadTexture(unit.sprite);
            unit.spriteLoaded = false;
        }
    }
    enemies.clear();
}

void BattleScene::CreateEnemyGroupRandom() {
    ClearEnemies();

    const int count = GetRandomValue(minEncounterEnemies, maxEncounterEnemies);
    for (int i = 0; i < count; ++i) {
        AddEnemyToGroup(RollRandomEnemyArchetype(), RollEnemyLevel(), -1);
    }
}

void BattleScene::CreateEnemyGroupFromSave(const BattleSaveData& saveData) {
    ClearEnemies();

    const int count = std::clamp(saveData.enemyCount, 1, kMaxBattleEnemies);
    for (int i = 0; i < count; ++i) {
        AddEnemyToGroup(saveData.enemyType[static_cast<size_t>(i)], saveData.enemyLevel[static_cast<size_t>(i)], saveData.enemyHp[static_cast<size_t>(i)]);
    }
}

int BattleScene::RollEnemyLevel() const {
    const int playerLevel = player.GetLevel();
    const int minLevel = std::max(1, playerLevel - 1 + encounterEnemyLevelBonus);
    const int maxLevel = std::max(minLevel, playerLevel + 1 + encounterEnemyLevelBonus);
    return GetRandomValue(minLevel, maxLevel);
}

int BattleScene::CalculateBattleExpReward() const {
    int totalExp = 0;
    for (const EnemyUnit& unit : enemies) {
        totalExp += unit.enemy->GetExpReward();
    }
    return std::max(1, totalExp);
}

int BattleScene::CalculateBattleGoldReward() const {
    int totalGold = 0;
    for (const EnemyUnit& unit : enemies) {
        totalGold += 8 + unit.enemy->GetLevel() * 3;
    }
    return std::max(1, totalGold);
}

std::string BattleScene::AwardLootForVictory() {
    const int goldReward = CalculateBattleGoldReward();
    player.AddGold(goldReward);

    const std::vector<items::ItemId> loot = items::RollLootForEncounter(encounterFloor);
    std::string text = " Gold +" + std::to_string(goldReward) + ".";
    if (loot.empty()) {
        return text;
    }

    std::vector<std::string> acquiredNames;
    acquiredNames.reserve(loot.size());
    for (const items::ItemId id : loot) {
        if (!player.CanAddItemToInventory(id, 1)) {
            continue;
        }

        player.AddItemToInventory(id, 1);
        acquiredNames.push_back(items::GetItemDefinition(id).name);
    }

    if (acquiredNames.empty()) {
        return text;
    }

    text += " Loot: ";
    for (size_t i = 0; i < acquiredNames.size(); ++i) {
        text += acquiredNames[i];
        if (i + 1 < acquiredNames.size()) {
            text += ", ";
        }
    }
    text += ".";

    return text;
}

std::string BattleScene::BuildVictoryText(int expAward, int levelUps) const {
    std::string result = "Victory! You gain " + std::to_string(expAward) + " EXP.";
    if (levelUps > 0) {
        result += " Level up +" + std::to_string(levelUps) + " (now level " + std::to_string(player.GetLevel()) + ").";
    }
    return result;
}

void BattleScene::AddEnemyToGroup(EnemyArchetype type, int level, int hp) {
    EnemyUnit unit{};
    unit.enemy = CreateEnemy(type, level);
    unit.dotDamage = 0;
    unit.dotTurns = 0;
    unit.attackDebuffAmount = 0;
    unit.attackDebuffTurns = 0;
    if (hp >= 0) {
        unit.enemy->SetHp(hp);
    }

    const std::string spritePath = ResolveAssetPath(unit.enemy->GetSpritePath());
    if (FileExists(spritePath.c_str())) {
        unit.sprite = LoadTexture(spritePath.c_str());
        unit.spriteLoaded = unit.sprite.id != 0;
    } else {
        unit.spriteLoaded = false;
    }

    enemies.push_back(std::move(unit));
}

int BattleScene::FindFirstAliveEnemyIndex() const {
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i].enemy->IsAlive()) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

bool BattleScene::AreAllEnemiesDefeated() const {
    for (const EnemyUnit& unit : enemies) {
        if (unit.enemy->IsAlive()) {
            return false;
        }
    }

    return true;
}
