#pragma once

#ifndef GAME_H
#define GAME_H

#include <string>

class LWindow;
class ConfigManager;
class SceneManager;
class ResourceManager;
class EventManager;

class Game {
public:
	LWindow* m_window;
	ConfigManager* config_manager;
	SceneManager* scene_manager;
	ResourceManager* resource_manager;
	EventManager* event_manager;

	Game(const std::string config);

	void close();

	//Create window and dynamically load initial scene
	bool create_window();

	//Game loop
	void run();
};

#endif