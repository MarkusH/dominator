/**
 * @author Markus Holtermann
 * @date May 12, 2011
 * @file main.cpp
 */

#include <iostream>
#include <QtGui/QApplication>
#include <gui/mainwindow.hpp>
#include <clocale>
#include <util/config.hpp>

int main(int argc, char **argv) {

	// this prevents that the atof functions fails on German systems
	// since they use "," as a separator for floats
	setlocale(LC_ALL,"C");

	using namespace util;
	Config::instance().load("data/config.xml");

	QApplication app(argc, argv);

	gui::MainWindow mainwindow(&app);

	int result = app.exec();

	return result;
}
