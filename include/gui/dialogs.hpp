/**
 * @author Markus Holtermann
 * @date Jun 7, 2011
 * @file gui/dialogs.hpp
 */

#ifndef DIALOGS_HPP_
#define DIALOGS_HPP_

#include <util/erroradapters.hpp>
#include <QtGui/QDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QWidget>

class QDialogButtonBox;
class QDoubleSpinBox;
class QGridLayout;
class QImage;
class QLabel;
class QListWidget;
class QPushButton;
class QStackedWidget;
class QListWidget;
class QListWidgetItem;
class QStackedWidget;

namespace gui {

/**
 * AboutDialog displays a short about box, naming the authors
 */
class AboutDialog: public QDialog {
Q_OBJECT
public:
	/**
	 * Constructor
	 *
	 * @param parent 	if given, the instance will be set modal in respect
	 * to the parent widget
	 */
	AboutDialog(QWidget* parent = 0);
};

/**
 * the GravityDialog will be used to set the environment gravity in the
 * sim::Simulation
 */
class GravityDialog: public QDialog {
Q_OBJECT
public:
	/**
	 * Constructor
	 *
	 * @param gravity 	the initial gravity value in the range from -100 to 0.
	 * 					If the dialog is canceled, the return value will be
	 * 					that given gravity.
	 * @param parent 	if given, the instance will be set modal in respect to
	 * 					the parent widget
	 */
	GravityDialog(const float gravity, QWidget* parent = 0);
	/**
	 * Call this method to get the new gravity. GravityDialog will handle the
	 * actions `Accept` and `Reject` itself.
	 *
	 * @return returns the new gravity
	 */
	float run();

private:
	/**
	 * GravityDialog::m_gravity is a QDoubleSpinBox for the gravity selection
	 */
	QDoubleSpinBox* m_gravity;

	/**
	 * Local variable that holds the given gravity.
	 */
	float m_value;
	/**
	 * GravityDialog::m_rangeLow defines the internal representation for the
	 * internal minimum gravity
	 */
	static const float m_rangeLow;
	/**
	 * GravityDialog::m_rangeHigh defines the internal representation for the
	 * internal maximum gravity
	 */
	static const float m_rangeHigh;
};

class QtErrorListerner: public util::ErrorListener {
public:
	void displayError(const std::string& message);
};

class MessageDialog: public QMessageBox {
Q_OBJECT
public:
	typedef enum {
		QINFO = 0, /**< displays an "i". */
		QWARNING, /**< displays an "!" */
		QERROR
	/**< displays an "x" */
	} MessageType;
	MessageDialog(const std::string title, const std::string message, const MessageType type = QINFO);
};

class ConfigDialog: public QDialog {
Q_OBJECT
public:
	ConfigDialog(QWidget* parent = 0);
public slots:
	void changePage(QListWidgetItem* current, QListWidgetItem* previous);

private:
	void createPages();
	QListWidget* contentsWidget;
	QStackedWidget* pagesWidget;
};

class ConfigurationPage: public QWidget {
public:
	ConfigurationPage(QWidget *parent = 0);
protected:
	QGridLayout* m_layout;
};

class SettingsPage: public ConfigurationPage {
public:
	SettingsPage(QWidget *parent = 0);
};

class DataPage: public ConfigurationPage {
public:
	DataPage(QWidget *parent = 0);
};

}
#endif /* DIALOGS_HPP_ */
