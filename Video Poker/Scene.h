#pragma once

#ifndef SCENE_H
#define SCENE_H

#include <SDL.h>

class Game;

class Scene {
protected:
	Game* m_game = nullptr;
public:
	Scene(Game* game) : m_game(game) {};

	virtual void update() {};
	virtual void render(float interpolation) {};
	virtual void handleEvents(SDL_Event& e) {}
	virtual bool load() { return true; };
	virtual bool unload() { return true; };
};

#endif