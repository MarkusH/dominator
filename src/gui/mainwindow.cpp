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

	m_modifyBox = new ModifyBox();
	app->processEvents();

	m_toolBox = new ToolBox();
	m_toolBox->addWidget(m_modifyBox);
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
