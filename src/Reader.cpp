#include "Reader.h"
#include <map>
#include <json/json.h>
#include <iostream>
#include <fstream>

/* Access external memory and confirm initialization */
GameData Reader::readGameData() {
    std::ifstream file("data/data.json", std::ifstream::binary);
    Json::Value data;
    file >> data;

    GameData external;

    external.SCREENWIDTH = data["screen"]["width"].asInt();
    external.SCREENHEIGHT = data["screen"]["height"].asInt();

    external.SPEED = data["entity"]["player"]["speed"].asFloat();

    for (Json::Value string : data["entity"]["player"]["texture"]) {
        external.SPRITEPATHS.push_back(string.asString());
    }
    for (Json::Value string : data["entity"]["npc"]["texture"]) {
        external.NPCPATHS.push_back(string.asString());
    }
    for (Json::Value string : data["map"]["texture"]) {
        external.TILEPATHS.push_back(string.asString());
    }

    external.SPRITEWIDTH = data["entity"]["width"].asInt();
    external.SPRITEHEIGHT = data["entity"]["height"].asInt();
    external.TILEWIDTH = data["map"]["tilewidth"].asInt();
    external.TILEHEIGHT = data["map"]["tileheight"].asInt();
    external.MAPWIDTH = data["map"]["mapwidth"].asInt();
    external.MAPHEIGHT = data["map"]["mapheight"].asInt();

    /* Obtain tileset identifiers */
    std::map<std::string, int> tileset = { };
    for (int i = 0; i < data["map"]["id"].size(); i++) {
        tileset[data["map"]["id"][i].asString()] = i;
    }

    /* Convert identifiers to integers */
    std::vector<int> row = { };
    for (int i = 0; i < external.MAPHEIGHT; i++) {
        for (int j = 0; j < external.MAPWIDTH; j++) {
            row.push_back(tileset[data["layout"]["map"][j + i * external.MAPWIDTH].asString()]);
        }
        external.MAP.push_back(row);
        row = { };
    }

    /* Obtain spriteset identifiers */
    std::map<std::string, int> npcset = { };
    for (int i = 0; i < data["entity"]["npc"]["id"].size(); i++) {
        npcset[data["entity"]["npc"]["id"][i].asString()] = i;
    }

    /* Convert identifiers to integers */
    row = { };
    for (int i = 0; i < external.MAPHEIGHT; i++) {
        for (int j = 0; j < external.MAPWIDTH; j++) {
            /* EMPTY sprite case - render nothing */
            if (data["layout"]["npc"][j + i * external.MAPWIDTH].asString() == "EMPTY") {
                row.push_back(-1);
            }
            else {
                row.push_back(npcset[data["layout"]["npc"][j + i * external.MAPWIDTH].asString()]);
            }
        }
        external.NPC.push_back(row);
        row = { };
    }
    std::printf(">> SUCCESS\n");
    std::printf(">> CONFIRM:\n");

    std::printf("SCREENWIDTH: %d\n", external.SCREENWIDTH);
    std::printf("SCREENHEIGHT: %d\n", external.SCREENHEIGHT);
    std::printf("SPEED: %.1f\n", external.SPEED);
    std::printf("SPRITEWIDTH: %d\n", external.SPRITEWIDTH);
    std::printf("SPRITEHEIGHT: %d\n", external.SPRITEHEIGHT);
    std::printf("TILEWIDTH: %d\n", external.TILEWIDTH);
    std::printf("TILEHEIGHT: %d\n", external.TILEHEIGHT);
    std::printf("MAPWIDTH: %d\n", external.MAPWIDTH);
    std::printf("MAPHEIGHT: %d\n", external.MAPHEIGHT);

    std::printf(">> SPRITEPATHS\n");
    for (std::string string : external.SPRITEPATHS) {
        std::printf("%s\n", string.c_str());
    }
    std::printf(">> NPCPATHS\n");
    for (std::string string : external.NPCPATHS) {
        std::printf("%s\n", string.c_str());
    }
    std::printf(">> TILEPATHS\n");
    for (std::string string : external.TILEPATHS) {
        std::printf("%s\n", string.c_str());
    }

    std::printf(">> LAYOUT\n");
    for (auto id : tileset) {
        std::printf("%s:%d\n", id.first.c_str(), id.second);
    }
    for (std::vector<int> row : external.MAP) {
        for (int num : row) {
            std::printf("%2d ", num);
        }
        std::printf("\n");
    }

    for (auto id : npcset) {
        std::printf("%s:%d\n", id.first.c_str(), id.second);
    }
    for (std::vector<int> row : external.NPC) {
        for (int num : row) {
            std::printf("%2d ", num);
        }
        std::printf("\n");
    }
    std::printf(">> GAME DATA LOADED - BEGIN PROCESSING\n");

    return external;
}
