#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "p2Point.h"
#include "math.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	mouse_joint = NULL;
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	world->SetContactListener(this);

	// needed to create joints like mouse joint
	b2BodyDef bd;
	ground = world->CreateBody(&bd);

	return true;
}

// 
update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);

	for(b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if(c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			PhysBody* pb2 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			if(pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}

	return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreatePinball(){

	//Pinball Skeleton----------------

	int sonic[54] = {
		484, 29,
		484, 563,
		618, 656,
		618, 663,
		611, 663,
		611, 695,
		620, 695,
		620, 766,
		761, 766,
		761, 697,
		769, 697,
		769, 665,
		763, 665,
		763, 658,
		895, 568,
		894, 234,
		796, 169,
		894, 140,
		902, 155,
		901, 658,
		928, 658,
		929, 63,
		900, 28,
		628, 29,
		601, 64,
		577, 28,
		484, 28
	};


	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(0, 0);

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[27];

	for (uint i = 0; i < 27; i++)
	{
		p[i].x = PIXEL_TO_METERS(sonic[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(sonic[i * 2 + 1]);
	}

	shape.CreateLoop(p, 27);

	b2FixtureDef fixture;
	fixture.shape = &shape;
	

	b->CreateFixture(&fixture);

	delete[] p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	pbody->width = pbody->height = 0;
	bodies.add(pbody);
	return pbody;
}

PhysBody* ModulePhysics::CreatePolygon(int x, int y, int* points, uint count, body_type tbody, float density, float restitution, bool isSensor, int collision_type)
{
	b2BodyDef body;

	if (tbody == b_static)
		body.type = b2_staticBody;
	if (tbody == b_dynamic)
		body.type = b2_dynamicBody;
	if (tbody == b_kinematic)
		body.type = b2_kinematicBody;

	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	body.angle = 0.0f;

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape shape;
	b2Vec2* p = new b2Vec2[count / 2];
	for (uint i = 0; i < count / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.Set(p, count / 2);

	b2FixtureDef box_fixture;
	box_fixture.shape = &shape;
	box_fixture.density = density;
	box_fixture.restitution = restitution;
	box_fixture.isSensor = isSensor;

	b->CreateFixture(&box_fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	pbody->width = 0;
	pbody->height = 0;
	pbody->collision_type = collision_type;

	delete[] p;

	b->SetUserData(pbody);
	bodies.add(pbody);

	return pbody;
}

void ModulePhysics::CreateRevolJoint(PhysBody* flipbody, PhysBody* circlbody)
{
	b2RevoluteJointDef jointDef;
	jointDef.bodyA = circlbody->body;
	jointDef.bodyB = flipbody->body;

	jointDef.localAnchorA = circlbody->body->GetLocalCenter();
	jointDef.localAnchorB = flipbody->body->GetLocalCenter();

	jointDef.enableMotor = true;
	jointDef.enableLimit = true;
	jointDef.upperAngle = DEGTORAD * 20;
	jointDef.lowerAngle = DEGTORAD * -20;
	
	(b2RevoluteJoint*)world->CreateJoint(&jointDef);
}

void ModulePhysics::CreatePrismaJoint(PhysBody* body1, PhysBody* body2)
{
	b2PrismaticJointDef jointDef;
	jointDef.bodyA = body1->body;
	jointDef.bodyB = body2->body;
	jointDef.collideConnected = false;

	jointDef.localAxisA.Set(0, -1);
	jointDef.localAxisA.Normalize();

	jointDef.localAnchorA = body1->body->GetLocalCenter();
	jointDef.localAnchorB = body2->body->GetLocalCenter();


	jointDef.enableLimit = true;
	jointDef.upperTranslation = 2;
	jointDef.lowerTranslation = 0;
	

	(b2PrismaticJoint*)world->CreateJoint(&jointDef);
}

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius, body_type tbody, int collision_type)
{
	b2BodyDef body;

	if (tbody == b_static)
		body.type = b2_staticBody;
	if (tbody == b_dynamic)
		body.type = b2_dynamicBody;
	if (tbody == b_kinematic)
		body.type = b2_kinematicBody;

	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.density = 2.0f;

	if (body.type == b2_staticBody){
		fixture.restitution = 1.5f;
	}

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;
	pbody->collision_type = collision_type;
	bodies.add(pbody);

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height, body_type tbody, bool isSensor, int collision_type)
{
	b2BodyDef body;

	if (tbody == b_static)
		body.type = b2_staticBody;
	if (tbody == b_dynamic)
		body.type = b2_dynamicBody;
	if (tbody == b_kinematic)
		body.type = b2_kinematicBody;

	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.isSensor = isSensor;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;
	pbody->collision_type = collision_type;
	bodies.add(pbody);

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangleSensor(int x, int y, int width, int height, int collision_type)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width;
	pbody->height = height;
	pbody->collision_type = collision_type;

	bodies.add(pbody);

	return pbody;
}

PhysBody* ModulePhysics::CreateChain(int x, int y, int* points, int size, body_type tbody)
{
	b2BodyDef body;

	if (tbody == b_static)
		body.type = b2_staticBody;
	if (tbody == b_dynamic)
		body.type = b2_dynamicBody;
	if (tbody == b_kinematic)
		body.type = b2_kinematicBody;

	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for(uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.shape = &shape;

	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = 0;
	bodies.add(pbody);

	return pbody;
}

// 
update_status ModulePhysics::PostUpdate()
{
	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for(b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch(f->GetType())
			{
				// Draw circles ------------------------------------------------
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();
					App->renderer->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
				}
				break;

				// Draw polygons ------------------------------------------------
				case b2Shape::e_polygon:
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
					int32 count = polygonShape->GetVertexCount();
					b2Vec2 prev, v;

					for(int32 i = 0; i < count; ++i)
					{
						v = b->GetWorldPoint(polygonShape->GetVertex(i));
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

						prev = v;
					}

					v = b->GetWorldPoint(polygonShape->GetVertex(0));
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
				}
				break;

				// Draw chains contour -------------------------------------------
				case b2Shape::e_chain:
				{
					b2ChainShape* shape = (b2ChainShape*)f->GetShape();
					b2Vec2 prev, v;

					for(int32 i = 0; i < shape->m_count; ++i)
					{
						v = b->GetWorldPoint(shape->m_vertices[i]);
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
						prev = v;
					}

					v = b->GetWorldPoint(shape->m_vertices[0]);
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
				}
				break;

				// Draw a single segment(edge) ----------------------------------
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
					b2Vec2 v1, v2;

					v1 = b->GetWorldPoint(shape->m_vertex0);
					v1 = b->GetWorldPoint(shape->m_vertex1);
					App->renderer->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
				}
				break;
			}

			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && mouse_collision == NULL)
			{
				b2Vec2 mouse_position;

				mouse_position.x = PIXEL_TO_METERS(App->input->GetMouseX());
				mouse_position.y = PIXEL_TO_METERS(App->input->GetMouseY());

				if (f->TestPoint(mouse_position) == true)
				{
					mouse_collision = b;

					b2MouseJointDef jointdef;
					jointdef.bodyA = ground;
					jointdef.bodyB = mouse_collision;
					jointdef.target = mouse_position;
					jointdef.dampingRatio = 0.5f;
					jointdef.frequencyHz = 2.0f;
					jointdef.maxForce = 100.0f * mouse_collision->GetMass();
					mouse_joint = (b2MouseJoint*)world->CreateJoint(&jointdef);
				}
			}

			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT && mouse_joint != NULL)
			{
				b2Vec2 mouse_position;

				mouse_position.x = PIXEL_TO_METERS(App->input->GetMouseX());
				mouse_position.y = PIXEL_TO_METERS(App->input->GetMouseY());

				mouse_joint->SetTarget(mouse_position);

				App->renderer->DrawLine(App->input->GetMouseX(), App->input->GetMouseY(), METERS_TO_PIXELS(mouse_collision->GetPosition().x), METERS_TO_PIXELS(mouse_collision->GetPosition().y), 255, 0, 0);
			}

			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP && mouse_joint != NULL)
			{
				world->DestroyJoint(mouse_joint);
				mouse_joint = NULL;
				mouse_collision = NULL;
			}
		}
	}

	return UPDATE_CONTINUE;
}


// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");
	p2List_item<PhysBody*>* item = bodies.getFirst();

	while (item != NULL)
	{
		delete item->data;
		item = item->next;
	}

	bodies.clear();
	// Delete the whole physics world!
	delete world;

	return true;
}

void ModulePhysics::DestroyBody(PhysBody* body)
{
	assert(body);
	bodies.del(bodies.findNode(body));
	delete body;
}

void PhysBody::GetPosition(int& x, int &y) const
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

	while(fixture != NULL)
	{
		if(fixture->GetShape()->TestPoint(body->GetTransform(), p) == true)
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

	while(fixture != NULL)
	{
		if(fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			// do we want the normal ?

			float fx = x2 - x1;
			float fy = y2 - y1;
			float dist = sqrtf((fx*fx) + (fy*fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return output.fraction * dist;
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

void ModulePhysics::BeginContact(b2Contact* contact)
{
	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	if(physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if(physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}