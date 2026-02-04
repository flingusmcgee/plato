#include "Reader.h"
#include <json/json.h>
#include <iostream>
#include <fstream>

/* Access external memory and confirm initialization */
Game Reader::readGameData() {
    std::ifstream file("data/data.json", std::ifstream::binary);
    Json::Value data;
    file >> data;

    Game external;

    /* Paths */
    for (Json::Value string : data["entity"]["player"]["texture"]) {
        external.SPRITEPATHS.push_back(string.asString());
    }
    for (Json::Value string : data["entity"]["npc"]["texture"]) {
        external.NPCPATHS.push_back(string.asString());
    }
    for (Json::Value string : data["map"]["texture"]) {
        external.TILEPATHS.push_back(string.asString());
    }
    for (Json::Value string : data["game"]["icons"]) {
        external.ICONPATHS.push_back(string.asString());
    }
    for (Json::Value string : data["game"]["bgmusic"]) {
        external.AUDIOPATHS.push_back(string.asString());
    }
    /* Screen */
    external.SCREENWIDTH = data["game"]["screenwidth"].asInt();
    external.SCREENHEIGHT = data["game"]["screenheight"].asInt();
    external.FONTPATH = data["game"]["font"]["path"].asString();
    external.FONTSIZE = data["game"]["font"]["size"].asInt();
    external.FONTSPEED = data["game"]["font"]["speed"].asInt();
    /* Player */
    external.SPEED = data["entity"]["player"]["speed"].asFloat();
    external.SPRITEWIDTH = data["entity"]["width"].asFloat();
    external.SPRITEHEIGHT = data["entity"]["height"].asFloat();
    external.RANGE = data["entity"]["player"]["range"].asInt();
    external.BREADTH = data["entity"]["player"]["breadth"].asInt();
    /* Map */
    external.TILEWIDTH = data["map"]["tilewidth"].asFloat();
    external.TILEHEIGHT = data["map"]["tileheight"].asFloat();
    external.MAPWIDTH = data["map"]["mapwidth"].asInt();
    external.MAPHEIGHT = data["map"]["mapheight"].asInt();

    /* Obtain tileset identifiers */
    std::map<std::string, int> tileset = { };
    for (int i = 0; i < data["map"]["id"].size(); ++i) {
        tileset[data["map"]["id"][i].asString()] = i;
        external.MAPSET.push_back(data["map"]["id"][i].asString());
    }

    /* Convert identifiers to integers */
    std::vector<int> row = { };
    for (int i = 0; i < external.MAPHEIGHT; ++i) {
        for (int j = 0; j < external.MAPWIDTH; ++j) {
            row.push_back(tileset[data["layout"]["map"][j + i * external.MAPWIDTH].asString()]);
        }
        external.MAP.push_back(row);
        row = { };
    }

    /* Obtain spriteset identifiers */
    std::map<std::string, int> npcset = { };
    for (int i = 0; i < data["entity"]["npc"]["id"].size(); ++i) {
        npcset[data["entity"]["npc"]["id"][i].asString()] = i;
        external.NPCSET.push_back(data["entity"]["npc"]["id"][i].asString());
    }

    /* Convert identifiers to integers */
    row = { };
    for (int i = 0; i < external.MAPHEIGHT; ++i) {
        for (int j = 0; j < external.MAPWIDTH; ++j) {
            row.push_back(npcset[data["layout"]["npc"][j + i * external.MAPWIDTH].asString()]);
        }
        external.NPC.push_back(row);
        row = { };
    }

    file.close();
    file.open("data/dialogue.json", std::ifstream::binary);
    file >> data;

    std::vector<std::string> lines = { };
    for (auto entity : data["text"]) {
        for (auto line : entity) {
            lines.push_back(line.asString());
        }
        external.DIALOGUE.push_back(lines);
        lines = { };
    }

    file.close();

    std::printf(">> SUCCESS\n");
    std::printf(">> CONFIRM\n\n");

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
    std::printf(">> ICONPATHS\n");
    for (std::string string : external.ICONPATHS) {
        std::printf("%s\n", string.c_str());
    }
    std::printf(">> AUDIOPATHS\n");
    for (std::string string : external.AUDIOPATHS) {
        std::printf("%s\n", string.c_str());
    }

    std::printf("\n>> SCREEN\n");
    std::printf("SCREENWIDTH: %d\n", external.SCREENWIDTH);
    std::printf("SCREENHEIGHT: %d\n", external.SCREENHEIGHT);
    std::printf("FONTPATH: %s\n", external.FONTPATH.c_str());
    std::printf("FONTSIZE: %d\n", external.FONTSIZE);
    std::printf("FONTSPEED: %d\n", external.FONTSPEED);

    std::printf("\n>> PLAYER\n");
    std::printf("SPEED: %.1f\n", external.SPEED);
    std::printf("SPRITEWIDTH: %.1f\n", external.SPRITEWIDTH);
    std::printf("SPRITEHEIGHT: %.1f\n", external.SPRITEHEIGHT);
    std::printf("RANGE: %d\n", external.RANGE);
    std::printf("BREADTH: %d\n", external.BREADTH);

    std::printf("\n>> MAP\n");
    std::printf("TILEWIDTH: %.1f\n", external.TILEWIDTH);
    std::printf("TILEHEIGHT: %.1f\n", external.TILEHEIGHT);
    std::printf("MAPWIDTH: %d\n", external.MAPWIDTH);
    std::printf("MAPHEIGHT: %d\n", external.MAPHEIGHT);

    std::printf("\n>> LAYOUT\n");
    for (auto id : tileset) {
        std::printf("%s: %d\n", id.first.c_str(), id.second);
    }
    for (std::vector<int> row : external.MAP) {
        for (int num : row) {
            std::printf("%2d ", num);
        }
        std::printf("\n");
    }

    for (auto id : npcset) {
        std::printf("%s: %d\n", id.first.c_str(), id.second);
    }
    for (std::vector<int> row : external.NPC) {
        for (int num : row) {
            std::printf("%2d ", num);
        }
        std::printf("\n");
    }

    std::printf(">> DIALOGUE\n");
    int dialogueidx = 0;
    for (auto entity : external.DIALOGUE) {
        std::printf("%s: ", external.NPCSET[dialogueidx].c_str());
        for (auto line : entity) {
            std::printf("%s ", line.c_str());
        }
        std::printf("\n");
        dialogueidx++;
    }
    std::printf(">> GAME DATA LOADED - BEGIN PROCESSING\n");

    return external;
}
