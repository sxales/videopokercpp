#include <SDL.h>
#include <SDL_image.h>

#include "Game.h"
#include "LWindow.h"
#include "SceneManager.h"
#include "ConfigManager.h"
#include "ResourceManager.h"
#include "EventManager.h"
#include "GenericListener.h"

#include "SplashScene.h"
#include "TitleScene.h"
#include "PlayScene.h"
#include "CharacterSelectScene.h"

Game::Game(const std::string config) {
	config_manager = new ConfigManager(config);

	create_window();

	resource_manager = new ResourceManager(m_window->mRenderer);

	//scene transition events
	event_manager = new EventManager();
	event_manager->subscribe<SpashTimeout>(new GenericListener(nullptr, [&](Scene* _scene, const Event* evt) {
		scene_manager->popScene();
		scene_manager->pushScene(new TitleScene(this));
		}));
	event_manager->subscribe<TitleInteract>(new GenericListener(nullptr, [&](Scene* _scene, const Event* evt) {
		scene_manager->popScene();
		scene_manager->pushScene(new CharacterSelectScene(this));
		}));
	event_manager->subscribe<TitleTimeout>(new GenericListener(nullptr, [&](Scene* _scene, const Event* evt) {
		scene_manager->popScene();
		PlayScene* scene = new PlayScene(this);
		scene->start_demo();
		scene_manager->pushScene(scene);
		}));
	event_manager->subscribe<PlayEnds>(new GenericListener(nullptr, [&](Scene* _scene, const Event* evt) {
		scene_manager->popScene();
		scene_manager->pushScene(new TitleScene(this));
		}));
	event_manager->subscribe<CharacterSelected>(new GenericListener(nullptr, [&](Scene* _scene, const Event* evt) {
		scene_manager->popScene();
		PlayScene* scene = new PlayScene(this);
		scene->select_dealer(dynamic_cast<const CharacterSelected*>(evt)->dealer);
		scene_manager->pushScene(scene);
		}));

	//Initialize scene manager
	scene_manager = new SceneManager();
}

void Game::close()
{
	//Destroy window
	m_window->free();

	//delete window;
	delete config_manager;
	delete scene_manager;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

//Create window and dynamically load initial scene
bool Game::create_window()
{
	//Start up SDL and create window
	//Initialize SDL
	bool success = true;
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) > 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}

	//Set texture filtering to linear
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
	{
		printf("Warning: Linear texture filtering not enabled!");
	}

	//Create window
	m_window = m_window->getInstance();
	if (!m_window->init(config_manager->getString("WindowTitle").c_str(), config_manager->getInt("WindowWidth"), config_manager->getInt("WindowHeight")))
	{
		printf("Windows could not be created! SDL Error: %s\n", SDL_GetError());
		success = false;
	}

	return success;
}

//Game loop
void Game::run()
{
	//Main loop flag
	bool quit = false;

	//Event handler
	SDL_Event e;

	//TODO load dynamic scene from config
	scene_manager->pushScene(new SplashScene(this));

	uint32_t time_step_ms = std::floor(1000.0 / config_manager->getInt("UpdatesPerSecond"));
	uint32_t next_game_step = SDL_GetTicks(); // initial value

	//While application is running
	while (!quit)
	{
		//Handle events on queue
		while (SDL_PollEvent(&e) != 0)
		{
			//User requests quit
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}

			//Handle window events
			m_window->handleEvent(e);
			// (Where your code calls SDL_PollEvent())

			if (e.type == SDL_KEYUP) {
				if (e.key.keysym.sym == SDLK_F11) {
					m_window->toggleFullScreen();
				}
			}

			//Handle input for the player
			scene_manager->handleEvents(e);
		}

		uint32_t now = SDL_GetTicks();
		uint8_t computer_is_too_slow_limit = 5; // max number of updates per frame

		// Loop until all steps are executed or computer_is_too_slow_limit is reached
		while ((now > next_game_step) && (computer_is_too_slow_limit--)) {
			scene_manager->update();
			next_game_step += time_step_ms; // count 1 game tick done
		}
		float interpolation = (float)(next_game_step - now) / (float)time_step_ms;

		scene_manager->render(interpolation);

		// Rendering
		SDL_RenderPresent(m_window->mRenderer);

		SDL_Delay(2);//cap at 200fps to prevent wg from using too much gpu rendering a static screen
	}

	//Free resources and close SDL
	close();
}