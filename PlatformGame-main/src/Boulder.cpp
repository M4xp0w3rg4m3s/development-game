#include "Boulder.h"
#include "Engine.h"
#include "Textures.h"
#include "Audio.h"
#include "Input.h"
#include "Render.h"
#include "Scene.h"
#include "Log.h"
#include "Physics.h"

Boulder::Boulder() : Entity(EntityType::BOULDER)
{
	name = "boulder";
}

Boulder::~Boulder() {}

bool Boulder::Awake() {
	return true;
}

bool Boulder::Start() {

	// Initilize textures
	texture = Engine::GetInstance().textures.get()->Load("Assets/Textures/boulder.png");

	// Add a physics to the boulder - initialize the physics body
	Engine::GetInstance().textures.get()->GetSize(texture, texW, texH);
	pbody = Engine::GetInstance().physics.get()->CreateRectangle((int)position.getX() + texH / 2, (int)position.getY() + texH / 2, texW-4, texH-2, bodyType::DYNAMIC);

	pbody->body->GetFixtureList()[0].SetFriction(100.0f);

	// Assign collider type
	pbody->ctype = ColliderType::BOULDER;

	//// Add friction and weight
	b2MassData boulderMass;
	boulderMass.mass = 500.0f;
	boulderMass.center = pbody->body->GetLocalCenter();
	pbody->body->SetMassData(&boulderMass);

	pbody->body->SetFixedRotation(true);


	return true;
}

void Boulder::SetNewPos(b2Vec2 pos)
{
	pbody->body->SetTransform({ PIXEL_TO_METERS(pos.x) + 1, PIXEL_TO_METERS(pos.y) }, 0);

}

bool Boulder::Update(float dt)
{
	// Boulder physics - update the position of the object from the physics.  

	b2Transform pbodyPos = pbody->body->GetTransform();
	position.setX(METERS_TO_PIXELS(pbodyPos.p.x) - texW / 2);
	position.setY(METERS_TO_PIXELS(pbodyPos.p.y) - texH / 2);

	Engine::GetInstance().render.get()->DrawTexture(texture, (int)position.getX(), (int)position.getY()+2);

	return true;
}

bool Boulder::CleanUp()
{

	Engine::GetInstance().physics->DeletePhysBody(pbody);

	return true;
}