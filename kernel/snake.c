#include <kernel/snake.h>
#include <kernel/graphics/vesafb.h>
#include <kernel/devices/keyboard.h>
#include <kernel/tty.h>

#include <kernel/libk/string.h>
#include <kernel/libk/math.h>

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

void renderPlayer(int x, int y, int scale, int* tailX, int* tailY, size_t tailLength)
{
    int lim = tailLength;
	// Renders tail circles
	for (int i = 0; i < lim - 1; i++)
	{
		// Render the current tail circle
		draw_filled_circle(tailX[i], tailY[i], scale / 2, 0, 220, 0);

		// Calculate the midpoint between current and next tail circles
		int midX = (tailX[i] + tailX[i + 1]) / 2;
		int midY = (tailY[i] + tailY[i + 1]) / 2;

		// Render the interpolated circle
		draw_filled_circle(midX, midY, scale / 2, 0, 220, 0);
	}

	if (tailLength > 0)
	{
		draw_filled_circle( tailX[tailLength - 1], tailY[tailLength - 1], scale / 2, 0, 220, 0);

		int midX = (tailX[tailLength - 1] + x) / 2;
		int midY = (tailY[tailLength - 1] + y) / 2;
		draw_filled_circle( midX, midY, scale / 2, 0, 220, 0);
	}

	// Renders the player circle
	draw_filled_circle(x, y, scale / 2, 0, 130, 0);
}

void renderFood(int foodx, int foody, int scale)
{
	draw_filled_circle( foodx, foody, scale / 2, 255, 0, 0);
}

void renderScore(size_t tailLength, int scale, int x_offset, int y_offset)
{
    char* score_str = (char*)"S c o r e : ";
    //char number[32];
    //itoa(tailLength * 10, number);
    //strcat(score_str, number);
	draw_text_string(score_str , x_offset + ((scale * scale) / 2) - 75, y_offset, VESA_WHITE, VESA_BLACK, false);
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
int* getFoodSpawn(int* foodLoc, int* tailX, int* tailY, int playerX, int playerY, int scale, size_t tailLength, int x_offset, int y_offset)
{
	bool valid = false;
	int x = 0;
	int y = 0;
	//srand(12345);
	x = 10 + x_offset; // + scale * (rand() % scale);
	y = 10 + y_offset; //+ scale * (rand() % scale);
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
		foodLoc[1] = -100;
        foodLoc[2] = -100;
		return foodLoc;
	}

	foodLoc[0] = x;
    foodLoc[1] = y;

	return foodLoc;
}



void gameOver(int scale, size_t tailLength, int x_offset, int y_offset)
{
	
    int tmr = 0;
    while (tmr<100000)
    {
        tmr++;
    }

    char* gm_over_str = (char*)"G a m e  O v e r";
	draw_text_string(gm_over_str , x_offset + ((scale * scale) / 2) - 50, y_offset + ((scale * scale) / 2), VESA_RED, VESA_BLACK, false);

    char* score_str = (char*)"S c o r e : ";
    char number[32];
    itoa(tailLength * 10, number);
    strcat(score_str, number);
	draw_text_string(score_str , x_offset + ((scale * scale) / 2) - 50, y_offset, VESA_GREEN, VESA_BLACK, false);

    char* retry_str = (char*)"P r e s s  E n t e r  t o  p l a y  a g a i n";
    draw_text_string(retry_str , x_offset + ((scale * scale) / 2) - 150, y_offset + ((scale * scale) / 2) - 175, VESA_WHITE, VESA_BLACK, false);


    
	while (true)
	{
        kbd_event event = keyboard_buffer_pop();

		if (keyboard_event_convert(event) == '\n')
		{
            draw_fill(0, 0, VESA_WIDTH, VESA_HEIGHT, VESA_BLACK);
            draw_square(x_offset - 10, y_offset - 10, VESA_WIDTH - 2*(x_offset - 10), VESA_HEIGHT - 2*(y_offset - 10), VESA_RED);
			return;
		}
		
	}
}


void youWin(int scale, size_t tailLength, int x_offset, int y_offset)
{
	
    int tmr = 0;
    while (tmr<1000000)
    {
        tmr++;
    }

    char* gm_over_str = (char*)"Y o u  w o n !";
	draw_text_string(gm_over_str , x_offset + ((scale * scale) / 2) - 50, y_offset + ((scale * scale) / 2), VESA_RED, VESA_BLACK, false);

    char* score_str = (char*)"S c o r e : ";
    char number[32];
    itoa(tailLength * 10, number);
    strcat(score_str, number);
	draw_text_string(score_str , x_offset + ((scale * scale) / 2) - 50, y_offset, VESA_GREEN, VESA_BLACK, false);

    char* retry_str = (char*)"P r e s s  E n t e r  t o  p l a y  a g a i n";
    draw_text_string(retry_str , x_offset + ((scale * scale) / 2) - 150, y_offset + ((scale * scale) / 2) - 175, VESA_WHITE, VESA_BLACK, false);


    
	while (true)
	{
        kbd_event event = keyboard_buffer_pop();

		if (keyboard_event_convert(event) == '\n')
		{
            draw_fill(0, 0, VESA_WIDTH, VESA_HEIGHT, VESA_BLACK);
            draw_square(x_offset - 10, y_offset - 10, VESA_WIDTH - 2*(x_offset - 10), VESA_HEIGHT - 2*(y_offset - 10), VESA_RED);
			return;
		}
		
	}
}


typedef struct  {
 int x;
 int y;
 int h;
 int w;
} GameObject;


void snake()
{

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

	bool inputThisFrame = false;
	bool redo = false;

	GameObject food;
	food.w = scale;
	food.h = scale;
	food.x = x;
	food.y = y;
    int foodLoc[2];

	getFoodSpawn(foodLoc, tailX, tailY, x, y, scale, tailLength, x_offset, y_offset);
	food.x = foodLoc[0];
	food.y = foodLoc[1];
    
    draw_square(x_offset - 10, y_offset - 10, VESA_WIDTH - 2*(x_offset - 10), VESA_HEIGHT - 2*(y_offset - 10), VESA_RED);

	// Main game loop, this constantly runs and keeps everything updated
	while (true)
	{
        float delta = 0.01;
        kbd_event event = keyboard_buffer_pop();
		started = false;
		inputThisFrame = false;

		// Check win condition, tail needs to fill all tiles
		if (tailLength >= (size_t)(scale * scale - 1))
		{
			youWin( scale, tailLength, x_offset, y_offset);
			x = x_offset + scale * scale / 2;
			y = y_offset + scale * scale / 2;
			up = false;
			left = false;
			right = false;
			down = false;
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
            tty_printf("%c %d %d\n", keyboard_event_convert(event), x, y);
			// Simply exit the program when told to
			if (keyboard_event_convert(event) == 'q')
			{
				return;
			}

			// If a key is pressed
			if (inputThisFrame == false)
			{
				// Then check for the key being pressed and change direction accordingly
				if (down == false && keyboard_event_convert(event) == 'w')
				{
					up = true;
					left = false;
					right = false;
					down = false;
					inputThisFrame = true;
				}
				else if (right == false && keyboard_event_convert(event) == 'a')
				{
					up = false;
					left = true;
					right = false;
					down = false;
					inputThisFrame = true;
				}
				else if (up == false && keyboard_event_convert(event) == 's')
				{
					up = false;
					left = false;
					right = false;
					down = true;
					inputThisFrame = true;
				}
				else if (left == false && keyboard_event_convert(event) == 'd')
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


		if ( scale != scale)
		{
			size_t tail_size = get_int_array_size(tailX);

			if (tail_size != tailLength)
			{
                if(tail_size>0)
                {
                    tailX[tail_size-1] = prevX;
				    tailY[tail_size-1] = prevY;
                }else
                {
                    tailX[0] = prevX;
				    tailY[0] = prevY;
                }
				
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
		
        }
		// Game over if player has collided with a tail block, also reset everything
		for (size_t i = 0; i < tailLength; i++)
		{

			if (x == tailX[i] && y == tailY[i])
			{
				gameOver( scale, tailLength, x_offset, y_offset);
				x = x_offset + scale * scale / 2;
				y = y_offset + scale * scale / 2;
				up = false;
				left = false;
				right = false;
				down = false;
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
		if (x < x_offset || y < y_offset || x > (int)(VESA_WIDTH - x_offset) || y > (int)(VESA_HEIGHT - y_offset))
		{
			gameOver( scale, tailLength, x_offset, y_offset);
			x = x_offset + scale * scale / 2;
			y = y_offset + scale * scale / 2;
			up = false;
			left = false;
			right = false;
			down = false;
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
        
		renderPlayer(x, y, scale, tailX, tailY, tailLength);
		renderScore(tailLength, scale, x_offset, y_offset);
		if (started)
		{
			renderFood( food.x, food.y, scale);
		}
        
        
	}

	return ;
}

