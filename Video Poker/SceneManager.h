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
	std::list<std::unique_ptr<Scene>> m_scenes;
public:
	~SceneManager() {
		while (!m_scenes.empty()) popScene();
	}
	void update() {
		if (m_scenes.empty()) return;
		m_scenes.back()->update();
	};
	void render(float interpolation) {
		if (m_scenes.empty()) return;
		m_scenes.back()->render(interpolation);
	};
	void handleEvents(SDL_Event& e) {
		if (m_scenes.empty()) return;
		m_scenes.back()->handleEvents(e);
	};

	void pushScene(std::unique_ptr<Scene> scene) {
		if (!scene) {
			std::cerr << "Invalid scene!";
			return;
		}
		if (!scene->load()) {
			std::cerr << "Unable to load scene!";
			return;
		}

		m_scenes.push_back(std::move(scene));
	};

	void pushScene(Scene* scene) {
		pushScene(std::unique_ptr<Scene>(scene));
	}

	void popScene() {
		if (m_scenes.empty()) return;

		std::unique_ptr<Scene>& back = m_scenes.back();

		if (!back) return;

		if (!back->unload()) std::cerr << "Unable to unload scene!";

		m_scenes.pop_back();
	};
};

#endif