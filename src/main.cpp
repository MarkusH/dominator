/**
 * @author Markus Holtermann
 * @date May 12, 2011
 * @file main.cpp
 */

//#define UNIT_TESTS
#ifdef UNIT_TESTS

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>

int main (int argc, char* argv[])
{
    CPPUNIT_NS::TestResult testresult;

    CPPUNIT_NS::TestResultCollector collectedresults;
    testresult.addListener(&collectedresults);

    CPPUNIT_NS::BriefTestProgressListener progress;
    testresult.addListener(&progress);

    CPPUNIT_NS::TestRunner testrunner;
    testrunner.addTest(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
    testrunner.run (testresult);

    CPPUNIT_NS::CompilerOutputter compileroutputter(&collectedresults, std::cerr);
    compileroutputter.write();

    return collectedresults.wasSuccessful() ? 0 : 1;
}
#else

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

	std::cout << "Totally Unrelated Studios proudly presents:" << std::endl
			  << "\tDOMINATOR" << std::endl << std::endl;

	QApplication app(argc, argv);

	gui::MainWindow mainwindow(&app);

	int result = app.exec();
	//Config::instance().save("data/config.xml");

	return result;
}

#endif
