#pragma once

#include <SDL.h>
#include "Game.h"
#include "Scene.h"
#include "LWindow.h"
#include "ResourceManager.h"

#include "Event.h"

struct SplashComplete : public Event {

};

class SplashScene : public Scene {
private:
	const int SCREEN_WIDTH = 1280;
	const int SCREEN_HEIGHT = 720;
	int count = 0;
	int opacity = 255;
	const int TIMEOUT = 10;
public:
	SplashScene(Game* m_game) : Scene( m_game) {}
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
		SDL_SetRenderDrawColor(LWindow::getInstance()->mRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(LWindow::getInstance()->mRenderer);

		SDL_Rect dealerTexture = { 0, 0, 279, 115 };
		SDL_Rect dealerResize = { (SCREEN_WIDTH - 279) / 2, (SCREEN_HEIGHT - 115) / 2, 279, 115 };
		//m_game->resource_manager->getAsset("logo")->render(, &dealerTexture, NULL);
		SDL_RenderCopyEx(m_game->m_window->mRenderer, m_game->resource_manager->getAsset("logo"), &dealerTexture, &dealerResize, NULL, NULL, SDL_FLIP_NONE);//top left corner
		/*SDL_Rect outline1 = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
		SDL_SetRenderDrawColor(LWindow::getInstance()->mRenderer, 0xFF, 0xFF, 0xFF, opacity);
		SDL_RenderFillRect(LWindow::getInstance()->mRenderer, &outline1);*/

		//Update screen
		SDL_RenderPresent(LWindow::getInstance()->mRenderer);
	}
	void update() {
		if (++count > TIMEOUT) m_game->event_manager->trigger(SplashComplete());
		/*else if (count <= 10) opacity -= 25;
		else if (count <= TIMEOUT) opacity += 25;

		if (opacity < 0) opacity = 0;
		else if (opacity > 255) opacity = 255;*/
	}
	void handleEvents(SDL_Event& e) {}
};