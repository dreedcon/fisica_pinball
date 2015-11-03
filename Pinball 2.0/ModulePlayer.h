#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

class PhysBody;

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();

public:

	PhysBody* flp1;
	PhysBody* flp1_circl;
	

	PhysBody* flp2;
	PhysBody* flp2_circl;
	

	PhysBody* spr1;
	PhysBody* spr2;
};