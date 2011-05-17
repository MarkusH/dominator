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

	QSlider *sl_rotate_x;
	QSlider *sl_rotate_y;
	QSlider *sl_rotate_z;

	Render *render_window;
};

#endif /* MAINWINDOW_HPP_ */
