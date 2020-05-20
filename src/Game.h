#ifndef GAME_H
#define GAME_H

#include <list>
#include <memory>
#include <Windows.h>

#include "olcPixelGameEngine.h"

struct SnakeBlock
{
	olc::vf2d previousPos;
	olc::vf2d currentPos;
};

struct Food
{
	olc::vf2d position;
	//how much points are awarded
	int points;
	olc::Pixel color;
};

enum class FoodType
{
	APPLE = 1,
	ORANGE
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
	void DrawScore(olc::vf2d position, olc::vf2d size);

	//Snake manipulation methods
	void DrawSnake();
	void MoveSnake(float delta);
	void CreateSnake();

	void SpawnFood(FoodType type);
	void DrawFood();
private:
	GameState mGameState;

	std::unique_ptr<std::list<SnakeBlock>> mSnake;
	std::vector<Food> mFood;
	olc::vf2d mFoodSize{ 32, 32 };
	//the higher the speed the slower food will spawn
	int mFoodSpawningSpeed = 240;

	olc::vf2d mStartingPos{ 160, 160 };
	olc::vf2d mSnakeSize{ 32, 32 };
	olc::vf2d mSnakeDirection{ 1, 0 };

	olc::vf2d mBlockSize{ 32, 32 };

	//the higher the mGameSpeed the slower the game will run
	int mGameSpeed = 4;
	int mCurrentFrameNum = 0;

	int mCurrentScore = 0;
};
#endif