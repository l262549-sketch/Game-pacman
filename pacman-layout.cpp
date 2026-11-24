#include "raylib.h"
#include <cmath>

// Map Dimensions and Tile Sizing
const int MAP_ROWS = 31;
const int MAP_COLS = 28;
const int TILE_SIZE = 22;

// Plain C-style struct to hold map configuration and layout state
struct TileMap {
    int grid[MAP_ROWS][MAP_COLS];
    int tileSize;
    int screenOffsetX;
    int screenOffsetY;
};

// Function to construct and return initial map data
TileMap CreatePacmanMap() {
    TileMap map;
    map.tileSize = TILE_SIZE;
    map.screenOffsetX = 20;
    map.screenOffsetY = 50;

    // Grid Encoding:
    // 0 = Empty Path
    // 1 = Outer Wall / Obstacle
    // 2 = Small Pellet
    // 3 = Power Pellet
    // 4 = Ghost House Door
    int layout[MAP_ROWS][MAP_COLS] = 
    {
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}, // Row 0
        {1,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1}, // Row 1
        {1,2,1,1,2,1,1,1,1,2,1,1,1,1,1,1,1,1,2,1,1,1,1,2,1,1,2,1}, // Row 2
        {1,2,1,1,2,1,1,1,1,2,1,1,1,1,1,1,1,1,2,1,1,1,1,2,1,1,2,1}, // Row 3
        {1,2,1,1,2,1,1,1,1,2,1,1,2,2,2,2,1,1,2,1,1,1,1,2,1,1,2,1}, // Row 4
        {1,2,1,1,2,2,2,2,2,2,1,1,2,1,1,2,1,1,2,2,2,2,2,2,1,1,2,1}, // Row 5
        {1,2,1,1,1,1,2,1,1,2,1,1,2,1,1,2,1,1,2,1,1,2,1,1,1,1,2,1}, // Row 6
        {1,2,1,1,1,1,2,1,1,2,1,1,2,1,1,2,1,1,2,1,1,2,1,1,1,1,2,1}, // Row 7
        {1,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,1}, // Row 8
        {1,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,1}, // Row 9
        {0,0,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,0,0}, // Row 10
        {0,0,1,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,1,0,0}, // Row 11
        {1,1,1,2,1,1,2,1,1,2,1,1,1,0,0,1,1,1,2,1,1,2,1,1,2,1,1,1}, // Row 12 (Ghost House entrance)
        {2,2,2,2,1,1,2,1,1,2,1,0,0,0,0,0,0,1,2,1,1,2,1,1,2,2,2,2}, // Row 13 (Upper Warp Tunnel)
        {1,1,1,1,1,1,2,2,2,2,1,0,0,0,0,0,0,1,2,2,2,2,1,1,1,1,1,1}, // Row 14
        {1,1,1,1,1,1,2,1,1,2,1,0,0,0,0,0,0,1,2,1,1,2,1,1,1,1,1,1}, // Row 15
        {2,2,2,2,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,2,2,2,2}, // Row 16 (Lower Warp Tunnel)
        {1,1,1,2,1,1,2,1,1,2,2,2,2,2,2,2,2,2,2,1,1,2,1,1,2,1,1,1}, // Row 17
        {0,0,1,2,2,2,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,2,2,2,1,0,0}, // Row 18
        {0,0,1,2,1,1,2,1,1,1,1,1,2,1,1,2,1,1,1,1,1,2,1,1,2,1,0,0}, // Row 19
        {0,0,1,2,1,1,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,1,1,2,1,0,0}, // Row 20
        {0,0,1,2,1,1,1,1,1,2,1,1,2,1,1,2,1,1,2,1,1,1,1,1,2,1,0,0}, // Row 21
        {1,1,1,2,1,1,1,1,1,2,1,1,2,2,2,2,1,1,2,1,1,1,1,1,2,1,1,1}, // Row 22 (Pac-Man spawn: Row 22, Col 13)
        {1,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,1}, // Row 23
        {1,2,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,2,1}, // Row 24
        {1,2,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,2,1}, // Row 25
        {1,2,1,1,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,1,1,2,1}, // Row 26
        {1,2,1,1,2,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,2,1,1,2,1}, // Row 27
        {1,2,1,1,2,1,1,1,1,1,1,1,2,1,1,2,1,1,1,1,1,1,1,2,1,1,2,1}, // Row 28
        {1,2,2,2,2,2,2,2,2,2,2,2,2,1,1,2,2,2,2,2,2,2,2,2,2,2,2,1}, // Row 29
        {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}  // Row 30
    };
 


    //this code dx
    // Copy array elements into struct
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
            // Convert grid array indices (row, col) to screen pixel coordinates (x, y)
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

                case 0: // Empty Path: Do nothing
                default:
                    break;
            }
        }
    }
}

int main() 
{
    // Set screen size based on tile map size plus padding
    const int screenWidth = (MAP_COLS * TILE_SIZE) + 40;
    const int screenHeight = (MAP_ROWS * TILE_SIZE) + 40;

    InitWindow(screenWidth, screenHeight, "Pac-Man Map - Raylib (Procedural)");
    SetTargetFPS(60);

    TileMap pacmanMap = CreatePacmanMap();

    // Game loop
    while (!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(BLACK);
            DrawMap(pacmanMap);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
