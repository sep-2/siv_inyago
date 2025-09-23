# pragma once
# include "Common.hpp"

// ゲーム本編のシーン
class Game : public App::Scene
{
public:

	Game(const InitData& init);

	void update() override;

	void draw() const override;

private:
	Grid<Rect> tiles{ Size{ 13, 13 } };

	// inyago の現在位置（タイル座標系ではなくシーン座標）
	Vec2 m_inyagoPos;

	// タイルサイズ（描画時の inyago のサイズにも使用）
	int m_tileSize = 0;

	double m_boardStart = 0.0;
	Point m_direction{ 1, 0 };
	Optional<Point> m_turnReservation;
	double m_moveSpeed = 240.0;

	// フィールド上に出現させる魚の座標を保持（常時 2 匹）
	Array<Vec2> m_fishPositions{ Vec2::Zero(), Vec2::Zero() };
	// 魚を描画するためのテクスチャ（🐟 の絵文字を利用）
	Texture m_fishTexture{ U"🐟"_emoji };
	// 取得した魚の数（スコア）
	int32 m_score = 0;

	// 先頭が辿ったタイルの履歴（先頭は最新）
	Array<Point> m_pathHistory;
	// スコアに応じて追従する猫の描画座標
	Array<Vec2> m_tailPositions;
	// 現在踏んでいるタイルのインデックスを保持
	Point m_currentTile{ 0, 0 };
	// 自己衝突でゲーム終了フラグ
	bool m_gameOver = false;

	// ランダムなタイルを選んで魚を 2 匹分配置し直す
	void spawnFishes();
	// 任意の座標からタイルインデックスを逆算するヘルパー
	Point calcTileIndex(const Vec2& pos) const;
	// パス履歴とスコアから追従猫の位置配列を更新
	void updateTailPositions();
};
