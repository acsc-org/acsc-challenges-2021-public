#pragma once
#include "SDL.h"
#include "SDL_image.h"
#include <iostream>
#include <vector>
#include "TileComponent.h"

constexpr auto FLAG_LEN = 35;
constexpr auto W_WIDTH = 720;
constexpr auto W_HEIGHT = 720;
constexpr auto BLOCK_W = W_WIDTH / 48;
constexpr auto BLOCK_H = W_HEIGHT / 48;

class Game {
public:
	Game();
	~Game();
	void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
	void handleEvents();
	void update();
	void render();
	void clean();
	bool running();
	static void AddTile(int x, int y, TileName id);
	static void DelTile(int x, int y);
	static void CleanTiles();
	static void Edit(int x, int y);
	static void Check();
	static void NextLayer();
	static void PreviousLayer();
	static SDL_Renderer* renderer;
	static 	SDL_Event event;
private:
	bool isRunning;
	static int state;
	static int cur_layer;
	SDL_Window* window;
	static bool check;
};