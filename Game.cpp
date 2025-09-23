# include "Game.hpp"

Game::Game(const InitData& init)
	: IScene{ init }
{

}

void Game::update()
{
}

void Game::draw() const
{
	Scene::SetBackground(ColorF{ 0.2 });
}
