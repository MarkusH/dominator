/*
 * dialogs.cpp
 *
 *  Created on: Jun 7, 2011
 *      Author: Markus Holtermann
 */

#include <gui/dialogs.hpp>

/**
 *
 * @param parent
 */
AboutDialog::AboutDialog(QWidget* parent) :
	QDialog(parent)
{
	QGridLayout* layout = new QGridLayout();
	QPixmap pixmap = QPixmap("data/splash.png");
	QLabel* icon = new QLabel();
	QString text;

	layout->setSizeConstraint(QLayout::SetFixedSize);
	layout->setVerticalSpacing(0);
	layout->setHorizontalSpacing(0);

	pixmap = pixmap.scaledToHeight(100, Qt::SmoothTransformation);
	icon->setPixmap(pixmap);
	layout->addWidget(icon, 0, 0, 2, 1, Qt::AlignTop);

	text = QString("\"DOMINATOR\" is a student's project at the\nDHBW Mannheim, Corporate State University.\n\n");
	layout->addWidget(new QLabel(text), 0, 1);

	text = QString("Authors:\n");
	text.append(" * Markus Döllinger\n");
	text.append(" * Robert Genz\n");
	text.append(" * Raffaela Heller\n");
	text.append(" * Markus Holtermann\n");
	text.append(" * Timo Hübner\n");
	text.append(" * Alexander Tenberge\n");
	text.append(" * Robert Waury\n");
	layout->addWidget(new QLabel(text), 1, 1);

	setWindowTitle("TUStudios - About");
	setLayout(layout);
}

/**
 *
 * @param parent
 */
GravityDialog::GravityDialog(const float gravity, QWidget* parent) :
	QDialog(parent)
{
	m_layout = new QGridLayout();
	m_gravity = new QDoubleSpinBox();
	m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	value = gravity;

	m_layout->setSizeConstraint(QLayout::SetFixedSize);
	m_layout->setVerticalSpacing(0);
	m_layout->setHorizontalSpacing(0);

	m_gravity->setRange(-10.0f, 10.0f);
	m_gravity->setValue(value * -0.25f);
	m_layout->addWidget(m_gravity, 0, 0);

	m_layout->addWidget(m_buttons, 1, 0);
	connect(m_buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(m_buttons, SIGNAL(rejected()), this, SLOT(reject()));

	setWindowTitle("TUStudios - Simulation Gravity");
	setLayout(m_layout);
}

float GravityDialog::run()
{
	if (exec() == QDialog::Accepted) {
		return m_gravity->value() * -4.0f;
	} else {
		return value;
	}
}
