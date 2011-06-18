/**
 * @author Markus Holtermann
 * @date Jun 7, 2011
 * @file gui/qutils.cpp
 */

#include <gui/qutils.hpp>
#include <stdio.h>

namespace gui {

QObjectAction::QObjectAction(const QString &text, sim::__Object::Type type, const bool freeze, const float mass, QWidget* parent) :
	QAction(text, parent)
{
	m_type = type;
	m_mass = mass;
	m_freeze = freeze;
	connect(this, SIGNAL(triggered()), this, SLOT(sendObjectActionTriggered()));
}

void QObjectAction::sendObjectActionTriggered()
{
	emit triggered(m_type, m_mass, m_freeze);
}

}
