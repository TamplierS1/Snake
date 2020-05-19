#include "Game.h"

int main()
{
	Game game;
	if (game.Construct(1600, 900, 1, 1, false, true))
		game.Start();

	return 0;
}