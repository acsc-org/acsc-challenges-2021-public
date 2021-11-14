#pragma once
#include "Entity.h"
#include "SDL.h"
#include "TextureLoader.h"

enum TileName {
	BLACK,
	GREEN,
	PURPLE,
	PLAYER
};


class TileComponent : public Component
{
private:
	SDL_Texture* texture;
	SDL_Rect srcRect, destRect;
public:

	SDL_Rect tileRect;
	int tileID;
	char* path;

	TileComponent() = default;
	TileComponent(Entity* en, int x, int y, TileName id) : Component(en)
	{
		tileRect.x = x;
		tileRect.y = y;
		tileRect.w = 48;
		tileRect.h = 48;
		tileID = id;
		switch (tileID)
		{
		case PLAYER:
			path = (char*)"assets/player.png";
			break;
		case BLACK:
			path = (char*)"assets/black.png";
			break;
		case GREEN:
			path = (char*)"assets/green.png";
			break;
		case PURPLE:
			path = (char*)"assets/purple.png";
			break;
		default:
			break;
		}
	}

	void init() override
	{
		setTexture(path);
		srcRect.x = srcRect.y = 0;
		srcRect.w = this->tileRect.w;
		srcRect.h = this->tileRect.h;
	}
	void setTexture(const char* path)
	{
		texture = TextureLoader::LoadTexture(path);
	}
	void render() override
	{
		TextureLoader::Draw(texture, srcRect, destRect);
	}
	void update() override
	{
		destRect.x = static_cast<int>(entity->get_pos_x());
		destRect.y = static_cast<int>(entity->get_pos_y());
		destRect.w = this->tileRect.w;
		destRect.h = this->tileRect.h;
	}
};
