#include <cstdlib>
#include <ctime>
#include <vector>
#include <utility>
#include <queue>

#include "game_level.h"
#include "game_object.h"

enum TYPE
{
    EMPTY,
    WALL,
    ENEMY,
    COIN,
};

void GameLevel::GenerateLevel(unsigned int walls, unsigned int enemies, unsigned int coins, unsigned int columns, unsigned int rows, unsigned int levelWidth, unsigned int levelHeight, bool hasEntry)
{
    // create random seed
    srand(time(NULL));

    std::vector<std::vector<unsigned int>> tiles(rows, std::vector<unsigned int>(columns, 0));

    // Create borders
    for (int i = 0; i < columns; i++)
    {
        tiles[0][i] = WALL;
        tiles[rows - 1][i] = WALL;
    }
    for (int i = 0; i < rows; i++)
    {
        tiles[i][0] = WALL;
        tiles[i][columns - 1] = WALL;
    }

    // Create walls
    for (int i = 0; i < walls; i++)
    {
        int x = rand() % (columns - 2) + 1;
        int y = rand() % (rows - 2) + 1;
        // Dont spawn if too close to spawn
        if (x < 3 || y < 3)
        {
            i--;
        }
        else
        {
            tiles[y][x] = WALL;
        }
    }

    // Create coins
    for (int i = 0; i < coins; i++)
    {
        int x = rand() % (columns - 2) + 1;
        int y = rand() % (rows - 2) + 1;
        if (tiles[y][x] != EMPTY)
        {
            i--;
            continue;
        }
        if (x < 3 || y < 3)
        {
            i--;
            continue;
        }
        tiles[y][x] = COIN;
    }

    // Create enemies
    for (int i = 0; i < enemies; i++)
    {
        int x = rand() % (columns - 2) + 1;
        int y = rand() % (rows - 2) + 1;
        if (tiles[y][x] != EMPTY)
        {
            i--;
        }
        if ( x < 6 || y < 6)
        {
            i--;
            continue;
        }
        tiles[y][x] = ENEMY;
    }

    if (hasEntry)
    {
        tiles[1][0] = EMPTY;
    }

    // Create exit
    tiles[rows - 2][columns - 1] = EMPTY;

    if (isGood(tiles, walls))
    {
        init(tiles, levelWidth, levelHeight);
    }
    else
    {
        GenerateLevel(walls, enemies, coins, columns, rows, levelWidth, levelHeight, hasEntry);
    }
}

void GameLevel::init(std::vector<std::vector<unsigned int>> tileData, unsigned int levelWidth, unsigned int levelHeight)
{
    unsigned int height = tileData.size();
    unsigned int width = tileData[0].size();

    float unitWidth = levelWidth / static_cast<float>(width);
    float unitHeight = levelHeight / static_cast<float>(height);

    for (unsigned int y = 0; y < height; y++)
    {
        for (unsigned int x = 0; x < width; x++)
        {
            if (tileData[y][x] == WALL)
            {
                GameObject obj(glm::vec2(unitWidth * x, unitHeight * y),
                               glm::vec2(unitWidth, unitHeight),
                               ResourceManager::GetTexture("wall"),
                               glm::vec3(1.0f, 1.0f, 1.0f),
                               glm::vec2(0.0f, 0.0f));
                this->Walls.push_back(obj);
            }
            else if (tileData[y][x] == COIN)
            {
                GameObject obj(glm::vec2(unitWidth * x, unitHeight * y),
                               glm::vec2(unitWidth, unitHeight),
                               ResourceManager::GetTexture("coin"),
                               glm::vec3(1.0f, 1.0f, 1.0f),
                               glm::vec2(0.0f, 0.0f));
                this->Coins.push_back(obj);
            }
            else if (tileData[y][x] == ENEMY)
            {
                Enemy obj(glm::vec2(unitWidth * x, unitHeight * y),
                          glm::vec2(unitWidth, unitHeight),
                          ResourceManager::GetTexture("enemy"),
                          glm::vec3(1.0f, 0.0f, 0.0f),
                          glm::vec2(100.0f, 100.0f));
                this->Enemies.push_back(obj);
            }
        }
    }
}

void GameLevel::Draw(SpriteRenderer &renderer)
{
    for (GameObject &wall : this->Walls)
    {
        wall.Draw(renderer);
    }
    for (GameObject &coin : this->Coins)
    {
        coin.Draw(renderer);
    }
    for (GameObject &enemy : this->Enemies)
    {
        enemy.Draw(renderer);
    }
}

bool GameLevel::isGood(std::vector<std::vector<unsigned int>> tileData, int Walls)
{
    unsigned int height = tileData.size();
    unsigned int width = tileData[0].size();

    std::queue<std::pair<int, int>> q;
    bool visited[height][width] = {false};
    q.push(std::make_pair(1, 1));

    while (q.size())
    {
        auto p = q.front();
        q.pop();
        if (p.first < 0 || p.first >= height || p.second < 0 || p.second >= width)
        {
            continue;
        }
        if (visited[p.first][p.second])
        {
            continue;
        }
        visited[p.first][p.second] = true;
        if (tileData[p.first][p.second] == WALL)
        {
            Walls--;
            continue;
        }
        q.push(std::make_pair(p.first - 1, p.second));
        q.push(std::make_pair(p.first + 1, p.second));
        q.push(std::make_pair(p.first, p.second + 1));
        q.push(std::make_pair(p.first, p.second - 1));
    }

    for (int i = 1; i < height - 1; i++)
    {
        for (int j = 1; j < width - 1; j++)
        {
            if (visited[i][j] == false)
            {
                return false;
            }
        }
    }
    if (visited[height - 2][width - 1] == false)
    {
        return false;
    }

    return true;
}