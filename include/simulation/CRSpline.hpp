/*
 * CRSpline.hpp
 *
 *  Created on: Jun 11, 2011
 *      Author: markus
 */

#ifndef CRSPLINE_HPP_
#define CRSPLINE_HPP_

#include <m3d/m3d.hpp>
#include <vector>
#include <list>

namespace sim {

using namespace m3d;

/**
 * Arc-length parameterized Catmull Rom spline.
 */
class CRSpline {
protected:
	/** Help point with t parameter of spline, the arc length
	 *  the position and tangent */
	struct HelpPoint {
		float t, len;
		Vec2f pos, tangent;
	};
	typedef std::vector<HelpPoint> HelpTable;
	typedef std::list<HelpPoint> HelpList;

	/** Table of help points */
	HelpTable m_table;

	/** The control points */
	std::vector<Vec2f> m_knots;

	/** The maximum error allowed when calculating the arc length */
	float m_error;

	/**
	 * Divides the given segment into two equally sized segments and checks whether
	 * the deviation of the linear segments deviates from the spline more than the
	 * specified error. If so, the two generated segments are again subdivided
	 * recursively.
	 *
	 * @param helpPoints The current list of help points
	 * @param point      The current help point of interest
	 * @param knot       The current knot to handle
	 * @return           The next help point to handle
	 */
	HelpList::iterator divideSegment(HelpList &helpPoints, HelpList::iterator cur, unsigned knot);

	/**
	 * Perform a binary search on the internal table in order to find
	 * the lower help point of the segment that contains the given length.
	 *
	 * @param length The length of the curve
	 * @return       The lower index of the two help points that span the segment
	 */
	unsigned binarySearch(float length);

	unsigned int BOUND(int index);
	Vec2f at(int index);
public:
	CRSpline(float error = 0.001f);
	virtual ~CRSpline();

	/**
	 * Calculates the arc length of the spline and updates
	 * the internal table.
	 *
	 * @return The length of the spline
	 */
	float update();

	/** @return The control points of the spline */
	inline std::vector<Vec2f>& knots();

	/**
	 * Get the spline parameter at the given length.
	 *
	 * @param length The length on the curve
	 * @return       The parameter of the curve at this length
	 */
	float getT(float length);

	/**
	 * Returns the position on the spline at the given length
	 * of the curve.
	 *
	 * @param length The length on the curve
	 * @return       The position at this length
	 */
	Vec3f getPos(float length);

	/**
	 * Returns the tangent on the spline at the given length
	 * of the curve.
	 *
	 * @param length The length on the curve
	 * @return       The tangent at this length
	 */
	Vec3f getTangent(float length);

	/**
	 * Returns the position and tangent at the given length
	 * of the curve.
	 *
	 * @param length The length on the curve
	 * @return       The position tangent at this length
	 */
	std::pair<Vec3f, Vec3f> getPoint(float length);

	/**
	 * Renders the knots in this spline.
	 */
	void renderKnots(bool link = true, const Vec3f& color = Vec3f());

	/**
	 * Renders the spline with the given accuracy. It has to be a
	 * value greater than 0 and smaller than 1, where 0.5 would mean
	 * that a spline segment is drawn using two lines.
	 *
	 * @param accuracy A value greater than 0 and smaller than 1
	 */
	void renderSpline(float accuracy, const Vec3f& color = Vec3f(0.0f, 1.0f, 0.0));

	/**
	 * Renders equidistant points on the spline with the given
	 * gap between them.
	 *
	 * @param gap           The length of the spline between the points
	 * @param tangent       Tangents will be rendered if True
	 * @param tangentLength The length of the tangents
	 */
	void renderPoints(float gap, const Vec3f& color = Vec3f(1.0f, 0.0f, 0.0), bool tangent = true, const Vec3f& tangentColor = Vec3f(0.0f, 0.0f, 1.0), float tangentLength = 2.5f);


	/** Interpolates the value in the given segment */
	static Vec2f interpolate(Vec2f p0, Vec2f p1, Vec2f p2, Vec2f p3, float t);
	/** Derives the value in the given segment */
	static Vec2f derive(Vec2f p0, Vec2f p1, Vec2f p2, Vec2f p3, float t);
};

inline std::vector<Vec2f>& CRSpline::knots()
{
	return m_knots;
}

inline unsigned int CRSpline::BOUND(int index) {
	if (index < 0)
		return 0;
	else if ((unsigned int)index >= m_knots.size() - 1)
		return m_knots.size() - 1;

	return index;
}

inline Vec2f CRSpline::at(int index)
{
	return m_knots[BOUND(index)];
}

}

#endif /* CRSPLINE_HPP_ */
