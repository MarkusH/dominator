/**
 * @author Markus Holtermann
 * @date Jun 7, 2011
 * @file gui/dialogs.hpp
 */

#ifndef DIALOGS_HPP_
#define DIALOGS_HPP_

#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>
#include <QtGui/QImage>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QMessageBox>

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

class MessageDialog: public QMessageBox {
Q_OBJECT
public:
	typedef enum {
		INFO = 0, /**< displays an "i". */
		QWARNING, /**< displays an "!" */
		QERROR   /**< displays an "x" */
	} MessageType;
	MessageDialog(QString title, QString message, MessageType type = INFO);
};

}
#endif /* DIALOGS_HPP_ */
