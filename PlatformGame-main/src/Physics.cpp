#include "Physics.h"
#include "Input.h"
#include "Engine.h"
#include "Log.h"
#include "math.h"
#include "SDL2/SDL_keycode.h"
#include "Log.h"
#include "Render.h"
#include "Player.h"
#include "Window.h"
#include "box2D/box2d.h"

Physics::Physics() : Module()
{
	// Initialise all the internal class variables, at least to NULL pointer
	world = NULL;
	debug = true;
	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
}

// Destructor
Physics::~Physics()
{
	// You should do some memory cleaning here, if required
}

bool Physics::Start()
{
	LOG("Creating Physics 2D environment");

	// Create a new World

	// Set this module as a listener for contacts
	world->SetContactListener(this);

	debug = false;

	return true;
}

// 
bool Physics::PreUpdate()
{
	bool ret = true;

	// Step (update) the World
	// WARNING: WE ARE STEPPING BY CONSTANT 1/60 SECONDS!
	float dt = Engine::GetInstance().GetDeltaTime() / 1000;
	world->Step(dt, 6, 2);

	// Because Box2D does not automatically broadcast collisions/contacts with sensors, 
	// we have to manually search for collisions and "call" the equivalent to the ModulePhysics::BeginContact() ourselves...
	for (b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		// For each contact detected by Box2D, see if the first one colliding is a sensor
		if (c->IsTouching() && c->GetFixtureA()->IsSensor())
		{
			// If so, we call the OnCollision listener function (only of the sensor), passing as inputs our custom PhysBody classes
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData().pointer;
			PhysBody* pb2 = (PhysBody*)c->GetFixtureB()->GetBody()->GetUserData().pointer;

			if (pb1 && pb2 && pb1->listener && !IsPendingToDelete(pb1) && !IsPendingToDelete(pb2))
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	return ret;
}

PhysBody* Physics::CreateRectangle(int x, int y, int width, int height, bodyType type)
{
	b2BodyDef body;

	if (type == DYNAMIC) body.type = b2_dynamicBody;
	if (type == STATIC) body.type = b2_staticBody;
	if (type == KINEMATIC) body.type = b2_kinematicBody;

	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	b->ResetMassData();

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->GetUserData().pointer = (uintptr_t)pbody;
	pbody->width = (int)(width * 0.5f);
	pbody->height = (int)(height * 0.5f);

	return pbody;
}

PhysBody* Physics::CreateCircle(int x, int y, int radious, bodyType type)
{
	// Create BODY at position x,y
	b2BodyDef body;

	if (type == DYNAMIC) body.type = b2_dynamicBody;
	if (type == STATIC) body.type = b2_staticBody;
	if (type == KINEMATIC) body.type = b2_kinematicBody;

	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	// Add BODY to the world
	b2Body* b = world->CreateBody(&body);

	// Create SHAPE
	b2CircleShape circle;
	circle.m_radius = PIXEL_TO_METERS(radious);

	// Create FIXTURE
	b2FixtureDef fixture;
	fixture.shape = &circle;
	fixture.density = 1.0f;
	b->ResetMassData();

	// Add fixture to the BODY
	b->CreateFixture(&fixture);

	// Create our custom PhysBody class
	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->GetUserData().pointer = (uintptr_t)pbody;
	pbody->width = (int)(radious * 0.5f);
	pbody->height = (int)(radious * 0.5f);

	// Return our PhysBody class
	return pbody;
}

PhysBody* Physics::CreateCircleSensor(int x, int y, int radious, bodyType type)
{
	// Create BODY at position x,y
	b2BodyDef body;

	if (type == DYNAMIC) body.type = b2_dynamicBody;
	if (type == STATIC) body.type = b2_staticBody;
	if (type == KINEMATIC) body.type = b2_kinematicBody;

	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	// Add BODY to the world
	b2Body* b = world->CreateBody(&body);

	// Create SHAPE
	b2CircleShape circle;
	circle.m_radius = PIXEL_TO_METERS(radious);

	// Create FIXTURE
	b2FixtureDef fixture;
	fixture.shape = &circle;
	fixture.density = 1.0f;
	fixture.isSensor = true;

	// Add fixture to the BODY
	b->CreateFixture(&fixture);

	b->ResetMassData();
	b2MassData totalMass;
	totalMass.mass = 0.f;
	totalMass.center = b->GetLocalCenter();
	b->SetMassData(&totalMass);

	// Create our custom PhysBody class
	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->GetUserData().pointer = (uintptr_t)pbody;
	pbody->width = (int)(radious * 0.5f);
	pbody->height = (int)(radious * 0.5f);

	// Return our PhysBody class
	return pbody;
}

PhysBody* Physics::CreateRectangleSensor(int x, int y, int width, int height, bodyType type)
{
	// Create BODY at position x,y
	b2BodyDef body;
	if (type == DYNAMIC) body.type = b2_dynamicBody;
	if (type == STATIC) body.type = b2_staticBody;
	if (type == KINEMATIC) body.type = b2_kinematicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	// Add BODY to the world
	b2Body* b = world->CreateBody(&body);

	// Create SHAPE
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	// Create FIXTURE
	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.isSensor = true;

	// Add fixture to the BODY
	b->CreateFixture(&fixture);

	// Create our custom PhysBody class
	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->GetUserData().pointer = (uintptr_t)pbody;
	pbody->width = width;
	pbody->height = height;

	// Return our PhysBody class
	return pbody;
}

PhysBody* Physics::CreateChain(int x, int y, int* points, int size, bodyType type)
{
	// Create BODY at position x,y
	b2BodyDef body;
	if (type == DYNAMIC) body.type = b2_dynamicBody;
	if (type == STATIC) body.type = b2_staticBody;
	if (type == KINEMATIC) body.type = b2_kinematicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	// Add BODY to the world
	b2Body* b = world->CreateBody(&body);

	// Create SHAPE
	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];
	for (unsigned int i = 0; i < (unsigned int)size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}
	shape.CreateLoop(p, size / 2);

	// Create FIXTURE
	b2FixtureDef fixture;
	fixture.shape = &shape;

	// Add fixture to the BODY
	b->CreateFixture(&fixture);

	// Clean-up temp array
	delete p;

	// Create our custom PhysBody class
	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	//b->SetUserData(pbody);
	pbody->width = pbody->height = 0;

	// Return our PhysBody class
	return pbody;
}

PhysBody* Physics::CreatePlayer(int x, int y, int width, int height, bodyType type)
{
	b2BodyDef body;

	if (type == DYNAMIC) body.type = b2_dynamicBody;
	if (type == STATIC) body.type = b2_staticBody;
	if (type == KINEMATIC) body.type = b2_kinematicBody;

	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);
	b2PolygonShape bottomBox;
	bottomBox.SetAsBox(PIXEL_TO_METERS(width) * 0.35f, PIXEL_TO_METERS(height) * 0.1f, {0, PIXEL_TO_METERS(height)*0.5f }, 0);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	b2FixtureDef bottomFixture;
	bottomFixture.shape = &bottomBox;
	bottomFixture.density = 1.0f;

	b->ResetMassData();

	b->CreateFixture(&fixture);
	b->CreateFixture(&bottomFixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->GetUserData().pointer = (uintptr_t)pbody;
	pbody->width = (int)(width * 0.5f);
	pbody->height = (int)(height * 0.5f);

	return pbody;

	return nullptr;
}

// 
bool Physics::PostUpdate()
{
	bool ret = true;

	// Activate or deactivate debug mode
	if (Engine::GetInstance().input.get()->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
		debug = !debug;

	//  Iterate all objects in the world and draw the bodies
	if (debug)
	{
		for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
		{
			for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
			{
				switch (f->GetType())
				{
					// Draw circles ------------------------------------------------
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					int width, height;
					Engine::GetInstance().window.get()->GetWindowSize(width, height);
					b2Vec2 pos = f->GetBody()->GetPosition();
					Engine::GetInstance().render.get()->DrawCircle(METERS_TO_PIXELS(pos.x),
						METERS_TO_PIXELS(pos.y),
						METERS_TO_PIXELS(shape->m_radius) * Engine::GetInstance().window.get()->GetScale(),
						255, 255, 255);
				}
				break;

				// Draw polygons ------------------------------------------------
				case b2Shape::e_polygon:
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
					int32 count = polygonShape->m_count;
					b2Vec2 prev, v;

					for (int32 i = 0; i < count; ++i)
					{
						v = b->GetWorldPoint(polygonShape->m_vertices[i]);
						if (i > 0)
							Engine::GetInstance().render.get()->DrawLine(METERS_TO_PIXELS(prev.x),
								METERS_TO_PIXELS(prev.y),
								METERS_TO_PIXELS(v.x),
								METERS_TO_PIXELS(v.y),
								255, 255, 100);

						prev = v;
					}

					v = b->GetWorldPoint(polygonShape->m_vertices[0]);
					Engine::GetInstance().render.get()->DrawLine(METERS_TO_PIXELS(prev.x),
						METERS_TO_PIXELS(prev.y),
						METERS_TO_PIXELS(v.x),
						METERS_TO_PIXELS(v.y),
						255, 255, 100);
				}
				break;

				// Draw chains contour -------------------------------------------
				case b2Shape::e_chain:
				{
					b2ChainShape* shape = (b2ChainShape*)f->GetShape();
					b2Vec2 prev, v;

					for (int32 i = 0; i < shape->m_count; ++i)
					{
						v = b->GetWorldPoint(shape->m_vertices[i]);
						if (i > 0)
							Engine::GetInstance().render.get()->DrawLine(METERS_TO_PIXELS(prev.x),
								METERS_TO_PIXELS(prev.y),
								METERS_TO_PIXELS(v.x),
								METERS_TO_PIXELS(v.y),
								100, 255, 100);
						prev = v;
					}

					v = b->GetWorldPoint(shape->m_vertices[0]);
					Engine::GetInstance().render.get()->DrawLine(METERS_TO_PIXELS(prev.x),
						METERS_TO_PIXELS(prev.y),
						METERS_TO_PIXELS(v.x),
						METERS_TO_PIXELS(v.y),
						100, 255, 100);
				}
				break;

				// Draw a single segment(edge) ----------------------------------
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
					b2Vec2 v1, v2;

					v1 = b->GetWorldPoint(shape->m_vertex0);
					v1 = b->GetWorldPoint(shape->m_vertex1);
					Engine::GetInstance().render.get()->DrawLine(METERS_TO_PIXELS(v1.x),
						METERS_TO_PIXELS(v1.y),
						METERS_TO_PIXELS(v2.x),
						METERS_TO_PIXELS(v2.y),
						100, 100, 255);
				}
				break;
				}

			}
		}
	}


	for (PhysBody* physBody : bodiesToDelete) {
		world->DestroyBody(physBody->body);
	}
	bodiesToDelete.clear();

	return ret;
}

// Called before quitting
bool Physics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}

// Callback function to collisions with Box2D
void Physics::BeginContact(b2Contact* contact)
{
	// Call the OnCollision listener function to bodies A and B, passing as inputs our custom PhysBody classes
	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData().pointer;

	if (physA && physA->listener != NULL && !IsPendingToDelete(physA)) {
		if (physB) // Ensure physB is also valid
		{
			physA->listener->OnCollision(physA, physB);
			physA->activeCollisions++;
		}
	}

	if (physB && physB->listener != NULL && !IsPendingToDelete(physB)) {
		if (physA) // Ensure physA is also valid
		{
			physB->listener->OnCollision(physB, physA);
			physB->activeCollisions++;
		}
	}
}

void Physics::EndContact(b2Contact* contact)
{
	// Call the OnCollision listener function to bodies A and B, passing as inputs our custom PhysBody classes
	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData().pointer;
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData().pointer;

	if (physA && physA->listener != NULL && !IsPendingToDelete(physA)) {
		if (physB) // Ensure physB is also valid
		{
			//physA->listener->OnCollision(physA, physB);
			physA->activeCollisions--;
		}
	}

	if (physB && physB->listener != NULL && !IsPendingToDelete(physB)) {
		if (physA) // Ensure physA is also valid
		{
			//physB->listener->OnCollision(physB, physA);
			physB->activeCollisions--;
		}
	}
}

void Physics::DeletePhysBody(PhysBody* physBody)
{
	bodiesToDelete.push_back(physBody);
}

bool Physics::IsPendingToDelete(PhysBody* physBody) {
	bool isPending = false;
	for (PhysBody* _physBody : bodiesToDelete) {
		if (_physBody == physBody) {
			isPending = true;
			break;
		}
	}

	return isPending;
}

//--------------- PhysBody

void PhysBody::GetPosition(int& x, int& y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x) - (width);
	y = METERS_TO_PIXELS(pos.y) - (height);
}

float PhysBody::GetRotation() const
{
	return RADTODEG * body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	b2Vec2 p(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
			return true;
		fixture = fixture->GetNext();
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while (fixture != NULL)
	{
		if (fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			// do we want the normal ?

			float fx = (float)(x2 - x1);
			float fy = (float)(y2 - y1);
			float dist = sqrtf((fx * fx) + (fy * fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return (int)(output.fraction * dist);
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

void PhysBody::CreateWeld(PhysBody* pbody, Vector2D position)
{
	b2WeldJointDef jointDef;
	jointDef.bodyA = this->body;
	jointDef.bodyB = pbody->body;
	jointDef.localAnchorA.Set(0.0f, 0.0f);
	jointDef.localAnchorB.Set(position.getX(), position.getY());
	jointDef.referenceAngle = 0.0f;
	jointDef.stiffness = 0.0f;
	jointDef.damping = 0.0f;

	b2Joint* weldJoint = Engine::GetInstance().physics.get()->GetWorld()->CreateJoint(&jointDef);
}
