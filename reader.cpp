#include "reader.h"
#include <map>
#include <json/json.h>
#include <iostream>
#include <fstream>

/* Access external memory and confirm initialization */
ExternalData Reader::readGameData() {
    std::ifstream file("data/data.json", std::ifstream::binary);
    Json::Value data;
    file >> data;

    ExternalData external;

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
    std::cout << ">> SUCCESS\n";
    std::cout << ">> CONFIRM:\n";

    std::cout << external.SCREENWIDTH << '\n';
    std::cout << external.SCREENHEIGHT << '\n';
    std::cout << external.SPEED << '\n';
    std::cout << external.SPRITEWIDTH << '\n';
    std::cout << external.SPRITEHEIGHT << '\n';
    std::cout << external.TILEWIDTH << '\n';
    std::cout << external.TILEHEIGHT << '\n';
    std::cout << external.MAPWIDTH << '\n';
    std::cout << external.MAPHEIGHT << '\n';

    std::cout << ">> SPRITEPATHS\n";
    for (std::string string : external.SPRITEPATHS) {
        std::cout << string << '\n';
    }
    std::cout << ">> NPCPATHS\n";
    for (std::string string : external.NPCPATHS) {
        std::cout << string << '\n';
    }
    std::cout << ">> TILEPATHS\n";
    for (std::string string : external.TILEPATHS) {
        std::cout << string << '\n';
    }

    std::cout << ">> LAYOUT\n";
    for (auto id : tileset) {
        std::cout << id.first << ":" << id.second << '\n';
    }
    for (std::vector<int> row : external.MAP) {
        for (int num : row) {
            std::cout << num << ' ';
        }
        std::cout << '\n';
    }

    for (auto id : npcset) {
        std::cout << id.first << ":" << id.second << '\n';
    }
    for (std::vector<int> row : external.NPC) {
        for (int num : row) {
            std::cout << num << ' ';
        }
        std::cout << '\n';
    }
    std::cout << ">> GAME DATA LOADED - BEGIN PROCESSING\n";

    return external;
}
