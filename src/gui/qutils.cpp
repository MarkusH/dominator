/**
 * @author Markus Holtermann
 * @date Jun 7, 2011
 * @file gui/qutils.cpp
 */

#include <gui/qutils.hpp>
#include <stdio.h>

namespace gui {

QObjectAction::QObjectAction(const QString &text, sim::__Object::Type type, QWidget* parent) :
	QAction(text, parent)
{
	m_type = type;
	connect(this, SIGNAL(triggered()), this, SLOT(sendObjectActionTriggered()));
}

void QObjectAction::sendObjectActionTriggered()
{
	std::cout << m_type << std::endl;
	emit triggered(m_type);
}

}
