#pragma once
#include "Player.h"
#include "Entity.h"
#include "TileComponent.h"

class Ground : public Entity
{
private:

	TileComponent* tile;
public:
	TileName type;
	Ground(int x, int y, TileName id) {
		this->pos_x = x;
		this->pos_y = y;
		this->type = id;
		tile = new TileComponent(this, this->pos_x, this->pos_y, id);
		tile->init();
	}
	void update() override
	{
		tile->update();
	}
	void render()  override
	{
		tile->render();
	}
};