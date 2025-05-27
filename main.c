#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define CELL_SIZE 20
#define MAX_SNAKE_LENGTH 100

typedef struct {
    int x, y;
} Vec2;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static Vec2 snake[MAX_SNAKE_LENGTH];
static int snakeLength = 5;
static Vec2 dir = { 1, 0 };  // Startbewegung: nach rechts
static Uint32 lastMoveTime = 0;
static const Uint32 moveDelay = 150; // ms

// === INITIALISIERUNG ===
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    SDL_SetAppMetadata("Snake", "0.1", "com.example.snake");
    
    if (SDL_Init(SDL_INIT_VIDEO) != true) {
        SDL_Log("Fehler bei SDL_Init: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    
    if (!SDL_CreateWindowAndRenderer("Snake mit SDL3", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
        SDL_Log("Fenster/Renderer konnte nicht erstellt werden: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // Starte Schlange in Mitte
    int startX = (WINDOW_WIDTH / CELL_SIZE) / 2;
    int startY = (WINDOW_HEIGHT / CELL_SIZE) / 2;
    for (int i = 0; i < snakeLength; ++i) {
        snake[i].x = startX - i;
        snake[i].y = startY;
    }

    return SDL_APP_CONTINUE;
}

// === EVENT-HANDLING ===
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    if (event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;

    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
        case SDLK_UP:    if (dir.y != 1) dir = (Vec2){ 0, -1 }; break;
        case SDLK_DOWN:  if (dir.y != -1) dir = (Vec2){ 0, 1 };  break;
        case SDLK_LEFT:  if (dir.x != 1) dir = (Vec2){ -1, 0 }; break;
        case SDLK_RIGHT: if (dir.x != -1) dir = (Vec2){ 1, 0 };  break;
        }
    }

    return SDL_APP_CONTINUE;
}

// === SPIEL-LOGIK & ZEICHNUNG ===
SDL_AppResult SDL_AppIterate(void* appstate) {
    Uint32 now = SDL_GetTicks();
    if (now - lastMoveTime >= moveDelay) {
        lastMoveTime = now;

        // Schlange bewegen (Körper nachziehen)
        for (int i = snakeLength - 1; i > 0; --i) {
            snake[i] = snake[i - 1];
        }

        // Kopf bewegen
        snake[0].x += dir.x;
        snake[0].y += dir.y;

        // Kollision mit Rand (optional)
        if (snake[0].x < 0 || snake[0].x * CELL_SIZE >= WINDOW_WIDTH ||
            snake[0].y < 0 || snake[0].y * CELL_SIZE >= WINDOW_HEIGHT) {
            SDL_Log("Kollision mit Rand!");
            return SDL_APP_SUCCESS;
        }
    }

    // Hintergrund löschen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Schlange zeichnen
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (int i = 0; i < snakeLength; ++i) {
        SDL_FRect rect = {
            snake[i].x * CELL_SIZE + 1,
            snake[i].y * CELL_SIZE + 1,
            CELL_SIZE - 2,
            CELL_SIZE - 2
        };
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

// === BEENDEN ===
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    SDL_Log("Spiel beendet.");
}
