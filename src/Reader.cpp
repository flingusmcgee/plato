#include "Reader.h"
#include "Game.h"
#include "Entity.h"
#include <json/json.h>
#include <iostream>
#include <fstream>

#if defined(__x86_64__) || defined(_M_X64)
    #define architecture "x86_64"
#elif defined(__aarch64__) || defined(_M_ARM64)
    #define architecture "ARM64"
#else
    #define architecture "UNKNOWN"
#endif

/* Access external memory and confirm initialization */
bool Reader::readGameData(Game& game) {
    const std::string PATHBASE = "../../";

    std::printf(">> ARCHITECTURE: %s\n\n", architecture);
    std::printf(">> LOADING\n");

    std::ifstream file(PATHBASE + "data/data.json", std::ifstream::binary);
    if (!file.is_open()) {
        SDL_SetError("\"data.json\" could not be opened.");
        return false;
    }
    Json::Value data;
    file >> data;

    /* Paths */
    for (Json::Value string : data["map"]["texture"]) {
        game.TILEPATHS.push_back(PATHBASE + string.asString());
    }
    for (Json::Value string : data["game"]["icons"]) {
        game.ICONPATHS.push_back(PATHBASE + string.asString());
    }
    for (Json::Value string : data["game"]["bgmusic"]) {
        game.AUDIOPATHS.push_back(PATHBASE + string.asString());
    }
    /* Screen */
    game.SCREENWIDTH = data["game"]["screenwidth"].asFloat();
    game.SCREENHEIGHT = data["game"]["screenheight"].asFloat();
    game.FONTPATH = PATHBASE + data["game"]["font"]["path"].asString();
    game.FONTSIZE = data["game"]["font"]["size"].asFloat();
    game.FONTSPEED = data["game"]["font"]["speed"].asFloat();
    game.ANIMATIONSPEED = data["game"]["animation"]["speed"].asFloat();
    game.SHOWBOXES = data["game"]["showboxes"].asBool();
    /* Map */
    game.MAXENTITIES = data["map"]["maxentities"].asInt();
    game.MAPWIDTH = data["map"]["mapwidth"].asInt();
    game.MAPHEIGHT = data["map"]["mapheight"].asInt();
    game.TILEWIDTH = data["map"]["tilewidth"].asInt();
    game.TILEHEIGHT = data["map"]["tileheight"].asInt();

    /* Obtain tileset identifiers */
    std::map<std::string, int> tileset = { };
    for (int i = 0; i < data["map"]["id"].size(); ++i) {
        tileset[data["map"]["id"][i].asString()] = i;
        game.MAPSET.push_back(data["map"]["id"][i].asString());
    }

    /* Convert identifiers to integers */
    std::vector<int> row = { };
    for (int i = 0; i < game.MAPHEIGHT; ++i) {
        for (int j = 0; j < game.MAPWIDTH; ++j) {
            row.push_back(tileset[data["layout"]["map"][j + i * game.MAPWIDTH].asString()]);
        }
        game.MAP.push_back(row);
        row = { };
    }

    /* Obtain entity identifiers */
    std::map<std::string, int> npcset = { };
    for (int i = 0; i < data["entityid"].size(); ++i) {
        npcset[data["entityid"][i].asString()] = i;
        game.NPCSET.push_back(data["entityid"][i].asString());
    }

    /* Convert identifiers to integers */
    row = { };
    for (int i = 0; i < game.MAPHEIGHT; ++i) {
        for (int j = 0; j < game.MAPWIDTH; ++j) {
            row.push_back(npcset[data["layout"]["npc"][j + i * game.MAPWIDTH].asString()]);
        }
        game.NPC.push_back(row);
        row = { };
    }

    /* Capture dialogue */
    file.close();
    file.open(PATHBASE + "data/dialogue.json", std::ifstream::binary);
    if (!file.is_open()) {
        SDL_SetError("\"dialogue.json\" could not be opened.");
        return false;
    }
    file >> data;

    std::vector<std::string> lines = { };
    for (auto entityid : game.NPCSET) {
        for (auto line : data[entityid]) {
            lines.push_back(line.asString());
        }
        game.DIALOGUE.push_back(lines);
        lines = { };
    }

    /* Convert entity data into templates */
    file.close();
    file.open(PATHBASE + "data/entity.json", std::ifstream::binary);
    if (!file.is_open()) {
        SDL_SetError("\"entity.json\" could not be opened.");
        return false;
    }
    file >> data;

    for (auto entityid : game.NPCSET) {
        EntityTemplate entityTemplate;
        int type = npcset[entityid];
        entityTemplate.type = type;
        if (type > 0) {
            Json::Value entity = data[entityid];
            entityTemplate.group = entity["group"].asString();
            for (auto path : entity["texture"]["paths"]) {
                entityTemplate.texturePaths.push_back(PATHBASE + path.asString());
            }
            entityTemplate.rect = { 0, 0, entity["texture"]["width"].asFloat(), entity["texture"]["height"].asFloat() };
            entityTemplate.hitbox = { 0, 0, entity["hitbox"]["width"].asFloat(), entity["hitbox"]["height"].asFloat() };
            entityTemplate.hitboxLayer = entity["hitbox"]["layer"].asInt();
            entityTemplate.speed = entity["speed"].asFloat();
            entityTemplate.range = entity["range"].asInt();
        }
        game.ENTITYTEMPLATES[entityid] = entityTemplate;
    }

    file.close();

    std::printf(">> SUCCESS\n");
    std::printf(">> CONFIRM\n\n");

    std::printf(">> TILEPATHS\n");
    for (std::string string : game.TILEPATHS) {
        std::printf("%s\n", (!string.empty()) ? string.c_str() : "[NULL]");
    }
    std::printf(">> ICONPATHS\n");
    for (std::string string : game.ICONPATHS) {
        std::printf("%s\n", (!string.empty()) ? string.c_str() : "[NULL]");
    }
    std::printf(">> AUDIOPATHS\n");
    for (std::string string : game.AUDIOPATHS) {
        std::printf("%s\n", (!string.empty()) ? string.c_str() : "[NULL]");
    }

    std::printf("\n>> SCREEN\n");
    std::printf("SCREENWIDTH: %.1f\n", game.SCREENWIDTH);
    std::printf("SCREENHEIGHT: %.1f\n", game.SCREENHEIGHT);
    std::printf("FONTPATH: %s\n", game.FONTPATH.c_str());
    std::printf("FONTSIZE: %.1f\n", game.FONTSIZE);
    std::printf("FONTSPEED: %.1f\n", game.FONTSPEED);
    std::printf("ANIMATIONSPEED: %.1f\n", game.ANIMATIONSPEED);
    std::printf("SHOWBOXES: %s\n", game.SHOWBOXES ? "TRUE" : "FALSE");

    std::printf("\n>> ENTITIES\n");
    for (auto entityid : game.NPCSET) {
        EntityTemplate entity = game.ENTITYTEMPLATES[entityid];
        std::printf("%s: %d\n", game.NPCSET[entity.type].c_str(), entity.type);
        if (entity.type > 0) {
            std::printf("  GROUP: %s\n", entity.group.c_str());
            std::printf("  TEXTURES: \n");
            for (auto path : entity.texturePaths) {
                std::printf("    %s\n", path.c_str());
            }
            std::printf("  RECT: %.1f x %.1f\n", entity.rect.w, entity.rect.h);
            std::printf("  HITBOX: %.1f x %.1f\n", entity.hitbox.w, entity.hitbox.h);
            std::printf("  SPEED: %.1f\n", entity.speed);
            if (entity.range) {
                std::printf("  RANGE: %d\n", entity.range);
            }
        }
    }

    std::printf("\n>> MAP\n");
    std::printf("MAXENTITIES: %d\n", game.MAXENTITIES);
    std::printf("MAPWIDTH: %d\n", game.MAPWIDTH);
    std::printf("MAPHEIGHT: %d\n", game.MAPHEIGHT);
    std::printf("TILEWIDTH: %d\n", game.TILEWIDTH);
    std::printf("TILEHEIGHT: %d\n", game.TILEHEIGHT);

    std::printf("\n>> LAYOUT\n");
    for (auto [k, v] : tileset) {
        std::printf("%s: %d\n", k.c_str(), v);
    }
    for (std::vector<int> row : game.MAP) {
        for (int num : row) {
            std::printf("%2d ", num);
        }
        std::printf("\n");
    }

    for (auto [k, v] : npcset) {
        std::printf("%s: %d\n", k.c_str(), v);
    }
    for (std::vector<int> row : game.NPC) {
        for (int num : row) {
            std::printf("%2d ", num);
        }
        std::printf("\n");
    }

    std::printf("\n>> DIALOGUE\n");
    int dialogueidx = 0;
    for (auto entity : game.DIALOGUE) {
        std::printf("%s: ", game.NPCSET[dialogueidx].c_str());
        for (auto line : entity) {
            std::printf("\"%s\" ", line.c_str());
        }
        std::printf("\n");
        dialogueidx++;
    }

    std::printf("\n>> GAME DATA LOADED - BEGIN PROCESSING\n");

    return true;
}
