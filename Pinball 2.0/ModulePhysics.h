#pragma once
#include "Module.h"
#include "Globals.h"
#include "Box2D/Box2D/Box2D.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -7.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

// Small class to return to other modules to track position and rotation of physics bodies

enum body_type
{
	b_dynamic,
	b_static,
	b_kinematic
};

class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL)
	{}

	void GetPosition(int& x, int &y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;

public:
	int collision_type;
	int width, height;
	b2Body* body;
	Module* listener;
};

// Module --------------------------------------
class ModulePhysics : public Module, public b2ContactListener // TODO
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();
	void DestroyBody(PhysBody* body);

	PhysBody* CreateCircle(int x, int y, int radius, body_type tbody, int collision_type);
	PhysBody* CreateRectangle(int x, int y, int width, int height, body_type tbody, bool isSensor, int collision_type);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height, int collision_type);
	PhysBody* CreateChain(int x, int y, int* points, int size, body_type tbody);
	PhysBody* CreatePinball();
	PhysBody* CreatePolygon(int x, int y, int* points, uint count, body_type tbody, float density, float restitution, bool isSensor, int collision_type);
	
	//Joints
	void CreateRevolJoint(PhysBody* flipbody, PhysBody* circlbody);
	void CreatePrismaJoint(PhysBody* body1, PhysBody* body2);

	// b2ContactListener ---
	void BeginContact(b2Contact* contact);

private:

	bool debug;
	b2World* world;
	p2List<PhysBody*> bodies;
	b2MouseJoint* mouse_joint;
	b2Body* ground;
	b2Body* mouse_collision;
};