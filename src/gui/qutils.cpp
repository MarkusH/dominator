/**
 * @author Markus Holtermann
 * @date Jun 7, 2011
 * @file gui/qutils.cpp
 */

#include <gui/qutils.hpp>
#include <gui/dialogs.hpp>
#include <QtGui/QPixmap>

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
}
