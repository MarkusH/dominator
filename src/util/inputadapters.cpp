/*
 * InputAdapters.cpp
 *
 *  Created on: May 25, 2011
 *      Author: Markus Doellinger
 */

#include <util/inputadapters.hpp>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QWheelEvent>
#include <iostream>
//#include <boost/foreach.hpp>

namespace util {

KeyAdapter::KeyAdapter()
	: m_alt(false),
	  m_ctrl(false),
	  m_shift(false)
{
}

void AsciiKeyAdapter::keyEvent(bool down, unsigned char key)
{
	if (down)
		m_pressed.insert(key);
	else
		m_pressed.erase(key);
}


void QtKeyAdapter::keyEvent(QKeyEvent* event)
{
	// do nothing if the key was already pressed
	if (event->isAutoRepeat())
		return;

	// if the text is empty, it was a special key
	if (!event->text().isEmpty()) {
		QString text = event->text().toLower();
		unsigned char key = text.toAscii().at(0);
		if (event->type() == QEvent::KeyPress)
			m_pressed.insert(key);
		else
		if (event->type() == QEvent::KeyRelease)
			m_pressed.erase(key);
	} else {
		m_alt = event->key() == Qt::Key_Alt && event->type() == QEvent::KeyPress;
		m_ctrl = event->key() == Qt::Key_Control && event->type() == QEvent::KeyPress;
		m_shift = event->key() == Qt::Key_Shift && event->type() == QEvent::KeyPress;
	}
}

MouseAdapter::MouseAdapter()
{
	m_down[0] = m_down[1] = m_down[2] = 0;
}

MouseAdapter::~MouseAdapter()
{
	m_listeners.clear();
}

void MouseAdapter::addListener(MouseListener* listener)
{
	m_listeners.push_back(listener);
}

void MouseAdapter::removeListener(MouseListener* listener)
{
	m_listeners.remove(listener);
	/*
	for (std::list<MouseListener*>::iterator itr = m_listeners.begin(); itr != m_listeners.end(); ++itr)
		if (*itr == listener)
			m_listeners.erase(itr);
			*/
}

bool MouseAdapter::isDown(Button button)
{
	return m_down[button];
}

void SimpleMouseAdapter::mouseMove(int x, int y)
{
	//BOOST_FOREACH(MouseListener* lis, m_listeners) {
	//	lis->mouseMove(x, y);
	//}

	for (std::list<MouseListener*>::iterator itr = m_listeners.begin();
			itr != m_listeners.end(); ++itr)
		(*itr)->mouseMove(x, y);
	m_x = x;
	m_y = y;
}

void SimpleMouseAdapter::mouseButton(Button button, bool down, int x, int y)
{
	m_down[button] = down;
	for (std::list<MouseListener*>::iterator itr = m_listeners.begin();
			itr != m_listeners.end(); ++itr)
		(*itr)->mouseButton(button, down, x, y);
	m_x = x;
	m_y = y;
}

void QtMouseAdapter::mouseEvent(QMouseEvent* event)
{
	Button button = LEFT;
	switch (event->button()) {
	case Qt::LeftButton:
		button = LEFT;
		break;

	case Qt::RightButton:
		button = RIGHT;
		break;

	case Qt::MiddleButton:
		button = MIDDLE;
		break;

	default:
		break;
	}

	switch (event->type()) {

	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
		m_down[button] = event->type() == QEvent::MouseButtonPress;
		for (std::list<MouseListener*>::iterator itr = m_listeners.begin();
				itr != m_listeners.end(); ++itr)
			(*itr)->mouseButton(button, m_down[button], event->x(), event->y());
		break;

	case QEvent::MouseButtonDblClick:
		for (std::list<MouseListener*>::iterator itr = m_listeners.begin();
				itr != m_listeners.end(); ++itr)
			(*itr)->mouseDoubleClick(button, event->x(), event->y());
		break;

	case QEvent::MouseMove:
		for (std::list<MouseListener*>::iterator itr = m_listeners.begin();
				itr != m_listeners.end(); ++itr)
			(*itr)->mouseMove(event->x(), event->y());
		break;

	default:
		break;
	}

	m_x = event->x();
	m_y = event->y();
}

void QtMouseAdapter::mouseWheelEvent(QWheelEvent* event) {
	for (std::list<MouseListener*>::iterator itr = m_listeners.begin();
			itr != m_listeners.end(); ++itr)
		(*itr)->mouseWheel(event->delta());
}

}
