#include "Game.h"

bool Game::OnUserCreate()
{
	CreateSnake();

	mGameState = GameState::MENU;

	return true;
}

bool Game::OnUserUpdate(float delta)
{
	ProcessInput();

	switch (mGameState)
	{
	case GameState::MENU:
		DrawMenuScreen();
		break;
	case GameState::ACTIVE:
		//limit the snake speed
		if (mCurrentFrameNum % mGameSpeed == 0)
		{
			MoveSnake(delta);
			mCurrentFrameNum = 0;
		}

		ResolveSnakeCollisions();

		Clear(olc::GREY);

		DrawSnake();

		mCurrentFrameNum++;
		break;
	case GameState::GAME_OVER:
		Clear(olc::GREY);

		DrawGameOverScreen();
		break;
	default:
		break;
	}

	return true;
}

void Game::ProcessInput()
{
	if (GetKey(olc::D).bPressed)
	{
		//restrict the snake from going inside itself
		if (mSnakeDirection.x == -1)
		{
			mSnakeDirection.x = -1;
		}
		else
		{
			mSnakeDirection.x = 1;
			mSnakeDirection.y = 0;
		}
	}
	else if (GetKey(olc::A).bPressed)
	{
		//restrict the snake from going inside itself
		if (mSnakeDirection.x == 1)
		{
			mSnakeDirection.x = 1;
		}
		else
		{
			mSnakeDirection.x = -1;
			mSnakeDirection.y = 0;
		}
	}

	if (GetKey(olc::W).bPressed)
	{
		//restrict the snake from going inside itself
		if (mSnakeDirection.y == 1)
		{
			mSnakeDirection.y = 1;
		}
		else
		{
			mSnakeDirection.y = -1;
			mSnakeDirection.x = 0;
		}
	}
	else if (GetKey(olc::S).bPressed)
	{
		//restrict the snake from going inside itself
		if (mSnakeDirection.y == -1)
		{
			mSnakeDirection.y = -1;
		}
		else
		{
			mSnakeDirection.y = 1;
			mSnakeDirection.x = 0;
		}
	}

	if (GetKey(olc::ENTER).bPressed)
	{
		mSnake.reset();
		CreateSnake();
		mGameState = GameState::ACTIVE;
	}
}

void Game::ResolveSnakeCollisions()
{
	//Check collisions against the snake itself
	bool first = true;

	const auto& head = *mSnake->begin();
	for (const auto& block : *mSnake)
	{
		//Skip the head, we do not need to check collisions against it
		if (first)
		{
			first = false;
			continue;
		}

		if (block.currentPos.x == head.currentPos.x && block.currentPos.y == head.currentPos.y)
		{
			mGameState = GameState::GAME_OVER;
			return;
		}
	}

	//Check collisions against the walls
	if (
		head.currentPos.x < 0.0f ||
		head.currentPos.x + mSnakeSize.x > ScreenWidth() ||
		head.currentPos.y < 0.0f ||
		head.currentPos.y + mSnakeSize.y > ScreenHeight()
		)
	{
		mGameState = GameState::GAME_OVER;
		return;
	}
}

void Game::DrawGameOverScreen()
{
	olc::vf2d screenSize{ static_cast<float>(ScreenWidth()), static_cast<float>(ScreenHeight()) };

	DrawStringDecal(olc::vf2d(screenSize.x / 2 - screenSize.x / 6, screenSize.y / 2 - screenSize.y / 6), "GAME OVER", olc::DARK_RED, olc::vf2d(8, 8));
	//second line is positioned below the GAME OVER line
	DrawStringDecal(olc::vf2d(screenSize.x / 2 - screenSize.x / 4, screenSize.y / 2 - screenSize.y / 12), "Press 'Enter' to start again", olc::DARK_RED, olc::vf2d(4, 4));
}

void Game::DrawMenuScreen()
{
	olc::vf2d screenSize{ static_cast<float>(ScreenWidth()), static_cast<float>(ScreenHeight()) };

	DrawStringDecal(olc::vf2d(screenSize.x / 2 - screenSize.x / 8, screenSize.y / 2 - screenSize.y / 6), "Snake", olc::DARK_RED, olc::vf2d(8, 8));
	//second line is positioned below the Snake line
	DrawStringDecal(olc::vf2d(screenSize.x / 2 - screenSize.x / 4.5, screenSize.y / 2 - screenSize.y / 12), "Press 'Enter' to start", olc::DARK_RED, olc::vf2d(4, 4));
}

void Game::DrawSnake()
{
	bool first = true;

	for (const auto& block : *mSnake)
	{
		//draw head
		if (first)
		{
			DrawRect(block.currentPos, mSnakeSize, olc::RED);
			FillRect(block.currentPos, mSnakeSize, olc::RED);
			first = false;

			continue;
		}

		//draw body
		DrawRect(block.currentPos, mSnakeSize, olc::GREEN);
		FillRect(block.currentPos, mSnakeSize, olc::GREEN);
	}
}

void Game::MoveSnake(float delta)
{
	bool first = true;

	auto previousBlock = *mSnake->begin();
	for (auto& block : *mSnake)
	{
		//move head
		if (first)
		{
			block.previousPos = block.currentPos;
			block.currentPos.x += mSnakeSize.x * mSnakeDirection.x;
			block.currentPos.y += mSnakeSize.y * mSnakeDirection.y;

			previousBlock = block;

			first = false;

			continue;
		}

		//move a snake block to the previous position of the previous block
		block.previousPos = block.currentPos;
		block.currentPos = previousBlock.previousPos;

		previousBlock = block;
	}
}

void Game::CreateSnake()
{
	mSnake = std::make_unique<std::list<SnakeBlock>>();

	//initialize the snake

	for (int i = mSnakeSize.x * 5 + mStartingPos.x; i > mStartingPos.x; i -= mSnakeSize.x)
	{
		mSnake->push_back(SnakeBlock{ olc::vf2d(i, mStartingPos.y), olc::vf2d(i, mStartingPos.y) });
	}

	mSnakeDirection = olc::vf2d(1, 0);
}