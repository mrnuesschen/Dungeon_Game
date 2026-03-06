#include "dungeon/DungeonGenerator.h"

#include <algorithm>
#include <array>
#include <queue>
#include <random>
#include <sstream>

namespace dungeon {

namespace {

struct Direction {
    int dx;
    int dy;
};

bool IsCarvableCell(const DungeonMap& map, int x, int y) {
    return x > 0 && y > 0 && x < map.width - 1 && y < map.height - 1;
}

bool IsFloorTile(const DungeonMap& map, int x, int y) {
    if (x < 0 || y < 0 || x >= map.width || y >= map.height) {
        return false;
    }

    const TileType tile = map.tiles[static_cast<size_t>(y)][static_cast<size_t>(x)];
    return tile == TileType::Floor || tile == TileType::Start || tile == TileType::Exit;
}

} // namespace

DungeonMap DungeonGenerator::GenerateRandomMaze(int width, int height, std::optional<uint32_t> seed) {
    DungeonMap map;
    map.width = NormalizeOddSize(width);
    map.height = NormalizeOddSize(height);
    map.tiles.assign(static_cast<size_t>(map.height), std::vector<TileType>(static_cast<size_t>(map.width), TileType::Wall));

    std::mt19937 rng(seed.has_value() ? *seed : std::random_device{}());

    const CellPos startCell{1, 1};
    map.start = startCell;
    map.tiles[static_cast<size_t>(startCell.y)][static_cast<size_t>(startCell.x)] = TileType::Floor;

    std::vector<CellPos> stack;
    stack.push_back(startCell);

    std::array<Direction, 4> directions{{
        {2, 0},
        {-2, 0},
        {0, 2},
        {0, -2}
    }};

    while (!stack.empty()) {
        const CellPos current = stack.back();
        std::shuffle(directions.begin(), directions.end(), rng);

        bool carved = false;
        for (const Direction& dir : directions) {
            const int nextX = current.x + dir.dx;
            const int nextY = current.y + dir.dy;

            if (!IsCarvableCell(map, nextX, nextY)) {
                continue;
            }

            if (map.tiles[static_cast<size_t>(nextY)][static_cast<size_t>(nextX)] != TileType::Wall) {
                continue;
            }

            const int betweenX = current.x + dir.dx / 2;
            const int betweenY = current.y + dir.dy / 2;

            map.tiles[static_cast<size_t>(betweenY)][static_cast<size_t>(betweenX)] = TileType::Floor;
            map.tiles[static_cast<size_t>(nextY)][static_cast<size_t>(nextX)] = TileType::Floor;
            stack.push_back(CellPos{nextX, nextY});
            carved = true;
            break;
        }

        if (!carved) {
            stack.pop_back();
        }
    }

    // Create extra random connectors to reduce linearity and add alternative routes.
    std::vector<CellPos> connectorCandidates;
    for (int y = 1; y < map.height - 1; ++y) {
        for (int x = 1; x < map.width - 1; ++x) {
            if (map.tiles[static_cast<size_t>(y)][static_cast<size_t>(x)] != TileType::Wall) {
                continue;
            }

            const bool leftFloor = IsFloorTile(map, x - 1, y);
            const bool rightFloor = IsFloorTile(map, x + 1, y);
            const bool upFloor = IsFloorTile(map, x, y - 1);
            const bool downFloor = IsFloorTile(map, x, y + 1);

            const bool horizontalConnector = leftFloor && rightFloor && !upFloor && !downFloor;
            const bool verticalConnector = upFloor && downFloor && !leftFloor && !rightFloor;

            if (horizontalConnector || verticalConnector) {
                connectorCandidates.push_back(CellPos{x, y});
            }
        }
    }

    std::shuffle(connectorCandidates.begin(), connectorCandidates.end(), rng);
    const int connectorCount = static_cast<int>(connectorCandidates.size());
    const int extraConnections = std::max(1, connectorCount / 5);
    for (int i = 0; i < extraConnections && i < connectorCount; ++i) {
        const CellPos& connector = connectorCandidates[static_cast<size_t>(i)];
        map.tiles[static_cast<size_t>(connector.y)][static_cast<size_t>(connector.x)] = TileType::Floor;
    }

    // Choose an exit by taking the farthest reachable floor tile from the start (BFS distance).
    std::vector<std::vector<int>> distance(static_cast<size_t>(map.height), std::vector<int>(static_cast<size_t>(map.width), -1));
    std::queue<CellPos> bfs;
    bfs.push(startCell);
    distance[static_cast<size_t>(startCell.y)][static_cast<size_t>(startCell.x)] = 0;

    CellPos farthest = startCell;
    int farthestDistance = 0;

    static const std::array<Direction, 4> cardinal{{
        {1, 0},
        {-1, 0},
        {0, 1},
        {0, -1}
    }};

    while (!bfs.empty()) {
        const CellPos current = bfs.front();
        bfs.pop();

        const int currentDistance = distance[static_cast<size_t>(current.y)][static_cast<size_t>(current.x)];
        if (currentDistance > farthestDistance) {
            farthestDistance = currentDistance;
            farthest = current;
        }

        for (const Direction& dir : cardinal) {
            const int nx = current.x + dir.dx;
            const int ny = current.y + dir.dy;

            if (!InBounds(map, nx, ny)) {
                continue;
            }

            if (distance[static_cast<size_t>(ny)][static_cast<size_t>(nx)] >= 0) {
                continue;
            }

            if (map.tiles[static_cast<size_t>(ny)][static_cast<size_t>(nx)] == TileType::Wall) {
                continue;
            }

            distance[static_cast<size_t>(ny)][static_cast<size_t>(nx)] = currentDistance + 1;
            bfs.push(CellPos{nx, ny});
        }
    }

    map.exit = farthest;
    map.tiles[static_cast<size_t>(map.start.y)][static_cast<size_t>(map.start.x)] = TileType::Start;
    map.tiles[static_cast<size_t>(map.exit.y)][static_cast<size_t>(map.exit.x)] = TileType::Exit;

    return map;
}

bool DungeonGenerator::IsWalkable(const DungeonMap& map, int x, int y) {
    if (!InBounds(map, x, y)) {
        return false;
    }

    return map.tiles[static_cast<size_t>(y)][static_cast<size_t>(x)] != TileType::Wall;
}

std::string DungeonGenerator::ToDebugString(const DungeonMap& map) {
    std::ostringstream out;

    for (int y = 0; y < map.height; ++y) {
        for (int x = 0; x < map.width; ++x) {
            const TileType tile = map.tiles[static_cast<size_t>(y)][static_cast<size_t>(x)];
            switch (tile) {
            case TileType::Wall:
                out << '#';
                break;
            case TileType::Floor:
                out << '.';
                break;
            case TileType::Start:
                out << 'S';
                break;
            case TileType::Exit:
                out << 'E';
                break;
            }
        }
        out << '\n';
    }

    return out.str();
}

int DungeonGenerator::NormalizeOddSize(int size) {
    int clamped = std::max(5, size);
    if (clamped % 2 == 0) {
        clamped += 1;
    }
    return clamped;
}

bool DungeonGenerator::InBounds(const DungeonMap& map, int x, int y) {
    return x >= 0 && y >= 0 && x < map.width && y < map.height;
}

} // namespace dungeon
