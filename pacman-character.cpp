//include all the file ise to make pacman character


struct Pacman {
    Vector2 position;   // Current screen pixel coordinates (x, y)
    Vector2 direction;  // Movement direction vector (e.g., {1, 0} for Right)
    float speed;        // Movement speed in pixels per frame
    float radius;       // Circle size in pixels for drawing and collision
};


Vector2 GetTileCenter(TileMap map, int row, int col) {
    return Vector2{
        (float)(map.screenOffsetX + col * map.tileSize + map.tileSize / 2),
        (float)(map.screenOffsetY + row * map.tileSize + map.tileSize / 2)
    };
}


bool CanMove(TileMap map, Vector2 candidatePos, float radius) {
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
            if (map.grid[row][col] == 1) { // Wall tile
                return false;
            }
        }
    }
    return true;
}


Vector2 requestedDir = pacman.direction;

if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) requestedDir = Vector2{ 1, 0 };
if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) requestedDir = Vector2{ -1, 0 };
if (IsKeyDown(KEY_UP)    || IsKeyDown(KEY_W)) requestedDir = Vector2{ 0, -1 };
if (IsKeyDown(KEY_DOWN)  || IsKeyDown(KEY_S)) requestedDir = Vector2{ 0, 1 };


Vector2 nextPos = {
    pacman.position.x + requestedDir.x * pacman.speed,
    pacman.position.y + requestedDir.y * pacman.speed
};

if (CanMove(pacmanMap, nextPos, pacman.radius)) {
    pacman.direction = requestedDir;
    pacman.position = nextPos;
} else {
    Vector2 continuePos = {
        pacman.position.x + pacman.direction.x * pacman.speed,
        pacman.position.y + pacman.direction.y * pacman.speed
    };
    if (CanMove(pacmanMap, continuePos, pacman.radius)) {
        pacman.position = continuePos;
    }
}

DrawCircleV(pacman.position, pacman.radius, YELLOW);

typedef struct Vector2 {
    float x; // Horizontal component
    float y; // Vertical component
} Vector2;



// vector2 is raylib word which is used to define its pisition in 2 d graph
