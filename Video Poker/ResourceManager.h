#pragma once

#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <SDL.h>
#include <SDL_image.h>

#include <string>
#include <map>
using namespace std;

class ResourceManager {
private:
	SDL_Renderer* m_renderer;
	~ResourceManager();
	std::map<std::string, SDL_Texture*> m_assets;
	SDL_Texture* loadFromFile(std::string path);
	void freeAsset(SDL_Texture* texture);
public:
	ResourceManager(SDL_Renderer* renderer);
	bool addAsset(std::string key, std::string path);
	bool addAsset(std::string key, SDL_Texture* texture);
	SDL_Texture* getAsset(std::string key);
	void removeAsset(std::string key);
	void removeAllAssets();
	void close();
};

#endif