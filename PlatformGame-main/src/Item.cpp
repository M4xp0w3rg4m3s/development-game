#include "Item.h"
#include "Engine.h"
#include "Textures.h"
#include "Render.h"
#include "Log.h"
#include "Physics.h"
#include "EntityManager.h"
#include "Sprite.h"

Item::Item() : Entity(EntityType::ITEM)
{
	name = "item";

	this->texH = 32;
	this->texW = 32;

	currentType = ItemType::COIN;

	//initilize textures
	texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/Items/goldCoin.png");
}

Item::Item(const char* texturePath, int texW, int texH, ItemType currentType) : Entity(EntityType::ITEM)
{
	name = "item";

	this->texH = texH;
	this->texW = texW;

	this->currentType = currentType;

	//initilize textures
	switch (currentType)
	{
	case ItemType::SHURIKEN:
		texture = Engine::GetInstance().textures.get()->Load(texturePath);
		animator = new Sprite(texture);

		animator->SetNumberAnimations(1);
		animator->AddKeyFrame(0, { 0,0,13,13 });
		animator->AddKeyFrame(0, { 1 * 13,0,13,13 });
		animator->SetAnimationDelay(0, 100);
		animator->SetAnimation(0);
		animator->SetLoop(true);
		break;
	case ItemType::COIN:
		texture = Engine::GetInstance().textures.get()->Load(texturePath);
		break;
	default:
		break;
	}
}

Item::~Item() 
{
	delete animator;
}

bool Item::Awake() {
	return true;
}

bool Item::Start() {
	//Add a physics to an item - initialize the physics body
	Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);
	pbody = Engine::GetInstance().physics.get()->CreateCircle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texH / 2, bodyType::DYNAMIC);

	// Assign collider type
	pbody->ctype = ColliderType::ITEM;

	pbody->body->SetGravityScale(0);

	return true;
}

bool Item::Update(float dt)
{
	//Add a physics to an item - update the position of the object from the physics.  
	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texH / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	if (animator != nullptr)
	{
		animator->Update();
		animator->Draw((int)position.getX(), (int)position.getY(), 0, 0);
	}
	else
	{
		Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY());
	}
	
	return true;
}

bool Item::CleanUp()
{
	Engine::GetInstance().textures.get()->UnLoad(texture);
	Engine::GetInstance().physics->DeletePhysBody(pbody);

	return true;
}

void Item::OnCollision(PhysBody* physA, PhysBody* physB)
{
	switch (physB->ctype)
	{
	case ColliderType::PLAYER:
		Engine::GetInstance().entityManager->DeleteEntity(this);
		break;
	case ColliderType::UNKNOWN:
		LOG("Collision UNKNOWN");
		break;
	default:
		break;
	}
}
