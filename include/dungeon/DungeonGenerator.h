#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace dungeon {

enum class TileType : uint8_t {
    Wall = 0,
    Floor = 1,
    Start = 2,
    Exit = 3
};

struct CellPos {
    int x = 0;
    int y = 0;
};

struct DungeonMap {
    int width = 0;
    int height = 0;
    CellPos start{};
    CellPos exit{};
    std::vector<std::vector<TileType>> tiles;
};

class DungeonGenerator {
public:
    // Generates a fully connected maze-like dungeon using randomized DFS backtracking.
    static DungeonMap GenerateRandomMaze(int width, int height, std::optional<uint32_t> seed = std::nullopt);

    static bool IsWalkable(const DungeonMap& map, int x, int y);
    static std::string ToDebugString(const DungeonMap& map);

private:
    static int NormalizeOddSize(int size);
    static bool InBounds(const DungeonMap& map, int x, int y);
};

} // namespace dungeon
