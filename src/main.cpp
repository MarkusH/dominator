/*
 * main.cpp
 *
 *      Author: Markus Holtermann
 */
#include "mainwindow.hpp"

#include <iostream>
#include <QtGui/QApplication>

int main(int argc, char **argv) {
	std::cout << "Totally Unrelated Studios proudly presents\n\tDOMINATOR\n\n";

	QApplication app(argc, argv);

	MainWindow mainwindow(&app);

	return app.exec();
}
