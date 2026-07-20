#pragma once

#include <SDL.h>
#include "Game.h"
#include "Scene.h"
#include "LWindow.h"
#include "ResourceManager.h"
#include "ConfigManager.h"

#include "Event.h"

struct SpashTimeout : public Event {};

class SplashScene : public Scene {
private:
	int SCREEN_WIDTH = 0;
	int SCREEN_HEIGHT = 0;
	int count = 0;
	int TIMEOUT = 0;
public:
	SplashScene(Game* m_game) : Scene( m_game) {
		SCREEN_WIDTH = m_game->config_manager->getInt("WindowWidth");
		SCREEN_HEIGHT = m_game->config_manager->getInt("WindowHeight");
		TIMEOUT = m_game->config_manager->getInt("SplashTimeout");
	}
	bool load() {
		m_game->resource_manager->addAsset("logo", "Data/Textures/vyne.png");

		return true;
	}
	bool unload() {
		m_game->resource_manager->removeAsset("logo");

		return true;
	}
	void render(float interpolation) {
		//Clear screen
		SDL_SetRenderDrawColor(m_game->m_window->mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(m_game->m_window->mRenderer);

		SDL_Rect dealerTexture = { 0, 0, 279, 115 };
		SDL_Rect dealerResize = { (SCREEN_WIDTH - 279) / 2, (SCREEN_HEIGHT - 115) / 2, 279, 115 };
		//m_game->resource_manager->getAsset("logo")->render(, &dealerTexture, NULL);
		SDL_RenderCopyEx(m_game->m_window->mRenderer, m_game->resource_manager->getAsset("logo"), &dealerTexture, &dealerResize, NULL, NULL, SDL_FLIP_NONE);//top left corner

		SDL_SetTextureAlphaMod(m_game->resource_manager->getAsset("logo"), 255 - count * (255 / TIMEOUT));

		//Update screen
		SDL_RenderPresent(m_game->m_window->mRenderer);
	}
	void update() {
		if (++count > TIMEOUT) m_game->event_manager->trigger(SpashTimeout());
	}
	void handleEvents(SDL_Event& e) {}
};