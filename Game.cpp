# include "Game.hpp"

Game::Game(const InitData& init)
	: IScene{ init }
{
	const int TileSize = Scene::Height()/13;
	const int start = (Scene::Width() - Scene::Height()) / 2;

	// チェスボード状に tiles を初期化
	for (int y = 0; y < 13; ++y)
	{
		for (int x = 0; x < 13; ++x)
		{
			tiles[y][x] = Rect{ start + x * TileSize, y * TileSize, TileSize, TileSize };
		}
	}
}

void Game::update()
{
}

void Game::draw() const
{
	Scene::SetBackground(ColorF{ 0.2 });

	// チェスボード状に描画
	for (int y = 0; y < 13; ++y)
	{
		for (int x = 0; x < 13; ++x)
		{
			const bool isWhite = ((x + y) % 2 == 0);
			tiles[y][x].draw(isWhite ? ColorF{ 1.0 } : ColorF{ 0.9 });
		}
	}
}
