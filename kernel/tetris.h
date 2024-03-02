#ifndef _TETRIS_CONSTANTS
#define _TETRIS_CONSTANTS

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <kernel/graphics/render.h>

#define WINDOW_TITLE "Tetris"

// a block 'pixel' of a playing field is 15px by 15px in size
#define BLOCK_SIZE 26

// standard size of a tetris playing field
#define PLAYFIELD_HEIGHT 22
#define PLAYFIELD_WIDTH 10


typedef struct
{

    // an array of rotation schemes of a tetromino.
    // each rotation scheme is represented as 16 bits which form 4x4 matrix.
    // row-major order convention is used to interpret this matrix.
    uint16_t rotation[4];

    // RGBA convention: 0xAABBGGRR
    uint32_t color;

} Tetromino;

typedef struct
{

    Tetromino type;

    // expected values from 0 to 4 which are the indices of Tetromino.rotation
    uint8_t rotation;

    uint8_t x;
    uint8_t y;

} Tetromino_Movement;

typedef enum
{
    NONE,
    DOWN,
    LEFT,
    RIGHT,
    DROP,
    ROTATE,
    // soft-drop tetrominos
    AUTO_DROP,
    RESTART
} Tetris_Action;

typedef struct UserEvent
{
    uint32_t type;        /**< ::USEREVENT through ::LASTEVENT-1 */
    uint32_t timestamp;   /**< In milliseconds, populated using GetTicks() */
    uint32_t windowID;    /**< The associated window if any */
    int32_t code;        /**< User defined event code */
    void *data1;        /**< User defined data pointer */
    void *data2;        /**< User defined data pointer */
} UserEvent;

typedef enum
{
    EMPTY = 0xFFB3C0CC,
    TEAL = 0xFFFFDB7F,
    BLUE = 0xFFD97400,
    ORANGE = 0XFF1B85FF,
    YELLOW = 0xFF00DCFF,
    GREEN = 0xFF40CC2E,
    PURPLE = 0xFF4B1485,
    RED = 0xFF4B59F2

} Color_Block;

// default tetris action
// defines the action to apply to current tetromino


// tetromino data
static const Tetromino TETRA_I = {
    {0x0F00, 0x2222, 0x00F0, 0x4444},
    TEAL};

static const Tetromino TETRA_J = {
    {0x8E00, 0x6440, 0x0E20, 0x44C0},
    BLUE};

static const Tetromino TETRA_L = {
    {0x2E00, 0x4460, 0x0E80, 0xC440},
    ORANGE};

static const Tetromino TETRA_O = {
    {0x6600, 0x6600, 0x6600, 0x6600},
    YELLOW};

static const Tetromino TETRA_S = {
    {0x6C00, 0x4620, 0x06C0, 0x8c40},
    GREEN};

static const Tetromino TETRA_T = {
    {0x4E00, 0x4640, 0x0E40, 0x4C40},
    PURPLE};

static const Tetromino TETRA_Z = {
    {0xC600, 0x2640, 0x0C60, 0x4C80},
    RED};

// simple array to store coords of blocks rendered on playing field.
// Each tetromino has 4 blocks with total of 4 coordinates.
//
// To access a coord, if 0 <= i < 4, then
//      x = i * 2, y = x + 1
//




// bool array of the playfield.
// Use row-major order convention to access (x,y) coord.
// Origin is 'top-left' -- like matrices.
// Zero-based indexing.


// Every time AUTO_DROP event is executed, the current tetromino will drop by one
// block. If the drop is unsucessful equal to the number of times of lock_delay_threshold,
// the tetromino freezes in place.
//
// Lock when ++lock_delay_count % lock_delay_threshold == 0
static const uint8_t lock_delay_threshold = 2;


// Queue to determine the next tetromino.
// Knuth shuffle algorithm is applied.


typedef enum
{
    FIRSTEVENT     = 0,     /**< Unused (do not remove) */

    /* Application events */
    QUIT           = 0x100, /**< User-requested quit */

    /* These application events have special meaning on iOS, see README-ios.md for details */
    APP_TERMINATING,        /**< The application is being terminated by the OS
                                     Called on iOS in applicationWillTerminate()
                                     Called on Android in onDestroy()
                                */
    APP_LOWMEMORY,          /**< The application is low on memory, free memory if possible.
                                     Called on iOS in applicationDidReceiveMemoryWarning()
                                     Called on Android in onLowMemory()
                                */
    APP_WILLENTERBACKGROUND, /**< The application is about to enter the background
                                     Called on iOS in applicationWillResignActive()
                                     Called on Android in onPause()
                                */
    APP_DIDENTERBACKGROUND, /**< The application did enter the background and may not get CPU for some time
                                     Called on iOS in applicationDidEnterBackground()
                                     Called on Android in onPause()
                                */
    APP_WILLENTERFOREGROUND, /**< The application is about to enter the foreground
                                     Called on iOS in applicationWillEnterForeground()
                                     Called on Android in onResume()
                                */
    APP_DIDENTERFOREGROUND, /**< The application is now interactive
                                     Called on iOS in applicationDidBecomeActive()
                                     Called on Android in onResume()
                                */

    LOCALECHANGED,  /**< The user's locale preferences have changed. */

    /* Display events */
    DISPLAYEVENT   = 0x150,  /**< Display state change */

    /* Window events */
    WINDOWEVENT    = 0x200, /**< Window state change */
    SYSWMEVENT,             /**< System specific event */

    /* Keyboard events */
    KEYDOWN        = 0x300, /**< Key pressed */
    KEYUP,                  /**< Key released */
    TEXTEDITING,            /**< Keyboard text editing (composition) */
    TEXTINPUT,              /**< Keyboard text input */
    KEYMAPCHANGED,          /**< Keymap changed due to a system event such as an
                                     input language or keyboard layout change.
                                */

    /* Mouse events */
    MOUSEMOTION    = 0x400, /**< Mouse moved */
    MOUSEBUTTONDOWN,        /**< Mouse button pressed */
    MOUSEBUTTONUP,          /**< Mouse button released */
    MOUSEWHEEL,             /**< Mouse wheel motion */

    /* Joystick events */
    JOYAXISMOTION  = 0x600, /**< Joystick axis motion */
    JOYBALLMOTION,          /**< Joystick trackball motion */
    JOYHATMOTION,           /**< Joystick hat position change */
    JOYBUTTONDOWN,          /**< Joystick button pressed */
    JOYBUTTONUP,            /**< Joystick button released */
    JOYDEVICEADDED,         /**< A new joystick has been inserted into the system */
    JOYDEVICEREMOVED,       /**< An opened joystick has been removed */

    /* Game controller events */
    CONTROLLERAXISMOTION  = 0x650, /**< Game controller axis motion */
    CONTROLLERBUTTONDOWN,          /**< Game controller button pressed */
    CONTROLLERBUTTONUP,            /**< Game controller button released */
    CONTROLLERDEVICEADDED,         /**< A new Game controller has been inserted into the system */
    CONTROLLERDEVICEREMOVED,       /**< An opened Game controller has been removed */
    CONTROLLERDEVICEREMAPPED,      /**< The controller mapping was updated */
    CONTROLLERTOUCHPADDOWN,        /**< Game controller touchpad was touched */
    CONTROLLERTOUCHPADMOTION,      /**< Game controller touchpad finger was moved */
    CONTROLLERTOUCHPADUP,          /**< Game controller touchpad finger was lifted */
    CONTROLLERSENSORUPDATE,        /**< Game controller sensor was updated */

    /* Touch events */
    FINGERDOWN      = 0x700,
    FINGERUP,
    FINGERMOTION,

    /* Gesture events */
    DOLLARGESTURE   = 0x800,
    DOLLARRECORD,
    MULTIGESTURE,

    /* Clipboard events */
    CLIPBOARDUPDATE = 0x900, /**< The clipboard changed */

    /* Drag and drop events */
    DROPFILE        = 0x1000, /**< The system requests a file open */
    DROPTEXT,                 /**< text/plain drag-and-drop event */
    DROPBEGIN,                /**< A new set of drops is beginning (NULL filename) */
    DROPCOMPLETE,             /**< Current set of drops is now complete (NULL filename) */

    /* Audio hotplug events */
    AUDIODEVICEADDED = 0x1100, /**< A new audio device is available */
    AUDIODEVICEREMOVED,        /**< An audio device has been removed. */

    /* Sensor events */
    SENSORUPDATE = 0x1200,     /**< A sensor was updated */

    /* Render events */
    RENDER_TARGETS_RESET = 0x2000, /**< The render targets have been reset and their contents need to be updated */
    RENDER_DEVICE_RESET, /**< The device has been reset and all textures need to be recreated */

    /* Internal events */
    POLLSENTINEL = 0x7F00, /**< Signals the end of an event poll cycle */

    /** Events ::USEREVENT through ::LASTEVENT are for your use,
     *  and should be allocated with RegisterEvents()
     */
    USEREVENT    = 0x8000,

    /**
     *  This last event is only for bounding internal arrays
     */
    LASTEVENT    = 0xFFFF
} EventType;


void draw_playing_field();
Color_Block get_playfield(uint8_t x, uint8_t y);
void set_playfield(Renderer_t renderer, uint8_t x, uint8_t y, Color_Block color);

void initTetris();
void updateTetris();
void lockTetromino();

void spawn_tetromino();
bool render_tetromino(Renderer_t renderer, Tetromino_Movement tetra_request, uint8_t current_coords[]);
bool render_current_tetromino(Renderer_t renderer, Tetromino_Movement tetra_request);

int rrand(int m);
void shuffle(void *obj, size_t nmemb, size_t size);
int tetris();

#endif