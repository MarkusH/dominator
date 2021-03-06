/**
 * @author Markus Doellinger, Robert Waury
 * @date Jun 1, 2011
 * @file simulation/compound.hpp
 */

#ifndef COMPOUND_HPP_
#define COMPOUND_HPP_

#include <m3d/m3d.hpp>
#include <simulation/object.hpp>
#include <boost/tr1/memory.hpp>
#include <list>
#include <simulation/joint.hpp>

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
 * After adding the compound to the simulation, new objects should not be added. DO NOT EVER
 * ROTATE THE COMPOUND AROUND ANYTHING BUT THE ORIGIN OF THE COMPOUND. This would destroy the
 * joints during serialization.
 */
class __Compound : public __Object {
protected:
	int m_nextID;
	Mat4f m_matrix;
	std::list<Object> m_nodes;
	std::list<Joint> m_joints;

	__Compound();
	__Compound(const Mat4f& matrix);
	__Compound(const Vec3f& position);
public:
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

	Hinge createHinge(const Vec3f& pivot, const Vec3f& pinDir, const Object& child, const Object& parent,
			bool limited = false, float minAngle = -1.0f, float maxAngle = 1.0f);
	Slider createSlider(const Vec3f& pivot, const Vec3f& pinDir, const Object& child, const Object& parent,
			bool limited = false, float minDist = -1.0f, float maxDist = 1.0f);
	BallAndSocket createBallAndSocket(const Vec3f& pivot, const Vec3f& pinDir,
			const Object& child, const Object& parent,
			bool limited = false, float coneAngle = 0.0f, float minTwist = 0.0f, float maxTwist = 0.0f);

	virtual bool contains(const NewtonBody* const body);
	virtual bool contains(const __Object* object);
	virtual void genBuffers(ogl::VertexBuffer& vbo);
	virtual void render();

	static Compound createCompound(const Mat4f& matrix = Mat4f::identity());

	/**
	 * Saves Compound object to XML
	 *
	 * @param compound	Reference to Compound object to save
	 * @param parent	Pointer to root node
	 * @param doc		Pointer to XML document
	 */
	static void save(__Compound& compound, rapidxml::xml_node<>* parent, rapidxml::xml_document<>* doc);

	/**
	 * Loads Compound from XML node
	 *
	 * @param	node	Pointer to XML node
	 * @throws	rapidxml::parse_error	attribute not found
	 * @return	The generated Compound object
	 */
	static Compound load(rapidxml::xml_node<>* node);
};


inline const Mat4f& __Compound::getMatrix() const
{
	return m_matrix;
}

}

#endif /* COMPOUND_HPP_ */
