#include <time.h>
#include "Game.h"
#include "ResourceManager.h"
#include "LWindow.h"
#include "Deck.h"

Game::Game() {
	srand(time(NULL));

	_fontsize = (SCREEN_WIDTH * (1 - SCREENRATIO)) / (25);

	_tick = 0;
	_nexttick = DEMO_DELAY;

	_cardwidth = (SCREEN_WIDTH * (1 - SCREENRATIO) - _fontsize) / 5;
	_cardheight = _cardwidth * CARDHEIGHT / CARDWIDTH;
	int vpos = (SCREEN_HEIGHT - _cardheight) / 2;
	int hpos = SCREEN_WIDTH * SCREENRATIO + (SCREEN_WIDTH * (1 - SCREENRATIO) - _cardwidth * 5) / 2;

	int buttonheight = _fontsize * 2;
	int buttonwidth = SCREEN_WIDTH / 7;

	btnhold1 = Button("", hpos, vpos, _cardwidth, _cardheight);
	btnhold2 = Button("", hpos + _cardwidth * 1, vpos, _cardwidth, _cardheight);
	btnhold3 = Button("", hpos + _cardwidth * 2, vpos, _cardwidth, _cardheight);
	btnhold4 = Button("", hpos + _cardwidth * 3, vpos, _cardwidth, _cardheight);
	btnhold5 = Button("", hpos + _cardwidth * 4, vpos, _cardwidth, _cardheight);

	btndraw = Button("draw", SCREEN_WIDTH * SCREENRATIO + (SCREEN_WIDTH * (1 - SCREENRATIO) - buttonwidth * 2) / 2, SCREEN_HEIGHT - buttonheight - _fontsize / 2, buttonwidth * 2, buttonheight);
	btndeal = Button("deal", SCREEN_WIDTH * SCREENRATIO + (SCREEN_WIDTH * (1 - SCREENRATIO) - buttonwidth * 2) / 2, SCREEN_HEIGHT - buttonheight - _fontsize / 2, buttonwidth * 2, buttonheight);

	btnincrease = Button("+", SCREEN_WIDTH * SCREENRATIO + (SCREEN_WIDTH * (1 - SCREENRATIO) - _fontsize * 3) / 2, (SCREEN_HEIGHT / 2 - _fontsize * 3.5), _fontsize * 3, _fontsize * 3);
	btndecrease = Button("-", SCREEN_WIDTH * SCREENRATIO + (SCREEN_WIDTH * (1 - SCREENRATIO) - _fontsize * 3) / 2, (SCREEN_HEIGHT / 2 + _fontsize * 0.5), _fontsize * 3, _fontsize * 3);
	btnconfirm = Button("done", SCREEN_WIDTH * SCREENRATIO + (SCREEN_WIDTH * (1 - SCREENRATIO) - buttonwidth * 2) / 2, SCREEN_HEIGHT - buttonheight - _fontsize / 2, buttonwidth * 2, buttonheight);

	btnquit= Button("X", SCREEN_WIDTH - _fontsize * 2, 0, _fontsize * 2, _fontsize * 2);

	int fs = (SCREEN_WIDTH * SCREENRATIO) / 9;
	int dealerwidth = SCREEN_WIDTH * SCREENRATIO * 0.85;
	vpos = (SCREEN_HEIGHT - (dealerwidth + fs * 11.75)) / 2;
	hpos = (SCREEN_WIDTH * SCREENRATIO - dealerwidth) / 2;
	btndealer = Button("", hpos, vpos, dealerwidth, dealerwidth);

	shoe.init(1);

	_state = title;
}

bool Game::load() {
	ResourceManager::getInstance()->addAsset("font", "Data/Textures/font.png");
	ResourceManager::getInstance()->addAsset("cards", "Data/Textures/cards.png");
	ResourceManager::getInstance()->addAsset("dealer", "Data/Textures/dealer_all_small.png");
	ResourceManager::getInstance()->addAsset("tileset", "Data/Textures/tiles.png");
	ResourceManager::getInstance()->addAsset("royalflush", "Data/Textures/ROYALFLUSH.png");

	return true;
}

bool Game::unload() {
	ResourceManager::getInstance()->removeAsset("font");
	ResourceManager::getInstance()->removeAsset("cards");
	ResourceManager::getInstance()->removeAsset("dealer");
	ResourceManager::getInstance()->removeAsset("tileset");
	ResourceManager::getInstance()->removeAsset("royalflush");

	return true;
}

void Game::render(float interpolation) {
	//Clear screen
	SDL_SetRenderDrawColor(LWindow::getInstance()->mRenderer, 27, 94, 32, 0xFF);
	SDL_RenderClear(LWindow::getInstance()->mRenderer);

	//draw title screen
	if (_state == title) {
		int col = 15;
		int row = 6;

		int adjustedwidth = SCREEN_WIDTH / col;
		int adjustedheight = SCREEN_HEIGHT / row;

		SDL_Rect cardResize = { 0, 0, adjustedwidth, adjustedheight };

		for (int i = 0; i < row * col; i++) {
			SDL_Rect cardTexture;
			if (_tick % 2 == 0) cardTexture = { 13 * CARDWIDTH, CARDHEIGHT * (i % 2), CARDWIDTH, CARDHEIGHT };
			else cardTexture = { 13 * CARDWIDTH, CARDHEIGHT * ((i + 1) % 2), CARDWIDTH, CARDHEIGHT };
			ResourceManager::getInstance()->getAsset("cards")->render(3 + adjustedwidth * (i % col), adjustedheight * (floor(i / col)), &cardTexture, &cardResize);
		}

		int w = SCREEN_HEIGHT / 2 - _fontsize * 2;
		int vpos = (SCREEN_HEIGHT - w) / 2;
		int hpos = (SCREEN_WIDTH - w) / 2;
		//draw dealer
		//drawBox(ctx, 20, hpos, vpos, w, w);
		int c = row * col * 2 / 5;
		SDL_Rect dealerTexture = { DEALERSIZE * 0, 0, DEALERSIZE, DEALERSIZE };
		SDL_Rect dealerResize = { 0, 0, w - 12, w - 12 };
		ResourceManager::getInstance()->getAsset("royalflush")->render(hpos + 5, vpos + 5, &dealerTexture, &dealerResize);

		int height = _fontsize * 2;
		int width = _fontsize * 13;
		vpos = (vpos - _fontsize * 2) / 2;
		hpos = (SCREEN_WIDTH - width) / 2;

		drawBox(20, hpos, vpos - _fontsize / 2, width, height);
		renderText("Video Poker!", (SCREEN_WIDTH - _fontsize * 12) / 2, vpos, ResourceManager::getInstance()->getAsset("font"), super, _fontsize);

		if (_tick % 2 == 0) renderText("Click to start", (SCREEN_WIDTH - _fontsize * 14) / 2, w + (SCREEN_HEIGHT - w) / 2 + vpos, ResourceManager::getInstance()->getAsset("font"), blue, _fontsize);
	}
	else if (_state > title && _state < gameover) btnquit.draw();

	//draw cards
	if (_state > deal && _state < gameover) {
		int vpos = (SCREEN_HEIGHT - _cardheight) / 2;
		int hpos = SCREEN_WIDTH * SCREENRATIO + (SCREEN_WIDTH * (1 - SCREENRATIO) - _cardwidth * 5) / 2;
		int fs = _cardwidth / 5;
		for (int i = 0; i < hand.size(); i++) {
			SDL_Rect cardTexture = { (hand[i].face - 1) * CARDWIDTH, hand[i].suit * CARDHEIGHT, CARDWIDTH, CARDHEIGHT };
			SDL_Rect cardResize = { 0, 0, _cardwidth, _cardheight };
			ResourceManager::getInstance()->getAsset("cards")->render(hpos + (_cardwidth)*i, vpos, &cardTexture, &cardResize);
			if (hold[i]) renderText("hold", hpos + (_cardwidth * i + (fs/2)), vpos + (_cardheight - fs) / 2, ResourceManager::getInstance()->getAsset("font"), super, fs);
		}
	}
	else if (_state == deal) {
		int vpos = (SCREEN_HEIGHT - _cardheight) / 2;
		int hpos = SCREEN_WIDTH * SCREENRATIO + (SCREEN_WIDTH * (1 - SCREENRATIO) - _cardwidth * 5) / 2;
		for (int i = 0; i < 5; i++) {
			SDL_Rect cardTexture = { 13 * CARDWIDTH, 0 * CARDHEIGHT * (i % 2), CARDWIDTH, CARDHEIGHT };
			SDL_Rect cardResize = { 0, 0, _cardwidth, _cardheight };
			ResourceManager::getInstance()->getAsset("cards")->render(hpos + (_cardwidth)*i, vpos, &cardTexture, &cardResize);
		}
	}

	//draw buttons
	if (demo && _state < gameover) {
		int buttonheight = _fontsize * 2;
		int buttonwidth = _fontsize * 8;
		if (_tick % 2 == 0) renderText("DEMO", (SCREEN_WIDTH * SCREENRATIO) + (SCREEN_WIDTH * (1 - SCREENRATIO) - buttonwidth) / 2, SCREEN_HEIGHT - buttonheight - _fontsize / 2, ResourceManager::getInstance()->getAsset("font"), super, _fontsize * 2);
	}
	else if (_state == deal) btndeal.draw();
	else if (_state == keep_or_discard) btndraw.draw();
	else if (_state == setbet) {
		btnconfirm.draw();
		btnincrease.draw();
		btndecrease.draw();
	}

	//draw scoreboard
	if (_state >= setbet && _state < gameover) {
		if (displayedbank < bank) displayedbank += round(bet * 0.05);
		else displayedbank = bank;

		//renderText("", x, y, ResourceManager::getInstance()->getAsset("font"), white, fs);

		std::string txt = "" +to_string(bet);
		int fs = (SCREEN_WIDTH * (1 - SCREENRATIO)) / (25);
		int vpos = fs / 2;
		int hpos = SCREEN_WIDTH * SCREENRATIO + (SCREEN_WIDTH * (1 - SCREENRATIO) - fs * (17 + txt.length())) / 2;

		drawBox(20, hpos, vpos, fs * (17 + txt.length()), fs * 2);
		renderText("bank", hpos + fs / 2, vpos + fs / 2, ResourceManager::getInstance()->getAsset("font"), white, fs);
		renderText(fill(to_string(displayedbank), 6, "0"), hpos + fs / 2 + fs * 5, vpos + fs / 2, ResourceManager::getInstance()->getAsset("font"), pink, fs);

		renderText("bet", hpos + fs / 2 + fs * 12, vpos + fs / 2, ResourceManager::getInstance()->getAsset("font"), white, fs);
		renderText(to_string(bet), hpos + fs / 2 + fs * 16, vpos + fs / 2, ResourceManager::getInstance()->getAsset("font"), pink, fs);

		//draw dealer
		fs = (SCREEN_WIDTH * SCREENRATIO) / 9;
		int dealerwidth = SCREEN_WIDTH * SCREENRATIO * 0.85;
		vpos = (SCREEN_HEIGHT - (dealerwidth + fs * 11.75)) / 2;
		hpos = (SCREEN_WIDTH * SCREENRATIO - dealerwidth) / 2;
		//drawBox(20, hpos, vpos, dealerwidth, dealerwidth);
		SDL_Rect outline = { hpos, vpos, dealerwidth, dealerwidth };
		SDL_SetRenderDrawColor(LWindow::getInstance()->mRenderer, 224, 224, 0, 255);
		SDL_RenderFillRect(LWindow::getInstance()->mRenderer, &outline);
		SDL_Rect dealerTexture = { DEALERSIZE * (_dealer), DEALERSIZE * (_level), DEALERSIZE, DEALERSIZE };
		SDL_Rect dealerResize = { 0, 0, dealerwidth - 10, dealerwidth - 10 };
		ResourceManager::getInstance()->getAsset("dealer")->render(hpos + 5, vpos + 5, &dealerTexture, &dealerResize);

		//draw payouts
		hpos = ((SCREEN_WIDTH * SCREENRATIO) - fs * 8) / 2;
		vpos = vpos + dealerwidth + fs / 2;

		for (int i = 0; i < nothing; i++) {
			std::string txt = fill(to_string(PAYOUT[i] * bet), 6, ".");
			if (outcome == i && _tick % 2 == 0) renderText(LABEL[i] + txt, hpos, vpos + fs * i, ResourceManager::getInstance()->getAsset("font"), logo, fs);
			else renderText(LABEL[i] + txt, hpos, vpos + fs * i, ResourceManager::getInstance()->getAsset("font"), yellow, fs);
		}

		if (_level < MAXLEVEL) {
			renderText("Affec.", hpos + fs * 1, vpos + fs * 10, ResourceManager::getInstance()->getAsset("font"), yellow, fs);

			int xpbarwidth = ((fs * 8) - 3) * ((double)_currentxp / (double)_nextxp);
			SDL_Rect xpbaroutline = { hpos , vpos + fs * 11, fs * 8, fs };
			SDL_RenderDrawRect(LWindow::getInstance()->mRenderer, &xpbaroutline);

			if (xpbarwidth > 0) {
				SDL_Rect xpbarlight = { hpos + 1, vpos + 1 + (fs * 11), xpbarwidth - 2, fs / 2 };
				SDL_SetRenderDrawColor(LWindow::getInstance()->mRenderer, 255, 255, 255, 255);
				SDL_RenderFillRect(LWindow::getInstance()->mRenderer, &xpbarlight);

				SDL_Rect xpbarshadow = { hpos + 3, vpos + (fs * 11.5), xpbarwidth - 1, fs / 2 - 1 };
				SDL_SetRenderDrawColor(LWindow::getInstance()->mRenderer, 0, 0, 0, 255);
				SDL_RenderFillRect(LWindow::getInstance()->mRenderer, &xpbarshadow);

				SDL_Rect xpbar = { hpos + 2, vpos + 2 + (fs * 11), xpbarwidth - 1, fs - 5 };
				SDL_SetRenderDrawColor(LWindow::getInstance()->mRenderer, 102, 2, 60, 255);
				SDL_RenderFillRect(LWindow::getInstance()->mRenderer, &xpbar);
			}
		}

		if (debug) renderText("debug on", hpos, vpos + fs * 12.5, ResourceManager::getInstance()->getAsset("font"), red, fs);
	}
	else if (_state == gameover) {
		int dealerwidth = SCREEN_HEIGHT / 2 - _fontsize * 2;
		int vpos = (SCREEN_HEIGHT - (dealerwidth + _fontsize * 2.5)) / 2;
		int hpos = (SCREEN_WIDTH - dealerwidth) / 2;
		//draw dealer
		//drawBox(20, hpos, vpos, dealerwidth, dealerwidth);
		SDL_Rect outline = { hpos, vpos, dealerwidth, dealerwidth };
		SDL_SetRenderDrawColor(LWindow::getInstance()->mRenderer, 224, 224, 0, 255);
		SDL_RenderFillRect(LWindow::getInstance()->mRenderer, &outline);
		SDL_Rect dealerTexture = { DEALERSIZE * (_dealer), DEALERSIZE * (_level), DEALERSIZE, DEALERSIZE };
		SDL_Rect dealerResize = { 0, 0, dealerwidth - 10, dealerwidth - 10 };
		ResourceManager::getInstance()->getAsset("dealer")->render(hpos + 5, vpos + 5, &dealerTexture, &dealerResize);

		renderText("Game Over!", (SCREEN_WIDTH - _fontsize * 10) / 2, vpos / 2, ResourceManager::getInstance()->getAsset("font"), super, _fontsize);

		drawBox(20, (SCREEN_WIDTH - _fontsize * 13) / 2 - _fontsize / 2, vpos + dealerwidth + _fontsize / 2, _fontsize * 14, _fontsize * 2);
		renderText("See you again", (SCREEN_WIDTH - _fontsize * 13) / 2, vpos + dealerwidth + _fontsize, ResourceManager::getInstance()->getAsset("font"), logo, _fontsize);

		std::string timer = "" + to_string((int)round((_nexttick - _tick) / 3 + 1));

		renderText(timer, (SCREEN_WIDTH - _fontsize * timer.length()) / 2, SCREEN_HEIGHT - (vpos / 2), ResourceManager::getInstance()->getAsset("font"), yellow, _fontsize);
	}

	//draw dealer dialog
	if (_state == setbet) {
		int fs = (SCREEN_WIDTH * SCREENRATIO) / 9;
		int dealerwidth = SCREEN_WIDTH * SCREENRATIO * 0.85;
		int vpos = (SCREEN_HEIGHT - (dealerwidth + fs * 11.75)) / 2 + _fontsize / 2;
		int hpos = (SCREEN_WIDTH * SCREENRATIO);

		drawBox(20, hpos - _fontsize / 2, vpos - _fontsize / 2, _fontsize * 12, _fontsize * 2);
		renderText("Select bet!", hpos, vpos, ResourceManager::getInstance()->getAsset("font"), logo, _fontsize);
	}

	//draw messages
	for (int i = 0; i < _messages.size(); i++) {
		_messages[i].y += _messages[i].delta;
		drawBox(20, _messages[i].x - _messages[i].size / 2, _messages[i].y - _messages[i].size / 2, _messages[i].message.length() * _messages[i].size + _messages[i].size, _messages[i].size * 2);
		renderText(_messages[i].message, _messages[i].x, _messages[i].y, ResourceManager::getInstance()->getAsset("font"), _messages[i].type, _messages[i].size);
	}

	//Update screen
	SDL_RenderPresent(LWindow::getInstance()->mRenderer);
}

void Game::update() {
	++_tick;

	if (demo) {
		if (!debug) --demo_timeout;
		if (demo_timeout < 0) {
			std::fill(std::begin(hold), std::end(hold), false);
			outcome = nothing;
			bank = 500;
			bet = defaultbet;
			displayedbank = bank;
			_nexttick = _tick + DEMO_DELAY;
			_dealer = 0;
			demo = false;
			_level = 0;
			_currentxp = 0;
			//demo_timeout = 300;
			_messages.clear();
			_state = title;
		}
	}

	if (_state == title && _nexttick < _tick) {
		demo = true;
		demo_timeout = 150;
		bank = 500;
		bet = defaultbet;
		displayedbank = bank;
		_dealer = rand() % MAXDEALER;
		_nexttick = _tick + 5;
		_state = deal;
	}
	else if (demo && _state == deal && _nexttick < _tick) {
		bank -= bet;
		hand.clear();
		shoe.shuffle();
		//for (int i = 0; i < 5; i++) hand.push_back(Card(club, 14, 0));
		for (int i = 0;i < 5;i++) hand.push_back(shoe.deal());
		//_index = 0;
		_nexttick = _tick + 5;
		_state = flip;
		
	}
	else if (demo && _state == flip && _nexttick < _tick) {
		process(evaluate());
		_nexttick = _tick + 5;
		_state = keep_or_discard;
	}
	else if (demo && _state == keep_or_discard && _nexttick < _tick) {
		for (int i = 0; i < 5; i++) {
			if (!hold[i]) hand[i] = Card(club, 14, 0);
		}
		_index = 0;
		_nexttick = _tick + 1;
		_state = draw;
	}
	else if (_state == draw && _nexttick < _tick) {
		_nexttick = _tick + 1;
		while (_index < 5 && hold[_index]) _index++;
		if (_index >= 5) _state = score;
		else if (!hold[_index]) hand[_index++] = shoe.deal();
	}
	else if (_state == score) {
		_nexttick = _tick + 5;
		outcome = evaluate();
		
		int fs = (SCREEN_WIDTH * SCREENRATIO) / 9;
		int dealerwidth = SCREEN_WIDTH * SCREENRATIO * 0.85;
		int vpos = (SCREEN_HEIGHT - (dealerwidth + fs * 11.75)) / 2 + _fontsize / 2;
		int hpos = (SCREEN_WIDTH * SCREENRATIO);

		if (outcome < nothing) {
			std::string txt = translate_outcome(outcome);
			_messages.push_back(Message(txt, logo, hpos, vpos, _fontsize, 7));
			bank += bet * PAYOUT[outcome];
		}

		if (_level < MAXLEVEL) _currentxp += 5 * (bet / 250.0);

		_state = reset;
	}
	else if (_state == reset && _nexttick < _tick) {
		std::fill(std::begin(hold), std::end(hold), false);
		outcome = nothing;

		if (_currentxp >= _nextxp) {
			++_level;
			_currentxp = 0;
			_nextxp *= 2;
		}

		if (bank < bet) {
			_nexttick = _tick + 25;
			_currentxp = 0;
			_state = gameover;
		}
		else {
			_nexttick = _tick + 5;
			_state = deal;
		}
	}
	else if (_state == gameover && _nexttick < _tick) {
		bank = 500;
		bet = defaultbet;
		displayedbank = bank;
		_nexttick = _tick + DEMO_DELAY;
		demo = false;
		_dealer = 0;
		_level = 0;
		_currentxp = 0;
		_nextxp = 10;
		//demo_timeout = 300;
		_state = title;
	}

	//update messages
	_messages.erase(std::remove_if(_messages.begin(), _messages.end(), [](Message& m) {
		return (++m.tick >= m.timeout);
	}), _messages.end());
}

void Game::handleEvents(SDL_Event& e) {
	if (e.type == SDL_MOUSEMOTION) {
		btnhold1.check(e.motion.x, e.motion.y);
		btnhold2.check(e.motion.x, e.motion.y);
		btnhold3.check(e.motion.x, e.motion.y);
		btnhold4.check(e.motion.x, e.motion.y);
		btnhold5.check(e.motion.x, e.motion.y);
		btndraw.check(e.motion.x, e.motion.y);
		btndeal.check(e.motion.x, e.motion.y);
		btnconfirm.check(e.motion.x, e.motion.y);
		btnincrease.check(e.motion.x, e.motion.y);
		btndecrease.check(e.motion.x, e.motion.y);
		btnquit.check(e.motion.x, e.motion.y);
	}
	else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
		if (demo) demo_timeout = -1;
		else if (_state == title) {
			_state = setbet;
			_dealer = rand() % MAXDEALER;
			//std::fill(std::begin(hold), std::end(hold), false);
		}
		else if (btnquit.check(e.motion.x, e.motion.y)) {
			std::fill(std::begin(hold), std::end(hold), false);
			_messages.clear();
			bank = 500;
			bet = defaultbet;
			displayedbank = bank;
			_nexttick = _tick + DEMO_DELAY;
			demo = false;
			_level = 0;
			_currentxp = 0;
			_nextxp = 10;
			outcome = nothing;
			//demo = false;
			_state = title;
		}
		else if (_state == setbet) {
			if (btnconfirm.check(e.motion.x, e.motion.y)) _state = deal;
			else if (btnincrease.check(e.motion.x, e.motion.y)) {
				if (bet < defaultbet * 5) bet += defaultbet;
			}
			else if (btndecrease.check(e.motion.x, e.motion.y)) {
				if (bet > defaultbet) bet -= defaultbet;
			}
			else if (btndealer.check(e.motion.x, e.motion.y)) {
				_dealer = (_dealer + 1) % MAXDEALER;
			}
		}
		else if (_state == deal && btndeal.check(e.motion.x, e.motion.y)) {
			bank -= bet;
			hand.clear();
			shoe.shuffle();
			//for (int i = 0; i < 5; i++) hand.push_back(Card(club, 14, 0));
			for (int i = 0;i < 5;i++) hand.push_back(shoe.deal());
			//_index = 0;
			//_nexttick = _tick + 10;
			_state = keep_or_discard;
		}
		else if (_state == keep_or_discard) {
			if (btnhold1.check(e.motion.x, e.motion.y)) hold[0] = !hold[0];
			else if (btnhold2.check(e.motion.x, e.motion.y)) hold[1] = !hold[1];
			else if (btnhold3.check(e.motion.x, e.motion.y)) hold[2] = !hold[2];
			else if (btnhold4.check(e.motion.x, e.motion.y)) hold[3] = !hold[3];
			else if (btnhold5.check(e.motion.x, e.motion.y)) hold[4] = !hold[4];
			else if (btndraw.check(e.motion.x, e.motion.y)) {
				_index = 0;
				for (int i = 0; i < 5; i++) {
					if (!hold[i]) hand[i] = Card(club, 14, 0);
				}
				_nexttick = _tick+1;
				_state = draw;
			}
			else if (btndealer.check(e.motion.x, e.motion.y)) {
				std::fill(std::begin(hold), std::end(hold), false);

				int fs = (SCREEN_WIDTH * SCREENRATIO) / 9;
				int dealerwidth = SCREEN_WIDTH * SCREENRATIO * 0.85;
				int vpos = (SCREEN_HEIGHT - (dealerwidth + fs * 11.75)) / 2 + _fontsize / 2;
				int hpos = (SCREEN_WIDTH * SCREENRATIO);

				Outcome _outcome = evaluate();

				process(_outcome);

				std::string txt = translate_outcome(_outcome);
				_messages.push_back(Message(txt, logo, hpos, vpos, _fontsize, 7));
			}
		}
	}
	else if (e.type == SDL_KEYUP) {
		switch (e.key.keysym.sym) {
		case SDLK_d:
			debug = !debug;
			break;
		}
		if (debug) {
			switch (e.key.keysym.sym) {
			case SDLK_UP:
				if (bet < defaultbet * 5) bet += defaultbet;
				break;
			case SDLK_DOWN:
				if (bet > defaultbet) bet -= defaultbet;
				break;
			case SDLK_m:
				bank += 500;
				break;
			case SDLK_n:
				_dealer = (_dealer + 1) % MAXDEALER;
				break;
			}
		}
	}
}

#define FONT_SIZE 40
void Game::renderText(std::string message, int x, int y, LTexture* font, int subtype, int size) {
	for (int i = 0; i < message.length(); i++) {
		int ch = message.at(i);
		SDL_Rect spriteTexture;
		if (ch < 126 && ch >= 32) {
			//select sprite texture
			spriteTexture = { (ch - 32) * FONT_SIZE, subtype * FONT_SIZE, FONT_SIZE, FONT_SIZE };
		}
		else {
			//draw question mark
			//select sprite texture
			spriteTexture = { 41 * FONT_SIZE,  subtype * FONT_SIZE, FONT_SIZE, FONT_SIZE };
		}
		//scale to on screen size
		SDL_Rect rescale = SDL_Rect{ 0, 0, size, size };
		//renter the actor
		font->render(x + (i * size), y, &spriteTexture, &rescale, 0);
	}
}

void Game::drawBox(int color, int x, int y, int width, int height) {
	SDL_Rect texture = { 64 * (color % 8), 64 * floor(color / 8), 16, 16 };//top left corner
	SDL_Rect resize = { 0, 0, 16, 16 };
	ResourceManager::getInstance()->getAsset("tileset")->render(x, y, &texture, &resize, 0.0, NULL, SDL_FLIP_NONE);//top left corner
	ResourceManager::getInstance()->getAsset("tileset")->render(x, y+height - 16, &texture, &resize, 0.0, NULL, SDL_FLIP_VERTICAL);//bottom left 
	ResourceManager::getInstance()->getAsset("tileset")->render(x+width - 16, y, &texture, &resize, 0.0, NULL, SDL_FLIP_HORIZONTAL);//top right corner
	ResourceManager::getInstance()->getAsset("tileset")->render(x+width - 16, y+height - 16, &texture, &resize, 0.0, NULL, SDL_RendererFlip(SDL_FLIP_VERTICAL|SDL_FLIP_HORIZONTAL));//bottom right corner

	texture = { 64 * (color % 8) + 16, 64 * (int)floor(color / 8), 16, 16 };
	resize = { 0, 0, width - 32, 16 };
	ResourceManager::getInstance()->getAsset("tileset")->render(x + 16, y, &texture, &resize, 0.0, NULL, SDL_FLIP_NONE);//top
	ResourceManager::getInstance()->getAsset("tileset")->render(x + 16, y + height - 16, &texture, &resize, 0.0, NULL, SDL_FLIP_VERTICAL);//bottom
	
	texture = { 64 * (color % 8), 64 * (int)floor(color / 8) + 16, 16, 16 };
	resize = { 0, 0, 16, height - 32 };
	ResourceManager::getInstance()->getAsset("tileset")->render(x, y + 16, &texture, &resize, 0.0, NULL, SDL_FLIP_NONE);//left
	ResourceManager::getInstance()->getAsset("tileset")->render(x + width - 16, y + 16, &texture, &resize, 0.0, NULL, SDL_FLIP_HORIZONTAL);//right

	texture = { 64 * (color % 8) + 16, 64 * (int)floor(color / 8) + 16, 16, 16 };
	resize = { 0, 0, width - 32, height - 32 };
	ResourceManager::getInstance()->getAsset("tileset")->render(x + 16, y + 16, &texture, &resize, 0.0, NULL, SDL_FLIP_NONE);//fill
}

std::string Game::translate_outcome(Outcome outcome) {
	std::string txt = "Error!";
	switch (outcome) {
	case royal_flush:
		txt = "Royal Flush!";
		break;
	case straight_flush:
		txt = "Straight Flush!";
		break;
	case four_of_a_kind:
		txt = "Four of a Kind!";
		break;
	case full_house:
		txt = "Full House!";
		break;
	case flush:
		txt = "Flush!";
		break;
	case straight:
		txt = "Straight!";
		break;
	case three_of_a_kind:
		txt = "Three of a Kind!";
		break;
	case two_pair:
		txt = "Two Pair!";
		break;
	case jacks_or_better:
		txt = "Pair!";
		break;
	case low_pair:
		txt = "Low Pair!";
		break;
	case high_card:
		txt = "High Card!";
		break;
	case four_card_flush:
		txt = "4-Card Flush!";
		break;
	case four_card_straight:
		txt = "4-Card Straight!";
		break;
	case four_card_straight_flush:
		txt = "4-Card Straight Flush!";
		break;
	case three_card_flush:
		txt = "3-Card Flush!";
		break;
	case three_card_straight:
		txt = "3-Card Straight!";
		break;
	case three_card_straight_flush:
		txt = "3-Card Straight Flush!";
		break;
	case nothing:
		txt = "Nothing!";
		break;
	}
	return txt;
}

void Game::process(Outcome _outcome) {
	int suits[4] = { 0, 0, 0, 0 };
	int faces[14] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	for (int i = 0; i < 5; i++) {
		faces[hand[i].face]++;
		suits[hand[i].suit]++;
	}

	if (_outcome == royal_flush || _outcome == straight_flush || _outcome == straight || _outcome == flush || _outcome == full_house) {
		std::fill(std::begin(hold), std::end(hold), true);
	}
	else if (_outcome == four_of_a_kind || _outcome == three_of_a_kind || _outcome == two_pair || _outcome == jacks_or_better || _outcome == low_pair) {
		int face = -1;
		for (int i = 0; i < 14; i++) {
			if (faces[i] > 1) {
				for (int j = 0; j < 5; j++) {
					if (hand[j].face == i) hold[j] = true;
				}
			}
		}
	}
	else if (_outcome == four_card_flush || _outcome == four_card_straight_flush || _outcome == three_card_flush || _outcome == three_card_straight_flush) {
		int suit = -1;
		for (int i = 0; i < 4; i++) {
			if (suits[i] > 2) suit = i;;
		}
		for (int i = 0; i < 5; i++) {
			if (hand[i].suit == suit) hold[i] = true;
		}
	}
	else if (_outcome == four_card_straight || _outcome == three_card_straight) {
		int threshold = 4;
		if (_outcome == three_card_straight) threshold = 3;

		int c = 0;
		int highest = -1;
		for (int i = 0; i < 14; i++) {
			if (faces[i] == 1) {
				if (++c == threshold) highest = i;
			}
			else c = 0;
		}
		if (c == threshold-1 && faces[1] == 1) highest = 1;

		for (int j = 0; j < threshold; j++) {
			for (int i = 0; i < 5; i++) {
				if (hand[i].face == highest-j) hold[i] = true;
			}
		}
	}
	else if (_outcome == high_card) {
		//suited 2HC, suited HC+T,unsuited 2HC, 1HC
		int pay_cards = faces[1] + faces[13] + faces[12] + faces[11];
		int ten = faces[10];

		if (pay_cards == 1) {//1HC
			int suit = -1;
			for (int i = 0; i < 5; i++) {
				if (hand[i].face > 10 || hand[i].face == 1) {
					suit = hand[i].suit;
					hold[i] = true;
				}
			}
			if (faces[10] >= 1) {//suited HC+T
				for (int i = 0; i < 5; i++) {
					if (hand[i].face == 10 && hand[i].suit == suit) hold[i] = true;
				}
			}
		}
		else { //more than 1 HC
			std::vector<int> indexes;
			int index_ten = -1;
			for (int i = 0; i < 5; i++) {
				if (hand[i].face > 10 || hand[i].face == 1) {
					indexes.push_back(i);
				}
				else if (hand[i].face == 10) index_ten = i;
			}

			int suit = -1;
			for (auto& i : indexes) {
				for (auto& j : indexes) {
					if (i != j && hand[i].suit == hand[j].suit) {//suited 2HC
						suit = hand[i].suit;
						hold[i] = true;
						hold[j] = true;
					}
				}
			}

			if (suit != -1 && faces[10] >= 1) {//check for an open-ended 3 royal flush
				if (suit == hand[index_ten].suit) hold[index_ten] = true;
			}

			if (suit == -1 && faces[10] >= 1) {
				for (auto& i : indexes) {
					if (hand[i].suit == hand[index_ten].suit) {//suited HC+T
						suit = hand[i].suit;
						hold[i] = true;
						hold[index_ten] = true;
					}
				}
			}

			if (suit == -1) {//unsuited 2HC
				hold[indexes[0]] = true;
				hold[indexes[1]] = true;
			}
		}
	}
}

Outcome Game::evaluate() {
	int suits[4] = {0, 0, 0, 0};
	int faces[14] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	bool flush = false;
	for (int i = 0; i < 5; i++) {
		faces[hand[i].face]++;
		if (++suits[hand[i].suit] == 5) flush = true;
	}
	bool straight = false;
	bool four_straight = false;
	bool three_straight = false;
	bool three = false;
	bool pair = false;
	bool jacks = false;
	int c = 0;
	for (int i = 0; i < 14; i++) {
		if (faces[i] == 1) {
			//c++;
			if (++c == 5) straight = true;
			else if (c == 4) four_straight = true;
			else if (c == 3) three_straight = true;
		}
		else c = 0;

		if (faces[i] == 4) return four_of_a_kind;
		else if (faces[i] == 3) three = true;
		else if (pair && faces[i] == 2) return two_pair;
		else if (faces[i] == 2) {
			pair = true;
			if (i > 10 || i == 1) jacks = true;
		}
	}
	if (c == 4 && faces[1] == 1) straight = true;

	bool four_flush = suits[0] == 4 || suits[1] == 4 || suits[2] == 4 || suits[3] == 4;
	bool three_flush = suits[0] == 3 || suits[1] == 3 || suits[2] == 3 || suits[3] == 3;

	bool four_straight_flush = false, three_straight_flush = false;

	if ((four_flush && four_straight) || (three_flush && three_straight)) {
		//possible partial straight flush
		std::vector<std::vector<int>> suited_hand(4);
		for (int i = 0; i < 5; i++) {
			suited_hand[hand[i].suit].push_back(hand[i].face);
			if (hand[i].face == 1) suited_hand[hand[i].suit].push_back(14);//ace are 1 and 14
		}

		for (auto& s : suited_hand) {
			if (s.size() < 3) continue;
			sort(s.begin(), s.end());

			int c = 1;
			for (size_t j = 1; j < s.size(); ++j) {
				if (s[j] == s[j - 1] + 1) c++;
				else c = 1;

				if (c == 4) four_straight_flush = true;
				else if (c == 3) three_straight_flush = true;
			}
			
			//if (c == 3 && faces[1] == 1) four_straight_flush = true;
			//else if (c == 2 && faces[1] == 1) three_straight_flush = true;
		}
	}

	//console.log(faces);

	int pay_cards = faces[1] + faces[13] + faces[12] + faces[11];

	if (faces[1] == 1 && faces[13] == 1 && faces[12] == 1 && faces[11] == 1 && faces[10] == 1 && flush) return royal_flush;
	else if (straight && flush) return straight_flush;
	else if (flush) return Outcome::flush;
	else if (straight) return Outcome::straight;
	else if (pair && three) return full_house;
	else if (three) return three_of_a_kind;
	else if (pair && jacks) return jacks_or_better;
	else if (four_straight_flush) return four_card_straight_flush;//since this is the same as 4 flush maybe consolidate
	else if (four_flush) return four_card_flush;
	else if (pair != jacks) return low_pair;
	else if (four_straight) return four_card_straight;
	else if (three_straight_flush) return three_card_straight_flush;//royal too
	else if (pay_cards > 0) return high_card;//this might need to be split in 4 (suited 2HC, suited HC+T,unsuited 2HC, 1HC)
	else if (three_flush) return three_card_flush;//probably not worth it
	else if (three_straight) return three_card_straight;//probably not worth it
	else return nothing;
}

std::string Game::fill(std::string txt, int n, std::string pad) {
	std::string s = "" + txt;
	while (s.length() < n) {
		s = pad + s;
	}
	return s;
};