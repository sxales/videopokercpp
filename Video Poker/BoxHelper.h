#pragma once

#include <SDL.h>

class BoxHelper {
public:
	void drawBox(SDL_Renderer* renderer, SDL_Texture* tileset, int color, int x, int y, int width, int height) {
		SDL_Rect texture = { 64 * (color % 8), 64 * (int)floor(color / 8), 16, 16 };//top left corner
		SDL_Rect resize = { x, y, 16, 16 };
		SDL_RenderCopyEx(renderer, tileset, &texture, &resize, 0.0, NULL, SDL_FLIP_NONE);//top left corner

		resize = { x, y + height - 16, 16, 16 };
		SDL_RenderCopyEx(renderer, tileset, &texture, &resize, 0.0, NULL, SDL_FLIP_VERTICAL);//bottom left 

		resize = { x + width - 16, y, 16, 16 };
		SDL_RenderCopyEx(renderer, tileset, &texture, &resize, 0.0, NULL, SDL_FLIP_HORIZONTAL);//top right corner

		resize = { x + width - 16, y + height - 16, 16, 16 };
		SDL_RenderCopyEx(renderer, tileset, &texture, &resize, 0.0, NULL, SDL_RendererFlip(SDL_FLIP_VERTICAL | SDL_FLIP_HORIZONTAL));//bottom right corner

		texture = { 64 * (color % 8) + 16, 64 * (int)floor(color / 8), 16, 16 };
		resize = { x + 16, y, width - 32, 16 };
		//m_game->resource_manager->getAsset("tileset")->render(, &texture, &resize, 0.0, NULL, SDL_FLIP_NONE);//top
		SDL_RenderCopyEx(renderer, tileset, &texture, &resize, 0.0, NULL, SDL_FLIP_NONE);//top

		resize = { x + 16, y + height - 16, width - 32, 16 };
		SDL_RenderCopyEx(renderer, tileset, &texture, &resize, 0.0, NULL, SDL_FLIP_VERTICAL);//bottom

		texture = { 64 * (color % 8), 64 * (int)floor(color / 8) + 16, 16, 16 };
		resize = { x, y + 16, 16, height - 32 };
		SDL_RenderCopyEx(renderer, tileset, &texture, &resize, 0.0, NULL, SDL_FLIP_NONE);//left

		resize = { x + width - 16, y + 16, 16, height - 32 };
		SDL_RenderCopyEx(renderer, tileset, &texture, &resize, 0.0, NULL, SDL_FLIP_HORIZONTAL);//right

		texture = { 64 * (color % 8) + 16, 64 * (int)floor(color / 8) + 16, 16, 16 };
		resize = { x + 16, y + 16, width - 32, height - 32 };
		SDL_RenderCopyEx(renderer, tileset, &texture, &resize, 0.0, NULL, SDL_FLIP_NONE);//fill
	}
};