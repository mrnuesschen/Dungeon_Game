#include "dungeon/DungeonTraversal.h"

#include "raylib.h"

namespace dungeon {

DungeonTraversal::DungeonTraversal(const DungeonMap& mapRef)
    : map(&mapRef),
      playerPos(mapRef.start) {}

void DungeonTraversal::ResetToStart() {
    if (map != nullptr) {
        playerPos = map->start;
    }
}

const CellPos& DungeonTraversal::GetPlayerPosition() const {
    return playerPos;
}

MoveResult DungeonTraversal::UpdateFromInputWASD() {
    int dx = 0;
    int dy = 0;

    if (IsKeyPressed(KEY_W)) {
        dy = -1;
    } else if (IsKeyPressed(KEY_S)) {
        dy = 1;
    } else if (IsKeyPressed(KEY_A)) {
        dx = -1;
    } else if (IsKeyPressed(KEY_D)) {
        dx = 1;
    } else {
        return MoveResult::NoInput;
    }

    return TryMoveByDelta(dx, dy);
}

MoveResult DungeonTraversal::TryMoveByDelta(int dx, int dy) {
    if (map == nullptr) {
        return MoveResult::Blocked;
    }

    if ((dx != 0 && dy != 0) || (dx == 0 && dy == 0)) {
        return MoveResult::Blocked;
    }

    const int nextX = playerPos.x + dx;
    const int nextY = playerPos.y + dy;

    if (!DungeonGenerator::IsWalkable(*map, nextX, nextY)) {
        return MoveResult::Blocked;
    }

    playerPos.x = nextX;
    playerPos.y = nextY;

    if (playerPos.x == map->exit.x && playerPos.y == map->exit.y) {
        return MoveResult::ReachedExit;
    }

    return MoveResult::Moved;
}

} // namespace dungeon
