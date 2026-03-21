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
    /* Map */
    external.TILEWIDTH = data["map"]["tilewidth"].asInt();
    external.TILEHEIGHT = data["map"]["tileheight"].asInt();
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

    /* Obtain entity identifiers */
    std::map<std::string, int> npcset = { };
    for (int i = 0; i < data["entityid"].size(); ++i) {
        npcset[data["entityid"][i].asString()] = i;
        external.NPCSET.push_back(data["entityid"][i].asString());
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
    for (auto entityid : external.NPCSET) {
        for (auto line : data[entityid]) {
            lines.push_back(line.asString());
        }
        external.DIALOGUE.push_back(lines);
        lines = { };
    }

    file.close();
    file.open("data/entity.json", std::ifstream::binary);
    file >> data;

    for (auto entityid : external.NPCSET) {
        EntityTemplate entityTemplate;
        int type = npcset[entityid];
        entityTemplate.type = type;
        if (type > 0) {
            Json::Value entity = data[entityid];
            entityTemplate.group = entity["group"].asString();
            for (auto path : entity["texture"]["paths"]) {
                entityTemplate.texturePaths.push_back(path.asString());
            }
            entityTemplate.rect = { 0, 0, entity["texture"]["width"].asFloat(), entity["texture"]["height"].asFloat() };
            entityTemplate.hitbox = { 0, 0, entity["hitbox"]["width"].asFloat(), entity["hitbox"]["height"].asFloat() };
            entityTemplate.speed = entity["speed"].asFloat();
            entityTemplate.range = entity["range"].asInt();
            entityTemplate.breadth = entity["breadth"].asInt();
        }
        external.ENTITYTEMPLATES[entityid] = entityTemplate;
    }

    file.close();

    std::printf(">> SUCCESS\n");
    std::printf(">> CONFIRM\n\n");

    std::printf(">> TILEPATHS\n");
    for (std::string string : external.TILEPATHS) {
        std::printf("%s\n", (!string.empty()) ? string.c_str() : "[NULL]");
    }
    std::printf(">> ICONPATHS\n");
    for (std::string string : external.ICONPATHS) {
        std::printf("%s\n", (!string.empty()) ? string.c_str() : "[NULL]");
    }
    std::printf(">> AUDIOPATHS\n");
    for (std::string string : external.AUDIOPATHS) {
        std::printf("%s\n", (!string.empty()) ? string.c_str() : "[NULL]");
    }

    std::printf("\n>> SCREEN\n");
    std::printf("SCREENWIDTH: %d\n", external.SCREENWIDTH);
    std::printf("SCREENHEIGHT: %d\n", external.SCREENHEIGHT);
    std::printf("FONTPATH: %s\n", external.FONTPATH.c_str());
    std::printf("FONTSIZE: %d\n", external.FONTSIZE);
    std::printf("FONTSPEED: %d\n", external.FONTSPEED);

    std::printf("\n>> ENTITIES\n");
    for (auto entityid : external.NPCSET) {
        EntityTemplate entity = external.ENTITYTEMPLATES[entityid];
        std::printf("%s: %d\n", external.NPCSET[entity.type].c_str(), entity.type);
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
            if (entity.breadth) {
                std::printf("  BREADTH: %d\n", entity.breadth);
            }
        }
    }

    std::printf("\n>> MAP\n");
    std::printf("TILEWIDTH: %d\n", external.TILEWIDTH);
    std::printf("TILEHEIGHT: %d\n", external.TILEHEIGHT);
    std::printf("MAPWIDTH: %d\n", external.MAPWIDTH);
    std::printf("MAPHEIGHT: %d\n", external.MAPHEIGHT);

    std::printf("\n>> LAYOUT\n");
    for (auto [s, d] : tileset) {
        std::printf("%s: %d\n", s.c_str(), d);
    }
    for (std::vector<int> row : external.MAP) {
        for (int num : row) {
            std::printf("%2d ", num);
        }
        std::printf("\n");
    }

    for (auto [s, d] : npcset) {
        std::printf("%s: %d\n", s.c_str(), d);
    }
    for (std::vector<int> row : external.NPC) {
        for (int num : row) {
            std::printf("%2d ", num);
        }
        std::printf("\n");
    }

    std::printf("\n>> DIALOGUE\n");
    int dialogueidx = 0;
    for (auto entity : external.DIALOGUE) {
        std::printf("%s: ", external.NPCSET[dialogueidx].c_str());
        for (auto line : entity) {
            std::printf("\"%s\" ", line.c_str());
        }
        std::printf("\n");
        dialogueidx++;
    }

    std::printf("\n>> GAME DATA LOADED - BEGIN PROCESSING\n");

    return external;
}
