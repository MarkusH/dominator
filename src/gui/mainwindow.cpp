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

	this->setWindowTitle("TUStudios - DOMINATOR");

	createMenu();
	app->processEvents();

	createModifyBox();
	app->processEvents();

	createToolBox();
	app->processEvents();

	m_renderWindow = new Render(new QString("data/models/monkey.3ds"), this);
	m_renderWindow->setMinimumWidth(400);
	m_renderTimer= new QTimer(this);
	m_renderTimer->start();
	connect(m_renderTimer, SIGNAL(timeout()), m_renderWindow, SLOT(updateGL()));
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

void MainWindow::createToolBox() {
	m_toolBox = new QWidget();
	QVBoxLayout *layout = new QVBoxLayout();

	layout->addWidget(new QLabel("3D objects:"));

	m_tb3DOjects = new QComboBox();
	layout->addWidget(m_tb3DOjects);

	layout->addWidget(new QLabel("Stones:"));

	m_tbStones = new QComboBox();
	layout->addWidget(m_tbStones);

	layout->addWidget(new QLabel("Templates:"));

	m_tbTemplate = new QComboBox();
	layout->addWidget(m_tbTemplate);

	layout->addWidget(new QLabel("Textures:"));

	m_tbTexture = new QComboBox();
	layout->addWidget(m_tbTexture);

//	layout->addWidget(m_modifyBox);

	m_toolBox->setLayout(layout);
	m_toolBox->setMaximumWidth(250);
}

void MainWindow::createModifyBox() {
	m_modifyBox = new QWidget();
	QGridLayout *layout = new QGridLayout();

	layout->addWidget(new QLabel("Size:"), 0, 0, 1, 3);

	m_mbSizeX = new QSpinBox();
	layout->addWidget(m_mbSizeX, 1, 0);

	m_mbSizeY = new QSpinBox();
	layout->addWidget(m_mbSizeY, 1, 1);

	m_mbSizeZ = new QSpinBox();
	layout->addWidget(m_mbSizeZ, 1, 2);

	layout->addWidget(new QLabel("Location:"), 2, 0, 1, 3);

	m_mbLocX = new QSpinBox();
	layout->addWidget(m_mbLocX, 3, 0);

	m_mbLocY = new QSpinBox();
	layout->addWidget(m_mbLocY, 3, 1);

	m_mbLocZ = new QSpinBox();
	layout->addWidget(m_mbLocZ, 3, 2);

	layout->addWidget(new QLabel("Rotation:"), 4, 0, 1, 3);

	m_mbRotX = new QSpinBox();
	layout->addWidget(m_mbRotX, 5, 0);

	m_mbRotY = new QSpinBox();
	layout->addWidget(m_mbRotY, 5, 1);

	m_mbRotZ = new QSpinBox();
	layout->addWidget(m_mbRotZ, 5, 2);

	m_modifyBox->setLayout(layout);
	m_modifyBox->setMaximumSize(250, 250);
}
