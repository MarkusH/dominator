/**
 * @author Markus Holtermann
 * @date Jun 17, 2011
 * @file gui/qutils.hpp
 */

#ifndef QUTILSS_HPP_
#define QUTILSS_HPP_

#include <QtGui/QAction>
#include <simulation/simulation.hpp>

namespace gui {

/**
 *
 */
class QObjectAction: public QAction {
Q_OBJECT
public:
	/**
	 *
	 */
	QObjectAction(sim::__Object::Type type, QWidget* parent = 0);
	sim::__Object::Type getType();
	float getMass();
	bool getFreezeState();
	m3d::Vec3f getSize();

private:
	sim::__Object::Type m_type;
	float m_mass;
	bool m_freeze;
	m3d::Vec3f m_size;
};

inline sim::__Object::Type QObjectAction::getType()
{
	return m_type;
}

inline float QObjectAction::getMass()
{
	return m_mass;
}

inline bool QObjectAction::getFreezeState()
{
	return m_freeze;
}

inline m3d::Vec3f QObjectAction::getSize()
{
	return m_size;
}

}
#endif /* QUTILSS_HPP_ */
