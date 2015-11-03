#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleWindow.h"

/*
Collision type

Flippers type 0
Bouncers type 1;
MiddleObjects type 2;
Pyramid type 3;
RightObject type 4;

*/

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ray_on = false;
	sensed = false;

	lifes = 4;
	puntuation = 0;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	// Music
	App->audio->PlayMusic("pinball/music.ogg", 0.0f);

	scenario = App->textures->Load("pinball/sonic.png");
	scenario2 = App->textures->Load("pinball/sonic2.png");
	pBallText = App->textures->Load("pinball/sonic_ball.png"); 
	boing_fx = App->audio->LoadFx("pinball/boing.wav");
	pyramidText = App->textures->Load("pinball/pyramid.png");

	//Animations
	PyramidAnim = App->textures->Load("pinball/pyramidAnim.png");
	BouncerAnim = App->textures->Load("pinball/bouncAnim.png");

	flp1text = App->textures->Load("pinball/flipper1.png");
	flp2text = App->textures->Load("pinball/flipper2.png");
	piston = App->textures->Load("pinball/piston.png");

	//Pinball Skeleton--------
	App->physics->CreatePinball();

	//Objectes--------------

	//Objecte1----
	int objecte1[8] = {
		532, 480,
		532, 555,
		610, 601,
		535, 480
	};

	App->physics->CreatePolygon(0, 0, objecte1, 8, b_static, 1.0f, 1.5f, false, 2);

	//Objecte2----
	int objecte2[8] = {
		840, 482,
		766, 600,
		842, 551,
		842, 483
	};

	App->physics->CreatePolygon(0, 0, objecte2, 8, b_static, 1.0f, 1.5f, false,2);

	//Objecte 3----
	int objecte3[14] = {
		868, 364,
		891, 325,
		887, 312,
		871, 311,
		847, 349,
		853, 364,
		868, 364
	};

	App->physics->CreatePolygon(0, 0, objecte3, 13, b_static, 1.0f, 1.5f, false, 4);

	//Objecte 4----
	int objecte4[8] = {
		489, 348,
		561, 417,
		490, 414,
		490, 348
	};

	pyramid = App->physics->CreatePolygon(0, 0, objecte4, 8, b_kinematic, 1.0f, 1.0f, false, 3);
	
	//Boles-----
	bounc1 = App->physics->CreateCircle(664, 243, 16, b_static, 1);
	bounc2 = App->physics->CreateCircle(716, 132, 16, b_static, 1);
	bounc3 = App->physics->CreateCircle(762, 383, 16, b_static, 1);
	bounc4 = App->physics->CreateCircle(618, 391, 16, b_static, 1);
	//-----------------------

	//Sensors

	sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 6, SCREEN_WIDTH, 50, 0);

	//Player Ball--------------
	pBall = App->physics->CreateCircle(915, 454, 12, b_dynamic, 0);
	pBall->listener = this;

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	App->textures->Unload(scenario);
	App->textures->Unload(scenario2);
	App->textures->Unload(pBallText);
	App->textures->Unload(pyramidText);
	App->textures->Unload(PyramidAnim);
	App->textures->Unload(BouncerAnim);
	App->textures->Unload(flp1text);
	App->textures->Unload(flp2text);
	App->textures->Unload(piston);
	
	App->physics->DestroyBody(bounc1);
	App->physics->DestroyBody(bounc2);
	App->physics->DestroyBody(bounc3);
	App->physics->DestroyBody(bounc4);

	App->physics->DestroyBody(sensor);
	App->physics->DestroyBody(pyramid);
	App->physics->DestroyBody(pBall);

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	App->renderer->Blit(scenario, 0, 0);
	
	//Pyramid settings----------------------
	int pyra_x, pyra_y; //To get a pyramid position
	int ball_x, ball_y;

	pyramid->GetPosition(pyra_x, pyra_y);

	if (pyra_y <= -200)
		pyramid->body->SetLinearVelocity(b2Vec2(0, 1));
	if (pyra_y >= 0)
		pyramid->body->SetLinearVelocity(b2Vec2(0, -1));

	App->renderer->Blit(pyramidText, pyra_x + 490, pyra_y + 348);
	//--------------------------------------
	
	//Ball settings-------------------------
	pBall->GetPosition(ball_x, ball_y);

	App->renderer->Blit(pBallText, ball_x,ball_y, NULL, 1.0f, pBall->GetRotation());

	if (ball_y >= 730){

		pBall->body->SetTransform(b2Vec2(PIXEL_TO_METERS(915), PIXEL_TO_METERS(454)), 0.0f);
		lifes--;
	}

	if (lifes <= 0)
	{
		last_punt = puntuation;
		puntuation = 0;
		lifes = 4;
	}
	//----------------------------------------


	//DrawAnim---------------------------------
	if (paintBouncer == true){
		App->renderer->Blit(BouncerAnim, x, y);
		fpsbouncer++;
	}
	if (paintPyramid == true){
		App->renderer->Blit(PyramidAnim, pyra_x + 490, pyra_y + 348);
		fpspyram++;
	}
	if (paintRightObject == true){
		App->renderer->Blit(RightObjectAnim, x, y);
	}

	if (fpsbouncer == 20){
		paintBouncer = false;
		fpsbouncer = 0;
	}

	if (fpspyram == 20){
		paintPyramid = false;
		fpspyram = 0;
	}
	
	//-------------------------------------

	char title[50];
	sprintf_s(title, "Balls: %d Score: %06d Last Score: %06d", lifes, puntuation, last_punt);
	App->window->SetTitle(title);

	App->renderer->Blit(scenario2, 0, 0);
	
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int collided = 0;
	bool colliding = true;

	while (colliding && collided == 0){

		if (bodyB != NULL){

			if (bodyB->collision_type == 0){
				collided = 1;
			}

			if (bodyB->collision_type == 1){

				collided = 1;
				App->audio->PlayFx(boing_fx);

				paintBouncer = true;
				bodyB->GetPosition(x, y);

				puntuation = puntuation + 30;
	
			}

			if (bodyB->collision_type == 2){
				collided = 1;
				puntuation = puntuation + 10;
				App->audio->PlayFx(boing_fx);
			}

			if (bodyB->collision_type == 3){
				collided = 1;
				
				App->audio->PlayFx(boing_fx);

				paintPyramid = true;
				bodyB->GetPosition(x, y);

				puntuation = puntuation + 5;
			}

			if (bodyB->collision_type == 4){
				collided = 1;
				puntuation = puntuation + 50;
				App->audio->PlayFx(boing_fx);

				paintRightObject = true;
				bodyB->GetPosition(x, y);
			}
		}
		else{
			colliding = false;
		}
	}
	
}