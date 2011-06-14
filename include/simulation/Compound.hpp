/*
 * Compound.hpp
 *
 *  Created on: Jun 1, 2011
 *      Author: markus
 */

#ifndef COMPOUND_HPP_
#define COMPOUND_HPP_

#include <m3d/m3d.hpp>
#include <simulation/Object.hpp>
#ifdef _WIN32
#include <boost/tr1/memory.hpp>
#else
#include <tr1/memory>
#endif
#include <list>
#include <simulation/Joint.hpp>

namespace sim {

using namespace m3d;

class __Compound;
typedef std::tr1::shared_ptr<__Compound> Compound;

/**
 * Aggregates multiple objects to one compound object. The compound has a matrix,
 * all child objects are aligned to this matrix when added to the compound. That means
 * when adding an object, its position has to be relative to the compound. Not however,
 * that after that all objects have a global position which is not relative to the compound.
 * When moving the compound, the children will be moved accordingly.
 *
 * After adding the compound to the simulation, new objects should not be added.
 */
class __Compound : public __Object {
protected:
	int m_nextID;
	Mat4f m_matrix;
	std::list<Object> m_nodes;
	std::list<Joint> m_joints;
public:
	__Compound();
	__Compound(const Mat4f& matrix);
	__Compound(const Vec3f& position);
	virtual ~__Compound();

	void add(Object object);

	virtual const Mat4f& getMatrix() const;
	virtual void setMatrix(const Mat4f& matrix);

	/** @param state Sets the freeze state of all nodes to state */
	virtual void setFreezeState(int state);

	/** @return True, if all nodes are in freeze state, False otherwise */
	virtual int getFreezeState();

	virtual void getAABB(Vec3f& min, Vec3f& max);

	virtual float convexCastPlacement(bool apply = true, std::list<NewtonBody*>* noCollision = NULL);

	Hinge createHinge(const Vec3f& pivot, const Vec3f& pinDir, const Object& child, const Object& parent);
	BallAndSocket createBallAndSocket(const Vec3f& pivot, const Vec3f& pinDir, const Object& child, const Object& parent);

	virtual bool contains(const NewtonBody* const body);
	virtual bool contains(const __Object* object);
	virtual void genBuffers(ogl::VertexBuffer& vbo);
	virtual void render();

	static void save(const __Compound& compound /* node */);
	static Compound load(/*node */);
};


inline const Mat4f& __Compound::getMatrix() const
{
	return m_matrix;
}

}

#endif /* COMPOUND_HPP_ */
