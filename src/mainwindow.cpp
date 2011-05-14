/*
 * mainwindow.cpp
 *
 *      Author: Markus Holtermann
 */

#include <iostream>

#include "mainwindow.hpp"
#include "render/render.hpp"
#include <QtGui/QPixmap>
#include <QtGui/QSplashScreen>
#include <QtGui/QMessageBox>

MainWindow::MainWindow(QApplication *app, QWidget *parent) :
	QWidget(parent) {

	QPixmap pixmap("data/splash.png");
	QSplashScreen splash(pixmap);
	splash.show();
	app->processEvents();

	bt_close = new QPushButton("Close", this);
	bt_close->move(10, 10);
	connect(bt_close, SIGNAL(clicked()), this, SLOT(OnClosePressed()));

	bt_render_window = new QPushButton("Render Window", this);
	bt_render_window->move(10, 50);
	connect(bt_render_window, SIGNAL(clicked()), this,
			SLOT(OnRenderWindowPressed()));

	this->resize(250, 150);
	this->setWindowTitle("TUStudios - DOMINATOR");
	this->show();

	splash.finish(this);
}

void MainWindow::OnClosePressed() {
	QMessageBox msgBox;
	msgBox.setText("Closing the main window");
	msgBox.setInformativeText("Do you want to exit?");
	msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox.setDefaultButton(QMessageBox::No);
	int ret = msgBox.exec();
	if (ret == QMessageBox::Yes) {
		this->close();
	}
}

void MainWindow::OnRenderWindowPressed() {
	Render * render = new Render();
	render->show();
}
