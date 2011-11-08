/**
 * @author Markus Holtermann
 * @date May 13, 2011
 * @file gui/mainwindow.cpp
 */

#include <gui/mainwindow.hpp>

#include <iostream>

#include <gui/dialogs.hpp>
#include <gui/qutils.hpp>
#include <newton/util.hpp>
#include <sound/soundmgr.hpp>
#include <util/config.hpp>

#include <QtCore/QList>
#include <QtCore/QTextCodec>
#include <QtCore/QTemporaryFile>
#include <QtCore/QString>

#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QFileDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QWidget>
#include <QtGui/QSplitter>
#include <QtGui/QStatusBar>
#include <QtGui/QVBoxLayout>

namespace gui {

MainWindow::MainWindow(QApplication* app)
{
	m_modified = true;
	m_tmp_file = NULL;

	// load the splash screen
	SplashScreen splash(100);
	splash.show();
	app->processEvents();

	util::ErrorAdapter::instance().addListener(new QtErrorListerner());

	initialize();
	splash.updateProgress(5, "Creating UI – Menues");

	createMenu();
	splash.updateProgress(15, "Creating UI – Notification area");
	app->processEvents();

	createStatusBar();
	splash.updateProgress(20, "Creating UI – Building interfaces");
	app->processEvents();

	m_toolBox = new ToolBox();
	splash.updateProgress(50, "Loading materials");
	m_toolBox->loadMaterials(QString::fromStdString(util::Config::instance().get<std::string>("materialsxml", "data/materials.xml")));
	splash.updateProgress(60, "Loading sounds");
	snd::SoundMgr::instance().LoadSound(util::Config::instance().get<std::string>("sounds", "data/sounds"));
	splash.updateProgress(70, "Loading music");
	snd::SoundMgr::instance().LoadMusic(util::Config::instance().get<std::string>("music", "data/music"));
	snd::SoundMgr::instance().setMusicEnabled(util::Config::instance().get("enableMusic", false));
	app->processEvents();
	splash.updateProgress(80, "Creating UI – Apply screen resolution");

	m_renderWidget = new RenderWidget(this);
	m_renderWidget->setMinimumWidth(400);
	m_renderWidget->show();
	app->processEvents();
	splash.updateProgress(85, "Creating UI – Building dependencies");

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

	// connect the newly created widgets with specific slots
	connect(m_renderWidget, SIGNAL(framesPerSecondChanged(int)), this, SLOT(updateFramesPerSecond(int)));
	connect(m_renderWidget, SIGNAL(objectsCountChanged(int)), this, SLOT(updateObjectsCount(int)));

	connect(m_renderWidget, SIGNAL(objectSelected(sim::Object)), m_toolBox, SLOT(updateData(sim::Object)));
	connect(m_renderWidget, SIGNAL(objectSelected(sim::__Object::Type)), m_toolBox, SLOT(showModificationWidgets(sim::__Object::Type)));
	connect(m_renderWidget, SIGNAL(objectSelected()), m_toolBox, SLOT(deselectInteraction()));
	connect(m_renderWidget, SIGNAL(objectSelected()), m_toolBox, SLOT(hideModificationWidgets()));

	connect(m_toolBox, SIGNAL(interactionSelected(sim::Simulation::InteractionType)), this, SLOT(selectInteraction(sim::Simulation::InteractionType)));
	splash.updateProgress(90, "Finished loading");

	showMaximized();
	m_renderWidget->updateGL();
	m_renderWidget->m_timer->start();
	m_toolBox->updateMaterials();
	splash.updateProgress(100, "Starting ...");

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

	m_stop = new QAction("&Stop", this);
	m_stop->setEnabled(false);
	m_stop->setShortcut(Qt::Key_F9);
	connect(m_stop, SIGNAL(triggered()), this, SLOT(onSimulationControlsPressed()));
	m_menuSimulation->addAction(m_stop);

	m_stop_no_reset = new QAction("&Stop (no reset)", this);
	m_stop_no_reset->setEnabled(false);
	m_stop_no_reset->setShortcut(Qt::CTRL | Qt::Key_F9);
	connect(m_stop_no_reset, SIGNAL(triggered()), this, SLOT(onSimulationControlsPressed()));
	m_menuSimulation->addAction(m_stop_no_reset);

	m_menuSimulation->addSeparator();

	m_gravity = new QAction("&Gravity", this);
	connect(m_gravity, SIGNAL(triggered()), this, SLOT(onGravityPressed()));
	m_menuSimulation->addAction(m_gravity);

	// Options
	m_menuOptions = menuBar()->addMenu("&Options");

	m_sound_play = new QAction("&Play Music", this);
	m_sound_play->setEnabled(!util::Config::instance().get("enableMusic", false));
	connect(m_sound_play, SIGNAL(triggered()), this, SLOT(onSoundControlsPressed()));
	m_menuOptions->addAction(m_sound_play);

	m_sound_stop = new QAction("&Stop Music", this);
	m_sound_stop->setEnabled(util::Config::instance().get("enableMusic", false));
	connect(m_sound_stop, SIGNAL(triggered()), this, SLOT(onSoundControlsPressed()));
	m_menuOptions->addAction(m_sound_stop);

	m_menuOptions->addSeparator();

	m_preferences = new QAction("&Preferences", this);
	connect(m_preferences, SIGNAL(triggered()), this, SLOT(onPreferencesPressed()));
	m_menuOptions->addAction(m_preferences);

	// Help
	m_menuHelp = menuBar()->addMenu("&Help");

	m_about = new QAction("&Info", this);
	m_about->setShortcuts(QKeySequence::HelpContents);
	connect(m_about, SIGNAL(triggered()), this, SLOT(onAboutPressed()));
	m_menuHelp->addAction(m_about);
}

void MainWindow::createStatusBar()
{
	m_framesPerSec = new QLabel("nA");
	m_framesPerSec->setMinimumSize(m_framesPerSec->sizeHint());
	m_framesPerSec->setAlignment(Qt::AlignLeft);
	m_framesPerSec->setToolTip("Current frames per second not yet initialized.");
	statusBar()->addWidget(m_framesPerSec);

	m_objectsCount = new QLabel("1 objects");
	m_objectsCount->setMinimumSize(m_objectsCount->sizeHint());
	m_objectsCount->setAlignment(Qt::AlignLeft);
	m_objectsCount->setToolTip("There is 1 object in the world");
	statusBar()->addWidget(m_objectsCount);

	m_simulationStatus = new QLabel("");
	m_simulationStatus->setMinimumSize(m_simulationStatus->sizeHint());
	m_simulationStatus->setAlignment(Qt::AlignLeft);
	statusBar()->addWidget(m_simulationStatus, 1);

	m_currentFilename = new QLabel("unsaved document");
	m_currentFilename->setMinimumSize(m_currentFilename->sizeHint());
	m_currentFilename->setAlignment(Qt::AlignRight);
	m_currentFilename->setToolTip("The world is not saved");
	statusBar()->addWidget(m_currentFilename, 8);
}

void MainWindow::updateFramesPerSecond(int frames)
{
	m_framesPerSec->setText(QString("%1 fps   ").arg(frames));
}

void MainWindow::updateObjectsCount(int count)
{
	switch (count) {
	case 0:
		m_objectsCount->setText("No objects   ");
		break;
	case 1:
		m_objectsCount->setText(QString("%1 object   ").arg(count));
		break;
	default:
		m_objectsCount->setText(QString("%1 objects   ").arg(count));
		break;
	}
}

void MainWindow::selectInteraction(sim::Simulation::InteractionType type)
{
	sim::Simulation::instance().setInteractionType(util::RIGHT, type);
}

void MainWindow::onNewPressed()
{
	/// @todo check for m_renderWidget->isModified()
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
	QFileDialog dialog(this);
	sim::Simulation::instance().setEnabled(false); //stop the simulation
	if (QObject::sender() == m_saveas) {
		QFileDialog dialog(this, "TUStudios Dominator - Save As");
	} else {
		QFileDialog dialog(this, "TUStudios Dominator - Save");
	}
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setFileMode(QFileDialog::AnyFile); //This is default
	dialog.setDirectory(QString::fromStdString(util::Config::instance().get<std::string>("levels", "data/levels/new_level.xml")) + "/new_level.xml");
	dialog.setFilter("TUStudios Dominator (*.xml)");
	if (m_filename == "" || QObject::sender() == m_saveas) {
		if (dialog.exec()) {
			m_filename = dialog.selectedFiles().first();
		} else {
			return;
		}
	}
	m_currentFilename->setText(m_filename);
	sim::Simulation::instance().save(m_filename.toStdString());
	m_modified = false;
}

void MainWindow::onOpenPressed()
{
	QFileDialog dialog(this, "TUStudios Dominator - Load");
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setDirectory(QString::fromStdString(util::Config::instance().get<std::string>("levels", "data/levels/")));
	dialog.setFilter("TUStudios Dominator (*.xml)");
	if (dialog.exec()) {
		sim::Simulation::instance().setEnabled(false);
		m_filename = dialog.selectedFiles().first();
		m_currentFilename->setText(m_filename);
		sim::Simulation::instance().load(m_filename.toStdString());
		m_modified = false;
	}
}

void MainWindow::onSimulationControlsPressed()
{
	bool status;
	if (QObject::sender() == m_play) {
		sim::Simulation::instance().setEnabled(false);
		m_tmp_file = new QTemporaryFile();
		m_tmp_file->open();
		sim::Simulation::instance().save(m_tmp_file->fileName().toStdString());
		status = true;
	} else {
		status = false;
	}

	// Disable to prevent inconsistencies with reset files
	m_new->setEnabled(!status);
	m_open->setEnabled(!status);
	m_save->setEnabled(!status);
	m_saveas->setEnabled(!status);

	m_play->setEnabled(!status);
	m_stop->setEnabled(status);
	m_stop_no_reset->setEnabled(status);
	sim::Simulation::instance().setEnabled(status);

	if (!status && m_tmp_file) {
		if (QObject::sender() == m_stop) {
			sim::Simulation::instance().load(m_tmp_file->fileName().toStdString());
		}
		m_tmp_file->close();
		free(m_tmp_file);
	}

	if (status) {
		m_simulationStatus->setText("Simulation started");
	} else {
		m_simulationStatus->setText("Simulation stopped");
	}
}

void MainWindow::onGravityPressed()
{
	GravityDialog* dialog = new GravityDialog(newton::gravity, this);
	newton::gravity = dialog->run();
}

void MainWindow::onSoundControlsPressed()
{
	bool status;
	if (QObject::sender() == m_sound_play) {
		status = true;
	} else {
		status = false;
	}
	m_sound_play->setEnabled(!status);
	m_sound_stop->setEnabled(status);
	snd::SoundMgr::instance().setMusicEnabled(status);
	//util::Config::instance().set("enableMusic", status);
}

void MainWindow::onPreferencesPressed()
{
	ConfigDialog configdialog;
	if (configdialog.exec()) {
		util::Config::instance().save("data/config.xml");
		MessageDialog("The configuration has been saved.", "You should restart the program in order to activate the changes.",
				gui::MessageDialog::QINFO);
	} else {
		util::Config::instance().destroy();
		util::Config::instance().load("data/config.xml");
	}
}

/**
 * @todo Implement some kind of help document. Maybe a link to the user
 * documentation
 */
void MainWindow::onHelpPressed()
{
}

void MainWindow::onAboutPressed()
{
	AboutDialog* about = new AboutDialog(this);
	about->exec();
}

}
