#pragma once

#include "dungeon/DungeonGenerator.h"

namespace dungeon {

enum class MoveResult {
    NoInput,
    Blocked,
    Moved,
    ReachedExit
};

class DungeonTraversal {
public:
    explicit DungeonTraversal(const DungeonMap& map);

    void ResetToStart();
    const CellPos& GetPlayerPosition() const;

    // Tries to move one tile using W/A/S/D (single-step on key press).
    MoveResult UpdateFromInputWASD();

    // Tries to move one tile by delta values (-1, 0, 1).
    MoveResult TryMoveByDelta(int dx, int dy);

private:
    const DungeonMap* map;
    CellPos playerPos;
};

} // namespace dungeon
