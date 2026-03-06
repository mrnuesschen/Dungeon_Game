#pragma once

#include "GameTypes.h"

#include <vector>

namespace save_system {

constexpr int kMaxGameSaveSlots = 6;

bool SaveBattle(const BattleSaveData& data);
bool LoadBattle(BattleSaveData& outData);
bool BattleSaveExists();
void DeleteBattleSave();

int CreateInnSave(const BattleSaveData& data);
bool LoadGameSlot(int slotIndex, BattleSaveData& outData);
bool DeleteGameSlot(int slotIndex);
std::vector<SaveSlotPreview> ListGameSaves();

bool SaveGallery(const GalleryData& data);
GalleryData LoadGallery();

}
