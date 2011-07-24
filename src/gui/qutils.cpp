/**
 * @author Markus Holtermann
 * @date Jun 7, 2011
 * @file gui/qutils.cpp
 */

#include <gui/qutils.hpp>
#include <gui/dialogs.hpp>
#include <QtCore/QFileInfo>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QMouseEvent>
#include <QtGui/QPixmap>
#include <QtGui/QProgressBar>
#include <QtGui/QWheelEvent>

namespace gui {

QObjectAction::QObjectAction(sim::__Object::Type type, QWidget* parent) :
	QAction(parent)
{
	setText(QString::fromStdString(sim::__Object::TypeName[type]));
	m_type = type;
	m_mass = sim::__Object::TypeMass[type];
	m_freeze = sim::__Object::TypeFreezeState[type];
	m_size = sim::__Object::TypeSize[type];
	m_filename = "";
}

QObjectAction::QObjectAction(QFileInfo fileinfo, QWidget* parent) :
	QAction(parent)
{
	QString name = fileinfo.baseName().toLower();
	if(!name.isEmpty())
		name[0] = name.at(0).toUpper();

	m_type = sim::__Object::COMPOUND;
	setText(name);
	m_mass = sim::__Object::TypeMass[m_type];
	m_freeze = sim::__Object::TypeFreezeState[m_type];
	m_size = sim::__Object::TypeSize[m_type];
	m_filename = fileinfo.absoluteFilePath();
}

SplashScreen::SplashScreen(int max) :
		QSplashScreen()
{
	QPixmap pixmap = QPixmap("data/splash.png");
	setPixmap(pixmap);
	m_bar = new QProgressBar(this);
	m_bar->setGeometry(10, 10, width() - 20, 20);
	m_bar->setMaximum(max);
	m_bar->setValue(0);
	m_message = new QLabel(this);
	m_message->setGeometry(10, 30, width() - 20, 20);
	m_message->setText("Loading ...");
}

void SplashScreen::updateProgress(const int progress, const QString message)
{
	m_bar->setValue(progress);
	m_bar->repaint(10, 10, width() - 20, 20);
	m_message->setText(message);
}

void QtErrorListerner::displayError(const std::string& message)
{
	gui::MessageDialog("Error", message, gui::MessageDialog::QERROR);
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

void QtMouseAdapter::mouseEvent(QMouseEvent* event)
{
	util::Button button = util::LEFT;
	switch (event->button()) {
	case Qt::LeftButton:
		button = util::LEFT;
		break;

	case Qt::RightButton:
		button = util::RIGHT;
		break;

	case Qt::MiddleButton:
		button = util::MIDDLE;
		break;

	default:
		break;
	}

	switch (event->type()) {

	case QEvent::MouseButtonPress:
	case QEvent::MouseButtonRelease:
		m_down[button] = event->type() == QEvent::MouseButtonPress;
		for (std::list<util::MouseListener*>::iterator itr = m_listeners.begin();
				itr != m_listeners.end(); ++itr)
			(*itr)->mouseButton(button, m_down[button], event->x(), event->y());
		break;

	case QEvent::MouseButtonDblClick:
		for (std::list<util::MouseListener*>::iterator itr = m_listeners.begin();
				itr != m_listeners.end(); ++itr)
			(*itr)->mouseDoubleClick(button, event->x(), event->y());
		break;

	case QEvent::MouseMove:
		for (std::list<util::MouseListener*>::iterator itr = m_listeners.begin();
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
	for (std::list<util::MouseListener*>::iterator itr = m_listeners.begin();
			itr != m_listeners.end(); ++itr)
		(*itr)->mouseWheel(event->delta());
}
}
