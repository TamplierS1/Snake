#include "Game.h"

bool Game::OnUserCreate()
{
	CreateSnake();

	mGameState = GameState::MENU;

	SpawnFood(FoodType::APPLE);

	return true;
}

bool Game::OnUserUpdate(float delta)
{
	ProcessInput();

	switch (mGameState)
	{
	case GameState::MENU:

		Clear(olc::GREY);
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

		DrawFood();
		DrawSnake();
		DrawScore(olc::vf2d(ScreenWidth() - 100.0f, 50.0f), olc::vf2d(4, 4));

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
	//Horizontal input
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

	//Vertical input
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

	//Other
	if (GetKey(olc::ENTER).bPressed)
	{
		mSnake.reset();
		CreateSnake();

		mFood.clear();

		SpawnFood(FoodType::APPLE);

		mGameState = GameState::ACTIVE;
		mCurrentScore = 0;
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
		head.currentPos.x + mBlockSize.x > ScreenWidth() ||
		head.currentPos.y < 0.0f ||
		head.currentPos.y + mBlockSize.y > ScreenHeight()
		)
	{
		mGameState = GameState::GAME_OVER;
		return;
	}

	//Check collisions against food
	for (int i = 0; i < mFood.size(); i++)
	{
		if (mFood[i].position.x == head.currentPos.x && mFood[i].position.y == head.currentPos.y)
		{
			//increase score
			mCurrentScore += mFood[i].points;

			//create new snake block
			olc::vf2d talePosition{ mSnake->back().currentPos };
			mSnake->push_back(
				SnakeBlock{
					olc::vf2d(talePosition.x + mSnakeDirection.x * mBlockSize.x, talePosition.y + mSnakeDirection.y * mBlockSize.y),
					olc::vf2d(talePosition.x + mSnakeDirection.x * mBlockSize.x, talePosition.y + mSnakeDirection.y * mBlockSize.y)
				});

			//erase eaten food and spawn a new one
			mFood.erase(mFood.begin() + i);
			SpawnFood(FoodType::APPLE);
		}
	}
}

void Game::DrawGameOverScreen()
{
	olc::vf2d screenSize{ static_cast<float>(ScreenWidth()), static_cast<float>(ScreenHeight()) };

	DrawStringDecal(olc::vf2d(screenSize.x / 2 - screenSize.x / 6, screenSize.y / 2 - screenSize.y / 6), "GAME OVER", olc::DARK_RED, olc::vf2d(8, 8));
	//second line is positioned below the GAME OVER line
	DrawStringDecal(olc::vf2d(screenSize.x / 2 - screenSize.x / 4, screenSize.y / 2 - screenSize.y / 12), "Press 'Enter' to start again", olc::DARK_RED, olc::vf2d(4, 4));

	DrawScore(olc::vf2d(screenSize.x / 2, screenSize.y / 2 + screenSize.y / 4), olc::vf2d(8, 8));
}

void Game::DrawMenuScreen()
{
	olc::vf2d screenSize{ static_cast<float>(ScreenWidth()), static_cast<float>(ScreenHeight()) };

	DrawStringDecal(olc::vf2d(screenSize.x / 2 - screenSize.x / 8, screenSize.y / 2 - screenSize.y / 6), "Snake", olc::DARK_RED, olc::vf2d(8, 8));
	//second line is positioned below the Snake line
	DrawStringDecal(olc::vf2d(screenSize.x / 2 - screenSize.x / 4.5, screenSize.y / 2 - screenSize.y / 12), "Press 'Enter' to start", olc::DARK_RED, olc::vf2d(4, 4));
}

void Game::DrawScore(olc::vf2d position, olc::vf2d size)
{
	olc::vf2d screenSize{ static_cast<float>(ScreenWidth()), static_cast<float>(ScreenHeight()) };

	DrawStringDecal(position, std::to_string(mCurrentScore), olc::DARK_RED, size);
}

void Game::DrawSnake()
{
	bool first = true;

	for (const auto& block : *mSnake)
	{
		//draw head
		if (first)
		{
			DrawRect(block.currentPos, mBlockSize, olc::RED);
			FillRect(block.currentPos, mBlockSize, olc::RED);
			first = false;

			continue;
		}

		//draw body
		DrawRect(block.currentPos, mBlockSize, olc::GREEN);
		FillRect(block.currentPos, mBlockSize, olc::GREEN);
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
			block.currentPos.x += mBlockSize.x * mSnakeDirection.x;
			block.currentPos.y += mBlockSize.y * mSnakeDirection.y;

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
	for (int i = mBlockSize.x * 5 + mStartingPos.x; i > mStartingPos.x; i -= mBlockSize.x)
	{
		mSnake->push_back(SnakeBlock{ olc::vf2d(i, mStartingPos.y), olc::vf2d(i, mStartingPos.y) });
	}

	mSnakeDirection = olc::vf2d(1, 0);
}

void Game::SpawnFood(FoodType type)
{
	srand(time(NULL));

	olc::vf2d position{ float(rand() % (ScreenWidth() / 32) * 32), float(rand() % (ScreenHeight() / 32) * 32) };

	while (position.x <= 0.0f || position.x + mBlockSize.x >= ScreenWidth())
	{
		position.x = float(rand() % (ScreenWidth() / 32) * 32);
	}

	while (position.y <= 0.0f || position.y + mBlockSize.y >= ScreenHeight())
	{
		position.y = float(rand() % (ScreenHeight() / 32) * 32);
	}

	olc::Pixel color;

	switch (type)
	{
	case FoodType::APPLE:
		color = olc::RED;
		break;
	case FoodType::ORANGE:
		color = olc::DARK_BLUE;
		break;
	default:
		break;
	}

	Food food{ position, int(type), color };

	mFood.push_back(food);
}

void Game::DrawFood()
{
	for (const auto& food : mFood)
	{
		DrawRect(food.position, mBlockSize, food.color);
		FillRect(food.position, mBlockSize, food.color);
	}
}