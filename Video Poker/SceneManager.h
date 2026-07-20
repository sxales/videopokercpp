#pragma once

#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <list>
#include <memory>
#include <iostream>
#include <SDL.h>
#include "Scene.h"

//class Scene;
//class Message;

class SceneManager {
private:
	Scene* m_scene;
public:
	~SceneManager() {
		m_scene->unload();
		delete m_scene;
	}
	void update() {
		if (!m_scene) return;
		m_scene->update();
	};
	void render(float interpolation) {
		if (!m_scene) return;
		m_scene->render(interpolation);
	};
	void handleEvents(SDL_Event& e) {
		if (!m_scene) return;
		m_scene->handleEvents(e);
	};

	void swapScene(Scene* newScene) {
		if (m_scene) {
			m_scene->unload();
			delete m_scene;
		}
		newScene->load();
		m_scene = newScene;
	}
};

#endif