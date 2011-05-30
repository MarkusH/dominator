/*
 * mainwindow.cpp
 *
 *      Author: Markus Holtermann
 */

#include <iostream>

#include <gui/mainwindow.hpp>
#include <gui/render.hpp>
#include <QtGui/QPixmap>
#include <QtGui/QSplashScreen>
#include <QtGui/QMessageBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QLabel>

#include <QtCore/QString>
#include <QtCore/QList>

MainWindow::MainWindow(QApplication *app) {

	QPixmap pixmap("data/splash.png");
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

	m_renderWindow = new Render(this);
	m_renderWindow->setMinimumWidth(400);
	m_renderWindow->show();

	m_splitter = new QSplitter(Qt::Horizontal);
	m_splitter->insertWidget(0, m_toolBox);
	m_splitter->insertWidget(1, m_renderWindow);

	QList<int> sizes;
	sizes.append(200);
	sizes.append(1300);
	m_splitter->setSizes(sizes);
	m_splitter->setStretchFactor(0, 1);
	m_splitter->setStretchFactor(1, 1);
	m_splitter->setChildrenCollapsible(false);

	setCentralWidget(m_splitter);

	connect(m_renderWindow, SIGNAL(framesPerSecondChanged(int)), this, SLOT(updateFramesPerSecond(int)));
	connect(m_renderWindow, SIGNAL(objectsCountChanged(int)), this, SLOT(updateObjectsCount(int)));
	connect(m_renderWindow, SIGNAL(objectSelected(const m3d::Mat4f*)), m_modifyBox, SLOT(updateData(const m3d::Mat4f*)));
	connect(m_renderWindow, SIGNAL(objectSelected(bool)), m_modifyBox, SLOT(updateData(bool)));
	connect(m_modifyBox, SIGNAL(changeSize(char, float)), m_renderWindow, SLOT(renderSize(char, float)));
	connect(m_modifyBox, SIGNAL(changeLocation(char, float)), m_renderWindow, SLOT(renderLocation(char, float)));
	connect(m_modifyBox, SIGNAL(changeRotation(char, float)), m_renderWindow, SLOT(renderRotation(char, float)));

	showMaximized();
	splash.finish(this);
}

void MainWindow::OnClosePressed() {
	QMessageBox msgBox;
	msgBox.setText("Do you want to exit DOMINATOR");
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);
	int ret = msgBox.exec();
	if (ret == QMessageBox::Yes) {
		this->close();
	}
}

void MainWindow::createMenu() {
	m_menuFile = menuBar()->addMenu("&File");

	m_new = new QAction("&New", this);
	m_new->setShortcuts(QKeySequence::New);
	m_menuFile->addAction(m_new);

	m_open = new QAction("&Open", this);
	m_open->setShortcuts(QKeySequence::Open);
	m_menuFile->addAction(m_open);

	m_save = new QAction("&Save", this);
	m_save->setShortcuts(QKeySequence::Save);
	m_menuFile->addAction(m_save);

	m_saveas = new QAction("Save &As", this);
	m_saveas->setShortcuts(QKeySequence::SaveAs);
	m_menuFile->addAction(m_saveas);

	m_menuFile->addSeparator();

	m_exit = new QAction("E&xit", this);
	m_exit->setShortcuts(QKeySequence::Quit);
	connect(m_exit, SIGNAL(triggered()), this, SLOT(OnClosePressed()));
	m_menuFile->addAction(m_exit);

	m_menuHelp = menuBar()->addMenu("&Help");

	m_help = new QAction("&Help", this);
	m_help->setShortcuts(QKeySequence::HelpContents);
	m_menuHelp->addAction(m_help);

	m_info = new QAction("&Info", this);
	m_menuHelp->addAction(m_info);
}

void MainWindow::createStatusBar() {
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

	m_currentFilename = new QLabel("unsaved document");
	m_currentFilename->setMinimumSize(m_currentFilename->sizeHint());
	m_currentFilename->setAlignment(Qt::AlignRight);
	m_currentFilename->setToolTip("The world is not saved");
	statusBar()->addWidget(m_currentFilename, 2);
}

void MainWindow::updateFramesPerSecond(int frames) {
	m_framesPerSec->setText(QString("%1 fps").arg(frames));
}

void MainWindow::updateObjectsCount(int count) {
	m_objectsCount->setText(QString("%1").arg(count));
}

void MainWindow::initialize() {
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

	this->setWindowTitle("TUStudios - DOMINATOR");
}
