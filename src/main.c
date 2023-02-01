#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define clamp(min, v, y) v > y ? y : (v < min ? min : v)

// constants
const uint16_t WINDOW_WIDTH  = 1200;
const uint16_t WINDOW_HEIGHT = 800;
const char* WINDOW_TITLE = "sdl test";

const uint8_t CIRCLE_VELOCITY = 1;
const uint8_t CIRCLE_RADIUS = 50;

const char* PRESS_START_2P_PATH = "./res/PressStart2P-Regular.ttf";
const uint8_t FONT_SIZE = 30;
const SDL_Color WHITE = { 255, 255, 255, 255 };
const SDL_Color BLACK = { 0, 0, 0, 0 };
const char* TEXT = "mucho texto";

const bool CENTER_ON_WINDOW = true;

SDL_Window *window;
SDL_Renderer *renderer;

bool KEYS[400];

void handle_input(SDL_Point *circle_position) {
	// Up Key
	if (KEYS[SDLK_w]) {
		circle_position->y -= CIRCLE_VELOCITY; 
	}

	// Down Key
	if (KEYS[SDLK_s]) {
		circle_position->y += CIRCLE_VELOCITY; 
	}

	// Right Key
	if (KEYS[SDLK_d]) {
		circle_position->x += CIRCLE_VELOCITY; 
	}

	// Left Key
	if (KEYS[SDLK_a]) {
		circle_position->x -= CIRCLE_VELOCITY; 
	}
}

void draw_circle(SDL_Renderer *renderer, const SDL_Color color, int32_t center_x, int32_t center_y, int32_t radius) {
	const int32_t diameter = (radius * 2);

	int32_t x = (radius - 1);
	int32_t y = 0;
	int32_t tx = 1;
	int32_t ty = 1;
	int32_t error = (tx - diameter);

	while (x >= y) {
		//  Each of the following renders an octant of the circle
		SDL_Point points[8] = {
			{ center_x + x, center_y - y },
			{ center_x + x, center_y + y },
			{ center_x - x, center_y - y },
			{ center_x - x, center_y + y },
			{ center_x + y, center_y - x },
			{ center_x + y, center_y + x },
			{ center_x - y, center_y - x },
			{ center_x - y, center_y + x }
		};

		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		SDL_RenderDrawPoints(renderer, points, 8);

		if (error <= 0) {
			++y;
			error += ty;
			ty += 2;
		}

		if (error > 0) {
			--x;
			tx += 2;
			error += (tx - diameter);
		}
	}
}

SDL_Texture *render_text(const char* message, const char* font_file, 
	SDL_Color color, const uint8_t font_size, SDL_Renderer *renderer) {
	TTF_Font *font = TTF_OpenFont(font_file, font_size);
	if (!font) {
		fprintf(stderr, "Cannot open font file \"%s\": %s\n", font_file, TTF_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_Surface *surface = TTF_RenderText_Blended(font, message, color);
	if (!surface) {
		fprintf(stderr, "Cannot render font \"%s\" to SDL_Surface: %s\n", font_file, TTF_GetError());
		TTF_CloseFont(font);
		exit(EXIT_FAILURE);
	}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	if (!texture) {
		fprintf(stderr, "Cannot render font \"%s\" from SDL_Surface to SDL_Texture: %s\n", font_file, TTF_GetError());
		exit(EXIT_FAILURE);
	}

	SDL_FreeSurface(surface);
	TTF_CloseFont(font);

	return texture;
}

int main() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Failure initializing SDL: %s\n", SDL_GetError());
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < 322; i++) {
		KEYS[i] = false;
	}

	window = SDL_CreateWindow(WINDOW_TITLE, 
		SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED, 
		WINDOW_WIDTH, 
		WINDOW_HEIGHT, 
		SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (TTF_Init() < 0) {
		fprintf(stderr, "Failure initializing SDL_ttf: %s\n", TTF_GetError());
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

	SDL_Texture *text = render_text(TEXT, PRESS_START_2P_PATH, WHITE, FONT_SIZE, renderer);
	if (!text) {
		fprintf(stderr, "Cannot generate SDL_Texture to render: %s\n", TTF_GetError());
		TTF_Quit();
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

	SDL_Rect text_rect;
	SDL_QueryTexture(text, NULL, NULL, &text_rect.w, &text_rect.h);

	if (CENTER_ON_WINDOW) {
		text_rect.x = WINDOW_WIDTH / 2 - text_rect.w / 2;
		text_rect.y = 40;
	}

	SDL_Point circle_position = { (WINDOW_WIDTH-CIRCLE_RADIUS)/2, (WINDOW_HEIGHT-CIRCLE_RADIUS)/2 }; 

	bool window_open = true;
	while (window_open) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				window_open = false;
				break;
            case SDL_KEYDOWN:
                KEYS[event.key.keysym.sym] = true;
                break;
            case SDL_KEYUP:
                KEYS[event.key.keysym.sym] = false;
                break;
			}
		}

		if (KEYS[SDLK_ESCAPE])
			window_open = false;
		handle_input(&circle_position);

		circle_position.x = clamp(CIRCLE_RADIUS, circle_position.x, WINDOW_WIDTH - CIRCLE_RADIUS);
		circle_position.y = clamp(CIRCLE_RADIUS, circle_position.y, WINDOW_HEIGHT - CIRCLE_RADIUS);

		SDL_SetRenderDrawColor(renderer, BLACK.r, BLACK.g, BLACK.b, BLACK.a);
		SDL_RenderClear(renderer);
		SDL_RenderCopy(renderer, text, NULL, &text_rect);
		draw_circle(renderer, WHITE, circle_position.x, circle_position.y, CIRCLE_RADIUS);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyTexture(text);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}