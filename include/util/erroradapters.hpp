#ifndef ERRORADAPTERS_HPP_
#define ERRORADAPTERS_HPP_

#include <vector>
#include <string>
#include <stdexception>
#include <xml/rapidxml.hpp>


namespace util {

	class ErrorAdapter {
		public;
			ErrorApdapter();
			~ErrorAdapter();

			void displayErrorMessage(std::string& function, std::vector<std::string>& args);
			void displayErrorMessage(std::string& function, std::vector<std::string>& args, rapidxml::parse_error& e);
			void displayErrorMessage(std::string& function, std::vector<std::string>& args, std::runtime_error& e);
		
	}

	class QtErrorAdapter : public ErrorAdapter {
		public:
			QtErrorApdapter();
			~QtErrorAdapter();

			void displayQtMessageBox();
	}

}

#endif /* END ERRORADAPTERS_HPP_ */