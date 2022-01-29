#include <stdio.h>

#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "game_level.h"

#include "gltext.h"

bool CheckCollision(GameObject &one, GameObject &two);
void handlePlayerWallCollisions(UFO &player, std::vector<GameObject> &Walls);
void handlePlayerCoinCollision(UFO &player, std::vector<GameObject> &Coins, unsigned int &score);
void handleEnemyWallCollisions(Enemy &enemy, std::vector<GameObject> &Walls);
void handleEnemyEnemyCollisions(std::vector<Enemy> &Enemies);

SpriteRenderer *Renderer;
SpriteRenderer *UFORenderer;

UFO *Player;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_MENU), Keys(), Width(width), Height(height), Lights(1), Level(0)
{
}

Game::~Game()
{
    delete Renderer;
    delete UFORenderer;
}

void Game::Init()
{
    ResourceManager::LoadShader("shaders/sprite.vs", "shaders/sprite.frag", nullptr, "sprite");
    ResourceManager::LoadShader("shaders/ufo.vs", "shaders/ufo.frag", nullptr, "ufo");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
                                      static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("sprite").Use().SetInteger("Lights", this->Lights);

    ResourceManager::GetShader("ufo").Use().SetInteger("image", 0);
    ResourceManager::GetShader("ufo").SetMatrix4("projection", projection);

    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    UFORenderer = new SpriteRenderer(ResourceManager::GetShader("ufo"));

    ResourceManager::LoadTexture("textures/ufo.png", true, "ufo");
    ResourceManager::LoadTexture("textures/meteor.png", true, "meteor");
    ResourceManager::LoadTexture("textures/enemy.png", true, "enemy");
    ResourceManager::LoadTexture("textures/coin.png", true, "coin");
    ResourceManager::LoadTexture("textures/wall.png", true, "wall");
    ResourceManager::LoadTexture("textures/bg.png", false, "background");

    GameLevel one, two, three;
    one.GenerateLevel(25, 3, 5, 30, 24, this->Width, this->Height, false);
    two.GenerateLevel(20, 3, 10, 30, 24, this->Width, this->Height, true);
    three.GenerateLevel(15, 3, 10, 30, 24, this->Width, this->Height, true);
    this->Levels.push_back(one);
    this->Levels.push_back(two);
    this->Levels.push_back(three);

    Player = new UFO(glm::vec2(35.0f, 35.0f), glm::vec2(25.0f, 25.0f), ResourceManager::GetTexture("ufo"), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(200.0f, 200.0f));

    if (!gltInit())
    {
        fprintf(stderr, "Failed to initialize glText\n");
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    this->Text = gltCreateText();
    gltSetText(this->Text, "Score: 0");
}

void Game::Update(float dt)
{
    if (this->State == GAME_ACTIVE)
    {
        handlePlayerWallCollisions(*Player, this->Levels[Level].Walls);
        handlePlayerCoinCollision(*Player, this->Levels[Level].Coins, this->Score);
        for (Enemy &enemy : this->Levels[Level].Enemies)
        {
            enemy.Move(Player->Position, dt);
            if (CheckCollision(*Player, enemy))
            {
                if (!enemy.Destroyed)
                {
                    Player->Destroyed = true;
                    this->State = GAME_OVER;
                }
            }
            handleEnemyWallCollisions(enemy, this->Levels[Level].Walls);
        }
        handleEnemyEnemyCollisions(this->Levels[Level].Enemies);

        // Handle score updates
        char scoreStr[20];
        sprintf(scoreStr, "Score: %d", this->Score);
        gltSetText(this->Text, scoreStr);

        if (Player->Position.x > this->Width)
        {
            Player->Position.x = 0.0f;
            Player->Position.y = 32.0f;
            if (Level < 2)
                Level++;
            else if ( Level == 2)
                this->State = GAME_WIN;
        }
        else if (Player->Position.x < 0.0f)
        {
            Player->Position.x = this->Width - 35.0f;
            Player->Position.y = this->Height - 56.0f;
            if (Level > 0)
                Level--;
        }

        // Handle lighting
        glm::vec3 lightPos = glm::vec3(Player->Position.x, Player->Position.y, 1.0f);
        ResourceManager::GetShader("sprite").Use().SetVector3f("LightPos", lightPos);
        ResourceManager::GetShader("sprite").Use().SetInteger("Lights", this->Lights);
    }
}

void Game::ProcessInput(float dt)
{
    if (this->State == GAME_ACTIVE)
        Player->ProcessInput(Keys, dt);
}

void Game::Render()
{
    if (this->State == GAME_ACTIVE)
    {
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f, glm::vec3(1.0f));
        Levels[Level].Draw(*Renderer);
        Player->Draw(*UFORenderer);

        gltBeginDraw();

        gltColor(1.0f, 1.0f, 1.0f, 1.0f);
        gltDrawText2D(this->Text, 0.0f, 0.0f, 1.0f);

        gltEndDraw();
    }
    if (this->State == GAME_MENU)
    {
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f, glm::vec3(1.0f));
        gltBeginDraw();

        GLTtext* text = gltCreateText();
        gltSetText(text, "Press Enter to start");

        gltColor(1.0f, 1.0f, 1.0f, 1.0f);
        gltDrawText2D(text, 200.0f, 300.0f, 3.0f);

        gltEndDraw();
    }
    if (this->State == GAME_OVER)
    {
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f, glm::vec3(1.0f));
        gltBeginDraw();

        GLTtext* text = gltCreateText();
        GLTtext* text2 = gltCreateText();
        gltSetText(text, "Game Over");
        gltSetText(text2, "Press Enter to exit");

        gltColor(1.0f, 1.0f, 1.0f, 1.0f);
        gltDrawText2D(text, 350.0f, 300.0f, 3.0f);
        gltDrawText2D(text2, 350.0f, 350.0f, 1.0f);

        gltEndDraw();
    }
    if (this->State == GAME_WIN)
    {
        Renderer->DrawSprite(ResourceManager::GetTexture("background"), glm::vec2(0.0f, 0.0f), glm::vec2(this->Width, this->Height), 0.0f, glm::vec3(1.0f));
        gltBeginDraw();

        GLTtext* text = gltCreateText();
        GLTtext* text2 = gltCreateText();
        gltSetText(text, "You win!");
        gltSetText(text2, "Press Enter to exit");

        gltColor(1.0f, 1.0f, 1.0f, 1.0f);
        gltDrawText2D(text, 350.0f, 300.0f, 3.0f);
        gltDrawText2D(this->Text, 350.0f, 350.0f, 2.0f);
        gltDrawText2D(text2, 350.0f, 400.0f, 1.0f);

        gltEndDraw();
    }
}

bool CheckCollision(GameObject &one, GameObject &two) // AABB - AABB collision
{
    // collision x-axis?
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
                      two.Position.x + two.Size.x >= one.Position.x;
    // collision y-axis?
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
                      two.Position.y + two.Size.y >= one.Position.y;
    // collision only if on both axes
    return collisionX && collisionY;
}

void handlePlayerWallCollisions(UFO &player, std::vector<GameObject> &Walls)
{
    for (GameObject &Wall : Walls)
    {
        if (CheckCollision(player, Wall))
        {
            Player->Position.x -= Player->deltaX;
            Player->Position.y -= Player->deltaY;
            break;
        }
    }
}

void handlePlayerCoinCollision(UFO &player, std::vector<GameObject> &Coins, unsigned int &score)
{
    for (GameObject &Coin : Coins)
    {
        if (CheckCollision(player, Coin) && !Coin.Destroyed)
        {
            Coin.Destroyed = true;
            score += 10;
        }
    }
}

void handleEnemyWallCollisions(Enemy &enemy, std::vector<GameObject> &Walls)
{
    for (GameObject &Wall : Walls)
    {
        if (CheckCollision(enemy, Wall))
        {
            enemy.Position.x -= enemy.deltaX;
            enemy.Position.y -= enemy.deltaY;
            break;
        }
    }
}

void handleEnemyEnemyCollisions(std::vector<Enemy> &Enemies)
{
    for (int i = 0; i < Enemies.size(); i++)
    {
        for (int j = i + 1; j < Enemies.size(); j++)
        {
            if (CheckCollision(Enemies[i], Enemies[j]))
            {
                if (!Enemies[i].Destroyed && !Enemies[j].Destroyed)
                {
                    Enemies[i].Destroyed = true;
                    Enemies[j].Destroyed = true;
                }
            }
        }
    }
}
