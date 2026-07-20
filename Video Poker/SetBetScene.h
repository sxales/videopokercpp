#pragma once
#pragma once

#include <SDL.h>
#include <vector>

#include "Game.h"
#include "Scene.h"
#include "LWindow.h"

#include "ResourceManager.h"
#include "ConfigManager.h"
#include "EventManager.h"
#include "Button.h"

#include "BitmapFont.h"
#include "BoxHelper.h"
#include "ButtonHelper.h"

#include "Event.h"

struct BetSet : public Event {
	int dealer;
	int bet;
	BetSet(int d, int b) : dealer(d), bet(b) {}
};

struct SetBetTimeout : public Event {};

class SetBetScene : public Scene {
private:
	enum Outcome { royal_flush, straight_flush, four_of_a_kind, full_house, flush, straight, three_of_a_kind, two_pair, jacks_or_better, nothing, low_pair, high_card, four_card_flush, four_card_straight, four_card_straight_flush, three_card_flush, three_card_straight, three_card_straight_flush };
	const int PAYOUT[10] = { 250, 50, 25, 9, 6, 4, 3, 2, 1, 0 };
	const std::string LABEL[10] = { "RF", "SF", "4K", "FH", "FL", "ST", "3K", "2P", "JB", "ER" };
	int SCREEN_WIDTH = 0;
	int SCREEN_HEIGHT = 0;
	int DEALERSIZE = 0;
	int MAXDEALER = 0;
	float SCREENRATIO = 0.2;
	int fontsize = 0;
	int TIMEOUT = 0;
	int count = 0;
	Button btnincrease, btndecrease, btnconfirm;
	int bet = 50;
	int defaultbet = 50;
	int dealer = 0;
public:
	SetBetScene(Game* m_game) : Scene(m_game) {
		SCREEN_WIDTH = m_game->config_manager->getInt("WindowWidth");
		SCREEN_HEIGHT = m_game->config_manager->getInt("WindowHeight");
		TIMEOUT = m_game->config_manager->getInt("CharacterSelectTimeout");
		DEALERSIZE = m_game->config_manager->getInt("DealerSize");
		MAXDEALER = m_game->config_manager->getInt("MaxDealer");

		fontsize = ((SCREEN_WIDTH * (1 - SCREENRATIO)) / (25));

		int buttonheight = fontsize * 2;
		int buttonwidth = SCREEN_WIDTH / 7;

		btnincrease = Button("+", SCREEN_WIDTH * SCREENRATIO + (SCREEN_WIDTH * (1 - SCREENRATIO) - FONTSIZE * 3) / 2, (SCREEN_HEIGHT / 2 - FONTSIZE * 3.5), FONTSIZE * 3, FONTSIZE * 3);
		btndecrease = Button("-", SCREEN_WIDTH * SCREENRATIO + (SCREEN_WIDTH * (1 - SCREENRATIO) - FONTSIZE * 3) / 2, (SCREEN_HEIGHT / 2 + FONTSIZE * 0.5), FONTSIZE * 3, FONTSIZE * 3);
		btnconfirm = Button("done", SCREEN_WIDTH * SCREENRATIO + (SCREEN_WIDTH * (1 - SCREENRATIO) - buttonwidth * 2) / 2, SCREEN_HEIGHT - buttonheight - FONTSIZE / 2, buttonwidth * 2, buttonheight);

	}
	bool load() {
		return true;
	}
	bool unload() {
		return true;
	}
	void render(float interpolation) {
		//Clear screen
		SDL_SetRenderDrawColor(m_game->m_window->mRenderer, 27, 94, 32, 0xFF);
		SDL_RenderClear(m_game->m_window->mRenderer);

		BitmapFont font;
		BoxHelper box;
		ButtonHelper bhelper;

		std::string txt = "" + to_string(bet);
		int fs = (SCREEN_WIDTH * (1 - SCREENRATIO)) / (25);
		int vpos = fs / 2;
		int hpos = SCREEN_WIDTH * SCREENRATIO + (SCREEN_WIDTH * (1 - SCREENRATIO) - fs * (17 + txt.length())) / 2;

		box.drawBox(m_game->m_window->mRenderer, m_game->resource_manager->getAsset("tileset"), 20, hpos, vpos, fs * (17 + txt.length()), fs * 2);
		font.renderText(m_game->m_window->mRenderer, "bank", hpos + fs / 2, vpos + fs / 2, m_game->resource_manager->getAsset("font"), white, fs);
		font.renderText(m_game->m_window->mRenderer, "000500", hpos + fs / 2 + fs * 5, vpos + fs / 2, m_game->resource_manager->getAsset("font"), pink, fs);

		font.renderText(m_game->m_window->mRenderer, "bet", hpos + fs / 2 + fs * 12, vpos + fs / 2, m_game->resource_manager->getAsset("font"), white, fs);
		font.renderText(m_game->m_window->mRenderer, to_string(bet), hpos + fs / 2 + fs * 16, vpos + fs / 2, m_game->resource_manager->getAsset("font"), pink, fs);

		fs = (SCREEN_WIDTH * SCREENRATIO) / 9;
		int dealerwidth = SCREEN_WIDTH * SCREENRATIO * 0.85;
		vpos = (SCREEN_HEIGHT - (dealerwidth + fs * 11.75)) / 2 + FONTSIZE / 2;
		hpos = (SCREEN_WIDTH * SCREENRATIO);

		box.drawBox(m_game->m_window->mRenderer, m_game->resource_manager->getAsset("tileset"), 20, hpos - FONTSIZE / 2, vpos - FONTSIZE / 2, FONTSIZE * 12, FONTSIZE * 2);
		font.renderText(m_game->m_window->mRenderer, "Select bet!", hpos, vpos, m_game->resource_manager->getAsset("font"), 0, FONTSIZE);

		bhelper.drawButton(m_game->m_window->mRenderer, m_game->resource_manager->getAsset("tileset"), m_game->resource_manager->getAsset("font"), btnconfirm);
		bhelper.drawButton(m_game->m_window->mRenderer, m_game->resource_manager->getAsset("tileset"), m_game->resource_manager->getAsset("font"), btnincrease);
		bhelper.drawButton(m_game->m_window->mRenderer, m_game->resource_manager->getAsset("tileset"), m_game->resource_manager->getAsset("font"), btndecrease);

		//draw dealer
		vpos = (SCREEN_HEIGHT - (dealerwidth + fs * 11.75)) / 2;
		hpos = (SCREEN_WIDTH * SCREENRATIO - dealerwidth) / 2;
		//drawBox(20, hpos, vpos, dealerwidth, dealerwidth);
		SDL_Rect outline = { hpos, vpos, dealerwidth, dealerwidth };
		SDL_SetRenderDrawColor(m_game->m_window->mRenderer, 224, 224, 0, 255);
		SDL_RenderFillRect(m_game->m_window->mRenderer, &outline);
		SDL_Rect dealerTexture = { DEALERSIZE * (dealer), 0, DEALERSIZE, DEALERSIZE };
		SDL_Rect dealerResize = { hpos + 5, vpos + 5, dealerwidth - 10, dealerwidth - 10 };

		SDL_RenderCopyEx(m_game->m_window->mRenderer, m_game->resource_manager->getAsset("dealer"), &dealerTexture, &dealerResize, NULL, NULL, SDL_FLIP_NONE);

		//draw payouts
		hpos = ((SCREEN_WIDTH * SCREENRATIO) - fs * 8) / 2;
		vpos = vpos + dealerwidth + fs / 2;

		for (int i = 0; i < nothing; i++) {
			std::string txt = fill(to_string(PAYOUT[i] * bet), 6, ".");
			font.renderText(m_game->m_window->mRenderer, LABEL[i] + txt, hpos, vpos + fs * i, m_game->resource_manager->getAsset("font"), 6, fs);
		}

		//Update screen
		SDL_RenderPresent(m_game->m_window->mRenderer);
	}
	void update() {
		if (++count > TIMEOUT) m_game->event_manager->trigger(SetBetTimeout());
	}
	void handleEvents(SDL_Event& e) {
		if (e.type == SDL_MOUSEMOTION) {
			btnconfirm.check(e.motion.x, e.motion.y);
			btnincrease.check(e.motion.x, e.motion.y);
			btndecrease.check(e.motion.x, e.motion.y);
		}
		else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
			if (btnconfirm.check(e.motion.x, e.motion.y)) {
				m_game->event_manager->trigger(BetSet(dealer, bet));
				return;
			}
			else if (btnincrease.check(e.motion.x, e.motion.y)) {
				if (bet < defaultbet * 5) bet += defaultbet;
			}
			else if (btndecrease.check(e.motion.x, e.motion.y)) {
				if (bet > defaultbet) bet -= defaultbet;
			}
		}
		else if (e.type == SDL_KEYUP) {
			switch (e.key.keysym.sym) {
			case SDLK_ESCAPE:
				m_game->event_manager->trigger(SetBetTimeout());
				return;
				break;
			}
		}
	}

	std::string fill(std::string txt, int n, std::string pad) {
		std::string s = "" + txt;
		while (s.length() < n) {
			s = pad + s;
		}
		return s;
	};

	void select_dealer(int d) {
		dealer = d % MAXDEALER;
	}
};