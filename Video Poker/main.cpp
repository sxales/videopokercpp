#include "Game.h"

int main(int argc, char* args[])
{
	Game game = Game("Data/config.txt");

	game.run();

	return 0;
}