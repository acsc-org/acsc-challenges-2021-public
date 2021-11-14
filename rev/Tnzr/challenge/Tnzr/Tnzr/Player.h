#pragma once
#include "Game.h"
#include "KeyboardController.h"
#include "TileComponent.h"

class Player : public Entity
{
private:
	std::vector<Component*> components;
public:
	Player(int x = 0, int y = 0) {
		this->pos_x = x;
		this->pos_y = y;
		this->speed = 3;
		components.push_back(new KeyboardController(this));
		components.push_back(new TileComponent(this, this->pos_x, this->pos_y, PLAYER));
		for (auto& c : components) c->init();
	}
	void update() override
	{
		for (auto& c : components) c->update();
		int t;
		t = pos_x + velocity_x * speed;
		if (t <= 0)
			t = 0;
		else if (t >= W_WIDTH - 47)
			t = W_WIDTH - 47;
		pos_x = t;

		t = pos_y + velocity_y * speed;
		if (t <= 0)
			t = 0;
		else if (t >= W_HEIGHT - 47)
		{
			t = W_HEIGHT - 47;
		}
		pos_y = t;
	}
	void render()  override
	{
		for (auto& c : components) c->render();
	}

};

