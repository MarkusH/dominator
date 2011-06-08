/*
 * dialogs.hpp
 *
 *  Created on: Jun 7, 2011
 *      Author: Markus Holtermann
 */

#ifndef DIALOGS_HPP_
#define DIALOGS_HPP_

#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QGridLayout>
#include <QtGui/QImage>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QDialogButtonBox>

/**
 * AboutDialog displays a short about box, naming the authors
 */
class AboutDialog: public QDialog {
Q_OBJECT
public:
	AboutDialog(QWidget* parent = 0);
};

/**
 * the GravityDialog will be used to set the environment gravity in the simulation
 */
class GravityDialog: public QDialog {
Q_OBJECT
public:
	GravityDialog(const float gravity, QWidget* parent = 0);
	float run();

private:
	QGridLayout* m_layout;
	QDoubleSpinBox* m_gravity;
	QDialogButtonBox* m_buttons;

	float value;
};

#endif /* DIALOGS_HPP_ */
