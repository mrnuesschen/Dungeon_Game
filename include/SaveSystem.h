#pragma once

#include "GameTypes.h"

namespace save_system {

bool SaveBattle(const BattleSaveData& data);
bool LoadBattle(BattleSaveData& outData);
bool BattleSaveExists();
void DeleteBattleSave();

bool SaveGallery(const GalleryData& data);
GalleryData LoadGallery();

}
