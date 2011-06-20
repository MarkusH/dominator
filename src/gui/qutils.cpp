/**
 * @author Markus Holtermann
 * @date Jun 7, 2011
 * @file gui/qutils.cpp
 */

#include <gui/qutils.hpp>
#include <stdio.h>

namespace gui {

QObjectAction::QObjectAction(sim::__Object::Type type, QWidget* parent) :
	QAction(parent)
{
	setText(QString::fromStdString(sim::__Object::name[type]));
	m_type = type;
	m_mass = sim::__Object::mass[type];
	m_freeze = sim::__Object::freezeState[type];
	m_size = sim::__Object::size[type];
}

}
