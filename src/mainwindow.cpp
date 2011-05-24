/*
 * mainwindow.cpp
 *
 *      Author: Markus Holtermann
 */

#include <iostream>

#include <mainwindow.hpp>
#include <render/render.hpp>
#include <QtGui/QPixmap>
#include <QtGui/QSplashScreen>
#include <QtGui/QMessageBox>
#include <QtCore/QString>
#include <QtGui/QSlider>
#include <QtCore/QTimer>

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

	bt_rotate_x_plus = new QPushButton("+", this);
	bt_rotate_x_plus->setGeometry(10, 50, 30, 30);

	bt_rotate_x_minus = new QPushButton("-", this);
	bt_rotate_x_minus->setGeometry(10, 90, 30, 30);

	bt_rotate_y_plus = new QPushButton("+", this);
	bt_rotate_y_plus->setGeometry(60, 50, 30, 30);

	bt_rotate_y_minus = new QPushButton("-", this);
	bt_rotate_y_minus->setGeometry(60, 90, 30, 30);

	bt_rotate_z_plus = new QPushButton("+", this);
	bt_rotate_z_plus->setGeometry(110, 50, 30, 30);

	bt_rotate_z_minus = new QPushButton("-", this);
	bt_rotate_z_minus->setGeometry(110, 90, 30, 30);

	render_window = new Render(new QString("data/models/monkey.3ds"), this);
	render_window->setGeometry(10, 170, 250, 250);
	render_window->show();
	connect(bt_rotate_x_plus, SIGNAL(clicked()), render_window,
			SLOT(setRotationXInc()));
	connect(bt_rotate_x_minus, SIGNAL(clicked()), render_window,
			SLOT(setRotationXDec()));
	connect(bt_rotate_y_plus, SIGNAL(clicked()), render_window,
			SLOT(setRotationYInc()));
	connect(bt_rotate_y_minus, SIGNAL(clicked()), render_window,
			SLOT(setRotationYDec()));
	connect(bt_rotate_z_plus, SIGNAL(clicked()), render_window,
			SLOT(setRotationZInc()));
	connect(bt_rotate_z_minus, SIGNAL(clicked()), render_window,
			SLOT(setRotationZDec()));

	timer_render = new QTimer(this);
	timer_render->start();
	connect(timer_render, SIGNAL(timeout()), render_window, SLOT(updateGL()));

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
	if (render_window->isVisible()) {
		bt_rotate_x_plus->setEnabled(false);
		bt_rotate_x_minus->setEnabled(false);
		bt_rotate_y_plus->setEnabled(false);
		bt_rotate_y_minus->setEnabled(false);
		bt_rotate_z_plus->setEnabled(false);
		bt_rotate_z_minus->setEnabled(false);
		render_window->hide();
		timer_render->stop();
		this->update();
	} else {
		timer_render->start();
		render_window->show();
		bt_rotate_x_plus->setEnabled(true);
		bt_rotate_x_minus->setEnabled(true);
		bt_rotate_y_plus->setEnabled(true);
		bt_rotate_y_minus->setEnabled(true);
		bt_rotate_z_plus->setEnabled(true);
		bt_rotate_z_minus->setEnabled(true);
	}
}
