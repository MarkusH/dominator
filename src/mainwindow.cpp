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
#include <QtCore/QString>
#include <QtGui/QSlider>

MainWindow::MainWindow(QApplication *app, QWidget *parent) :
	QWidget(parent) {

	QPixmap pixmap("data/splash.png");
	QSplashScreen splash(pixmap);
	splash.show();
	app->processEvents();

	bt_close = new QPushButton("Close", this);
	bt_close->setGeometry(10, 10, 120, 30);
	connect(bt_close, SIGNAL(clicked()), this, SLOT(OnClosePressed()));

	bt_render_window = new QPushButton("Render Window", this);
	bt_render_window->setGeometry(140, 10, 120, 30);
	connect(bt_render_window, SIGNAL(clicked()), this,
			SLOT(OnRenderWindowPressed()));

	sl_rotate_x = new QSlider(Qt::Horizontal, this);
	sl_rotate_x->setGeometry(10, 50, 250, 30);
	sl_rotate_x->setEnabled(false);
	sl_rotate_x->setRange(-360, 360);

	sl_rotate_y = new QSlider(Qt::Horizontal, this);
	sl_rotate_y->setGeometry(10, 90, 250, 30);
	sl_rotate_y->setEnabled(false);
	sl_rotate_y->setRange(-360, 360);

	sl_rotate_z = new QSlider(Qt::Horizontal, this);
	sl_rotate_z->setGeometry(10, 130, 250, 30);
	sl_rotate_z->setEnabled(false);
	sl_rotate_z->setRange(-360, 360);

	this->resize(270, 430);
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
	render_window = new Render(new QString("data/models/monkey.3ds"), this);
	render_window->setGeometry(10, 170, 250, 250);
	render_window->show();
	connect(sl_rotate_x, SIGNAL(valueChanged(int)), render_window,
			SLOT(setRotationX(int)));
	connect(sl_rotate_y, SIGNAL(valueChanged(int)), render_window,
			SLOT(setRotationY(int)));
	connect(sl_rotate_z, SIGNAL(valueChanged(int)), render_window,
			SLOT(setRotationZ(int)));
	sl_rotate_x->setEnabled(true);
	sl_rotate_y->setEnabled(true);
	sl_rotate_z->setEnabled(true);
}
