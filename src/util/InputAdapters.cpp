/*
 * InputAdapters.cpp
 *
 *  Created on: May 25, 2011
 *      Author: Markus Doellinger
 */

#include <util/InputAdapters.hpp>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>

namespace util {

KeyAdapter::KeyAdapter()
	: m_alt(false),
	  m_ctrl(false),
	  m_shift(false)
{
}

KeyAdapter::~KeyAdapter()
{
}

AsciiKeyAdapter::AsciiKeyAdapter()
{
}

AsciiKeyAdapter::~AsciiKeyAdapter()
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
		unsigned char key = event->text().toAscii().at(0);
		if (event->type() == QEvent::KeyPress)
			m_pressed.insert(key);
		else
		if (event->type() == QEvent::KeyRelease)
			m_pressed.erase(key);
	} else {
		m_alt = event->key() == Qt::Key_Alt;
		m_ctrl = event->key() == Qt::Key_Control;
		m_shift = event->key() == Qt::Key_Shift;
	}
}



MouseAdapter::MouseAdapter()
{
}

MouseAdapter::~MouseAdapter()
{
	m_listeners.clear();
}

void MouseAdapter::addListener(MouseListener* listener)
{
	m_listeners.push_back(listener);
};

void MouseAdapter::removeListener(MouseListener* listener)
{
	for (std::list<MouseListener*>::iterator itr = m_listeners.begin(); itr != m_listeners.end(); ++itr)
		if (*itr == listener)
			m_listeners.erase(itr);
}

bool MouseAdapter::isDown(Button button)
{
	return m_down[button];
}



SimpleMouseAdapter::SimpleMouseAdapter()
{
}

SimpleMouseAdapter::~SimpleMouseAdapter()
{
}

void SimpleMouseAdapter::mouseMove(int x, int y)
{
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
		(*itr)->mouseButton(button, down, y, x);
	m_x = x;
	m_y = y;
}

QtMouseAdapter::QtMouseAdapter()
{
}

QtMouseAdapter::~QtMouseAdapter()
{
}

void QtMouseAdapter::mouseEvent(QMouseEvent* event)
{
	//TODO: extract event data and call listeners
	// note that m_x and m_y must be set after the listeners
	// have been called. This is because the listeners must be able
	// to determine the delta in position
}

}
