#include "raylib.h"
#include <cmath>
#include <algorithm>
// Map Dimensions and Tile Sizing
const int MAP_ROWS = 31;
const int MAP_COLS = 28;
const int TILE_SIZE = 22;
// extra room under the map for score, lives and level
const int hudheight = 50;
// ghost house. the door is two tiles wide (row 12, cols 13 and 14),
// ghosts use col 13 to go in and out
const int doorrow = 12;
const int doorcol = 13;
const int homerow = 14;  // middle of the house, eyes come back here
const int homecol = 13;
const int exitrow = 11;  // corridor right above the door
const int tunnelrowa = 13;
const int tunnelrowb = 16;

const float frametime = 1.0f / 60.0f; // game is locked to 60 fps

struct Tile
{
    int row;
    int col;
};

// Plain C-style struct to hold map configuration and layout state
struct TileMap
{
    int grid[MAP_ROWS][MAP_COLS];
    int tileSize;
    int screenOffsetX;
    int screenOffsetY;
};

// Struct to store Pac-Man's state
struct Pacman
{
    Vector2 position;  // Pixel coordinates (x, y)
    Vector2 direction; // Velocity vector (e.g., {1, 0} for right)
    Vector2 facing;    // last direction he actually moved in, ghosts need this
    float speed;       // Movement speed in pixels per frame
    float radius;      // Collision and drawing radius
};

enum GhostName
{
    blinky, pinky, inky, clyde, ghostcount
};
enum GhostState
{
    inhouse, leaving, roaming, eyes
};
enum GameState
{
    ready, playing, eatfreeze, dying, levelclear, gameover
};

struct Ghost
{
    GhostName name;
    GhostState state;
    bool scared;      // blue and edible
    Vector2 pos;      // pixel position
    int row, col;     // last tile centre it reached
    int dr, dc;       // direction it is moving in
    int nextrow, nextcol; // tile it is heading to
    Color color;
    Tile corner;      // scatter corner
};

// everything that gets harder with the level number
struct LevelConfig
{
    float pacspeed, ghostspeed, scaredspeed, tunnelspeed, eyespeed;
    float scaredtime;     // seconds ghosts stay blue, 0 means they only turn around
    float scattertime[4];
    float chasetime[3];   // the 4th chase never ends
    int inkydots;         // dots eaten before inky leaves
    int clydedots;        // dots eaten before clyde leaves
    float idlelimit;      // seconds without eating before the next ghost is pushed out
};
struct Game
{
    TileMap map;
    Pacman pac;
    Ghost ghosts[ghostcount];
    LevelConfig cfg;
    int eyesfield[MAP_ROWS][MAP_COLS]; // steps to the house, used by eyes
    int level, lives, score, nextlife;
    int pelletsleft, dotseaten;
    GameState state;
    float statetimer;
    bool chasing;
    int phase;
    float phasetimer, scaredtimer;
    int chain;       // 0..3 -> 200, 400, 800, 1600
    float idletimer;
    int eatenindex;
    int popupvalue;
    Vector2 popuppos;
    float animtime;
};

// Function to construct and return initial map data
TileMap CreatePacmanMap()
{
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
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, // Row 0
        {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1}, // Row 1
        {1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1}, // Row 2
        {1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1}, // Row 3
        {1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 2, 2, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1}, // Row 4
        {1, 2, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 2, 2, 2, 2, 1, 1, 2, 1}, // Row 5
        {1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1}, // Row 6
        {1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1}, // Row 7
        {1, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 1}, // Row 8
        {1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1}, // Row 9
        {0, 0, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 0, 0}, // Row 10
        {0, 0, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 0, 0}, // Row 11
        {1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1, 0, 0, 1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1}, // Row 12 (Ghost House entrance)
        {2, 2, 2, 2, 1, 1, 2, 1, 1, 2, 1, 0, 0, 0, 0, 0, 0, 1, 2, 1, 1, 2, 1, 1, 2, 2, 2, 2}, // Row 13 (Upper Warp Tunnel)
        {1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 0, 0, 0, 0, 0, 0, 1, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1}, // Row 14
        {1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 0, 0, 0, 0, 0, 0, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1}, // Row 15
        {2, 2, 2, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 2, 2}, // Row 16 (Lower Warp Tunnel)
        {1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1}, // Row 17
        {0, 0, 1, 2, 2, 2, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 0, 0}, // Row 18
        {0, 0, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 0, 0}, // Row 19
        {0, 0, 1, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 1, 0, 0}, // Row 20
        {0, 0, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 0, 0}, // Row 21
        {1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 2, 2, 2, 2, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1}, // Row 22 (Pac-Man spawn: Row 22, Col 13)
        {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1}, // Row 23
        {1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1}, // Row 24
        {1, 2, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 2, 1}, // Row 25
        {1, 2, 1, 1, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 1, 1, 2, 1}, // Row 26
        {1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1}, // Row 27
        {1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 2, 1}, // Row 28
        {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1}, // Row 29
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}  // Row 30
    };

    for (int r = 0; r < MAP_ROWS; r++)
    {
        for (int c = 0; c < MAP_COLS; c++)
        {
            map.grid[r][c] = layout[r][c];
        }
    }

    // the layout had no door and no power pellets, so they get placed here
    map.grid[doorrow][13] = 4;
    map.grid[doorrow][14] = 4;
    map.grid[3][1] = 3;
    map.grid[3][26] = 3;
    map.grid[23][1] = 3;
    map.grid[23][26] = 3;

    return map;
}

// Procedural rendering function
// flash is only used for the white blink when a level is cleared
void DrawMap(TileMap map, bool flash)
{
    Color wallfill = flash ? RAYWHITE : DARKBLUE;

    for (int row = 0; row < MAP_ROWS; row++)
    {
        for (int col = 0; col < MAP_COLS; col++)
        {
            int x = map.screenOffsetX + (col * map.tileSize);
            int y = map.screenOffsetY + (row * map.tileSize);
            int tileType = map.grid[row][col];

            switch (tileType)
            {
            case 1: // Wall: Dark blue filled box with light blue border
                DrawRectangle(x, y, map.tileSize, map.tileSize, wallfill);
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
Vector2 GetTileCenter(TileMap map, int row, int col)
{
    return Vector2{
        (float)(map.screenOffsetX + col * map.tileSize + map.tileSize / 2),
        (float)(map.screenOffsetY + row * map.tileSize + map.tileSize / 2) };
}

// Checks if a candidate position will overlap with a wall tile (1), void (-1) or the ghost door (4)
bool CanMove(TileMap map, Vector2 candidatePos, float radius)
{
    // Check 4 bounding box points around Pac-Man's circular body
    // Tight 0.5px margin prevents lateral slack and wall jitter
    float margin = 0.5f;
    float checkPoints[4][2] = {
        {candidatePos.x - radius + margin, candidatePos.y - radius + margin}, // Top-Left
        {candidatePos.x + radius - margin, candidatePos.y - radius + margin}, // Top-Right
        {candidatePos.x - radius + margin, candidatePos.y + radius - margin}, // Bottom-Left
        {candidatePos.x + radius - margin, candidatePos.y + radius - margin}  // Bottom-Right
    };

    for (int i = 0; i < 4; i++)
    {
        int col = (int)((checkPoints[i][0] - map.screenOffsetX) / map.tileSize);
        int row = (int)((checkPoints[i][1] - map.screenOffsetY) / map.tileSize);

        // If outside the left/right edges, only allow movement inside the tunnels (rows 13 and 16)
        if (checkPoints[i][0] < map.screenOffsetX || col >= MAP_COLS)
        {
            if (row != 13 && row != 16)
                return false;
            continue;
        }
        if (row >= 0 && row < MAP_ROWS && col >= 0 && col < MAP_COLS)
        {
            int t = map.grid[row][col];
            if (t == 1 || t == -1 || t == 4) // wall, void, ghost door
            {
                return false;
            }
        }
    }
    return true;
}
int wrapcol(int c) // helpers
{
    return ((c % MAP_COLS) + MAP_COLS) % MAP_COLS; //for tunnels, +ve on rigt and -ve on left
}

Tile postotile(TileMap& map, Vector2 p)
{
    int col = (int)floorf((p.x - map.screenOffsetX) / map.tileSize);
    int row = (int)floorf((p.y - map.screenOffsetY) / map.tileSize);
    row = std::max(0, std::min(MAP_ROWS - 1, row));
    return Tile{ row, wrapcol(col) };
}
// difficulty per level
LevelConfig getlevelconfig(int level)
{
    LevelConfig c;
    int l = level - 1;
    c.pacspeed = 2.0f;
    c.ghostspeed = fminf(1.9f, 1.6f + 0.05f * l); // starts with 80% of pacman's speed and ends nearly at 95%
    c.scaredspeed = c.ghostspeed * 0.55f;
    c.tunnelspeed = c.ghostspeed * 0.55f;
    c.eyespeed = 4.0f;
    c.scaredtime = fmaxf(0.0f, 6.0f - 0.5f * l); // 6s reduced to 0s at level 13
    const float basescatter[4] = { 7.0f, 7.0f, 5.0f, 5.0f };
    const float basechase[3] = { 20.0f, 20.0f, 20.0f };
    float scatterscale = fmaxf(0.25f, 1.0f - 0.12f * l); // less scatter every level
    float chasescale = 1.0f + 0.15f * l;                 // more chase every level
    for (int i = 0; i < 4; i++)
    {
        c.scattertime[i] = basescatter[i] * scatterscale;
    }
    for (int i = 0; i < 3; i++)
    {
        c.chasetime[i] = basechase[i] * chasescale;
    }
    c.inkydots = std::max(0, 30 - 10 * l);
    c.clydedots = std::max(0, 60 - 20 * l);
    c.idlelimit = fmaxf(1.5f, 4.0f - 0.25f * l);
    return c;
}
// length of a scatter/chase phase in seconds, -1 means it never ends
float phaselength(const LevelConfig& c, int phase)
{
    if (phase >= 7)
    {
        return -1.0f;
    }
    if (phase % 2 == 0)
    {
        return c.scattertime[phase / 2];
    }
    return c.chasetime[phase / 2];
}
// ghost movement 

const int dirs[4][2] = { {-1, 0}, {0, -1}, {1, 0}, {0, 1} }; // up, left, down, right
void setdir(Ghost& g, int dr, int dc)
{
    g.dr = dr;
    g.dc = dc;
    g.nextrow = g.row + dr;
    g.nextcol = g.col + dc;
}

// col can be -1 or MAP_COLS here because of the tunnels
bool ghostcanenter(TileMap& map, const Ghost& g, int r, int c)
{
    if (r < 0 || r >= MAP_ROWS)
        return false;
    int t = map.grid[r][wrapcol(c)];
    if (t == 1 || t == -1)
        return false;
    if (t == 4)
        return g.state == leaving || g.state == eyes; // only ghost may use the door
    return true;
}
void reverseghost(Ghost& g)
{
    if (g.state != roaming)
        return;
    g.dr = -g.dr;
    g.dc = -g.dc;
    g.nextrow = g.row;
    g.nextcol = g.col;
}
// where each ghost is trying to go
Tile gettarget(Game& game, const Ghost& g)
{
    if (!game.chasing)
        return g.corner;

    Tile pac = postotile(game.map, game.pac.position);
    int fr = (int)game.pac.facing.y;
    int fc = (int)game.pac.facing.x;

    if (g.name == pinky) // aims 4 tiles in front of pacman
        return Tile{ pac.row + 4 * fr, pac.col + 4 * fc };

    if (g.name == inky) // mirrors blinky around a spot 2 tiles in front of pacman
    {
        Tile ahead = { pac.row + 2 * fr, pac.col + 2 * fc };
        const Ghost& red = game.ghosts[blinky];
        return Tile{ 2 * ahead.row - red.row, 2 * ahead.col - red.col };
    }
    if (g.name == clyde) // chases when far away, goes home when close
    {
        int dr = g.row - pac.row;
        int dc = g.col - pac.col;
        if (dr * dr + dc * dc >= 64)
            return pac;
        return g.corner;
    }
    return pac; // blinky goes straight for pac-man
}
// picking direction when a roaming ghost reaches tile
void choosedir(Game& game, Ghost& g)
{
    int options[4];
    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        if (dirs[i][0] == -g.dr && dirs[i][1] == -g.dc) // no turning back
            continue;
        if (!ghostcanenter(game.map, g, g.row + dirs[i][0], g.col + dirs[i][1]))
            continue;
        options[count++] = i;
    }
    if (count == 0) // dead end
    {
        setdir(g, -g.dr, -g.dc);
        return;
    }
    int pick = options[0];
    if (g.scared)
    {
        pick = options[GetRandomValue(0, count - 1)];
    }
    else
    {
        Tile target = gettarget(game, g);
        int best = 1000000;
        for (int k = 0; k < count; k++)
        {
            int i = options[k];
            int dr = (g.row + dirs[i][0]) - target.row;
            int dc = (g.col + dirs[i][1]) - target.col;
            int d = dr * dr + dc * dc;
            if (d < best)
            {
                best = d;
                pick = i;
            }
        }
    }
    setdir(g, dirs[pick][0], dirs[pick][1]);
}
// fills eyesfield with the number of steps from every tile to the middle of the house
void buildeyesfield(Game& game)
{
    for (int r = 0; r < MAP_ROWS; r++)
    {
        for (int c = 0; c < MAP_COLS; c++)
        {
            game.eyesfield[r][c] = -1;
        }
    }
    static int qr[MAP_ROWS * MAP_COLS];
    static int qc[MAP_ROWS * MAP_COLS];
    int head = 0, tail = 0;
    game.eyesfield[homerow][homecol] = 0;
    qr[tail] = homerow;
    qc[tail] = homecol;
    tail++;
    while (head < tail)
    {
        int r = qr[head];
        int c = qc[head];
        head++;
        for (int i = 0; i < 4; i++)
        {
            int nr = r + dirs[i][0];
            int nc = wrapcol(c + dirs[i][1]);
            if (nr < 0 || nr >= MAP_ROWS)
                continue;
            int t = game.map.grid[nr][nc];
            if (t == 1 || t == -1)
                continue;
            if (game.eyesfield[nr][nc] != -1)
                continue;
            game.eyesfield[nr][nc] = game.eyesfield[r][c] + 1;
            qr[tail] = nr;
            qc[tail] = nc;
            tail++;
        }
    }
}
// inside the house: slide sideways to the door column, then go straight up
void leavestep(Ghost& g)
{
    if (g.col < doorcol)
        setdir(g, 0, 1);
    else if (g.col > doorcol)
        setdir(g, 0, -1);
    else
        setdir(g, -1, 0);
}

// runs every time a ghost lands exactly on a tile centre
void onarrive(Game& game, Ghost& g)
{
    if (g.state == roaming)
    {
        choosedir(game, g);
    }
    else if (g.state == leaving)
    {
        if (g.row == exitrow)
        {
            g.state = roaming;
            choosedir(game, g);
        }
        else
        {
            leavestep(g);
        }
    }
    else if (g.state == eyes)
    {
        if (g.row == homerow && g.col == homecol) // made it home, come back to life
        {
            g.state = leaving;
            g.scared = false;
            leavestep(g);
            return;
        }
        int bestdir = -1;
        int bestval = 1000000;
        for (int i = 0; i < 4; i++)
        {
            int nr = g.row + dirs[i][0];
            int nc = g.col + dirs[i][1];
            if (!ghostcanenter(game.map, g, nr, nc))
                continue;
            int f = game.eyesfield[nr][wrapcol(nc)];
            if (f >= 0 && f < bestval)
            {
                bestval = f;
                bestdir = i;
            }
        }
        if (bestdir >= 0)
            setdir(g, dirs[bestdir][0], dirs[bestdir][1]);
        else
            setdir(g, -g.dr, -g.dc);
    }
}

float ghostspeed(Game& game, const Ghost& g)
{
    if (g.state == eyes)
        return game.cfg.eyespeed;
    if (g.state == leaving)
        return game.cfg.ghostspeed * 0.6f;

    float speed = game.cfg.ghostspeed;
    if (g.scared)
        speed = game.cfg.scaredspeed;

    bool intunnel = (g.row == tunnelrowa || g.row == tunnelrowb) && (g.col <= 3 || g.col >= 24);
    if (intunnel)
        speed = fminf(speed, game.cfg.tunnelspeed);
    return speed;
}

void moveghost(Game& game, Ghost& g)
{
    if (g.state == inhouse)
        return;

    float speed = ghostspeed(game, g);
    Vector2 target = GetTileCenter(game.map, g.nextrow, g.nextcol);
    float dx = target.x - g.pos.x;
    float dy = target.y - g.pos.y;
    float dist = sqrtf(dx * dx + dy * dy);

    if (dist <= speed)
    {
        g.pos = target;
        g.row = g.nextrow;
        g.col = g.nextcol;
        if (g.col < 0) // came out of the left side of a tunnel
        {
            g.col += MAP_COLS;
            g.pos.x += MAP_COLS * TILE_SIZE;
        }
        else if (g.col >= MAP_COLS)
        {
            g.col -= MAP_COLS;
            g.pos.x -= MAP_COLS * TILE_SIZE;
        }
        onarrive(game, g);
    }
    else
    {
        g.pos.x += dx / dist * speed;
        g.pos.y += dy / dist * speed;
    }
}

// lets the waiting ghosts out one at a time: pinky, inky, clyde
void releaseghosts(Game& game)
{
    for (int i = pinky; i < ghostcount; i++)
    {
        Ghost& g = game.ghosts[i];
        if (g.state != inhouse)
            continue;

        int needed = 0;
        if (i == inky)
            needed = game.cfg.inkydots;
        if (i == clyde)
            needed = game.cfg.clydedots;

        if (game.dotseaten >= needed || game.idletimer >= game.cfg.idlelimit)
        {
            g.state = leaving;
            leavestep(g);
            game.idletimer = 0.0f;
        }
        break; // only the first one waiting gets checked
    }
}

// switches between scatter and chase, ghosts turn around on every switch
void updatephase(Game& game)
{
    float length = phaselength(game.cfg, game.phase);
    if (length < 0.0f)
        return;

    game.phasetimer += frametime;
    if (game.phasetimer >= length)
    {
        game.phasetimer = 0.0f;
        game.phase++;
        game.chasing = (game.phase % 2 == 1);
        for (int i = 0; i < ghostcount; i++)
            reverseghost(game.ghosts[i]);
    }
}

// called when pac-man eats a power pellet
void startscared(Game& game)
{
    game.chain = 0;
    if (game.cfg.scaredtime <= 0.0f) // late levels, ghosts only turn around
    {
        for (int i = 0; i < ghostcount; i++)
            reverseghost(game.ghosts[i]);
        return;
    }

    game.scaredtimer = game.cfg.scaredtime;
    for (int i = 0; i < ghostcount; i++)
    {
        Ghost& g = game.ghosts[i];
        if (g.state == eyes)
            continue;
        g.scared = true;
        reverseghost(g);
    }
}
// game flow
void addscore(Game& game, int points)
{
    game.score += points;
    if (game.score >= game.nextlife)
    {
        game.lives++;
        game.nextlife += 10000;
    }
}
void resetghosts(Game& game)
{
    const Tile starts[ghostcount] = { {11, 13}, {14, 13}, {14, 11}, {14, 15} };
    const Tile corners[ghostcount] = { {0, 25}, {0, 2}, {30, 27}, {30, 0} };
    const Color colors[ghostcount] = { RED, PINK, SKYBLUE, ORANGE };
    for (int i = 0; i < ghostcount; i++)
    {
        Ghost& g = game.ghosts[i];
        g.name = (GhostName)i;
        g.state = (i == blinky) ? roaming : inhouse; // blinky starts outside
        g.scared = false;
        g.row = starts[i].row;
        g.col = starts[i].col;
        g.pos = GetTileCenter(game.map, g.row, g.col);
        g.color = colors[i];
        g.corner = corners[i];
        g.dr = 0;
        g.dc = 0;
        g.nextrow = g.row;
        g.nextcol = g.col;
        if (i == blinky)
            setdir(g, 0, -1);
    }
}
void resetactors(Game& game)
{
    game.pac.position = GetTileCenter(game.map, 22, 13);
    game.pac.direction = Vector2{ 0, 0 };
    game.pac.facing = Vector2{ -1, 0 };
    game.pac.speed = game.cfg.pacspeed;
    game.pac.radius = 10.0f;

    resetghosts(game);

    game.chasing = false; // every life starts in scatter
    game.phase = 0;
    game.phasetimer = 0.0f;
    game.scaredtimer = 0.0f;
    game.chain = 0;
    game.idletimer = 0.0f;
    game.dotseaten = 0;
}
void loadlevel(Game& game)
{
    game.map = CreatePacmanMap();
    game.pelletsleft = 0;
    for (int r = 0; r < MAP_ROWS; r++) {
        for (int c = 0; c < MAP_COLS; c++) {
            if (game.map.grid[r][c] == 2 || game.map.grid[r][c] == 3)
                game.pelletsleft++;
        }
    }
    game.cfg = getlevelconfig(game.level);
    buildeyesfield(game);
    resetactors(game);
    game.state = ready;
    game.statetimer = 2.0f;
}
void newgame(Game& game)
{
    game.level = 1;
    game.lives = 3;
    game.score = 0;
    game.nextlife = 10000;
    loadlevel(game);
}
// input, wall collision and tunnel teleport (same logic as before, just moved in here)
void movepac(Game& game)
{
    Pacman& pac = game.pac;
    //1. INPUT
    Vector2 requestedDir = pac.direction;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
        requestedDir = Vector2{ 1, 0 };
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
        requestedDir = Vector2{ -1, 0 };
    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
        requestedDir = Vector2{ 0, -1 };
    if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
        requestedDir = Vector2{ 0, 1 };
    //  2. Update position and wall collision
    Vector2 nextPos = {
        pac.position.x + requestedDir.x * pac.speed,
        pac.position.y + requestedDir.y * pac.speed };
    if (CanMove(game.map, nextPos, pac.radius))
    {
        pac.direction = requestedDir;
        pac.position = nextPos;
    }
    else
    {
        Vector2 continuePos = {
            pac.position.x + pac.direction.x * pac.speed,
            pac.position.y + pac.direction.y * pac.speed };
        if (CanMove(game.map, continuePos, pac.radius))
        {
            pac.position = continuePos;
        }
    }

    if (pac.direction.x != 0 || pac.direction.y != 0)
        pac.facing = pac.direction;

    // 3. TUNNEL TELEPORTATION ---
    float leftEdge = game.map.screenOffsetX;
    float rightEdge = game.map.screenOffsetX + (MAP_COLS * game.map.tileSize);

    if (pac.position.x < leftEdge)
    {
        pac.position.x = rightEdge;
    }
    else if (pac.position.x > rightEdge)
    {
        pac.position.x = leftEdge;
    }
}

void eatpellets(Game& game)
{
    Tile t = postotile(game.map, game.pac.position);
    int& cell = game.map.grid[t.row][t.col];

    if (cell == 2)
    {
        cell = 0;
        addscore(game, 10);
        game.pelletsleft--;
        game.dotseaten++;
        game.idletimer = 0.0f;
    }
    else if (cell == 3)
    {
        cell = 0;
        addscore(game, 50);
        game.pelletsleft--;
        game.dotseaten++;
        game.idletimer = 0.0f;
        startscared(game);
    }
}
void checkhits(Game& game)
{
    const float hitdist = 12.0f;
    for (int i = 0; i < ghostcount; i++)
    {
        Ghost& g = game.ghosts[i];
        if (g.state == eyes) // eyes cant hurt anyone
            continue;
        float dx = g.pos.x - game.pac.position.x;
        float dy = g.pos.y - game.pac.position.y;
        if (dx * dx + dy * dy >= hitdist * hitdist)
            continue;
        if (g.scared)
        {
            int points = 200 << game.chain; // 200, 400, 800, 1600
            if (game.chain < 3)
                game.chain++;
            addscore(game, points);
            g.state = eyes;
            g.scared = false;
            game.eatenindex = i;
            game.popupvalue = points;
            game.popuppos = g.pos;
            game.state = eatfreeze; // short pause after eating a ghost
            game.statetimer = 0.75f;
            return;
        }
        else
        {
            game.state = dying;
            game.statetimer = 0.0f;
            return;
        }
    }
}
void updateplaying(Game& game)
{
    if (game.scaredtimer > 0.0f)
    {
        game.scaredtimer -= frametime;
        if (game.scaredtimer <= 0.0f)
        {
            game.scaredtimer = 0.0f;
            for (int i = 0; i < ghostcount; i++)
                game.ghosts[i].scared = false;
        }
    }
    else
    {
        updatephase(game); // scatter/chase timer only runs while ghosts arent blue
    }
    game.idletimer += frametime;
    releaseghosts(game);

    movepac(game);
    eatpellets(game);
    if (game.pelletsleft <= 0)
    {
        game.state = levelclear;
        game.statetimer = 2.0f;
        return;
    }

    for (int i = 0; i < ghostcount; i++)
        moveghost(game, game.ghosts[i]);

    checkhits(game);
}
void updategame(Game& game)
{
    game.animtime += frametime;

    if (game.state == ready)
    {
        game.statetimer -= frametime;
        if (game.statetimer <= 0.0f)
            game.state = playing;
    }
    else if (game.state == playing)
    {
        updateplaying(game);
    }
    else if (game.state == eatfreeze)
    {
        game.statetimer -= frametime;
        if (game.statetimer <= 0.0f)
            game.state = playing;
    }
    else if (game.state == dying)
    {
        game.statetimer += frametime;
        if (game.statetimer >= 2.0f)
        {
            game.lives--;
            if (game.lives <= 0)
            {
                game.state = gameover;
            }
            else
            {
                resetactors(game);
                game.state = ready;
                game.statetimer = 1.5f;
            }
        }
    }
    else if (game.state == levelclear)
    {
        game.statetimer -= frametime;
        if (game.statetimer <= 0.0f)
        {
            game.level++;
            loadlevel(game);
        }
    }
    else if (game.state == gameover)
    {
        if (IsKeyPressed(KEY_ENTER))
            newgame(game);
    }
}
// drawing
// raylib needs triangle points in counter clockwise order, this sorts them out
void drawtri(Vector2 a, Vector2 b, Vector2 c, Color color)
{
    float cross = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    if (cross < 0)
        DrawTriangle(a, b, c, color);
    else
        DrawTriangle(a, c, b, color);
}

void drawghost(const Ghost& g, bool whiteflash, float time)
{
    Vector2 c = g.pos;
    if (g.state == inhouse)
        c.y += sinf(time * 6.0f + g.name) * 5.0f; // bobbing while waiting

    const float r = 9.0f;

    if (g.state != eyes)
    {
        Color body = g.color;
        if (g.scared)
            body = whiteflash ? RAYWHITE : BLUE;

        DrawCircleV(Vector2{ c.x, c.y - 2 }, r, body);
        DrawRectangle((int)(c.x - r), (int)(c.y - 2), (int)(2 * r), 8, body);
        float toothw = (2 * r) / 3.0f;
        for (int i = 0; i < 3; i++)
        {
            float x0 = c.x - r + i * toothw;
            drawtri(Vector2{ x0, c.y + 6 }, Vector2{ x0 + toothw, c.y + 6 },
                Vector2{ x0 + toothw / 2, c.y + 10 }, body);
        }
    }

    if (g.scared && g.state != eyes)
    {
        Color face = whiteflash ? RED : Color{ 255, 184, 174, 255 };
        DrawRectangle((int)c.x - 4, (int)c.y - 5, 2, 2, face);
        DrawRectangle((int)c.x + 2, (int)c.y - 5, 2, 2, face);
        for (int i = 0; i < 4; i++) // wavy mouth
        {
            float x0 = c.x - 6 + i * 3.0f;
            float y0 = c.y + 2 + ((i % 2) ? -2 : 0);
            float y1 = c.y + 2 + ((i % 2) ? 0 : -2);
            DrawLineV(Vector2{ x0, y0 }, Vector2{ x0 + 3.0f, y1 }, face);
        }
    }
    else
    {
        float ox = g.dc * 1.5f;
        float oy = g.dr * 1.5f;
        DrawCircleV(Vector2{ c.x - 3.5f, c.y - 3 }, 2.8f, RAYWHITE);
        DrawCircleV(Vector2{ c.x + 3.5f, c.y - 3 }, 2.8f, RAYWHITE);
        DrawCircleV(Vector2{ c.x - 3.5f + ox, c.y - 3 + oy }, 1.5f, DARKBLUE);
        DrawCircleV(Vector2{ c.x + 3.5f + ox, c.y - 3 + oy }, 1.5f, DARKBLUE);
    }
}
// text centered on a point, with a black box behind it so pellets dont show through
void drawcentered(const char* text, Vector2 center, int size, Color color, bool box)
{
    int w = MeasureText(text, size);
    if (box)
        DrawRectangle((int)(center.x - w / 2 - 4), (int)(center.y - size / 2 - 2), w + 8, size + 4, BLACK);
    DrawText(text, (int)(center.x - w / 2), (int)(center.y - size / 2), size, color);
}

void drawgame(Game& game)
{
    bool flashmaze = (game.state == levelclear) && (((int)(game.statetimer * 4.0f)) % 2 == 0);
    DrawMap(game.map, flashmaze);

    // pac-man
    Pacman& pac = game.pac;
    if (game.state == dying)
    {
        float p = fminf(1.0f, game.statetimer / 1.5f);
        if (p < 1.0f)
            DrawCircleSector(pac.position, pac.radius, -90.0f + 180.0f * p, 270.0f - 180.0f * p, 32, YELLOW);
    }
    else
    {
        DrawCircleV(pac.position, pac.radius, YELLOW);
    }

    // ghosts
    bool showghosts = game.state != levelclear && game.state != gameover &&
        !(game.state == dying && game.statetimer > 0.5f);
    if (showghosts)
    {
        bool whiteflash = false;
        if (game.scaredtimer > 0.0f)
        {
            float flashwindow = fminf(2.0f, game.cfg.scaredtime * 0.5f);
            if (game.scaredtimer < flashwindow)
                whiteflash = (((int)(game.scaredtimer * 6.0f)) % 2) == 0;
        }
        for (int i = 0; i < ghostcount; i++)
        {
            if (game.state == eatfreeze && i == game.eatenindex)
                continue; // the score popup takes its place
            drawghost(game.ghosts[i], whiteflash, game.animtime);
        }
    }

    if (game.state == eatfreeze)
        drawcentered(TextFormat("%d", game.popupvalue), game.popuppos, 14, SKYBLUE, false);

    // messages in the corridor under the ghost house
    Vector2 mid = { game.map.screenOffsetX + MAP_COLS * TILE_SIZE / 2.0f,
                   game.map.screenOffsetY + 17 * TILE_SIZE + TILE_SIZE / 2.0f };
    if (game.state == ready)
        drawcentered("READY!", mid, 20, YELLOW, true);
    if (game.state == gameover)
    {
        drawcentered("GAME OVER", mid, 22, RED, true);
        drawcentered("press enter to restart", Vector2{ mid.x, mid.y + 24 }, 14, RAYWHITE, true);
    }
    // score, lives and level
    int hudy = game.map.screenOffsetY + MAP_ROWS * TILE_SIZE + 12;
    DrawText(TextFormat("SCORE %d", game.score), 20, hudy, 20, RAYWHITE);
    for (int i = 0; i < game.lives; i++)
        DrawCircle(260 + i * 24, hudy + 10, 9, YELLOW);
    DrawText(TextFormat("LEVEL %d", game.level), 520, hudy, 20, RAYWHITE);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main()
{
    const int screenWidth = (MAP_COLS * TILE_SIZE) + 40;
    const int screenHeight = (MAP_ROWS * TILE_SIZE) + 40 + hudheight;

    InitWindow(screenWidth, screenHeight, "Pac-Man Map - Raylib (Procedural)");
    SetTargetFPS(60);

    Game game = {};
    newgame(game);

    // Game loop
    while (!WindowShouldClose())
    {
        updategame(game);

        BeginDrawing();
        ClearBackground(BLACK);
        drawgame(game);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
