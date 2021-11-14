#pragma once
#include "Entity.h"
#include "Game.h"

class KeyboardController : public Component
{
private:
	int cd[3] = { 0 };
public:
	KeyboardController() = default;
	KeyboardController(Entity* en) : Component(en)
	{

	}
	void update() override
	{
		for (int i = 0; i < sizeof(cd) / sizeof(cd[0]); i++){
			if (cd[i])
				cd[i] -= 1;
		}
		if (Game::event.type == SDL_KEYUP)
		{
			switch (Game::event.key.keysym.sym)
			{
			case SDLK_w:
				entity->set_velocity_y(0);
				break;
			case SDLK_a:
				entity->set_velocity_x(0);
				break;
			case SDLK_d:
				entity->set_velocity_x(0);
				break;
			case SDLK_s:
				entity->set_velocity_y(0);
				break;
			default:
				break;
			}
		}
		const Uint8* state = SDL_GetKeyboardState(NULL);
		if (state[SDL_SCANCODE_W]) { entity->set_velocity_y(-1); }
		if (state[SDL_SCANCODE_S]) { entity->set_velocity_y(1); }
		if (state[SDL_SCANCODE_A]) { entity->set_velocity_x(-1); }
		if (state[SDL_SCANCODE_D]) { entity->set_velocity_x(1); }
		if (state[SDL_SCANCODE_N]) { 
			if (!cd[0]) {
				Game::NextLayer();
				cd[0] = 30;
			}
		}
		if (state[SDL_SCANCODE_P]) { 
			if (!cd[1]) {
				Game::PreviousLayer();
				cd[1] = 30;
			}
		}
		if (state[SDL_SCANCODE_SPACE]) {
			if (!cd[2]) {
				Game::Edit(entity->get_pos_x(), entity->get_pos_y());
				cd[2] = 15;
			}
		}
		if (state[SDL_SCANCODE_C]) {
			Game::Check();
		}
	}
};