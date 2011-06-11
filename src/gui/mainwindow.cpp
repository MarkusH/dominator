/*
 * mainwindow.cpp
 *
 *      Author: Markus Holtermann
 */

#include <iostream>

#include <gui/mainwindow.hpp>
#include <QtGui/QPixmap>
#include <QtGui/QSplashScreen>
#include <QtGui/QMessageBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>

#include <QtCore/QString>
#include <QtCore/QList>

MainWindow::MainWindow(QApplication* app)
{
	m_modified = true;

	QPixmap pixmap = QPixmap("data/splash.png");
	QSplashScreen splash(pixmap);
	splash.show();
	app->processEvents();

	initialize();

	createMenu();
	app->processEvents();

	createStatusBar();
	app->processEvents();

	m_modifyBox = new ModifyBox();
	m_modifyBox->move(10, 250);
	app->processEvents();

	m_toolBox = new ToolBox();
	m_toolBox->addWidget(m_modifyBox);
	app->processEvents();

	m_renderWidget = new RenderWidget(this);
	m_renderWidget->setMinimumWidth(400);
	m_renderWidget->show();

	m_splitter = new QSplitter(Qt::Horizontal);
	m_splitter->insertWidget(0, m_toolBox);
	m_splitter->insertWidget(1, m_renderWidget);

	QList<int> sizes;
	sizes.append(200);
	sizes.append(1300);
	m_splitter->setSizes(sizes);
	m_splitter->setStretchFactor(0, 1);
	m_splitter->setStretchFactor(1, 1);
	m_splitter->setChildrenCollapsible(false);

	setCentralWidget(m_splitter);

	connect(m_renderWidget, SIGNAL(framesPerSecondChanged(int)), this, SLOT(updateFramesPerSecond(int)));
	connect(m_renderWidget, SIGNAL(objectsCountChanged(int)), this, SLOT(updateObjectsCount(int)));
	connect(m_renderWidget, SIGNAL(objectSelected(const m3d::Mat4f*)), m_modifyBox, SLOT(updateData(const m3d::Mat4f*)));
	connect(m_renderWidget, SIGNAL(objectSelected(bool)), m_modifyBox, SLOT(updateData(bool)));
	connect(m_toolBox, SIGNAL(interactionSelected(sim::Simulation::InteractionType)), this, SLOT(selectInteraction(sim::Simulation::InteractionType)));
	connect(m_modifyBox, SIGNAL(changeSize(char, float)), m_renderWidget, SLOT(renderSize(char, float)));
	connect(m_modifyBox, SIGNAL(changeLocation(char, float)), m_renderWidget, SLOT(renderLocation(char, float)));
	connect(m_modifyBox, SIGNAL(changeRotation(float, float, float)), m_renderWidget, SLOT(renderRotation(float, float, float)));

	showMaximized();
	splash.finish(this);
}

void MainWindow::initialize()
{
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
	m_filename = "";

	this->setWindowTitle("TUStudios - DOMINATOR");
}

void MainWindow::createMenu()
{
	// File
	m_menuFile = menuBar()->addMenu("&File");

	m_new = new QAction("&New", this);
	m_new->setShortcuts(QKeySequence::New);
	connect(m_new, SIGNAL(triggered()), this, SLOT(onNewPressed()));
	m_menuFile->addAction(m_new);

	m_open = new QAction("&Open", this);
	m_open->setShortcuts(QKeySequence::Open);
	connect(m_open, SIGNAL(triggered()), this, SLOT(onOpenPressed()));
	m_menuFile->addAction(m_open);

	m_save = new QAction("&Save", this);
	m_save->setShortcuts(QKeySequence::Save);
	connect(m_save, SIGNAL(triggered()), this, SLOT(onSavePressed()));
	m_menuFile->addAction(m_save);

	m_saveas = new QAction("Save &As", this);
	m_saveas->setShortcuts(QKeySequence::SaveAs);
	connect(m_saveas, SIGNAL(triggered()), this, SLOT(onSavePressed()));
	m_menuFile->addAction(m_saveas);

	m_menuFile->addSeparator();

	m_exit = new QAction("E&xit", this);
	m_exit->setShortcuts(QKeySequence::Quit);
	connect(m_exit, SIGNAL(triggered()), this, SLOT(onClosePressed()));
	m_menuFile->addAction(m_exit);

	// Simulation
	m_menuSimulation = menuBar()->addMenu("&Simulation");

	m_play = new QAction("&Play", this);
	m_play->setEnabled(true);
	m_play->setShortcut(Qt::Key_F9);
	connect(m_play, SIGNAL(triggered()), this, SLOT(onSimulationControlsPressed()));
	m_menuSimulation->addAction(m_play);

	m_menuSimulation->addSeparator();

	m_stop = new QAction("&Stop", this);
	m_stop->setEnabled(false);
	m_stop->setShortcut(Qt::Key_F9);
	connect(m_stop, SIGNAL(triggered()), this, SLOT(onSimulationControlsPressed()));
	m_menuSimulation->addAction(m_stop);

	m_gravity = new QAction("&Gravity", this);
	connect(m_gravity, SIGNAL(triggered()), this, SLOT(onGravityPressed()));
	m_menuSimulation->addAction(m_gravity);

	// Help
	m_menuHelp = menuBar()->addMenu("&Help");

	m_help = new QAction("&Help", this);
	m_help->setShortcuts(QKeySequence::HelpContents);
	connect(m_help, SIGNAL(triggered()), this, SLOT(onHelpPressed()));
	m_menuHelp->addAction(m_help);

	m_about = new QAction("&Info", this);
	m_about->setShortcut(Qt::ALT + Qt::Key_F1);
	connect(m_about, SIGNAL(triggered()), this, SLOT(onAboutPressed()));
	m_menuHelp->addAction(m_about);
}

void MainWindow::createStatusBar()
{
	m_framesPerSec = new QLabel("nA");
	m_framesPerSec->setMinimumSize(m_framesPerSec->sizeHint());
	m_framesPerSec->setAlignment(Qt::AlignLeft);
	m_framesPerSec->setToolTip("Current frames per second not yet initialized.");
	statusBar()->addWidget(m_framesPerSec, 2);

	m_objectsCount = new QLabel("1 objects");
	m_objectsCount->setMinimumSize(m_objectsCount->sizeHint());
	m_objectsCount->setAlignment(Qt::AlignLeft);
	m_objectsCount->setToolTip("There is 1 object in the world");
	statusBar()->addWidget(m_objectsCount, 2);

	m_simulationStatus = new QLabel("");
	m_simulationStatus->setMinimumSize(m_simulationStatus->sizeHint());
	m_simulationStatus->setAlignment(Qt::AlignLeft);
	statusBar()->addWidget(m_simulationStatus, 2);

	m_currentFilename = new QLabel("unsaved document");
	m_currentFilename->setMinimumSize(m_currentFilename->sizeHint());
	m_currentFilename->setAlignment(Qt::AlignRight);
	m_currentFilename->setToolTip("The world is not saved");
	statusBar()->addWidget(m_currentFilename, 2);
}

void MainWindow::updateFramesPerSecond(int frames)
{
	m_framesPerSec->setText(QString("%1 fps").arg(frames));
}

void MainWindow::updateObjectsCount(int count)
{
	m_objectsCount->setText(QString("%1").arg(count));
}

void MainWindow::selectInteraction(sim::Simulation::InteractionType type) {
	sim::Simulation::instance().setInteractionType(util::RIGHT, type);
}

void MainWindow::onNewPressed()
{
	// TODO: check for m_renderWidget->isModified()
	sim::Simulation::instance().init();
	sim::Simulation::instance().setEnabled(false);
}

void MainWindow::onClosePressed()
{
	QMessageBox msgBox;
	msgBox.setText("Do you want to exit DOMINATOR");
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);
	int ret = msgBox.exec();
	if (ret == QMessageBox::Yes) {
		this->close();
	}
}

void MainWindow::onSavePressed()
{
	if (m_filename == "" || QObject::sender() == m_saveas) {
		m_filename = QFileDialog::getSaveFileName(this, "TUStudios Dominator - Save file", 0, "TUStudios Dominator (*.tus)");
	}
	m_currentFilename->setText(m_filename);
	sim::Simulation::instance().save(m_filename.toStdString());
	m_modified = false;
}

void MainWindow::onOpenPressed()
{
	QFileDialog dialog(this);
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setFilter("TUStudios Dominator (*.tus)");
	if (dialog.exec()) {
		m_filename = dialog.selectedFiles().first();
		m_currentFilename->setText(m_filename);
		sim::Simulation::instance().load(m_filename.toStdString());
		m_modified = false;
	}
}

void MainWindow::onSimulationControlsPressed()
{
	bool set_active;
	if (QObject::sender() == m_play) {
		set_active = true;
		m_simulationStatus->setText("Simulation started");
	} else if (QObject::sender() == m_stop) {
		set_active = false;
		m_simulationStatus->setText("Simulation stopped");
	} else {
		return;
	}
	m_play->setEnabled(!set_active);
	m_stop->setEnabled(set_active);
	sim::Simulation::instance().setEnabled(set_active);
}

void MainWindow::onGravityPressed()
{
	GravityDialog* dialog = new GravityDialog(sim::Simulation::instance().getGravity(), this);
	sim::Simulation::instance().setGravity(dialog->run());
}

void MainWindow::onHelpPressed()
{
}

void MainWindow::onAboutPressed()
{
	AboutDialog* about = new AboutDialog(this);
	about->exec();
}
