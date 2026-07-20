#pragma once

#include <SDL.h>

#include "Button.h"
#include "BoxHelper.h"

class ButtonHelper {
public:
	void drawButton(SDL_Renderer* renderer, SDL_Texture* tileset, SDL_Texture* font, const Button button) {
		BoxHelper box;
		box.drawBox(renderer, tileset, (!button.enabled ? 22 : ((button.over) ? 10 : 12)), button.x, button.y, button.width, button.height);
		int fs = button.height / 2;
		BitmapFont bfont;
		bfont.renderText(renderer, button.txt, button.x + (button.width - fs * button.txt.length()) / 2, button.y + fs / 2, font, ((button.over || !button.enabled) ? 8 : 1), fs);
	}
};