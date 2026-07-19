#pragma once

#include <SDL.h>
#include <string>

constexpr int FONTSIZE = 40;

class BitmapFont {
public:
	BitmapFont() {};
	~BitmapFont() {};

	void renderText(SDL_Renderer* renderer, std::string message, int x, int y, SDL_Texture* font, int subtype, int size) {
		for (int i = 0; i < message.length(); i++) {
			char ch = message.at(i);
			SDL_Rect spriteTexture;
			if (ch < 126 && ch >= 32) {
				//select sprite texture
				spriteTexture = { (ch - 32) * FONTSIZE, subtype * FONTSIZE, FONTSIZE, FONTSIZE };
			}
			else {
				//draw question mark
				//select sprite texture
				spriteTexture = { 41 * FONTSIZE,  subtype * FONTSIZE, FONTSIZE, FONTSIZE };
			}
			//scale to on screen size
			SDL_Rect rescale = SDL_Rect{ x + (i * size), y, size, size };
			//renter the actor

			SDL_RenderCopyEx(renderer, font, &spriteTexture, &rescale, NULL, NULL, SDL_FLIP_NONE);
		}
	};
};