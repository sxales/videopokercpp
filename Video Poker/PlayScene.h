#pragma once

#ifndef PLAY_H
#define PLAY_H

#include <SDL.h>
#include <string>
#include <vector>
#include "LTexture.h"
#include "Scene.h"
#include "Message.h"
#include "Deck.h"
#include "Button.h"

#include "Event.h"

struct PlayEnds : public Event {
	int dealer, level, xp;
	PlayEnds(int d, int l, int x) : dealer(d), level(l), xp(x) {}
};

enum State { title, setbet, deal, flip, keep_or_discard, draw, score, reset, gameover };
enum Outcome { royal_flush, straight_flush, four_of_a_kind, full_house, flush, straight, three_of_a_kind, two_pair, jacks_or_better, nothing, low_pair, high_card, four_card_flush, four_card_straight, four_card_straight_flush, three_card_flush, three_card_straight, three_card_straight_flush };
enum FontColor { logo, white, super, red, green, blue, yellow, pink, gray };

class PlayScene : public Scene {
private:
	int SCREEN_WIDTH = 0, SCREEN_HEIGHT = 0;
	int CARDHEIGHT = 0, CARDWIDTH = 0;
	int DEALERSIZE = 0;
	int MAXDEALER = 0;
	int MAXLEVEL = 0;
	float SCREENRATIO = 0.2;
	int DEMO_DELAY = 0;
	int DEMO_TIMEOUT = 0;
	int SCORE_DELAY = 0;
	int HEARTSIZE = 0;

	const int PAYOUT[10] = { 250, 50, 25, 9, 6, 4, 3, 2, 1, 0 };
	const std::string LABEL[10] = { "RF", "SF", "4K", "FH", "FL", "ST", "3K", "2P", "JB", "ER"};
	int _dealer = 0;
	int _tick = 0, _nexttick = 0, _index = 0;

	int FONTSIZE = 0;
	int _cardwidth = 0, _cardheight = 0;

	int _level = 0, _currentxp = 0, _nextxp = 10;
	int _totalxp = 0;

	bool hold[5] = {false, false, false, false, false};

	int bank = 500, displayedbank = bank;
	int defaultbet = 50, bet = defaultbet;

	Button btnhold1, btnhold2, btnhold3, btnhold4, btnhold5;
	Button btndeal, btndraw;
	Button btndealer;
	Button btnquityes, btnquitno;
	bool show_quit_confirmation = false;

	Outcome outcome = nothing;
	State _state = title;

	bool debug = false;
	bool demo = false;
	int demo_timeout = 100;

	std::vector<Message> _messages;

	Deck shoe;
	std::vector<Card> hand;

	Outcome evaluate(std::vector<Card> _hand);
	std::string fill(std::string, int n, std::string pad);
	std::string translate_outcome(Outcome outcome);
	void process(Outcome _outcome);
public:
	PlayScene(Game* m_game);
	bool load();
	bool unload();
	void render(float interpolation);
	void update();
	void handleEvents(SDL_Event& e);
	void start_demo();
	void select_dealer(int d);
	void set_bet(int b);
};

#endif