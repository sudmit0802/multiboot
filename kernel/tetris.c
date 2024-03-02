#include <kernel/tetris.h>
#include <kernel/graphics/vesafb.h>
#include <kernel/devices/keyboard.h>
#include <kernel/graphics/render.h>
#include <kernel/mm/kheap.h>
#include <kernel/tty.h>

Tetris_Action TETROMINO_ACTION;
int tetris_seed=111;
static int score = 0;
static bool render_changed = false;
static bool userevent = false;
static uint8_t CURRENT_TETROMINO_COORDS[8] = {0};
static uint8_t GHOST_TETROMINO_COORDS[8] = {0};
static Tetromino_Movement CURRENT_TETROMINO;
static uint8_t lock_delay_count = 0;
static uint8_t tetromino_queue[7 * 4];
static uint8_t tetromino_queue_size = 7 * 4;
static uint8_t current_queue_index = 0;
static Color_Block playfield[PLAYFIELD_HEIGHT * PLAYFIELD_WIDTH];

int rrand(int m) {
    return (int)((double)m * ( rand(&tetris_seed) / (2147483647+1.0) ));
}

#define BYTE(X) ((unsigned char *)(X))

void shuffle(void *obj, size_t nmemb, size_t size) {

    void *temp = kmalloc(size);
    size_t n = nmemb;
    while ( n > 1 ) {
        size_t k = rrand(n--);
        memcpy(temp, BYTE(obj) + n*size, size);
        memcpy(BYTE(obj) + n*size, BYTE(obj) + k*size, size);
        memcpy(BYTE(obj) + k*size, temp, size);
    }
    kfree(temp);
}

void draw_block(Renderer_t renderer, uint8_t x, uint8_t y, uint32_t color) {


    if (x >= (int) PLAYFIELD_WIDTH || 
        y >= (int) PLAYFIELD_HEIGHT) {
        return;
    }

    // top-left coords of block
    uint16_t x_tl = (BLOCK_SIZE*PLAYFIELD_WIDTH) + x * (BLOCK_SIZE + 1) + 1;
    uint16_t y_tl = y * (BLOCK_SIZE + 1) + 1;

    // top-right coords of block
    uint16_t x_tr = x_tl + BLOCK_SIZE;
    uint16_t y_tr = y_tl;

    // bottom-left coords of block
    uint16_t x_bl = x_tl;
    uint16_t y_bl = y_tl + BLOCK_SIZE;
    
    
    render_square(renderer, x_tl, y_tl, BLOCK_SIZE, BLOCK_SIZE, color);
    render_fill(renderer, x_tl, y_tl, BLOCK_SIZE, BLOCK_SIZE, color);

    // draw grid lines
    uint32_t res_color = rgb(187,173,160);
    if(y <= 0) {
        // draw top horizontal grid line (from top-left to top-right)
        render_horizontal_line(renderer, x_tl, y_tl, BLOCK_SIZE, res_color);
    }

    if(x <= 0) {
        // draw left vertical grid line (from top-left to bottom-left)
        render_vertical_line(renderer, x_tl, y_tl, BLOCK_SIZE, res_color);
    }

    // draw bottom horizontal grid line (from bottom-left to bottom-right)
    render_horizontal_line(renderer, x_bl, y_bl, BLOCK_SIZE, res_color);

    // draw right vertical grid line (from top-right to bottom-right)
    render_vertical_line(renderer, x_tr, y_tr, BLOCK_SIZE, res_color);

    render_changed  = true;

}

void draw_playing_field(Renderer_t renderer) {

    // Clear the render
    render_fill(renderer, 0, 0, VESA_WIDTH, VESA_HEIGHT, 0xFF000000);


    int i = PLAYFIELD_HEIGHT * PLAYFIELD_WIDTH;
    while (i --> 0)
        set_playfield(renderer,  i % PLAYFIELD_WIDTH, i / PLAYFIELD_WIDTH, playfield[i]);

    render_changed=true;
}

void auto_drop_timer() {
    userevent=true;
}

void initTetris(Renderer_t renderer) {

    score = 0;
    TETROMINO_ACTION = NONE;


    int i = PLAYFIELD_HEIGHT * PLAYFIELD_WIDTH;
    while (i --> 0) {
        playfield[i] = EMPTY;
    }

    // build tetromino queue
    current_queue_index = 0;
    i = tetromino_queue_size;
    int n = 0;
    while(i --> 0) {
        if((i + 1) % 4 == 0) {
            n++;
        }
        tetromino_queue[i] = n;
    }

    // apply shuffle algorithm
    shuffle(tetromino_queue, tetromino_queue_size, sizeof(uint8_t));

    draw_playing_field(renderer);

    spawn_tetromino(renderer);

}

void lockTetromino(Renderer_t renderer) {

    lock_delay_count = 0;

    // lock tetromino in place
    int i = 4;
    while(i --> 0) {
        uint8_t x_coord = i * 2;
        uint8_t y_coord = x_coord + 1;

        uint8_t _x = CURRENT_TETROMINO_COORDS[x_coord];
        uint8_t _y = CURRENT_TETROMINO_COORDS[y_coord];

        CURRENT_TETROMINO_COORDS[x_coord] = 0;
        CURRENT_TETROMINO_COORDS[y_coord] = 0;

        set_playfield(renderer, _x, _y, CURRENT_TETROMINO.type.color);
    }

    // clear lines if any
    uint8_t row = PLAYFIELD_HEIGHT;
    int8_t row_to_copy_to = -1;

    uint8_t completed_lines = 0;

    while(row --> 0) {
        uint8_t col;
        bool complete_line = true;

        // check if line is complete
        for(col = 0; col < PLAYFIELD_WIDTH; col++) {
            if(get_playfield(col, row) == EMPTY) {

                complete_line = false;
                break;
            }
        }

        // clear line
        if(complete_line) {

            completed_lines++;

            if(row_to_copy_to < row) {
                row_to_copy_to = row;
            }

            for(col = 0; col < PLAYFIELD_WIDTH; col++) {
                set_playfield(renderer, col, row, EMPTY);
            }

        } else if(row_to_copy_to > row) {

            for(col = 0; col < PLAYFIELD_WIDTH; col++) {
                set_playfield(renderer, col, row_to_copy_to, get_playfield(col, row));
            }

            row_to_copy_to--;
        }

    }

    // update score

    if(completed_lines > 0) {
        // tetris
        score += completed_lines/4 * 800;
        completed_lines = completed_lines % 4;

        // triple
        score += completed_lines/3 * 500;
        completed_lines = completed_lines % 3;

        // double
        score += completed_lines/2 * 300;
        completed_lines = completed_lines % 2;

        // single
        score += completed_lines * 100;
    }


    spawn_tetromino(renderer);

}

void render_score(Renderer_t renderer) {
    // Show tetris score after all tetris operations are finished

    char score_value[16];
    char score_string[32] = "Score: ";
    itoa(score, score_value);
    strcat(score_string, score_value);
    render_text_string(renderer, score_string,  (BLOCK_SIZE*PLAYFIELD_WIDTH) - 60 - 10*(strlen(score_value)), 10, 0xFFFFFFFF, 0, false);

}

void updateTetris(Renderer_t renderer) {
    auto_drop_timer();
    // draw the scoreboard as needed
    int i = 4;
    bool on_top = false;
    while(i --> 0) {
        uint8_t x_coord = i * 2;
        uint8_t y_coord = x_coord + 1;
        uint8_t _y = CURRENT_TETROMINO_COORDS[y_coord];
        if(_y <= 1) {
            on_top = true;
            break;
        }
    }

    if(on_top) {

        // re-draw playfield area where score is located in
        int n = PLAYFIELD_WIDTH * 2 - 1;
        while(n --> 0) {
            int x = n % PLAYFIELD_WIDTH;
            int y = n / PLAYFIELD_WIDTH;
            set_playfield(renderer, x, y, get_playfield(x, y));
        }
        
        draw_playing_field(renderer);
        render_current_tetromino(renderer, CURRENT_TETROMINO);
        render_score(renderer);
    }
    

    Tetromino_Movement request = CURRENT_TETROMINO;

    // action from keyboard
    switch(TETROMINO_ACTION) {
        case NONE:
            // do nothing - don't bother redrawing
        break;

        case ROTATE:
            request.rotation = (request.rotation + 1) % 4;
            render_current_tetromino(renderer, request);
        break;

        case LEFT:
            request.x -= 1;
            render_current_tetromino(renderer, request);
        break;

        case RIGHT:
            request.x += 1;
            render_current_tetromino(renderer, request);

        break;

        case DROP:

            request.y += 1;
            while(render_current_tetromino(renderer, request))
                request.y += 1;

            lockTetromino(renderer);

        break;

        case DOWN:
            request.y += 1;
            if(!render_current_tetromino(renderer, request)) {
                lock_delay_count++;
            } else {
                lock_delay_count = 0;
            }
        break;

        case RESTART:
            initTetris(renderer);
        break;

        case AUTO_DROP:

            request.y += 1;
            if (!render_current_tetromino(renderer, request)) {
                lock_delay_count++;
            } else {
                lock_delay_count = 0;
            }

            if (lock_delay_count >= lock_delay_threshold) {
                lockTetromino(renderer);
            }

        break;
    }
    TETROMINO_ACTION = NONE;

}

void spawn_tetromino(Renderer_t renderer) {

    current_queue_index++;
    if(current_queue_index >= tetromino_queue_size) {
        current_queue_index = 0;

        // apply shuffle algorithm
        shuffle(tetromino_queue, tetromino_queue_size, sizeof(uint8_t));
    }

    Tetromino type;

    switch(tetromino_queue[current_queue_index]) {
        case 1:
            type = TETRA_I;
        break;
        case 2:
            type = TETRA_J;
        break;
        case 3:
            type = TETRA_L;
        break;
        case 4:
            type = TETRA_O;
        break;
        case 5:
            type = TETRA_S;
        break;
        case 6:
            type = TETRA_T;
        break;
        case 7:
            type = TETRA_Z;
        break;
    }

    Tetromino_Movement tetra_request = {
        type,
        0,
        3, 0
    };

    if(!render_current_tetromino(renderer, tetra_request)) {

        // Reset the game
        initTetris(renderer);
    }
}

bool can_render_tetromino(Tetromino_Movement tetra_request, uint8_t block_render_queue[]) {
    uint16_t bit, piece;
    uint8_t row = 0, col = 0;

    piece = tetra_request.type.rotation[tetra_request.rotation];
    uint8_t x = tetra_request.x;
    uint8_t y = tetra_request.y;


    // loop through tetramino data
    int i = 0;
    for (bit = 0x8000; bit > 0 && i < 8; bit = bit >> 1) {
        if (piece & bit) {

            uint8_t _x = x + col;
            uint8_t _y = y + row;
           
            // bounds check
            if ((_x >= PLAYFIELD_WIDTH)
                || (_y >= PLAYFIELD_HEIGHT)
                || get_playfield(_x, _y) != EMPTY) {
                
                // unable to render tetramino block
                return false;
                
            } else {
                 
                if(block_render_queue != NULL) {
                    block_render_queue[i * 2] = _x;
                    block_render_queue[i * 2 + 1] = _y;
                }

                i++;
            }
        }

        
        col++;
        col = col % 4;
        if(col == 0) {
            row++;
        }

    }

    return true;

}

bool render_current_tetromino(Renderer_t renderer, Tetromino_Movement tetra_request) {

    // create ghost
    Tetromino ghost = tetra_request.type;

    // change alpha to ~50%
    ghost.color = (ghost.color & 0x00FFFFFF) | 0x36000000;

    Tetromino_Movement ghost_request = tetra_request;
    ghost_request.type = ghost;
    
    // render ghost tetromino
    
    while(render_tetromino(renderer, ghost_request, GHOST_TETROMINO_COORDS))
    {   
        ghost_request.y += 1;
        
    }
    
    // change alpha to 90%
    tetra_request.type.color = (tetra_request.type.color & 0x00FFFFFF) | 0xE5000000;
    
    
    if(render_tetromino(renderer, tetra_request, CURRENT_TETROMINO_COORDS)) {
        CURRENT_TETROMINO = tetra_request;

        return true;
    }

    return false;
}

// render tetromino movement request
// returns true if tetromino is rendered succesfully; false otherwise
bool render_tetromino(Renderer_t renderer, Tetromino_Movement tetra_request, uint8_t current_coords[]) {

    uint8_t block_render_queue[8] = {0};
    
    if(!can_render_tetromino(tetra_request, block_render_queue))
    {
        return false;
    }

    // clear old tetromino position
    int i = 4;
    while(i --> 0) {
        uint8_t x_coord = i * 2;
        uint8_t y_coord = x_coord + 1;

        uint8_t _x = current_coords[x_coord];
        uint8_t _y = current_coords[y_coord];

        draw_block(renderer, _x, _y, EMPTY);
    }


    // render new tetromino blocks
    i = 4;
    while(i --> 0) {

        uint8_t x_coord = i * 2;
        uint8_t y_coord = x_coord + 1;

        // store and draw new tetromino position
        uint8_t _x = block_render_queue[x_coord];
        uint8_t _y = block_render_queue[y_coord];

        current_coords[x_coord] = _x;
        current_coords[y_coord] = _y;

        draw_block(renderer, _x, _y, tetra_request.type.color);

    }

    return true;
}

Color_Block get_playfield(uint8_t x, uint8_t y) {
    return playfield[(y * PLAYFIELD_WIDTH) + x];
}

void set_playfield(Renderer_t renderer, uint8_t x, uint8_t y, Color_Block color) {
    playfield[(y * PLAYFIELD_WIDTH) + x] = color;

    draw_block(renderer, x, y, color);
}

bool getInput() {

    if (userevent==true)
    {
        TETROMINO_ACTION = AUTO_DROP;
    }

    while(1) {
        kbd_event event = keyboard_buffer_pop();
        if(keyboard_event_convert(event)==0)
        {
            break;
        }
        switch (keyboard_event_convert(event)) {
            case 's':
                TETROMINO_ACTION = DOWN;
            break;

            case 'd':
                TETROMINO_ACTION = RIGHT;
            break;

            case 'a':
                TETROMINO_ACTION = LEFT;
            break;

            case 'w':
                TETROMINO_ACTION = ROTATE;
            break;

            case 'r':
                TETROMINO_ACTION = RESTART;
            break;

            case ' ':
                TETROMINO_ACTION = DROP;
            break;
            case 'q':
            {
                return true;
            }
            break;
            default:
            break;
        }    
    }
    return false;
}


int tetris() {

    Renderer_t renderer = kmalloc(VESA_WIDTH * sizeof(rgba_color[VESA_HEIGHT])); // Allocate memory for the renderer array

    if (renderer == NULL) {
       
        return -1;
    }

    for (size_t raw = 0; raw < VESA_WIDTH; raw++) {
        memset(renderer[raw], 0, VESA_HEIGHT * sizeof(rgba_color));
    }
    
    initTetris(renderer);
    bool quit = false;
    while(!quit) {

        quit = getInput();
        updateTetris(renderer);
        
        if(render_changed) {
            draw_rendered(renderer);
            render_changed = false;
        }
        size_t tmr = 0;
        while (tmr<1000000)
        {
            tmr++;
        }

    }

    kfree(renderer);
    return 0;
}