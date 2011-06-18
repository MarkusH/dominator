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
	QObjectAction(const QString &text, sim::__Object::Type type, const bool freeze = false, const float mass = -1.0f, QWidget* parent = 0);
	sim::__Object::Type getType();
	float getMass();
	bool getFreezeState();

private:
	sim::__Object::Type m_type;
	float m_mass;
	bool m_freeze;

private slots:
	void sendObjectActionTriggered();

signals:
	void triggered(sim::__Object::Type, float, bool);
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

}
#endif /* QUTILSS_HPP_ */
