/*
 * CRSpline.cpp
 *
 *  Created on: Jun 11, 2011
 *      Author: markus
 */

#include <simulation/crspline.hpp>
#include <GL/glew.h>
#include <newton/util.hpp>

namespace sim {

static const float CR_MAT[4][4] = {
		{ -0.5f,  1.5f, -1.5f,  0.5f },
		{  1.0f, -2.5f,  2.0f, -0.5f },
		{ -0.5f,  0.0f,  0.5f,  0.0f },
		{  0.0f,  1.0f,  0.0f,  0.0f }
};

static inline float lerp(float a, float a0, float a1, float t0, float t1) {
	return ((a1 - a) * t0 + (a - a0) * t1) / (a1 - a0);
}


CRSpline::CRSpline(float error)
	: m_error(error)
{
}

CRSpline::~CRSpline()
{
}

unsigned CRSpline::binarySearch(float length)
{
	unsigned upper = m_table.size();
	unsigned lower = 0;
	unsigned middle = 0;

	while (upper - lower > 1) {
		middle = (upper + lower) / 2;
		if (length >= m_table[middle].len)
			lower = middle;
		else
			upper = middle;
	}
	return lower;
}

float CRSpline::getT(float length)
{
    if (length == m_table.front().len)
    	return m_table.front().t;
    else if (length == m_table.back().len)
    	return m_table.back().t;

    unsigned lower = binarySearch(length);

    return lerp(length, m_table[lower].len, m_table[lower + 1].len,
    		m_table[lower].t, m_table[lower + 1].t);

}

Vec3f CRSpline::getPos(float length)
{
	if (length == m_table.front().len)
		return m_table.front().pos.xz3(
				newton::getVerticalPosition(m_table.front().pos.x, m_table.front().pos.y));
	else if (length == m_table.back().len)
		return m_table.back().pos.xz3(
				newton::getVerticalPosition(m_table.front().pos.x, m_table.back().pos.y));

	unsigned lower = binarySearch(length);

	Vec2f pos(lerp(length, m_table[lower].len, m_table[lower + 1].len,
			m_table[lower].pos.x, m_table[lower + 1].pos.x), lerp(length,
			m_table[lower].len, m_table[lower + 1].len, m_table[lower].pos.y,
			m_table[lower + 1].pos.y));

	return pos.xz3(newton::getVerticalPosition(pos.x, pos.y));
}

Vec3f CRSpline::getTangent(float length)
{
    if (length == m_table.front().len)
    	return m_table.front().tangent.xz3(0.0f);
    else if (length == m_table.back().len)
    	return m_table.back().tangent.xz3(0.0f);

    unsigned lower = binarySearch(length);

    Vec2f tangent(lerp(length, m_table[lower].len, m_table[lower + 1].len,
			m_table[lower].tangent.x, m_table[lower + 1].tangent.x), lerp(
			length, m_table[lower].len, m_table[lower + 1].len,
			m_table[lower].tangent.y, m_table[lower + 1].tangent.y));

    return tangent.xz3(0.0f);
}

std::pair<Vec3f, Vec3f> CRSpline::getPoint(float length)
{
	return std::make_pair(getPos(length), getTangent(length));
}

float CRSpline::update()
{
	m_table.clear();
	if (m_knots.size() < 3) return 0.0f;
    HelpList helpList;
    HelpList::iterator curPoint;

    HelpPoint helpPoint;

    // add the first help point to the table
    helpPoint.t = 0.0f;
    helpPoint.len = 0.0f;
    helpPoint.pos = interpolate(m_knots[0], m_knots[0], m_knots[1], m_knots[2], 0);
    helpPoint.tangent = derive(m_knots[0], m_knots[0], m_knots[1], m_knots[2], 0);
    helpList.push_back(helpPoint);

    curPoint = helpList.begin();

    for (unsigned i = 0; i < m_knots.size() - 1; ++i) {
    	// construct the new help point at the end of the current knot
        helpPoint.t = (float)(i + 1);
        helpPoint.pos = interpolate(at(i-1), at(i), at(i+1), at(i+2), 1.0f);
        helpPoint.tangent = derive(at(i-1), at(i), at(i+1), at(i+2), 1.0f);
        helpPoint.len = (helpPoint.pos - curPoint->pos).len() + helpList.back().len;
        helpList.push_back(helpPoint);

		// recursively subdivide the segments
        curPoint = divideSegment(helpList, curPoint, i);
    }

    // add the list of help point to the table
    curPoint = helpList.begin();
    while (curPoint != helpList.end()) {
        curPoint->t /= helpList.back().t;
        m_table.push_back(*curPoint);
        ++curPoint;
    }

    return (m_table.size() > 0) ? m_table.back().len : 0.0f;
}

CRSpline::HelpList::iterator CRSpline::divideSegment(HelpList &helpPoints, HelpList::iterator cur, unsigned knot)
{
    HelpList::iterator next = cur;
    ++next;

    // construct a new help point between the current and the next point
    HelpPoint middle;
    middle.t = (cur->t + next->t) * 0.5f;
    middle.pos = interpolate(at(knot - 1), at(knot), at(knot + 1), at(knot + 2), middle.t - (float)knot);
    middle.tangent = derive(at(knot - 1), at(knot), at(knot + 1), at(knot + 2), middle.t - (float)knot);

    // Calculate the distance between the points, i.e
    // the three edges of the triangle spanned by the
    // current, middle and next point
    float a = (middle.pos - cur->pos).len();
    float b = (next->pos - middle.pos).len();
    float c = (next->pos - cur->pos).len();

    // The error is a measure of the height of the triangle,
    // or the difference in the edge lengths
    float error = a + b - c;

    // subdivide the segments if the error exceeds the allowed error
    if (error > m_error) {

    	// add the increase in length to the subsequent help points
        HelpList::iterator itr = next;
        while (itr != helpPoints.end()) {
        	itr->len += error;
            ++itr;
        }

        // insert the middle
        middle.len = cur->len + a;
        helpPoints.insert(next, middle);

        // recursive subdivision
        next = divideSegment(helpPoints, cur, knot);
        next = divideSegment(helpPoints, next, knot);
    }

    return next;
}

void CRSpline::renderKnots(bool link, const Vec3f& color)
{
	Vec3f p;
	glColor3fv(&color.x);
	glBegin(GL_POINTS);
	for (unsigned i = 0; i < m_knots.size(); ++i) {
		p = m_knots[i].xz3(newton::getVerticalPosition(m_knots[i].x, m_knots[i].y));
		glVertex3fv(&p.x);
	}
	glEnd();

	if (link) {
		glBegin(GL_LINE_STRIP);
		for (unsigned i = 0; i < m_knots.size(); ++i) {
			p = m_knots[i].xz3(newton::getVerticalPosition(m_knots[i].x, m_knots[i].y));
			glVertex3fv(&p.x);
		}
		glEnd();
	}
}

void CRSpline::renderSpline(float accuracy, const Vec3f& color)
{
	if (m_knots.size() >= 3)  {
		glColor3fv(&color.x);
	    glBegin(GL_LINES);
	    Vec2f p;
	    Vec3f q;
	    unsigned int size = m_knots.size();

	    for (unsigned i = 0; i < size; ++i) {
			for (float t = 0.0f; t < 1.0f; t += 0.01f) {
				p = interpolate(at(i-1), at(i+0), at(i+1), at(i+2), t);
				q = p.xz3(newton::getVerticalPosition(p.x, p.y));
				glVertex3fv(&q.x);
				p = interpolate(at(i-1), at(i+0), at(i+1), at(i+2), t + 0.01f);
				q = p.xz3(newton::getVerticalPosition(p.x, p.y));
				glVertex3fv(&q.x);
			}
	    }
	    glEnd();
	}
}


void CRSpline::renderPoints(float gap, const Vec3f& color, bool tangent, const Vec3f& tangentColor, float tangentLength)
{
	if (m_table.size()) {
		for (float t = 0.0f; t < m_table.back().len; t += gap) {
			Vec3f p = getPos(t);
			Vec3f q = p + getTangent(t).normalized() * tangentLength;

			glColor3fv(&color.x);
			glBegin(GL_POINTS);
			glVertex3fv(&p.x);
			glEnd();

			if (tangent) {
				glColor3fv(&tangentColor.x);
				glBegin(GL_LINES);
				glVertex3fv(&p.x);
				glVertex3fv(&q.x);
				glEnd();
			}
		}
	}
}


Vec2f CRSpline::interpolate(Vec2f p0, Vec2f p1, Vec2f p2, Vec2f p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

    return p0 * (CR_MAT[0][0]*t3 + CR_MAT[1][0]*t2 + CR_MAT[2][0]*t + CR_MAT[3][0]) +
    	   p1 * (CR_MAT[0][1]*t3 + CR_MAT[1][1]*t2 + CR_MAT[2][1]*t + CR_MAT[3][1]) +
    	   p2 * (CR_MAT[0][2]*t3 + CR_MAT[1][2]*t2 + CR_MAT[2][2]*t + CR_MAT[3][2]) +
    	   p3 * (CR_MAT[0][3]*t3 + CR_MAT[1][3]*t2 + CR_MAT[2][3]*t + CR_MAT[3][3]);
}

Vec2f CRSpline::derive(Vec2f p0, Vec2f p1, Vec2f p2, Vec2f p3, float t)
{
    float t2 = t * t;

    return p0 * (CR_MAT[0][0]*t2*3 + CR_MAT[1][0]*t*2 + CR_MAT[2][0]) +
    	   p1 * (CR_MAT[0][1]*t2*3 + CR_MAT[1][1]*t*2 + CR_MAT[2][1]) +
    	   p2 * (CR_MAT[0][2]*t2*3 + CR_MAT[1][2]*t*2 + CR_MAT[2][2]) +
    	   p3 * (CR_MAT[0][3]*t2*3 + CR_MAT[1][3]*t*2 + CR_MAT[2][3]);
}

}
