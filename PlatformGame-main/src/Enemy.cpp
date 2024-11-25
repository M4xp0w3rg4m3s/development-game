#include "Enemy.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"
#include "Map.h"

Enemy::Enemy(EntityType type) : Entity(type)
{

}

Enemy::~Enemy() {
	delete animator;
	delete pathfinding;
}

bool Enemy::Awake() {
	return true;
}

bool Enemy::Start() {
	return true;
}

bool Enemy::Update(float dt)
{
	return true;
}

bool Enemy::CleanUp()
{
	return true;
}

void Enemy::SetPosition(Vector2D pos) {
	pos.setX(pos.getX() + texW / 2);
	pos.setY(pos.getY() + texH / 2);
	b2Vec2 bodyPos = b2Vec2(PIXEL_TO_METERS(pos.getX()), PIXEL_TO_METERS(pos.getY()));
	pbody->body->SetTransform(bodyPos, 0);
}

Vector2D Enemy::GetPosition() {
	b2Vec2 bodyPos = pbody->body->GetTransform().p;
	Vector2D pos = Vector2D(METERS_TO_PIXELS(bodyPos.x), METERS_TO_PIXELS(bodyPos.y));
	return pos;
}
Vector2D Enemy::GetCenterPosition()
{
	// Get the body's position in meters
	b2Vec2 bodyPos = pbody->body->GetTransform().p;

	// Convert width and height to meters for accurate calculations
	float halfWidthInMeters = PIXEL_TO_METERS(width) / 2.0f;
	float halfHeightInMeters = PIXEL_TO_METERS(height) / 2.0f;

	// Calculate the center position in meters
	b2Vec2 centerPosInMeters = { bodyPos.x , bodyPos.y };

	// Convert center position back to pixels
	Vector2D centerPos = Vector2D(METERS_TO_PIXELS(centerPosInMeters.x), METERS_TO_PIXELS(centerPosInMeters.y));

	return centerPos;
}
void Enemy::ResetPath() {
	Vector2D pos = GetPosition();
	Vector2D tilePos = Engine::GetInstance().map.get()->WorldToMap(pos.getX(), pos.getY());
	pathfinding->ResetPath(tilePos);
}

void Enemy::SetPathfindingType(EnemyType type)
{
	pathfinding->SetEnemyType(type);
}
