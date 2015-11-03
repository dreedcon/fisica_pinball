#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

public:

	PhysBody* sensor;
	PhysBody* pyramid;
	PhysBody* pBall;

	//Bouncers
	PhysBody* bounc1;
	PhysBody* bounc2;
	PhysBody* bounc3;
	PhysBody* bounc4;
	
	bool sensed;

	SDL_Texture* scenario;
	SDL_Texture* scenario2;
	SDL_Texture* pyramidText;
	SDL_Texture* pBallText;
	SDL_Texture* flp1text;
	SDL_Texture* flp2text;
	SDL_Texture* piston;

	uint boing_fx;
	p2Point<int> ray;
	bool ray_on;

	uint lifes;
	uint puntuation;
	uint last_punt;

	//Paint Animations Vars
	SDL_Texture* PyramidAnim;
	SDL_Texture* BouncerAnim;
	SDL_Texture* RightObjectAnim;

	bool paintBouncer = false;
	bool paintPyramid = false;
	bool paintRightObject = false;

	int x, y, fpspyram = 0, fpsbouncer = 0;

};
