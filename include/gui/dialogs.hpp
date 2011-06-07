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

class AboutDialog: public QDialog {
Q_OBJECT
public:
	AboutDialog(QWidget* parent = 0);
};

#endif /* DIALOGS_HPP_ */
