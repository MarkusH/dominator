/**
 * @author Markus Doellinger
 * @date Jun 7, 2011
 * @file simulation/domino.hpp
 */

#ifndef DOMINO_HPP_
#define DOMINO_HPP_

#include <simulation/object.hpp>

namespace sim {

class __Domino;
typedef std::tr1::shared_ptr<__Domino> Domino;

/**
 * This class encapsulates domino pieces. They are simple boxes
 * derived from __RigidBody. Since there are only three different
 * types of dominos, this class provides caching for the collision
 * objects and the geometry.
 */
class __Domino : public __RigidBody {
protected:
	/**
	 * DEPRECATED because collisions need to have materials.
	 *
	 * Collision geometry cache for the domino pieces, indexed by
	 * DOMINO_SMALL, DOMINO_MIDDLE, DOMINO_LARGE.
	 */
	static NewtonCollision* s_domino_collision[3];

	/**
	 * DEPRECATED because collisions need to have materials.
	 *
	 * Returns a new collision object with the given type and material.
	 *
	 * @param type       The domino type
	 * @param materialID The material of the domino
	 * @return           The collision geometry
	 */
	static NewtonCollision* getCollision(Type type, int materialID);

	/** Sizes for the domino pieces, indexed by
	 * DOMINO_SMALL, DOMINO_MIDDLE, DOMINO_LARGE. */
	static Vec3f s_domino_size[3];
public:
	/** Gaps for curves for the domino pieces, indexed by
	 * DOMINO_SMALL, DOMINO_MIDDLE, DOMINO_LARGE. */
	static float s_domino_gap[3];

	/**
	 * Constructs a new domino object.
	 *
	 * @param type
	 * @param matrix
	 * @param material
	 * @param freezeState
	 */
	__Domino(Type type, const Mat4f& matrix, const std::string& material = "", int freezeState = 1);

	virtual ~__Domino();

	/**
	 * Creates a new SubBuffer for the domino, but does not add any
	 * new geometry. The data is cached in the vertex buffer.
	 *
	 * @param vbo The vertex buffer to add the buffers to
	 */
	virtual void genBuffers(ogl::VertexBuffer& vbo);

	/**
	 * Generates the domino geometry cache.
	 *
	 * @param vbo The vertex buffer to add the data to
	 */
	static void genDominoBuffers(ogl::VertexBuffer& vbo);

	/**
	 * DEPRECATED because collisions need to have materials.
	 *
	 * Clears the domino geometry cache.
	 */
	static void freeCollisions();

	/**
	 * Creates a new domino object with the given attributes.
	 *
	 * @param type        The domino type
	 * @param matrix      The matrix of the domino
	 * @param mass        The mass, or -1 for auto-generated mass
	 * @param material    The material name
	 * @param doPlacement If true, places the domino using ray-casts
	 * @return            The newly created domino object
	 */
	static Domino createDomino(Type type, const Mat4f& matrix, float mass, const std::string& material = "", bool doPlacement = true);
};

}

#endif /* DOMINO_HPP_ */
