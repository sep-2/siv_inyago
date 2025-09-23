# pragma once
# include <Siv3D.hpp>

// シーンのステート
enum class State
{
	Title,
	Game,
};

// 共有するデータ
struct GameData
{
	// 直前のゲームのスコア
	int32 lastScore = 0;

	// ハイスコア
	Array<int32> highScores = { 10, 8, 6, 4, 2 };

	// inyagoのテクスチャ
	Texture inyago = Texture{ U"🐈"_emoji };


	// キャラの向き
	Point characterAngle = Point(0, 0);

};

using App = SceneManager<State, GameData>;
