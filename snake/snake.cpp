#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL2_gfxPrimitives.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

using namespace std;

void renderPlayer(SDL_Renderer *renderer, int x, int y, int scale, vector<int> tailX, vector<int> tailY, int tailLength)
{

	// Renders tail circles
	for (int i = 0; i < tailLength - 1; i++)
	{
		// Render the current tail circle
		filledCircleRGBA(renderer, tailX[i], tailY[i], scale / 2, 0, 220, 0, 255);

		// Calculate the midpoint between current and next tail circles
		int midX = (tailX[i] + tailX[i + 1]) / 2;
		int midY = (tailY[i] + tailY[i + 1]) / 2;

		// Render the interpolated circle
		filledCircleRGBA(renderer, midX, midY, scale / 2, 0, 220, 0, 255);
	}

	if (tailLength > 0)
	{
		filledCircleRGBA(renderer, tailX[tailLength - 1], tailY[tailLength - 1], scale / 2, 0, 220, 0, 255);

		int midX = (tailX[tailLength - 1] + x) / 2;
		int midY = (tailY[tailLength - 1] + y) / 2;

		filledCircleRGBA(renderer, midX, midY, scale / 2, 0, 220, 0, 255);
	}

	// Renders the player circle
	filledCircleRGBA(renderer, x, y, scale / 2, 0, 130, 0, 255);
}

void renderFood(SDL_Renderer *renderer, int foodx, int foody, int scale)
{
	filledCircleRGBA(renderer, foodx, foody, scale / 2, 255, 0, 0, 255);
}

void renderScore(SDL_Renderer *renderer, int tailLength, int scale, int x_offset, int y_offset)
{
	SDL_Color White = {255, 255, 255};

	TTF_Font *font = TTF_OpenFont("ArialRegular.ttf", 8);
	if (font == NULL)
	{
		cout << "Font loading error" << endl;
		return;
	}

	SDL_Surface *score = TTF_RenderText_Solid(font, (string("S c o r e : ") + to_string(tailLength * 10)).c_str(), White);
	SDL_Texture *scoreMessage = SDL_CreateTextureFromSurface(renderer, score);
	SDL_Rect scoreRect;
	scoreRect.w = 150;
	scoreRect.h = 50;
	scoreRect.x = x_offset + ((scale * scale) / 2) - (scoreRect.w / 2);
	scoreRect.y = y_offset;
	SDL_RenderCopy(renderer, scoreMessage, NULL, &scoreRect);

	TTF_CloseFont(font);
}

bool checkCollision(int foodx, int foody, int playerx, int playery)
{

	if (playerx == foodx && playery == foody)
	{
		return true;
	}

	return false;
}

// Get a valid spawn for the food which is not on top of a tail or player block
pair<int, int> getFoodSpawn(vector<int> tailX, vector<int> tailY, int playerX, int playerY, int scale, int tailLength, int x_offset, int y_offset)
{
	bool valid = false;
	int x = 0;
	int y = 0;
	srand(time(0));
	x = x_offset + scale * (rand() % scale);
	y = y_offset + scale * (rand() % scale);
	valid = true;

	// Check all tail blocks and player block
	for (int i = 0; i < tailLength; i++)
	{

		if ((x == tailX[i] && y == tailY[i]) || (x == playerX && y == playerY))
		{
			valid = false;
		}
	}

	if (!valid)
	{
		pair<int, int> foodLoc;
		foodLoc = make_pair(-100, -100);
		return foodLoc;
	}

	pair<int, int> foodLoc;
	foodLoc = make_pair(x, y);

	return foodLoc;
}

void gameOver(SDL_Renderer *renderer, SDL_Event event, int scale, int tailLength, int x_offset, int y_offset)
{
	SDL_Color Red = {255, 0, 0};
	SDL_Color White = {255, 255, 255};
	SDL_Color Green = {0, 255, 0};

	TTF_Font *font = TTF_OpenFont("ArialRegular.ttf", 8);
	if (font == NULL)
	{
		cout << "Font loading error" << endl;
		return;
	}
	SDL_Delay(1500);
	SDL_Surface *gameover = TTF_RenderText_Solid(font, "G a m e  O v e r", Red);
	SDL_Surface *retry = TTF_RenderText_Solid(font, "P r e s s  E n t e r  t o  r e t r y", White);
	SDL_Surface *score = TTF_RenderText_Solid(font, (string("S c o r e : ") + to_string(tailLength * 10)).c_str(), Green);
	SDL_Texture *gameoverMessage = SDL_CreateTextureFromSurface(renderer, gameover);
	SDL_Texture *retryMessage = SDL_CreateTextureFromSurface(renderer, retry);
	SDL_Texture *scoreMessage = SDL_CreateTextureFromSurface(renderer, score);
	SDL_Rect gameoverRect;
	SDL_Rect retryRect;
	SDL_Rect scoreRect;
	gameoverRect.w = 200;
	gameoverRect.h = 100;
	gameoverRect.x = x_offset + ((scale * scale) / 2) - (gameoverRect.w / 2);
	gameoverRect.y = y_offset + ((scale * scale) / 2) - (gameoverRect.h / 2) - 50;
	retryRect.w = 300;
	retryRect.h = 50;
	retryRect.x = x_offset + ((scale * scale) / 2) - ((retryRect.w / 2));
	retryRect.y = y_offset + (((scale * scale) / 2) - ((retryRect.h / 2)) + 150);
	scoreRect.w = 100;
	scoreRect.h = 30;
	scoreRect.x = x_offset + ((scale * scale) / 2) - (scoreRect.w / 2);
	scoreRect.y = y_offset;
	SDL_RenderCopy(renderer, gameoverMessage, NULL, &gameoverRect);
	SDL_RenderCopy(renderer, retryMessage, NULL, &retryRect);
	SDL_RenderCopy(renderer, scoreMessage, NULL, &scoreRect);

	TTF_CloseFont(font);

	while (true)
	{
		SDL_RenderPresent(renderer);

		if (SDL_PollEvent(&event))
		{

			if (event.type == SDL_QUIT)
			{
				exit(0);
			}

			if (event.key.keysym.scancode == SDL_SCANCODE_RETURN)
			{
				return;
			}
		}
	}
}

void youWin(SDL_Renderer *renderer, SDL_Event event, int scale, int tailLength, int x_offset, int y_offset)
{
	SDL_Color Red = {255, 0, 0};
	SDL_Color White = {255, 255, 255};
	SDL_Color Green = {0, 255, 0};
	SDL_Color Yellow = {255, 255, 0};

	TTF_Font *font = TTF_OpenFont("ArialRegular.ttf", 8);
	if (font == NULL)
	{
		cout << "Font loading error" << endl;
		return;
	}

	SDL_Surface *gameover = TTF_RenderText_Solid(font, "Y o u  w o n !", Yellow);
	SDL_Surface *retry = TTF_RenderText_Solid(font, "P r e s s  E n t e r  t o  p l a y  a g a i n", White);
	SDL_Surface *score = TTF_RenderText_Solid(font, (string("S c o r e : ") + to_string(tailLength * 10)).c_str(), Green);
	SDL_Texture *gameoverMessage = SDL_CreateTextureFromSurface(renderer, gameover);
	SDL_Texture *retryMessage = SDL_CreateTextureFromSurface(renderer, retry);
	SDL_Texture *scoreMessage = SDL_CreateTextureFromSurface(renderer, score);
	SDL_Rect gameoverRect;
	SDL_Rect retryRect;
	SDL_Rect scoreRect;
	gameoverRect.w = 200;
	gameoverRect.h = 100;
	gameoverRect.x = x_offset + ((scale * scale) / 2) - (gameoverRect.w / 2);
	gameoverRect.y = y_offset + ((scale * scale) / 2) - (gameoverRect.h / 2) - 50;
	retryRect.w = 300;
	retryRect.h = 50;
	retryRect.x = x_offset + ((scale * scale) / 2) - ((retryRect.w / 2));
	retryRect.y = y_offset + (((scale * scale) / 2) - ((retryRect.h / 2)) + 150);
	scoreRect.w = 100;
	scoreRect.h = 30;
	scoreRect.x = x_offset + ((scale * scale) / 2) - (scoreRect.w / 2);
	scoreRect.y = y_offset;
	SDL_RenderCopy(renderer, gameoverMessage, NULL, &gameoverRect);
	SDL_RenderCopy(renderer, retryMessage, NULL, &retryRect);
	SDL_RenderCopy(renderer, scoreMessage, NULL, &scoreRect);

	TTF_CloseFont(font);

	while (true)
	{
		SDL_RenderPresent(renderer);

		if (SDL_PollEvent(&event))
		{

			if (event.type == SDL_QUIT)
			{
				exit(0);
			}

			if (event.key.keysym.scancode == SDL_SCANCODE_RETURN)
			{
				return;
			}
		}
	}
}

int main(int argc, char *argv[])
{

	SDL_Init(SDL_INIT_EVERYTHING);

	if (TTF_Init() < 0)
	{
		cout << "Error: " << TTF_GetError() << endl;
	}

	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event event;

	int scale = 24;

	window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	int x_offset = (SCREEN_WIDTH - scale * scale) / 2;
	int y_offset = (SCREEN_HEIGHT - scale * scale) / 2;

	int tailLength = 0;

	vector<int> tailX;
	vector<int> tailY;

	int x = x_offset + scale * scale / 2;
	int y = y_offset + scale * scale / 2;

	SDL_Rect player;
	player.x = x;
	player.y = y;
	player.h = 0;
	player.w = 0;

	int prevX = 0;
	int prevY = 0;

	bool up = false;
	bool down = false;
	bool right = false;
	bool left = false;
	bool started = false;

	bool inputThisFrame = false;
	bool redo = false;

	SDL_Rect food;
	food.w = scale;
	food.h = scale;
	food.x = x;
	food.y = y;

	float time = SDL_GetTicks() / 100;

	pair<int, int> foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, tailLength, x_offset, y_offset);
	food.x = foodLoc.first;
	food.y = foodLoc.second;

	// Main game loop, this constantly runs and keeps everything updated
	while (true)
	{
		started = false;
		float newTime = SDL_GetTicks() / 100; // This value (100) is the speed at which the blocks are updated
		float delta = newTime - time;
		time = newTime;

		inputThisFrame = false;

		// Check win condition, tail needs to fill all tiles
		if (tailLength >= scale * scale - 1)
		{
			youWin(renderer, event, scale, tailLength, x_offset, y_offset);
			x = x_offset + scale * scale / 2;
			y = y_offset + scale * scale / 2;
			up = false;
			left = false;
			right = false;
			down = false;
			tailX.clear();
			tailY.clear();
			tailLength = 0;
			redo = false;
			foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, tailLength, x_offset, y_offset);

			if (food.x == -100 && food.y == -100)
			{
				redo = true;
			}

			food.x = foodLoc.first;
			food.y = foodLoc.second;
		}

		// Controls
		if (SDL_PollEvent(&event))
		{

			// Simply exit the program when told to
			if (event.type == SDL_QUIT)
			{
				exit(0);
			}

			// If a key is pressed
			if (event.type == SDL_KEYDOWN && inputThisFrame == false)
			{
				// Then check for the key being pressed and change direction accordingly
				if (down == false && event.key.keysym.scancode == SDL_SCANCODE_UP)
				{
					up = true;
					left = false;
					right = false;
					down = false;
					inputThisFrame = true;
				}
				else if (right == false && event.key.keysym.scancode == SDL_SCANCODE_LEFT)
				{
					up = false;
					left = true;
					right = false;
					down = false;
					inputThisFrame = true;
				}
				else if (up == false && event.key.keysym.scancode == SDL_SCANCODE_DOWN)
				{
					up = false;
					left = false;
					right = false;
					down = true;
					inputThisFrame = true;
				}
				else if (left == false && event.key.keysym.scancode == SDL_SCANCODE_RIGHT)
				{
					up = false;
					left = false;
					right = true;
					down = false;
					inputThisFrame = true;
				}
			}
		}

		// The previous position of the player block
		prevX = x;
		prevY = y;

		if (up)
		{
			started = true;
			y -= delta * scale;
		}
		else if (left)
		{
			started = true;
			x -= delta * scale;
		}
		else if (right)
		{
			started = true;
			x += delta * scale;
		}
		else if (down)
		{
			started = true;
			y += delta * scale;
		}

		if (redo == true)
		{
			redo = false;
			foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, tailLength, x_offset, y_offset);
			food.x = foodLoc.first;
			food.y = foodLoc.second;

			if (food.x == -100 && food.y == -100)
			{
				redo = true;
			}
		}

		// Collision detection, has played collided with food?
		if (checkCollision(food.x, food.y, x, y))
		{

			// Spawn new food after it has been eaten
			foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, tailLength, x_offset, y_offset);
			food.x = foodLoc.first;
			food.y = foodLoc.second;

			if (food.x == -100 && food.y == -100)
			{
				redo = true;
			}

			tailLength++;
		}

		// Only runs in the frames where the player block has moved
		if (delta * scale == scale)
		{

			// Update tail size and position
			if (tailX.size() != tailLength)
			{
				tailX.push_back(prevX);
				tailY.push_back(prevY);
			}

			// Loop through every tail block, move all blocks to the nearest block in front
			// This updates the blocks from end (farthest from player block) to the start (nearest to player block)
			for (int i = 0; i < tailLength; i++)
			{

				if (i > 0)
				{
					tailX[i - 1] = tailX[i];
					tailY[i - 1] = tailY[i];
				}
			}

			if (tailLength > 0)
			{
				tailX[tailLength - 1] = prevX;
				tailY[tailLength - 1] = prevY;
			}
		}

		// Game over if player has collided with a tail block, also reset everything
		for (int i = 0; i < tailLength; i++)
		{

			if (x == tailX[i] && y == tailY[i])
			{
				gameOver(renderer, event, scale, tailLength, x_offset, y_offset);
				x = x_offset + scale * scale / 2;
				y = y_offset + scale * scale / 2;
				up = false;
				left = false;
				right = false;
				down = false;
				tailX.clear();
				tailY.clear();
				tailLength = 0;
				redo = false;

				foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, tailLength, x_offset, y_offset);
				if (food.x == -100 && food.y == -100)
				{
					redo = true;
				}

				food.x = foodLoc.first;
				food.y = foodLoc.second;
			}
		}

		// Game over if player out of bounds, also resets the game state
		if (x < x_offset || y < y_offset || x > SCREEN_WIDTH - x_offset || y > SCREEN_HEIGHT - y_offset)
		{
			gameOver(renderer, event, scale, tailLength, x_offset, y_offset);
			x = x_offset + scale * scale / 2;
			y = y_offset + scale * scale / 2;
			up = false;
			left = false;
			right = false;
			down = false;
			tailX.clear();
			tailY.clear();
			tailLength = 0;
			redo = false;
			foodLoc = getFoodSpawn(tailX, tailY, x, y, scale, tailLength, x_offset, y_offset);
			food.x = foodLoc.first;
			food.y = foodLoc.second;

			if (food.x == -100 && food.y == -100)
			{
				redo = true;
			}
		}

		// Render everything
		renderPlayer(renderer, x, y, scale, tailX, tailY, tailLength);
		renderScore(renderer, tailLength, scale, x_offset, y_offset);
		if (started)
		{
			renderFood(renderer, food.x, food.y, scale);
		}

		SDL_RenderDrawLine(renderer, x_offset - 10, y_offset - 10, x_offset - 10, SCREEN_HEIGHT + 10 - y_offset);
		SDL_RenderDrawLine(renderer, x_offset - 10, SCREEN_HEIGHT + 10 - y_offset, SCREEN_WIDTH + 10 - x_offset, SCREEN_HEIGHT + 10 - y_offset);
		SDL_RenderDrawLine(renderer, SCREEN_WIDTH + 10 - x_offset, y_offset - 10, SCREEN_WIDTH + 10 - x_offset, SCREEN_HEIGHT + 10 - y_offset);
		SDL_RenderDrawLine(renderer, SCREEN_WIDTH + 10 - x_offset, y_offset - 10, x_offset - 10, y_offset - 10);

		// Put everything on screen
		// Nothing is actually put on screen until this is called
		SDL_RenderPresent(renderer);

		// Choose a color and fill the entire window with it, this resets everything before the next frame
		// This also give us the background color
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
	}

	SDL_DestroyWindow(window);

	TTF_Quit();

	SDL_Quit();

	return 0;
}