# include "Game.hpp"

Game::Game(const InitData& init)
	: IScene{ init }
{
	m_tileSize = Scene::Height() / 13;
	const int start = (Scene::Width() - Scene::Height()) / 2;

	// チェスボード状に tiles を初期化
	for (int y = 0; y < 13; ++y)
	{
		for (int x = 0; x < 13; ++x)
		{
			tiles[y][x] = Rect{ start + x * m_tileSize, y * m_tileSize, m_tileSize, m_tileSize };
		}
	}

	// inyago の初期位置（盤中央タイル中心）
	const Point centerIndex{ 13 / 2, 13 / 2 }; // (6,6)
	m_inyagoPos = Vec2{
		start + centerIndex.x * m_tileSize + (m_tileSize / 2.0),
		centerIndex.y * m_tileSize + (m_tileSize / 2.0)
	};
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

	// inyago をタイルサイズに合わせて正方形リサイズして中央描画
	getData().inyago
		.resized(m_tileSize, m_tileSize)   // 幅・高さをタイルサイズに揃える
		.drawAt(m_inyagoPos);
}
