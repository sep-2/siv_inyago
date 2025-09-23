#include "Game.hpp"

Game::Game(const InitData& init)
    : IScene{ init }
{
    m_tileSize = Scene::Height() / 13;
    const int start = (Scene::Width() - Scene::Height()) / 2;
    m_boardStart = start;

    // チェスボード状にタイルを構築
    for (int y = 0; y < 13; ++y)
    {
        for (int x = 0; x < 13; ++x)
        {
            tiles[y][x] = Rect{ start + x * m_tileSize, y * m_tileSize, m_tileSize, m_tileSize };
        }
    }

    // inyago の初期位置を盤面中央タイルの中央に配置
    const Point centerIndex{ 13 / 2, 13 / 2 };
    m_inyagoPos = Vec2{
        start + centerIndex.x * m_tileSize + (m_tileSize / 2.0),
        centerIndex.y * m_tileSize + (m_tileSize / 2.0)
    };

    m_trail.setSegmentLength(static_cast<double>(m_tileSize));
    m_trail.reset(m_inyagoPos, centerIndex);

    // スコアをリセットし、共有データにも初期値を反映
    m_score = 0;
    getData().lastScore = 0;

    // 初回分の魚を生成
    spawnFishes();
}

Point Game::calcTileIndex(const Vec2& pos) const
{
    // シーン座標からタイルインデックスを逆算する（盤面外にはみ出した場合も安全に収める）
    const int maxX = static_cast<int>(tiles.width()) - 1;
    const int maxY = static_cast<int>(tiles.height()) - 1;
    const double tileSize = static_cast<double>(m_tileSize);

    const int indexX = Clamp(static_cast<int>((pos.x - m_boardStart) / tileSize), 0, maxX);
    const int indexY = Clamp(static_cast<int>(pos.y / tileSize), 0, maxY);

    return Point{ indexX, indexY };
}

void Game::spawnFishes()
{
    Array<Point> usedTiles;
    const auto& occupied = m_trail.occupiedTiles();
    usedTiles.reserve(occupied.size() + m_fishPositions.size());

    for (const auto& tile : occupied)
    {
        if (not usedTiles.contains(tile))
        {
            usedTiles << tile;
        }
    }

    for (auto& fishPos : m_fishPositions)
    {
        Point candidate;

        do
        {
            candidate = Point{
                Random(0, static_cast<int>(tiles.width()) - 1),
                Random(0, static_cast<int>(tiles.height()) - 1)
            };
        } while (usedTiles.contains(candidate));

        usedTiles << candidate;
        fishPos = Vec2{ tiles[candidate.y][candidate.x].center() };
    }
}

void Game::update()
{
    // ゲームオーバー時は操作を受け付けず、R キーでリスタート
    if (m_gameOver)
    {
        if (KeyR.down())
        {
            changeScene(State::Game);
        }
        return;
    }

    // 入力された方向が現在の進行方向と直交しているかを判定するラムダ
    const auto canTurnTo = [this](const Point& dir)
    {
        // dir がゼロベクトルでなく、進行方向との内積が 0（直交）なら向き変更を許可
        return (dir != Point{ 0, 0 }) && ((m_direction.x * dir.x + m_direction.y * dir.y) == 0);
    };

    // パターン1: タイルを跨いでいる間は方向転換を予約しておく
    const auto queueTurn = [&](const Input& key, const Point& dir)
    {
        // キーを押した瞬間（down）だけを拾うことで予約が増幅しないようにする
        if (key.down() && canTurnTo(dir))
        {
            // 直交する方向だけ上書きし、タイル中央での処理に渡す
            m_turnReservation = dir;
        }
    };

    // 十字キーの 4 方向を監視して、パターン1 の予約処理を適用
    queueTurn(KeyUp, Point{ 0, -1 });
    queueTurn(KeyDown, Point{ 0, 1 });
    queueTurn(KeyLeft, Point{ -1, 0 });
    queueTurn(KeyRight, Point{ 1, 0 });

    // パターン2 の判定や移動量計算で利用するタイル・キャラクター寸法
    const double tileSize = static_cast<double>(m_tileSize);
    const double halfTile = tileSize * 0.5;
    const double characterHalf = tileSize * 0.4;
    const double margin = (halfTile - characterHalf) + 0.01;

    // 現在位置が属するタイルとその中心を把握
    const Point currentIndex = calcTileIndex(m_inyagoPos);
    const Rect& currentTile = tiles[currentIndex.y][currentIndex.x];
    const Vec2 tileCenter = Vec2{ currentTile.center() };

    // 中心との差分が一定以内なら「タイルの中にすっぽり収まっている」とみなす
    const bool fullyInside = (Abs(m_inyagoPos.x - tileCenter.x) <= margin)
        && (Abs(m_inyagoPos.y - tileCenter.y) <= margin);

    if (fullyInside)
    {
        const Point currentHeadTile = m_trail.headTile();

        // 新しいタイルへ踏み込んだタイミングで履歴を拡張し、自己衝突を判定
        if (currentIndex != currentHeadTile)
        {
            if (m_trail.registerHeadTile(currentIndex, m_score))
            {
                m_gameOver = true;
                getData().lastScore = m_score;
                return;
            }
        }

        Optional<Point> desiredTurn;

        // 押しっぱなしのキー入力を優先的に拾うラムダ
        auto capturePressed = [&](const Input& key, const Point& dir)
        {
            // まだ方向が決まっておらず、直交方向が押されていれば採用
            if (!desiredTurn && key.pressed() && canTurnTo(dir))
            {
                desiredTurn = dir;
            }
        };

        // タイルに収まった瞬間に押されている方向を優先的に適用
        capturePressed(KeyUp, Point{ 0, -1 });
        capturePressed(KeyDown, Point{ 0, 1 });
        capturePressed(KeyLeft, Point{ -1, 0 });
        capturePressed(KeyRight, Point{ 1, 0 });

        // 押下が無ければ、事前に予約しておいた方向へ切り替える
        if (!desiredTurn && m_turnReservation && canTurnTo(*m_turnReservation))
        {
            desiredTurn = m_turnReservation;
        }

        // 向きが決まったら切り替えてからタイルの中心にスナップする
        if (desiredTurn)
        {
            m_direction = *desiredTurn;
            m_turnReservation.reset();
            m_inyagoPos = tileCenter;
        }
    }

    // 現在の向きを 2D ベクトル化し、自動前進の処理につなげる
    const Vec2 directionVec{ static_cast<double>(m_direction.x), static_cast<double>(m_direction.y) };

    if (!directionVec.isZero())
    {
        // シーンの Δt を掛けて、速度に応じた移動量を算出
        Vec2 nextPos = m_inyagoPos + directionVec * (m_moveSpeed * Scene::DeltaTime());

        // 盤面の矩形範囲を算出し、端で止まるようにする
        const double boardWidth = tileSize * tiles.width();
        const double boardHeight = tileSize * tiles.height();

        const double minX = m_boardStart + characterHalf;
        const double maxXPos = m_boardStart + boardWidth - characterHalf;
        const double minY = characterHalf;
        const double maxYPos = boardHeight - characterHalf;

        // クランプ処理で盤面外にはみ出ないよう制御
        nextPos.x = Clamp(nextPos.x, minX, maxXPos);
        nextPos.y = Clamp(nextPos.y, minY, maxYPos);

        // 計算済みの位置を確定
        m_inyagoPos = nextPos;
    }

    // 頭の軌跡を更新
    m_trail.recordHeadPosition(m_inyagoPos, m_score);

    // 魚との接触を検出してスコアを更新
    const Circle inyagoHitBox{ m_inyagoPos, characterHalf };
    const double fishRadius = tileSize * 0.3;
    bool collected = false;

    for (const auto& fishPos : m_fishPositions)
    {
        if (inyagoHitBox.intersects(Circle{ fishPos, fishRadius }))
        {
            collected = true;
            break;
        }
    }

    if (collected)
    {
        ++m_score;
        getData().lastScore = m_score;
        spawnFishes();
    }

    m_trail.rebuildTailPositions(m_score);
}
