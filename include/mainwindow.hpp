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

};

#endif /* MAINWINDOW_HPP_ */
