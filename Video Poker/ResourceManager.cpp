#include "ResourceManager.h"

#include <iostream>

ResourceManager::ResourceManager(SDL_Renderer* renderer) : m_renderer(renderer) {
	//load error texture
	addAsset("Error", "Data/Textures/error.png");
}

ResourceManager::~ResourceManager() {
	removeAllAssets();
}

SDL_Texture* ResourceManager::loadFromFile(std::string path)
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	if (!m_renderer) printf("Renderer not set!\n");

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	if (!loadedSurface)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		/*
		//Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0xFF, 0, 0x80 ) ); //pink

		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(LWindow::getInstance()->mRenderer, loadedSurface );
		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			//Get image dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}*/

		//Convert surface to display format
		SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA8888, NULL);
		if (!formattedSurface)
		{
			printf("Unable to convert loaded surface to display format! %s\n", SDL_GetError());
		}
		else
		{
			//Create blank streamable texture
			newTexture = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);
			if (!newTexture)
			{
				printf("Unable to create blank texture! SDL Error: %s\n", SDL_GetError());
			}
			else
			{
				//Enable blending on texture
				SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);


				void* mPixels;
				int mPitch;

				//Lock texture for manipulation
				SDL_LockTexture(newTexture, &formattedSurface->clip_rect, &mPixels, &mPitch);

				//Copy loaded/formatted surface pixels
				memcpy(mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);

				//Get image dimensions
				int mWidth = formattedSurface->w;
				int mHeight = formattedSurface->h;

				//Get pixel data in editable format
				Uint32* pixels = (Uint32*)mPixels;
				int pixelCount = (mPitch / 4) * mHeight;

				//Map colors				
				Uint32 colorKey = SDL_MapRGB(formattedSurface->format, 0, 0xFF, 0xFF);
				Uint32 transparent = SDL_MapRGBA(formattedSurface->format, 0x00, 0xFF, 0xFF, 0x00);

				//Color key pixels
				for (int i = 0; i < pixelCount; ++i)
				{
					if (pixels[i] == colorKey)
					{
						pixels[i] = transparent;
					}
				}

				//Unlock texture to update
				SDL_UnlockTexture(newTexture);
				mPixels = NULL;
			}

			//Get rid of old formatted surface
			SDL_FreeSurface(formattedSurface);
		}
		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	//Return success
	return newTexture;
}

bool ResourceManager::addAsset(std::string key, SDL_Texture* texture) {
	if (!texture) return false;

	m_assets[key] = texture;

	return true;
}

bool ResourceManager::addAsset(std::string key, std::string path) {
	//Load texture
	SDL_Texture* temp = loadFromFile(path);

	if (!temp) {
		std::cerr << "Failed to load texture " << key.c_str() << "\n";
		delete temp;
		return false;
	}

	return addAsset(key, temp);
}

SDL_Texture* ResourceManager::getAsset(std::string key) {
	return m_assets[key];
}

void ResourceManager::freeAsset(SDL_Texture* texture) {
	SDL_DestroyTexture(texture);
	texture = nullptr;
}

void ResourceManager::removeAsset(std::string key) {
	freeAsset(m_assets[key]);
	m_assets.erase(key);
}

void ResourceManager::removeAllAssets() {
	for (auto it = m_assets.begin(); it != m_assets.end(); ++it) {
		freeAsset(it->second);
	}
	m_assets.clear();
}

void ResourceManager::close() {
	removeAllAssets();
}