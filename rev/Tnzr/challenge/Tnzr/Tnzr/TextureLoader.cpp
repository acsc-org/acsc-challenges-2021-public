#include "TextureLoader.h"
#include "Game.h"

std::map<const char*, SDL_Texture*> TextureLoader::textures;
SDL_Texture* TextureLoader::LoadTexture(const char* path)
{
	if (textures.find(path) != textures.end()) { return textures[path]; }

	SDL_Surface* tempSF = IMG_Load(path);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(Game::renderer, tempSF);
	SDL_FreeSurface(tempSF);
	textures[path] = tex;
	return tex;
}

void TextureLoader::Draw(SDL_Texture* tex, SDL_Rect src, SDL_Rect dest)
{
	SDL_RenderCopy(Game::renderer, tex, &src, &dest);
}