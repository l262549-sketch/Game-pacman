#include "raylib.h"

// Map Dimensions and Tile Sizing
const int MAP_ROWS = 21;
const int MAP_COLS = 19;
const int TILE_SIZE = 30;

// Plain C-style struct to hold map configuration and layout state
struct TileMap {
    int grid[MAP_ROWS][MAP_COLS];
    int tileSize;
    int screenOffsetX;
    int screenOffsetY;
};

// Struct to store Pac-Man's state
struct Pacman {
    Vector2 position;   // Pixel coordinates (x, y)
    Vector2 direction;  // Velocity vector (e.g., {1, 0} for right)
    float speed;        // Movement speed in pixels per frame
    float radius;       // Collision and drawing radius
};

// Function to construct and return initial map data
TileMap CreatePacmanMap() {
    TileMap map;
    map.tileSize = TILE_SIZE;
    map.screenOffsetX = 20;
    map.screenOffsetY = 20;

    // Grid Encoding:
    // 0 = Empty Path
    // 1 = Outer Wall / Obstacle
    // 2 = Small Pellet
    // 3 = Power Pellet
    // 4 = Ghost House Door
    int layout[MAP_ROWS][MAP_COLS] = 
    {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
        {1,3,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,3,1},
        {1,2,1,1,1,1,1,1,2,1,2,1,1,1,2,1,1,2,1},
        {1,2,1,2,2,2,2,1,2,1,2,1,1,1,2,1,1,2,1},
        {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1},
        {1,2,1,1,2,1,2,1,1,1,1,1,2,1,2,1,1,2,1},
        {1,2,2,2,2,1,2,2,2,1,2,2,2,1,2,2,2,2,1},
        {1,1,1,1,2,1,1,1,0,1,0,1,1,1,2,1,1,1,1},
        {0,0,0,1,2,1,0,0,0,0,0,0,0,1,2,1,0,0,0},
        {1,1,1,1,2,1,0,1,1,4,1,1,0,1,2,1,1,1,1},
        {0,0,0,0,2,0,0,1,0,0,0,1,0,0,2,0,0,0,0},
        {-1,1,1,1,2,1,0,1,1,1,1,1,0,1,2,1,1,1,-1},
        {0,0,0,1,2,1,0,0,0,0,0,0,0,1,2,1,0,0,0},
        {1,1,1,1,2,1,0,1,1,1,1,1,0,1,2,1,1,1,1},
        {1,2,2,2,2,2,2,2,2,1,2,2,2,2,2,2,2,2,1},
        {1,2,1,1,2,1,1,1,2,1,2,1,1,1,2,1,1,2,1},
        {1,3,2,1,2,2,2,2,2,0,2,2,2,2,2,1,2,3,1},
        {1,1,2,1,2,1,2,1,1,1,1,1,2,1,2,1,2,1,1},
        {1,2,2,2,2,1,2,2,2,1,2,2,2,1,2,2,2,2,1},
        {1,2,1,1,1,1,1,1,2,1,2,1,1,1,1,1,1,2,1},
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
    };

    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            map.grid[r][c] = layout[r][c];
        }
    }

    return map;
}

// Procedural rendering function
void DrawMap(TileMap map) {
    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            int x = map.screenOffsetX + (col * map.tileSize);
            int y = map.screenOffsetY + (row * map.tileSize);
            int tileType = map.grid[row][col];

            switch (tileType) {
                case 1: // Wall: Dark blue filled box with light blue border
                    DrawRectangle(x, y, map.tileSize, map.tileSize, DARKBLUE);
                    DrawRectangleLines(x, y, map.tileSize, map.tileSize, BLUE);
                    break;

                case 2: // Small Pellet: Centered small dot
                    DrawCircle(x + map.tileSize / 2, y + map.tileSize / 2, 3, RAYWHITE);
                    break;

                case 3: // Power Pellet: Centered large golden orb
                    DrawCircle(x + map.tileSize / 2, y + map.tileSize / 2, 7, GOLD);
                    break;

                case 4: // Ghost Gate: Pink horizontal strip
                    DrawRectangle(x, y + map.tileSize / 3, map.tileSize, map.tileSize / 3, PINK);
                    break;

                case 0: // Empty Path
                default:
                    break;
            }
        }
    }
}

// Converts grid (row, col) into pixel center coordinates
Vector2 GetTileCenter(TileMap map, int row, int col) {
    return Vector2{
        (float)(map.screenOffsetX + col * map.tileSize + map.tileSize / 2),
        (float)(map.screenOffsetY + row * map.tileSize + map.tileSize / 2)
    };
}

// Checks if a candidate position will overlap with a wall tile (1)
bool CanMove(TileMap map, Vector2 candidatePos, float radius) {
    // Check 4 bounding box points around Pac-Man's circular body
    float checkPoints[4][2] = {
        { candidatePos.x - radius + 2, candidatePos.y - radius + 2 }, // Top-Left
        { candidatePos.x + radius - 2, candidatePos.y - radius + 2 }, // Top-Right
        { candidatePos.x - radius + 2, candidatePos.y + radius - 2 }, // Bottom-Left
        { candidatePos.x + radius - 2, candidatePos.y + radius - 2 }  // Bottom-Right
    };

    for (int i = 0; i < 4; i++) {
        int col = (int)((checkPoints[i][0] - map.screenOffsetX) / map.tileSize);
        int row = (int)((checkPoints[i][1] - map.screenOffsetY) / map.tileSize);

        if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS) {
            if (map.grid[row][col] == 1) { // Tile 1 is a Wall
                return false;
            }
        }
    }
    return true;
}

int main() 
{
    const int screenWidth = (MAP_COLS * TILE_SIZE) + 40;
    const int screenHeight = (MAP_ROWS * TILE_SIZE) + 40;

    InitWindow(screenWidth, screenHeight, "Pac-Man Map - Raylib (Procedural)");
    SetTargetFPS(60);

    TileMap pacmanMap = CreatePacmanMap();

    // Spawn Pac-Man at Row 16, Column 9 (Empty path in bottom section)
    Pacman pacman;
    pacman.position = GetTileCenter(pacmanMap, 16, 9);
    pacman.direction = Vector2{ 0, 0 };
    pacman.speed = 2.0f;
    pacman.radius = 12.0f; // 24px diameter fits cleanly inside 30px tiles

    // Game loop
    while (!WindowShouldClose()) {
        // --- 1. INPUT ---
        Vector2 requestedDir = pacman.direction;

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) requestedDir = Vector2{ 1, 0 };
        if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) requestedDir = Vector2{ -1, 0 };
        if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) requestedDir = Vector2{ 0, -1 };
        if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) requestedDir = Vector2{ 0, 1 };

        // --- 2. UPDATE POSITION & WALL COLLISION ---
        // Attempt to move in the newly requested direction first
        Vector2 nextPos = {
            pacman.position.x + requestedDir.x * pacman.speed,
            pacman.position.y + requestedDir.y * pacman.speed
        };

        if (CanMove(pacmanMap, nextPos, pacman.radius)) {
            pacman.direction = requestedDir;
            pacman.position = nextPos;
        } 
        else {
            // If requested direction hits a wall, keep moving in current direction if possible
            Vector2 continuePos = {
                pacman.position.x + pacman.direction.x * pacman.speed,
                pacman.position.y + pacman.direction.y * pacman.speed
            };
            if (CanMove(pacmanMap, continuePos, pacman.radius)) {
                pacman.position = continuePos;
            }
        }

        // --- 3. RENDERING ---
        BeginDrawing();
            ClearBackground(BLACK);
            
            DrawMap(pacmanMap);

            // Draw Pac-Man as a yellow circle
            DrawCircleV(pacman.position, pacman.radius, YELLOW);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
