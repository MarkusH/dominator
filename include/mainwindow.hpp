/*
 * mainwindow.hpp
 *
 *      Author: Markus Holtermann
 */

#ifndef MAINWINDOW_HPP_
#define MAINWINDOW_HPP_

#include <QtGui/QApplication>
#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSlider>
#include <QtCore/QTimer>

class Render;

class MainWindow: public QWidget {
Q_OBJECT
public:
	MainWindow(QApplication *app, QWidget *parent = 0);

private slots:
	void OnClosePressed();
	void OnRenderWindowPressed();

private:
	QPushButton *bt_close;
	QPushButton *bt_render_window;

	QPushButton *bt_rotate_x_plus, *bt_rotate_x_minus;
	QPushButton *bt_rotate_y_plus, *bt_rotate_y_minus;
	QPushButton *bt_rotate_z_plus, *bt_rotate_z_minus;

	QTimer *timer_render;

	Render *render_window;
};

#endif /* MAINWINDOW_HPP_ */
