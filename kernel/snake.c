#include <kernel/graphics/vesafb.h>
#include <kernel/graphics/render.h>
#include <kernel/devices/keyboard.h>
#include <kernel/mm/kheap.h>
#include <kernel/libk/string.h>
#include <kernel/libk/math.h>

#include <kernel/snake.h>
#include <kernel/tty.h>

int seed = 888;

void renderPlayer(Renderer_t renderer, int x, int y, int scale, int* tailX, int* tailY, size_t tailLength)
{
    
	// Renders tail circles
	for (size_t i = 0; tailLength>0 && i < tailLength - 1; i++)
	{
		// Render the current tail circle
		render_filled_circle(renderer, tailX[i], tailY[i], scale / 2, 0, 220, 0);

		// Calculate the midpoint between current and next tail circles
		int midX = (tailX[i] + tailX[i + 1]) / 2;
		int midY = (tailY[i] + tailY[i + 1]) / 2;

		// Render the interpolated circle
		render_filled_circle(renderer, midX, midY, scale / 2, 0, 220, 0);
	}

	if (tailLength > 0)
	{
		render_filled_circle(renderer, tailX[tailLength - 1], tailY[tailLength - 1], scale / 2, 0, 220, 0);

		int midX = (tailX[tailLength - 1] + x) / 2;
		int midY = (tailY[tailLength - 1] + y) / 2;
		render_filled_circle(renderer, midX, midY, scale / 2, 0, 220, 0);
	}

	// Renders the player circle
	render_filled_circle(renderer, x, y, scale / 2, 0, 130, 0);
	render_head(renderer, x, y, scale / 2);
}

void renderFood(Renderer_t renderer, int foodx, int foody, int scale)
{
	render_filled_circle(renderer, foodx, foody, scale / 2, 255, 0, 0);
}

void renderScore(Renderer_t renderer, size_t tailLength, int scale, int x_offset, int y_offset)
{
    char score_str[] = "S c o r e : ";
    char number[32];
    memset(number, 0, sizeof(number));

    itoa(tailLength * 10, number);
    strcat(score_str, number);
	render_text_string(renderer, score_str , x_offset + ((scale * scale) / 2) - 75, y_offset, VESA_WHITE+0xFF000000, VESA_BLACK+0xFF000000, false);
}

size_t get_int_array_size(int* array)
{
    size_t length = 0;
    while (array[length]!=0 && length < max(VESA_WIDTH, VESA_HEIGHT))
    {
       length++;
    }
    return length;
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
void getFoodSpawn(int* foodLoc, int* tailX, int* tailY, int playerX, int playerY, int scale, size_t tailLength, int x_offset, int y_offset)
{
	bool valid = false;
	int x = 0;
	int y = 0;
	
	x =  x_offset + scale * (rand(&seed) % scale);
	y =  y_offset+ scale * (rand(&seed) % scale);
	valid = true;

	// Check all tail blocks and player block
	for (size_t i = 0; i < tailLength; i++)
	{
		if ((x == tailX[i] && y == tailY[i]) || (x == playerX && y == playerY))
		{
			valid = false;
		}
	}

    
	if (!valid)
	{
		foodLoc[0] = -100;
        foodLoc[1] = -100;
		return;
	}

	foodLoc[0] = x;
    foodLoc[1] = y;
	return;
}

void gameOver(Renderer_t renderer, int scale, size_t tailLength, int x_offset, int y_offset)
{
	
    render_fill(renderer, 0, 0, VESA_WIDTH, VESA_HEIGHT, VESA_BLACK+0xFF000000);

    char gm_over_str[] = "G a m e  O v e r";
	render_text_string(renderer, gm_over_str , x_offset + ((scale * scale) / 2) - 50, y_offset + ((scale * scale) / 2), VESA_RED+0xFF000000, VESA_BLACK+0xFF000000, false);

    char score_str[] = "S c o r e : ";
    char number[32];
    itoa(tailLength * 10, number);
    strcat(score_str, number);
	render_text_string(renderer, score_str , x_offset + ((scale * scale) / 2) - 50, y_offset, VESA_GREEN+0xFF000000, VESA_BLACK+0xFF000000, false);

    char retry_str[] = "P r e s s  E n t e r  t o  p l a y  a g a i n";
    render_text_string(renderer, retry_str , x_offset + ((scale * scale) / 2) - 150, y_offset + ((scale * scale) / 2) - 175, VESA_WHITE+0xFF000000, VESA_BLACK+0xFF000000, false);
    draw_rendered(renderer);

	while (true)
	{
        kbd_event event = keyboard_buffer_pop();

		if (keyboard_event_convert(event) == '\n')
		{
			return;
		}
		
	}
}


void youWin(Renderer_t renderer, int scale, size_t tailLength, int x_offset, int y_offset)
{
    
    char gm_over_str[] = "Y o u  w o n !";
	render_text_string(renderer, gm_over_str , x_offset + ((scale * scale) / 2) - 50, y_offset + ((scale * scale) / 2), VESA_RED+0xFF000000, VESA_BLACK+0xFF000000, false);
    render_fill(renderer, 0, 0, VESA_WIDTH, VESA_HEIGHT, VESA_BLACK+0xFF000000);

    char score_str[] = "S c o r e : ";
    char number[32];
    itoa(tailLength * 10, number);
    strcat(score_str, number);
	render_text_string(renderer, score_str , x_offset + ((scale * scale) / 2) - 50, y_offset, VESA_GREEN+0xFF000000, VESA_BLACK+0xFF000000, false);

    char retry_str[] = "P r e s s  E n t e r  t o  p l a y  a g a i n";
    render_text_string(renderer, retry_str , x_offset + ((scale * scale) / 2) - 150, y_offset + ((scale * scale) / 2) - 175, VESA_WHITE+0xFF000000, VESA_BLACK+0xFF000000, false);
    draw_rendered(renderer);
	while (true)
	{
        kbd_event event = keyboard_buffer_pop();

		if (keyboard_event_convert(event) == '\n')
		{
            
			return;
		}
		
	}
}





void snake()
{
    Renderer_t renderer = kmalloc(VESA_WIDTH * sizeof(rgba_color[VESA_HEIGHT])); // Allocate memory for the renderer array

    if (renderer == NULL) {
        return;
    }

    for (size_t raw = 0; raw < VESA_WIDTH; raw++) {
        memset(renderer[raw], 0, VESA_HEIGHT * sizeof(rgba_color));
    }

	int scale = 24;

	int x_offset = (VESA_WIDTH - scale * scale) / 2;
	int y_offset = (VESA_HEIGHT - scale * scale) / 2;

	size_t tailLength = 0;

	int tailX[VESA_WIDTH];
	int tailY[VESA_HEIGHT];
    memset(tailX, 0, sizeof(tailX));
    memset(tailY, 0, sizeof(tailY));

	int x = x_offset + scale * scale / 2;
	int y = y_offset + scale * scale / 2;


	int prevX = 0;
	int prevY = 0;

	bool up = false;
	bool down = false;
	bool right = false;
	bool left = false;
	bool started = false;

	
	bool redo = false;

	GameObject food;
	food.w = scale;
	food.h = scale;
	food.x = x;
	food.y = y;
    int foodLoc[2];
    memset(foodLoc,0,sizeof(foodLoc));

	getFoodSpawn(foodLoc, tailX, tailY, x, y, scale, tailLength, x_offset, y_offset);
	food.x = foodLoc[0];
	food.y = foodLoc[1];
    

	// Main game loop, this constantly runs and keeps everything updated
	while (true)
	{
        float delta = 1;
        kbd_event event = keyboard_buffer_pop();

		// Check win condition, tail needs to fill all tiles
		if (tailLength >= (size_t)(scale * scale - 1))
		{
			youWin(renderer, scale, tailLength, x_offset, y_offset);
			x = x_offset + scale * scale / 2;
			y = y_offset + scale * scale / 2;
			up = false;
			left = false;
			right = false;
			down = false;
			started = false;
			memset(tailX, 0, sizeof(tailX));
            memset(tailY, 0, sizeof(tailY));
			tailLength = 0;
			redo = false;

			getFoodSpawn(foodLoc, tailX, tailY, x, y, scale, tailLength, x_offset, y_offset);
			if (food.x == -100 && food.y == -100)
			{
				redo = true;
			}

			food.x = foodLoc[0];
			food.y = foodLoc[1];
		}

		// Controls
		if (keyboard_event_convert(event)!=0)
		{
            
			// Simply exit the program when told to
			if (keyboard_event_convert(event) == 'q')
			{
				return;
			}


				// Then check for the key being pressed and change direction accordingly
				if (down == false && keyboard_event_convert(event) == 'w')
				{
					up = true;
					left = false;
					right = false;
					down = false;
				}
				else if (right == false && keyboard_event_convert(event) == 'a')
				{
					up = false;
					left = true;
					right = false;
					down = false;
				}
				else if (up == false && keyboard_event_convert(event) == 's')
				{
					up = false;
					left = false;
					right = false;
					down = true;
				}
				else if (left == false && keyboard_event_convert(event) == 'd')
				{
					up = false;
					left = false;
					right = true;
					down = false;
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

        if (redo==true)
        {
            redo = false;
			getFoodSpawn(foodLoc, tailX, tailY, x, y, scale, tailLength, x_offset, y_offset);
			food.x = foodLoc[0];
			food.y = foodLoc[1];

			if (food.x == -100 && food.y == -100)
			{
				redo = true;
			}
		}

		// Collision detection, has played collided with food?
		if (checkCollision(food.x, food.y, x, y))
		{

			// Spawn new food after it has been eaten
			getFoodSpawn(foodLoc, tailX, tailY, x, y, scale, tailLength, x_offset, y_offset);
			food.x = foodLoc[0];
			food.y = foodLoc[1];
            
			if (food.x == -100 && food.y == -100)
			{
				redo = true;
			}
			tailLength++;
		}

			size_t tail_size = get_int_array_size(tailX);

            if (tail_size != tailLength)
			{
                tailX[tail_size] = prevX;
                tailY[tail_size] = prevY;
            }

			for (size_t i = 0; i < tailLength; i++)
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
		
        
		// Game over if player has collided with a tail block, also reset everything
		for (size_t i = 0; i < tailLength; i++)
		{

			if (x == tailX[i] && y == tailY[i])
			{
				gameOver(renderer, scale, tailLength, x_offset, y_offset);
				x = x_offset + scale * scale / 2;
				y = y_offset + scale * scale / 2;
				up = false;
				left = false;
				right = false;
				down = false;
				started = false;
                memset(tailX, 0, sizeof(tailX));
                memset(tailY, 0, sizeof(tailY));
				tailLength = 0;
				redo = false;

				getFoodSpawn(foodLoc, tailX, tailY, x, y, scale, tailLength, x_offset, y_offset);
				if (food.x == -100 && food.y == -100)
				{
					redo = true;
				}

				food.x = foodLoc[0];
				food.y = foodLoc[1];
			}
		}

		// Game over if player out of bounds, also resets the game state
		if (x < x_offset || y < y_offset || x >= (int)(VESA_WIDTH - x_offset) || y >= (int)(VESA_HEIGHT - y_offset))
		{
			gameOver(renderer, scale, tailLength, x_offset, y_offset);
			x = x_offset + scale * scale / 2;
			y = y_offset + scale * scale / 2;
			up = false;
			left = false;
			right = false;
			down = false;
			started = false;
            memset(tailX, 0, sizeof(tailX));
            memset(tailY, 0, sizeof(tailY));
			tailLength = 0;
			redo = false;
			getFoodSpawn(foodLoc, tailX, tailY, x, y, scale, tailLength, x_offset, y_offset);
			food.x = foodLoc[0];
			food.y = foodLoc[1];

			if (food.x == -100 && food.y == -100)
			{
				redo = true;
			}
		}

		// Render everything
        render_fill(renderer, 0, 0, VESA_WIDTH, VESA_HEIGHT, VESA_BLACK+0xFF000000);
        render_square(renderer, x_offset, y_offset, VESA_WIDTH - 2*x_offset, VESA_HEIGHT - 2*y_offset, VESA_RED+0xFF000000);
        renderPlayer(renderer, x, y, scale, tailX, tailY, tailLength);
        if (started)
		{
			renderFood(renderer, food.x, food.y, scale);
		}
		renderScore(renderer, tailLength, scale, x_offset, y_offset);
        draw_rendered(renderer);        
 
	}

	kfree(renderer);
	return;
}

