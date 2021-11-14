#include "Game.h"
#include "SDL_ttf.h"
#include <string>
#include <cstdlib>
#include <ctime>
#include "Player.h"
#include "Ground.h"
#include "flag.h"

SDL_Renderer* Game::renderer = nullptr;
SDL_Event Game::event;
int Game::cur_layer = 0;
int Game::state = 0;
bool Game::check = false;
Entity* layers[15 * 15] = { nullptr };
int canvas[FLAG_LEN][15][15] = { 0 };
TTF_Font* Font;
SDL_Color White;
std::vector<Entity*> entities;

Game::Game()
{};
Game::~Game()
{};
void Game::init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen)
{
	int flags = SDL_WINDOW_SHOWN;
	if (fullscreen)
		flags |= SDL_WINDOW_FULLSCREEN;

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "INIT FAIL" << std::endl;
		isRunning = false;
	}
	else
	{
		window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);
		renderer = SDL_CreateRenderer(window, -1, 0);
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		isRunning = true;
	}

	entities.push_back(new Player());

	for (int y = 0; y < W_HEIGHT / 48 ; y++)
	{
		for (int x = 0; x < W_WIDTH / 48; x++)
		{
			Game::AddTile(x, y, (TileName)canvas[0][y][x]);
		}
	}

	TTF_Init();
	Font = TTF_OpenFont("assets/Rationale-Regular.ttf", 72);
	White = { 255,255,255 };
}

void Game::handleEvents()
{
	SDL_PollEvent(&event);
	switch (event.type)
	{
	case SDL_QUIT:
		isRunning = false;
		break;
	default:
		break;
	}
}
void Game::update()
{
	for (auto& e : entities) { e->update(); }
	for (int tile = 0; tile < sizeof(layers)/sizeof(void*); tile++) {
		if (layers[tile]) {
			layers[tile]->update();
		}
	}
	
}

void Game::render()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	if (state && check) {
		std::string msg = "WRONG";
		SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Font, msg.c_str(), White);
		SDL_Texture* Message = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessage);
		SDL_Rect Message_rect;
		Message_rect.x = W_WIDTH / 2 - surfaceMessage->w/2;
		Message_rect.y = W_HEIGHT / 2 - surfaceMessage->h/2;
		Message_rect.w = surfaceMessage->w;
		Message_rect.h = surfaceMessage->h;
		SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
		SDL_FreeSurface(surfaceMessage);
	}
	else if (state && !check) {
		std::string msg = "NICE";
		SDL_Surface* surfaceMessage = TTF_RenderText_Solid(Font, msg.c_str(), White);
		SDL_Texture* Message = SDL_CreateTextureFromSurface(Game::renderer, surfaceMessage);
		SDL_Rect Message_rect;
		Message_rect.x = W_WIDTH / 2 - surfaceMessage->w/2;
		Message_rect.y = W_HEIGHT / 2 - surfaceMessage->h/2;
		Message_rect.w = surfaceMessage->w;
		Message_rect.h = surfaceMessage->h;
		SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
		SDL_FreeSurface(surfaceMessage);
	}
	for (int tile = 0; tile < sizeof(layers) / sizeof(void*); tile++) {
		if (layers[tile]) {
			layers[tile]->render();
		}
	}
	for (auto& p : entities) { p->render(); }
	
	SDL_RenderPresent(renderer);

}
void Game::clean()
{
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();

}

bool Game::running()
{
	return isRunning;
}

void Game::AddTile(int x, int y, TileName id)
{
	layers[y * 15 + x] = new Ground(x*48, y*48, id);
}

void Game::DelTile(int x, int y)
{
	delete layers[y * 15 + x];
	layers[y * 15 + x] = nullptr;
}

void Game::NextLayer() {
	if (cur_layer < FLAG_LEN - 1)
	{
		CleanTiles();
		cur_layer += 1;
		for (int y = 0; y < W_HEIGHT / 48; y++)
		{
			for (int x = 0; x < W_WIDTH / 48; x++)
			{
				Game::AddTile(x, y, (TileName)canvas[cur_layer][y][x]);
			}
		}
	}
}

void Game::PreviousLayer() {
	if (cur_layer > 0)
	{
		CleanTiles();
		cur_layer -= 1;
		for (int y = 0; y < W_HEIGHT / 48; y++)
		{
			for (int x = 0; x < W_WIDTH / 48; x++)
			{
				Game::AddTile(x, y, (TileName)canvas[cur_layer][y][x]);
			}
		}
	}
}

void Game::Edit(int x, int y) {
	int tx = x / 48;
	int ty = y / 48;
	canvas[cur_layer][ty][tx] = (canvas[cur_layer][ty][tx] + 1)%3;
	DelTile(tx, ty);
	AddTile(tx, ty, (TileName)canvas[cur_layer][ty][ tx]);
}

void Game::Check() {
	if (state == 0) {
		bool pass = false;
		for (int ch = 0; ch < FLAG_LEN; ch++) {
			for (int row = 0; row < 15; row++) {
				for (int col = 0; col < 15; col++) {
					for (int k = 0; k < 15; k++) {
						target[ch][row][col] -= canvas[ch][row][k] * rand_key[ch][k][col];
					}
					pass = pass || (target[ch][row][col]);
				}
			}
		}
		check = pass;
		state = 1;
		CleanTiles();
		entities.clear();
	}
}

void Game::CleanTiles()
{
	for (int y = 0; y < W_HEIGHT / 48; y++)
	{
		for (int x = 0; x < W_WIDTH / 48; x++)
		{
			Game::DelTile(x, y);
		}
	}
}