#include <GLFW/glfw3.h>
#include <cstdlib>
#include <vector>
#include <iostream>

#include "game_object.h"

GameObject::GameObject()
    : Position(0.0f, 0.0f), Size(20.0f, 20.0f), Velocity(0.0f), Color(1.0f), Rotation(0.0f), Sprite(), Destroyed(false) {}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity)
    : Position(pos), Size(size), Velocity(velocity), Color(color), Rotation(0.0f), Sprite(sprite), Destroyed(false) {}

void GameObject::Draw(SpriteRenderer &renderer)
{
    if (!this->Destroyed)
        renderer.DrawSprite(this->Sprite, this->Position, this->Size, this->Rotation, this->Color);
}

UFO::UFO() : GameObject(), deltaX(0.0f), deltaY(0.0f) {}
UFO::UFO(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity)
    : GameObject(pos, size, sprite, color, velocity), deltaX(0.0f), deltaY(0.0f) {}

void UFO::ProcessInput(bool Keys[], float dt)
{
    deltaX = 0.0f;
    deltaY = 0.0f;
    float velocityX = this->Velocity.x * dt;
    float velocityY = this->Velocity.y * dt;
    if (Keys[GLFW_KEY_LEFT] || Keys[GLFW_KEY_A])
    {
        this->Position.x -= velocityX;
        deltaX -= velocityX;
    }
    if (Keys[GLFW_KEY_RIGHT] || Keys[GLFW_KEY_D])
    {
        this->Position.x += velocityX;
        deltaX += velocityX;
    }
    if (Keys[GLFW_KEY_UP] || Keys[GLFW_KEY_W])
    {
        this->Position.y -= velocityY;
        deltaY -= velocityY;
    }
    if (Keys[GLFW_KEY_DOWN] || Keys[GLFW_KEY_S])
    {
        this->Position.y += velocityY;
        deltaY += velocityY;
    }
}

Enemy::Enemy() : GameObject() {}
Enemy::Enemy(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec3 color, glm::vec2 velocity)
    : GameObject(pos, size, sprite, color, velocity), deltaX(0.0f), deltaY(0.0f) {}

void Enemy::Move(glm::vec2 pos,float dt)
{
    deltaX = 0;
    deltaY = 0;
    int r = pos.x - this->Position.x;
    if (r >= 0)
    {
        this->Position.x += this->Velocity.x * dt;
        deltaX = this->Velocity.x * dt;
    }
    else
    {
        this->Position.x -= this->Velocity.x * dt;
        deltaX = -this->Velocity.x * dt;
    }
    r = pos.y - this->Position.y;
    if (r >= 0)
    {
        this->Position.y += this->Velocity.y * dt;
        deltaY = this->Velocity.y * dt;
    }
    else
    {
        this->Position.y -= this->Velocity.y * dt;
        deltaY = -this->Velocity.y * dt;
    }
}
