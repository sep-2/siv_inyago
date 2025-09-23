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

	
};
