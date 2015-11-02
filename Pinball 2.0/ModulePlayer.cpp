#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModulePhysics.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"

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

	int flip1[8] = {
		630, 660,
		675, 700,
		630, 686,
		630, 660
	};

	flp1 = App->physics->CreateFlipper(10, 2, flip1, 8, 1.0f, 0.0f, false);
	flp1_circl = App->physics->CreateStaticCircle(634 + 10, 684, 2);
	App->physics->CreateJoint(flp1, flp1_circl);

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		flp1->body->ApplyAngularImpulse(DEGTORAD * -360, true);
	}

	//if (App->scene_intro->){

	//}
	return UPDATE_CONTINUE;
}



