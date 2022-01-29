#ifndef GAMELEVEL_H
#define GAMELEVEL_H
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "game_object.h"
#include "sprite_renderer.h"
#include "resource_manager.h"


/// GameLevel holds all Tiles as part of a Breakout level and 
class GameLevel
{
public:
    // level state
    std::vector<GameObject> Walls;
    std::vector<GameObject> Coins;
    std::vector<Enemy> Enemies;
    // constructor
    GameLevel() { }
    // render level
    void Draw(SpriteRenderer &renderer);
    // Generates a level
    void GenerateLevel(unsigned int walls, unsigned int enemies, unsigned int coins, unsigned int columns, unsigned int rows, unsigned int levelWidth, unsigned int levelHeight, bool hasEntry);

private:
    // initialize level from tile data
    void init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight);
    bool isGood(std::vector<std::vector<unsigned int>> tileData, int Walls);
};

#endif