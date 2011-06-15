/*
 * main.cpp
 *
 *      Author: Markus Holtermann
 */
#include <iostream>
#include <QtGui/QApplication>
#include <gui/mainwindow.hpp>

int main(int argc, char **argv) {
	std::cout << "Totally Unrelated Studios proudly presents:" << std::endl
			  << "\tDOMINATOR" << std::endl << std::endl;

	QApplication app(argc, argv);

	gui::MainWindow mainwindow(&app);

	return app.exec();
}
