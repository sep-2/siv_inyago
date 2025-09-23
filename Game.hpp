# pragma once
# include "Common.hpp"

// タイトルシーン
class Game : public App::Scene
{
public:

	Game(const InitData& init);

	void update() override;

	void draw() const override;

private:
	Grid<Rect> tiles{ Size{ 13, 13 } };

	// inyago の現在位置（タイル中心）
	Vec2 m_inyagoPos;

	// タイルサイズ（描画時の inyago のサイズにも使用）
	int m_tileSize = 0;

	double m_boardStart = 0.0;
	Point m_direction{ 1, 0 };
	Optional<Point> m_turnReservation;
	double m_moveSpeed = 240.0;
};
