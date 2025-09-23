#include "Game.hpp"

void Game::draw() const
{
    Scene::SetBackground(ColorF{ 0.2 });

    // チェスボード状に盤面を描画
    for (int y = 0; y < 13; ++y)
    {
        for (int x = 0; x < 13; ++x)
        {
            const bool isWhite = ((x + y) % 2 == 0);
            tiles[y][x].draw(isWhite ? ColorF{ 1.0 } : ColorF{ 0.9 });
        }
    }

    // フィールド上の魚を描画（常に 2 匹）
    const double fishSize = m_tileSize * 0.6;
    const TextureRegion fishRegion = m_fishTexture.resized(fishSize, fishSize);
    for (const auto& fishPos : m_fishPositions)
    {
        fishRegion.drawAt(fishPos);
    }

    // 追従する猫を描画（古い順に描画して重なりを自然に）
    const double catSize = m_tileSize * 0.8;
    const TextureRegion catRegion = getData().inyago.resized(catSize, catSize);
    const auto& tail = m_trail.tailPositions();
    for (size_t i = tail.size(); i > 0; --i)
    {
        catRegion.drawAt(tail[i - 1], ColorF{ 1.0, 1.0, 1.0, 0.85 });
    }

    // 先頭の猫（プレイヤー）
    catRegion.drawAt(m_inyagoPos);

    // 現在のスコアを画面左上に表示
    const Font& uiFont = FontAsset(U"Bold");
    uiFont(U"Score: {}"_fmt(m_score)).draw(28, Vec2{ 20, 20 }, ColorF{ 0.15 });

    // ゲームオーバー時は中央にメッセージを表示
    if (m_gameOver)
    {
        uiFont(U"GAME OVER\nRキーでリスタート").drawAt(36, Scene::Center(), ColorF{ 0.2 });
    }
}
