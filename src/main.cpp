/**
 * @author Markus Holtermann
 * @date May 12, 2011
 * @file main.cpp
 */

#include <iostream>
#include <QtGui/QApplication>
#include <gui/mainwindow.hpp>
#include <clocale>

int main(int argc, char **argv) {

	// this prevents that the atof functions fails on German systems
	// since they use "," as a separator for floats
	setlocale(LC_ALL,"C");

	std::cout << "Totally Unrelated Studios proudly presents:" << std::endl
			  << "\tDOMINATOR" << std::endl << std::endl;

	QApplication app(argc, argv);

	gui::MainWindow mainwindow(&app);

	return app.exec();
}
