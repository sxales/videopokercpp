#pragma once

#include <SDL.h>
#include "Game.h"
#include "Scene.h"
#include "LWindow.h"
#include "ResourceManager.h"
#include "ConfigManager.h"
#include "EventManager.h"
#include "BitmapFont.h"
#include "BoxHelper.h"

#include "Event.h"

struct TitleInteract : public Event {};
struct TitleTimeout : public Event {};

class TitleScene : public Scene {
private:
	int SCREEN_WIDTH = 0, SCREEN_HEIGHT = 0;
	int CARDHEIGHT = 0, CARDWIDTH = 0;
	int DEMO_DELAY = 0;
	int DEALERSIZE = 0;
	int FONTSIZE = 0;
	int _tick = 0;
public:
	TitleScene(Game* m_game) : Scene(m_game) {
		SCREEN_WIDTH = m_game->config_manager->getInt("WindowWidth");
		SCREEN_HEIGHT = m_game->config_manager->getInt("WindowHeight");
		CARDHEIGHT = m_game->config_manager->getInt("CardHeight");
		CARDWIDTH = m_game->config_manager->getInt("CardWidth");
		DEMO_DELAY = m_game->config_manager->getInt("DemoDelay");
		DEALERSIZE = m_game->config_manager->getInt("DealerSize");

		FONTSIZE = (SCREEN_WIDTH) / (25);
	}
	bool load() {
		m_game->resource_manager->addAsset("font", "Data/Textures/font.png");
		m_game->resource_manager->addAsset("cards", m_game->config_manager->getString("CardTexture"));
		m_game->resource_manager->addAsset("royalflush", "Data/Textures/ROYALFLUSH.png");
		m_game->resource_manager->addAsset("tileset", "Data/Textures/tiles.png");

		return true;
	}
	bool unload() {
		m_game->resource_manager->removeAsset("font");
		m_game->resource_manager->removeAsset("cards");
		m_game->resource_manager->removeAsset("royalflush");
		m_game->resource_manager->removeAsset("tileset");

		return true;
	}
	void render(float interpolation) {
		//Clear screen
		SDL_SetRenderDrawColor(LWindow::getInstance()->mRenderer, 0x0, 0x0, 0x0, 0xFF);
		SDL_RenderClear(LWindow::getInstance()->mRenderer);

		BitmapFont font;
		BoxHelper box;

		int col = 15;
		int row = 6;

		int adjustedwidth = SCREEN_WIDTH / col;
		int adjustedheight = SCREEN_HEIGHT / row;

		for (int i = 0; i < row * col; i++) {
			SDL_Rect cardTexture;
			if (_tick % 2 == 0) cardTexture = { 13 * CARDWIDTH, CARDHEIGHT * (i % 2), CARDWIDTH, CARDHEIGHT };
			else cardTexture = { 13 * CARDWIDTH, CARDHEIGHT * ((i + 1) % 2), CARDWIDTH, CARDHEIGHT };

			SDL_Rect cardResize = { 3 + adjustedwidth * (i % col), adjustedheight * (floor(i / col)), adjustedwidth, adjustedheight };
			SDL_RenderCopyEx(m_game->m_window->mRenderer, m_game->resource_manager->getAsset("cards"), &cardTexture, &cardResize, NULL, NULL, SDL_FLIP_NONE);
		}

		int w = SCREEN_HEIGHT / 2 - FONTSIZE * 2;
		int vpos = (SCREEN_HEIGHT - w) / 2;
		int hpos = (SCREEN_WIDTH - w) / 2;
		//draw dealer
		//drawBox(ctx, 20, hpos, vpos, w, w);
		int c = row * col * 2 / 5;
		SDL_Rect dealerTexture = { DEALERSIZE * 0, 0, DEALERSIZE, DEALERSIZE };
		SDL_Rect dealerResize = { hpos + 5, vpos + 5, w - 12, w - 12 };

		SDL_RenderCopyEx(m_game->m_window->mRenderer, m_game->resource_manager->getAsset("royalflush"), &dealerTexture, &dealerResize, NULL, NULL, SDL_FLIP_NONE);

		int height = FONTSIZE * 2;
		int width = FONTSIZE * 13;
		vpos = (vpos - FONTSIZE * 2) / 2;
		hpos = (SCREEN_WIDTH - width) / 2;

		box.drawBox(m_game->m_window->mRenderer, m_game->resource_manager->getAsset("tileset"), 20, hpos, vpos - FONTSIZE / 2, width, height);
		font.renderText(m_game->m_window->mRenderer, "Video Poker!", (SCREEN_WIDTH - FONTSIZE * 12) / 2, vpos, m_game->resource_manager->getAsset("font"), 2, FONTSIZE);

		if (_tick % 2 == 0) font.renderText(m_game->m_window->mRenderer, "Click to start", (SCREEN_WIDTH - FONTSIZE * 14) / 2, w + (SCREEN_HEIGHT - w) / 2 + vpos, m_game->resource_manager->getAsset("font"), 5, FONTSIZE);

		//Update screen
		SDL_RenderPresent(LWindow::getInstance()->mRenderer);
	}
	void update() {
		if (++_tick > DEMO_DELAY) m_game->event_manager->trigger(TitleTimeout());
	}
	void handleEvents(SDL_Event& e) {
		if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT)
			m_game->event_manager->trigger(TitleInteract());
	}
};