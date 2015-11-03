#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModulePhysics.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleTextures.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	//Flippers
	//Fliper 1

	int flip1[12] = {
		691, 681,
		636, 691,
		630, 684,
		630, 677,
		635, 670,
		691, 681
	};

	flp1 = App->physics->CreatePolygon(10, 2, flip1, 12, b_dynamic, 2.0f, 0.0f, false, 0);
	flp1_circl = App->physics->CreateCircle(634 + 3, 684 - 5, 2, b_static,0);
	App->physics->CreateRevolJoint(flp1, flp1_circl);

	//Flipper 2
	int flip2[12] = {
		688, 680,
		743, 670,
		749, 678,
		749, 685,
		743, 693,
		688, 682
	};

	flp2 = App->physics->CreatePolygon(0, 0, flip2, 12, b_dynamic, 2.0f, 0.0f, false, 0);
	flp2_circl = App->physics->CreateCircle(748 - 5, 684 - 4, 2, b_static,0);
	App->physics->CreateRevolJoint(flp2, flp2_circl);

	//Spring
	spr1 = App->physics->CreateRectangle(915, 670, 10, 10, b_static,false, 0);
	spr2 = App->physics->CreateRectangle(915, 620, 23, 24, b_dynamic,false, 0);
	App->physics->CreatePrismaJoint(spr1, spr2);

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	App->physics->DestroyBody(flp1);
	App->physics->DestroyBody(flp1_circl);
	App->physics->DestroyBody(flp2);
	App->physics->DestroyBody(flp2_circl);
	App->physics->DestroyBody(spr1);
	App->physics->DestroyBody(spr2);

	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{
	int x, y;

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		flp1->body->ApplyAngularImpulse(DEGTORAD * -160, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		flp2->body->ApplyAngularImpulse(DEGTORAD * 160, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_UP)
	{
		flp1->body->ApplyAngularImpulse(DEGTORAD * 100, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_UP)
	{
		flp2->body->ApplyAngularImpulse(DEGTORAD * -100, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		spr2->body->ApplyForceToCenter(b2Vec2(0, 40), true);
	}
		spr2->body->ApplyForceToCenter(b2Vec2(0, -40), true);
	
	flp1_circl->GetPosition(x, y);
	App->renderer->Blit(App->scene_intro->flp1text, x - 20, y - 8, NULL, 1.0f, flp1->GetRotation());

	flp2_circl->GetPosition(x, y);
	App->renderer->Blit(App->scene_intro->flp2text, x - 40, y - 10, NULL, 1.0f, flp2->GetRotation());
	
	spr2->GetPosition(x, y);
	App->renderer->Blit(App->scene_intro->piston, x, y);

	return UPDATE_CONTINUE;
}



