/**
 * @author Markus Holtermann
 * @date May 12, 2011
 * @file main.cpp
 */

#include <iostream>
#include <QtGui/QApplication>
#include <gui/mainwindow.hpp>
#include <sound/SoundMgr.hpp>//test
int main(int argc, char **argv) {
	std::cout << "Totally Unrelated Studios proudly presents:" << std::endl
			  << "\tDOMINATOR" << std::endl << std::endl;

	QApplication app(argc, argv);

	gui::MainWindow mainwindow(&app);

	return app.exec();
}
