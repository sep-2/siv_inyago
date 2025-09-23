#pragma once
#include "Common.hpp"

class SnakeTrail
{
public:
	SnakeTrail() = default;

	void setSegmentLength(double length);
	void reset(const Vec2& headPosition, const Point& headTile);

	bool registerHeadTile(const Point& tile, int tailLength);
	void recordHeadPosition(const Vec2& headPosition, int tailLength);
	void rebuildTailPositions(int tailLength);

	const Array<Vec2>& tailPositions() const noexcept;
	const Array<Point>& occupiedTiles() const noexcept;
	Point headTile() const noexcept;

private:
	double m_segmentLength = 0.0;
	Array<Point> m_tiles;
	Array<Vec2> m_pathSamples;
	Array<Vec2> m_tailPositions;

	Vec2 samplePathAtDistance(double distance) const;
};
