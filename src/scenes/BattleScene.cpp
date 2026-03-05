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
    combatLog("Your turn: Attack, Defend, or Heal."),
      enemyActionDelay(0.0f),
      returnToMenuRequested(false),
      winEventPending(false),
    attackButton{Rectangle{620.0f, 345.0f, 140.0f, 52.0f}, "Attack", true},
    defendButton{Rectangle{770.0f, 345.0f, 140.0f, 52.0f}, "Defend", true},
    healButton{Rectangle{620.0f, 407.0f, 140.0f, 52.0f}, "Heal", true},
    menuButton{Rectangle{770.0f, 407.0f, 140.0f, 52.0f}, "Menu", true},
    restartButton{Rectangle{620.0f, 469.0f, 290.0f, 52.0f}, "New Battle", true},
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
    player.SetCharacterName(setupData.playerName);
    player.SetAvatarPath(setupData.avatarPath);
    ReloadPlayerAvatar();
}

PlayerClass BattleScene::GetPlayerClass() const {
    return player.GetClass();
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
}

void BattleScene::StartFromSave(const BattleSaveData& saveData) {
    player.SetClass(saveData.playerClass);
    player.SetCharacterName(saveData.playerName);
    player.SetAvatarPath(saveData.avatarPath);
    player.SetHp(saveData.playerHp);
    player.SetDefending(saveData.playerDefending);
    ReloadPlayerAvatar();

    CreateEnemyGroupFromSave(saveData);
    ReloadRandomBattleBackground();

    phase = saveData.phase;
    combatLog = "Save game loaded.";
    enemyActionDelay = 0.4f;
    returnToMenuRequested = false;
    winEventPending = false;
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

    attackButton.bounds = Rectangle{actionStartX, row1Y, buttonWidth, buttonHeight};
    defendButton.bounds = Rectangle{actionStartX + buttonWidth + pad, row1Y, buttonWidth, buttonHeight};
    healButton.bounds = Rectangle{actionStartX, row2Y, buttonWidth, buttonHeight};
    menuButton.bounds = Rectangle{actionStartX + buttonWidth + pad, row2Y, buttonWidth, buttonHeight};
    restartButton.bounds = Rectangle{actionStartX, row3Y, actionWidth, buttonHeight};

    if (ui::IsPressed(menuButton)) {
        returnToMenuRequested = true;
    }

    if ((phase == BattlePhase::Won || phase == BattlePhase::Lost) && ui::IsPressed(restartButton)) {
        StartNew();
    }

    if (phase == BattlePhase::EnemyTurn) {
        enemyActionDelay -= GetFrameTime();
        if (enemyActionDelay <= 0.0f) {
            int totalDamage = 0;
            int attackers = 0;
            bool defenseAvailable = player.IsDefending();

            for (EnemyUnit& unit : enemies) {
                if (!unit.enemy->IsAlive()) {
                    continue;
                }

                int damage = unit.enemy->RollAttack();
                if (defenseAvailable) {
                    damage = std::max(1, damage / 2);
                    defenseAvailable = false;
                }

                player.ApplyDamage(damage);
                totalDamage += damage;
                attackers++;

                if (!player.IsAlive()) {
                    break;
                }
            }

            player.SetDefending(false);

            if (attackers > 0) {
                combatLog = "Enemies hit you for " + std::to_string(totalDamage) + " damage.";
            } else {
                combatLog = "No enemies can attack.";
            }

            if (!player.IsAlive()) {
                phase = BattlePhase::Lost;
                combatLog += " You were defeated.";
            } else {
                phase = BattlePhase::PlayerTurn;
            }
        }
    }

    if (phase == BattlePhase::PlayerTurn) {
        if (ui::IsPressed(attackButton)) {
            const int targetIndex = FindFirstAliveEnemyIndex();
            if (targetIndex < 0) {
                phase = BattlePhase::Won;
                winEventPending = true;
                return;
            }

            const int damage = player.RollAttack();
            enemies[static_cast<size_t>(targetIndex)].enemy->ApplyDamage(damage);
            combatLog = "You attack " + enemies[static_cast<size_t>(targetIndex)].enemy->GetName() + " and deal " + std::to_string(damage) + " damage.";

            if (AreAllEnemiesDefeated()) {
                phase = BattlePhase::Won;
                combatLog += " Victory!";
                winEventPending = true;
            } else {
                phase = BattlePhase::EnemyTurn;
                enemyActionDelay = 0.6f;
            }
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
    DrawText("Player", static_cast<int>(pad), static_cast<int>(playerAreaTop + 8.0f * sy), static_cast<int>(26.0f * sf), RAYWHITE);

    const int enemyCount = static_cast<int>(enemies.size());
    const int cols = enemyCount <= 2 ? enemyCount : 2;
    const int rows = enemyCount <= 2 ? 1 : 2;
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

        EnemyUnit& unit = enemies[static_cast<size_t>(i)];
        Color textColor = unit.enemy->IsAlive() ? RAYWHITE : GRAY;
        DrawText(unit.enemy->GetName().c_str(), static_cast<int>(x + 8.0f * sx), static_cast<int>(y + 4.0f * sy), static_cast<int>(22.0f * sf), textColor);

        const float nameHeight = 24.0f * sf;
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
            DrawTextureEx(unit.sprite, Vector2{spriteDrawX, spriteDrawY}, 0.0f, scale, unit.enemy->IsAlive() ? WHITE : Color{140, 140, 140, 255});
        } else {
            DrawText("Sprite", static_cast<int>(spriteAreaLeft + 10.0f * sx), static_cast<int>(spriteAreaTop + spriteAreaHeight * 0.5f), static_cast<int>(18.0f * sf), LIGHTGRAY);
        }

        const float hpW = std::min(spriteDrawW, slotWidth * 0.5f);
        const float hpX = spriteDrawX + (spriteDrawW - hpW) * 0.5f;
        const float hpY = std::max(y + nameHeight + 4.0f * sy, spriteDrawY - hpH - 4.0f * sy);
        DrawHpBar(hpX, hpY, hpW, hpH, unit.enemy->GetHp(), unit.enemy->GetMaxHp());
    }

    if (playerAvatarLoaded) {
        const float avatarSize = std::min(170.0f * sf, playerAreaHeight - 34.0f * sy);
        const float avatarX = 20.0f * sx;
        const float avatarY = playerAreaTop + 36.0f * sy;
        const float scaleX = avatarSize / static_cast<float>(playerAvatarTexture.width);
        const float scaleY = avatarSize / static_cast<float>(playerAvatarTexture.height);
        const float scale = std::min(scaleX, scaleY);
        DrawTextureEx(playerAvatarTexture, Vector2{avatarX, avatarY}, 0.0f, scale, WHITE);
    } else {
        const float avatarSize = std::min(170.0f * sf, playerAreaHeight - 34.0f * sy);
        const float avatarX = 20.0f * sx;
        const float avatarY = playerAreaTop + 36.0f * sy;
        DrawRectangle(static_cast<int>(avatarX), static_cast<int>(avatarY), static_cast<int>(avatarSize), static_cast<int>(avatarSize), Color{60, 60, 75, 160});
        DrawText("Avatar", static_cast<int>(avatarX + 40.0f * sx), static_cast<int>(avatarY + avatarSize * 0.5f), static_cast<int>(22.0f * sf), LIGHTGRAY);
    }

    const float avatarSize = std::min(170.0f * sf, playerAreaHeight - 34.0f * sy);
    const float infoX = 20.0f * sx + avatarSize + 14.0f * sx;
    const float infoY = playerAreaTop + 36.0f * sy;
    DrawText(player.GetCharacterName().c_str(), static_cast<int>(infoX), static_cast<int>(infoY), static_cast<int>(28.0f * sf), RAYWHITE);
    DrawText(TextFormat("Class: %s", Player::ClassToString(player.GetClass())), static_cast<int>(infoX), static_cast<int>(infoY + 34.0f * sy), static_cast<int>(20.0f * sf), LIGHTGRAY);
    DrawHpBar(infoX, infoY + 64.0f * sy, 300.0f * sx, 18.0f * sy, player.GetHp(), player.GetMaxHp());
    DrawText(TextFormat("HP: %d / %d", player.GetHp(), player.GetMaxHp()), static_cast<int>(infoX), static_cast<int>(infoY + 86.0f * sy), static_cast<int>(18.0f * sf), RAYWHITE);

    const float logX = 20.0f * sx;
    const float logY = playerAreaTop + playerAreaHeight - 36.0f * sy;
    DrawText(combatLog.c_str(), static_cast<int>(logX), static_cast<int>(logY), static_cast<int>(18.0f * sf), RAYWHITE);

    attackButton.enabled = phase == BattlePhase::PlayerTurn;
    defendButton.enabled = phase == BattlePhase::PlayerTurn;
    healButton.enabled = phase == BattlePhase::PlayerTurn;

    ui::DrawButton(attackButton);
    ui::DrawButton(defendButton);
    ui::DrawButton(healButton);
    ui::DrawButton(menuButton);

    if (phase == BattlePhase::Won || phase == BattlePhase::Lost) {
        ui::DrawButton(restartButton);
    }

    if (phase == BattlePhase::Won) {
        DrawText("Victory!", static_cast<int>(438.0f * sx), static_cast<int>(enemyAreaHeight - 28.0f * sy), static_cast<int>(34.0f * sf), Color{240, 220, 90, 255});
    } else if (phase == BattlePhase::Lost) {
        DrawText("Defeat", static_cast<int>(430.0f * sx), static_cast<int>(enemyAreaHeight - 28.0f * sy), static_cast<int>(34.0f * sf), Color{220, 90, 90, 255});
    } else if (phase == BattlePhase::EnemyTurn) {
        DrawText("Enemy turn...", static_cast<int>(420.0f * sx), static_cast<int>(enemyAreaHeight - 28.0f * sy), static_cast<int>(24.0f * sf), LIGHTGRAY);
    }
}

BattleSaveData BattleScene::GetSaveData() const {
    BattleSaveData data;
    data.playerHp = player.GetHp();
    data.enemyCount = static_cast<int>(std::min<size_t>(enemies.size(), static_cast<size_t>(kMaxBattleEnemies)));
    data.playerDefending = player.IsDefending();
    data.playerClass = player.GetClass();
    data.phase = phase;
    data.playerName = player.GetCharacterName();
    data.avatarPath = player.GetAvatarPath();

    for (int i = 0; i < data.enemyCount; ++i) {
        data.enemyType[static_cast<size_t>(i)] = enemies[static_cast<size_t>(i)].enemy->GetArchetype();
        data.enemyHp[static_cast<size_t>(i)] = enemies[static_cast<size_t>(i)].enemy->GetHp();
    }

    for (int i = data.enemyCount; i < kMaxBattleEnemies; ++i) {
        data.enemyType[static_cast<size_t>(i)] = EnemyArchetype::SmallSlime;
        data.enemyHp[static_cast<size_t>(i)] = 0;
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

    const int count = GetRandomValue(1, kMaxBattleEnemies);
    for (int i = 0; i < count; ++i) {
        AddEnemyToGroup(RollRandomEnemyArchetype(), -1);
    }
}

void BattleScene::CreateEnemyGroupFromSave(const BattleSaveData& saveData) {
    ClearEnemies();

    const int count = std::clamp(saveData.enemyCount, 1, kMaxBattleEnemies);
    for (int i = 0; i < count; ++i) {
        AddEnemyToGroup(saveData.enemyType[static_cast<size_t>(i)], saveData.enemyHp[static_cast<size_t>(i)]);
    }
}

void BattleScene::AddEnemyToGroup(EnemyArchetype type, int hp) {
    EnemyUnit unit{};
    unit.enemy = CreateEnemy(type);
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
