#pragma once
#pragma once

#include <SDL.h>
#include <vector>

#include "Game.h"
#include "Scene.h"
#include "LWindow.h"

#include "ResourceManager.h"
#include "ConfigManager.h"
#include "EventManager.h"
#include "Button.h"

#include "BitmapFont.h"
#include "BoxHelper.h"
#include "ButtonHelper.h"

#include "Event.h"

struct CharacterSelected : public Event {
	int dealer;
	CharacterSelected(int d) : dealer(d) {}
};

class CharacterSelectScene : public Scene {
private:
	int SCREEN_WIDTH = 0;
	int SCREEN_HEIGHT = 0;
	int CHARACTER_WIDTH = 0;
	int CHARACTER_HEIGHT = 0;
	int MAXDEALER = 0;
	int fontsize = 0;
	Button buttons[12];
	Button btnrandom;
public:
	CharacterSelectScene(Game* m_game) : Scene(m_game) {
		if (!m_game) std::cerr << "Game pointer null?\n";
		if (!m_game->event_manager) std:cerr << "Event Manager pointer null?\n";

		SCREEN_WIDTH = m_game->config_manager->getInt("WindowWidth");
		SCREEN_HEIGHT = m_game->config_manager->getInt("WindowHeight");
		CHARACTER_WIDTH = m_game->config_manager->getInt("CharacterWidth");
		CHARACTER_HEIGHT = m_game->config_manager->getInt("CharacterHeight");
		MAXDEALER = m_game->config_manager->getInt("MaxDealer");

		if (MAXDEALER > 12) std::cerr << "Too many dealers!\n";

		int buttonheight = SCREEN_HEIGHT / 3;
		int buttonwidth = buttonheight * ((float)CHARACTER_WIDTH / (float)CHARACTER_HEIGHT);
		//int buttonheight = buttonwidth * ((float)CHARACTER_HEIGHT / (float)CHARACTER_WIDTH);

		int col = ((float)SCREEN_WIDTH / (float)buttonwidth) - 2;

		for (int i = 0;i < MAXDEALER; i++) {
			int hpos = 0;
			if (i < col) hpos = (SCREEN_WIDTH - (buttonwidth * col)) / 2;
			else hpos = (SCREEN_WIDTH - (buttonwidth * (MAXDEALER % col))) / 2;
			buttons[i] = Button("", hpos + buttonwidth * (i % col), ((SCREEN_HEIGHT - (buttonheight * 2)) / 2) + buttonheight * (i / col), buttonwidth, buttonheight);
		}

		fontsize = buttonheight / 4;
		btnrandom = Button("random", (SCREEN_WIDTH - fontsize * 6) / 2, (buttonheight * 2.5) + (fontsize / 2), fontsize * 6, fontsize);
	}
	bool load() {
		/*m_game->resource_manager->addAsset("characters", m_game->config_manager->getString("CharacterTexture"));
		m_game->resource_manager->addAsset("font", "Data/Textures/font.png");
		m_game->resource_manager->addAsset("tileset", "Data/Textures/tiles.png");*/

		return true;
	}
	bool unload() {
		/*m_game->resource_manager->removeAsset("characters");
		m_game->resource_manager->removeAsset("font");
		m_game->resource_manager->removeAsset("tileset");*/

		return true;
	}
	void render(float interpolation) {
		//Clear screen
		SDL_SetRenderDrawColor(m_game->m_window->mRenderer, 27, 94, 32, 0xFF);
		SDL_RenderClear(m_game->m_window->mRenderer);

		SDL_Texture* characters = m_game->resource_manager->getAsset("characters");

		SDL_SetRenderDrawColor(m_game->m_window->mRenderer, 224, 224, 0, 255);
		for (int i = 0; i < MAXDEALER; i++) {
			Button& b = buttons[i];
			SDL_Rect texture = { CHARACTER_WIDTH * i, (b.over) ? CHARACTER_HEIGHT : 0, CHARACTER_WIDTH, CHARACTER_HEIGHT};
			SDL_Rect resize = { b.x, b.y, b.width, b.height };
			SDL_RenderFillRect(m_game->m_window->mRenderer, &resize);

			resize = { b.x + 2, b.y + 2, b.width - 4, b.height - 4 };
			SDL_RenderCopyEx(m_game->m_window->mRenderer, characters, &texture, &resize, NULL, NULL, SDL_FLIP_NONE);
		}

		ButtonHelper helper;
		helper.drawButton(m_game->m_window->mRenderer, m_game->resource_manager->getAsset("tileset"), m_game->resource_manager->getAsset("font"), btnrandom);

		std::string txt = "select dealer";
		BoxHelper boxhelper;
		boxhelper.drawBox(m_game->m_window->mRenderer, m_game->resource_manager->getAsset("tileset"), 20, (SCREEN_WIDTH - ((txt.length() + 1) * fontsize / 2)) / 2, fontsize / 2, fontsize / 2 * (txt.length() + 1), fontsize);
		BitmapFont font;
		font.renderText(m_game->m_window->mRenderer, txt, (SCREEN_WIDTH - ((txt.length() + 2) * fontsize / 2)) / 2 + (fontsize / 2), 3 * fontsize / 4, m_game->resource_manager->getAsset("font"), logo, fontsize / 2);

		//Update screen
		SDL_RenderPresent(m_game->m_window->mRenderer);
	}
	void update() {
	}
	void handleEvents(SDL_Event& e) {
		if (e.type == SDL_MOUSEMOTION) {
			for (int i = 0; i < MAXDEALER; i++) buttons[i].check(e.motion.x, e.motion.y);
			btnrandom.check(e.motion.x, e.motion.y);
		}
		else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
			for (int i = 0; i < MAXDEALER; i++) {
				if (buttons[i].check(e.motion.x, e.motion.y)) m_game->event_manager->trigger(CharacterSelected(i % MAXDEALER));
			}
			if (btnrandom.check(e.motion.x, e.motion.y)) {
				srand(time(NULL));
				m_game->event_manager->trigger(CharacterSelected(rand() % MAXDEALER));
			}
		}
		else if (e.type == SDL_KEYUP) {
			switch (e.key.keysym.sym) {
			case SDLK_ESCAPE:
				m_game->event_manager->trigger(PlayEnds(0,0,0));
				return;
				break;
			}
		}
	}
};