#include "SnakeTrail.hpp"

namespace
{
	constexpr double Epsilon = 0.0001;
}

void SnakeTrail::setSegmentLength(double length)
{
	m_segmentLength = (length > 0.0) ? length : 1.0;
}

void SnakeTrail::reset(const Vec2& headPosition, const Point& headTile)
{
	m_tiles.clear();
	m_tiles << headTile;

	m_pathSamples.clear();
	m_pathSamples << headPosition;

	m_tailPositions.clear();
}

bool SnakeTrail::registerHeadTile(const Point& tile, int tailLength)
{
	if (!m_tiles.isEmpty() && m_tiles.front() == tile)
	{
		return false;
	}

	bool collision = false;

	for (size_t i = 0; i < m_tiles.size(); ++i)
	{
		if (m_tiles[i] == tile)
		{
			const bool isTailEnd = (tailLength > 0) && (i == (m_tiles.size() - 1));
			if (!isTailEnd)
			{
				collision = true;
			}
			break;
		}
	}

	if (collision)
	{
		return true;
	}

	m_tiles.insert(m_tiles.begin(), tile);

	const size_t maxLength = static_cast<size_t>(tailLength + 1);
	if (m_tiles.size() > maxLength)
	{
		m_tiles.pop_back();
	}

	return false;
}

void SnakeTrail::recordHeadPosition(const Vec2& headPosition, int tailLength)
{
	if (m_pathSamples.isEmpty())
	{
		m_pathSamples << headPosition;
		return;
	}

	const Vec2& latest = m_pathSamples.front();
	const Vec2 diff = headPosition - latest;

	if (diff.lengthSq() > Epsilon)
	{
		m_pathSamples.insert(m_pathSamples.begin(), headPosition);
	}
	else
	{
		m_pathSamples.front() = headPosition;
	}

	const double segment = (m_segmentLength > 0.0) ? m_segmentLength : 1.0;
	const double retainLength = segment * (tailLength + 1) + segment * 2.0;

	double accumulated = 0.0;
	size_t keepCount = m_pathSamples.size();

	for (size_t i = 1; i < m_pathSamples.size(); ++i)
	{
		accumulated += (m_pathSamples[i - 1] - m_pathSamples[i]).length();

		if (accumulated > retainLength)
		{
			keepCount = i + 1;
			break;
		}
	}

	if (keepCount < m_pathSamples.size())
	{
		m_pathSamples.erase(m_pathSamples.begin() + keepCount, m_pathSamples.end());
	}
}

void SnakeTrail::rebuildTailPositions(int tailLength)
{
	m_tailPositions.clear();

	if (tailLength <= 0 || m_pathSamples.isEmpty())
	{
		return;
	}

	const double segment = (m_segmentLength > 0.0) ? m_segmentLength : 1.0;
	const size_t tailCount = static_cast<size_t>(tailLength);

	for (size_t index = 0; index < tailCount; ++index)
	{
		const double distance = segment * (index + 1);
		m_tailPositions << samplePathAtDistance(distance);
	}
}

const Array<Vec2>& SnakeTrail::tailPositions() const noexcept
{
	return m_tailPositions;
}

const Array<Point>& SnakeTrail::occupiedTiles() const noexcept
{
	return m_tiles;
}

Point SnakeTrail::headTile() const noexcept
{
	return m_tiles.isEmpty() ? Point{ 0, 0 } : m_tiles.front();
}

Vec2 SnakeTrail::samplePathAtDistance(double distance) const
{
	if (m_pathSamples.size() <= 1)
	{
		return m_pathSamples.isEmpty() ? Vec2{ 0, 0 } : m_pathSamples.front();
	}

	double remaining = distance;

	for (size_t i = 1; i < m_pathSamples.size(); ++i)
	{
		const Vec2& from = m_pathSamples[i - 1];
		const Vec2& to = m_pathSamples[i];
		const Vec2 segment = to - from;
		const double segmentLength = segment.length();

		if (segmentLength <= Epsilon)
		{
			continue;
		}

		if (remaining <= segmentLength)
		{
			const double t = remaining / segmentLength;
			return from + segment * t;
		}

		remaining -= segmentLength;
	}

	return m_pathSamples.back();
}
