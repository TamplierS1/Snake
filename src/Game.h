#ifndef GAME_H
#define GAME_H

#include <list>
#include <memory>

#include "olcPixelGameEngine.h"

struct SnakeBlock
{
	olc::vf2d previousPos;
	olc::vf2d currentPos;
};

enum class GameState
{
	MENU,
	ACTIVE,
	GAME_OVER
};

class Game : public olc::PixelGameEngine
{
public:
	Game()
	{
		sAppName = "Snake";
	}

	bool OnUserCreate() override;
	bool OnUserUpdate(float delta) override;

	void ProcessInput();
	void ResolveSnakeCollisions();
	void DrawGameOverScreen();
	void DrawMenuScreen();

	//Snake manipulation methods
	void DrawSnake();
	void MoveSnake(float delta);
	void CreateSnake();
private:
	GameState mGameState;

	std::unique_ptr<std::list<SnakeBlock>> mSnake;

	olc::vf2d mStartingPos{ 150, 150 };
	olc::vf2d mSnakeSize{ 32, 32 };
	olc::vf2d mSnakeDirection{ 1, 0 };

	//the higher the mGameSpeed the slower the game will run
	int mGameSpeed = 12;
	int mCurrentFrameNum = 0;
};
#endif