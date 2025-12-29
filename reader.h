#pragma once
#include <string>
#include <vector>

struct ExternalData {
    int SCREENWIDTH;
    int SCREENHEIGHT;
    float SPEED;
    std::vector<std::string> SPRITEPATHS;
    std::vector<std::string> NPCPATHS;
    std::vector<std::string> TILEPATHS;
    int SPRITEWIDTH;
    int SPRITEHEIGHT;
    int TILEWIDTH;
    int TILEHEIGHT;
    int MAPWIDTH;
    int MAPHEIGHT;
    std::vector<std::vector<int>> MAP;
    std::vector<std::vector<int>> NPC;
};

class Reader {
    public:
        ExternalData readGameData();
};
