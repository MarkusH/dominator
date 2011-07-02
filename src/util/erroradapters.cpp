/*
 * util/erroradapter.cpp
 *
 *  Created on: Jun 30, 2011
 *      Author: Robert Waury, Markus Holtermann
 */

#include <util/erroradapters.hpp>
#include <gui/dialogs.hpp>
#include <iostream>

namespace util {

ErrorListener::ErrorListener() {
}

ErrorListener::~ErrorListener() {
}

void ErrorListener::displayError(const std::string& message) {
	std::cout << message << std::endl;
}

ErrorAdapter* ErrorAdapter::s_instance = NULL;

//ErrorAdapter::ErrorAdapter() {}
//ErrorAdapter::~ErrorAdapter() {}

void ErrorAdapter::createInstance() {
	destroyInstance();
	s_instance = new ErrorAdapter();
	s_instance->addListener(new ErrorListener());
}

void ErrorAdapter::destroyInstance() {
	if (s_instance)
		delete s_instance;
	s_instance = NULL;
}

void ErrorAdapter::displayErrorMessage(const std::string& function,
		const std::vector<std::string>& args) {
	std::string msg = function;
	msg += " with args ...";
	for (std::list<ErrorListener*>::iterator itr = m_listeners.begin();
			itr != m_listeners.end(); ++itr) {
		(*itr)->displayError(msg);
	}
}

void ErrorAdapter::displayErrorMessage(const std::string& function,
		const std::vector<std::string>& args, rapidxml::parse_error& e) {
	std::string msg = "Parse Exception: '";
	msg += e.what();
	msg += "' caught in '";
	msg += e.where<char>();
	msg += "'";
	for (std::list<ErrorListener*>::iterator itr = m_listeners.begin();
			itr != m_listeners.end(); ++itr) {
		(*itr)->displayError(msg);
	}
}

void ErrorAdapter::displayErrorMessage(const std::string& function,
		const std::vector<std::string>& args, std::runtime_error& e) {
	std::string msg = "Runtime Exception was caught when loading file ";
	msg += function;
	msg += " '";
	msg += e.what();
	msg += "'";
	for (std::list<ErrorListener*>::iterator itr = m_listeners.begin();
			itr != m_listeners.end(); ++itr) {
		(*itr)->displayError(msg);
	}
}

}
