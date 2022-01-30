#ifndef GAME_H
#define GAME_H

#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game_level.h"

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN,
    GAME_OVER
};

class Game
{
public:
    GameState                   State;	
    bool                        Keys[1024];
    unsigned int                Width, Height;
    unsigned int                Score;
    std::vector<GameLevel>      Levels;
    unsigned int                Lights;
    unsigned int                Level;
    float                       TimeInLight;
    float                       TimePlayed;

    Game(unsigned int width, unsigned int height);
    ~Game();

    void Init();
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void handleCollisions();
};

#endif