/*
 * modifybox.cpp
 *
 *      Author: Markus Holtermann
 */

#include <gui/modifybox.hpp>
#include <iostream>

ModifyBox::ModifyBox(QWidget *parent) :
	QWidget(parent)
{

	m_rx = m_ry = m_rz = 0;

	QVBoxLayout *layout = new QVBoxLayout();

	layout->addWidget(new QLabel("Size:"));

	m_mbSizeX = new QDoubleSpinBox();
	m_mbSizeX->setDecimals(precision);
	m_mbSizeX->setRange(0, 200);
	m_mbSizeX->setSingleStep(0.1);
	layout->addWidget(m_mbSizeX);

	m_mbSizeY = new QDoubleSpinBox();
	m_mbSizeY->setDecimals(precision);
	m_mbSizeY->setRange(0, 200);
	m_mbSizeY->setSingleStep(0.1);
	layout->addWidget(m_mbSizeY);

	m_mbSizeZ = new QDoubleSpinBox();
	m_mbSizeZ->setDecimals(precision);
	m_mbSizeZ->setRange(0, 200);
	m_mbSizeZ->setSingleStep(0.1);
	layout->addWidget(m_mbSizeZ);

	layout->addWidget(new QLabel("Location:"));

	m_mbLocX = new QDoubleSpinBox();
	m_mbLocX->setDecimals(precision);
	m_mbLocX->setRange(-100, 100);
	m_mbLocX->setSingleStep(0.1);
	layout->addWidget(m_mbLocX);

	m_mbLocY = new QDoubleSpinBox();
	m_mbLocY->setDecimals(precision);
	m_mbLocY->setRange(-1000, 1000);
	m_mbLocY->setSingleStep(0.1);
	layout->addWidget(m_mbLocY);

	m_mbLocZ = new QDoubleSpinBox();
	m_mbLocZ->setDecimals(precision);
	m_mbLocZ->setRange(-1000, 1000);
	m_mbLocZ->setSingleStep(0.1);
	layout->addWidget(m_mbLocZ);

	layout->addWidget(new QLabel("Rotation:"));

	m_mbRotX = new QDoubleSpinBox();
	m_mbRotX->setDecimals(precision);
	m_mbRotX->setRange(-360, 360);
	m_mbRotX->setSuffix("°");
	m_mbRotX->setSingleStep(0.1);
	m_mbRotX->setWrapping(true);
	layout->addWidget(m_mbRotX);

	m_mbRotY = new QDoubleSpinBox();
	m_mbRotY->setDecimals(precision);
	m_mbRotY->setRange(-360, 360);
	m_mbRotY->setSuffix("°");
	m_mbRotY->setSingleStep(0.1);
	m_mbRotY->setWrapping(true);
	layout->addWidget(m_mbRotY);

	m_mbRotZ = new QDoubleSpinBox();
	m_mbRotZ->setDecimals(precision);
	m_mbRotZ->setRange(-360, 360);
	m_mbRotZ->setSuffix("°");
	m_mbRotZ->setSingleStep(0.1);
	m_mbRotZ->setWrapping(true);
	layout->addWidget(m_mbRotZ);

	layout->addStretch(-1);

	setLayout(layout);
	setMaximumWidth(150);

	updateData(false);

	connect(m_mbSizeX, SIGNAL(valueChanged(double)), this, SLOT(signalSize(double)));
	connect(m_mbSizeY, SIGNAL(valueChanged(double)), this, SLOT(signalSize(double)));
	connect(m_mbSizeZ, SIGNAL(valueChanged(double)), this, SLOT(signalSize(double)));

	connect(m_mbLocX, SIGNAL(valueChanged(double)), this, SLOT(signalLocation(double)));
	connect(m_mbLocY, SIGNAL(valueChanged(double)), this, SLOT(signalLocation(double)));
	connect(m_mbLocZ, SIGNAL(valueChanged(double)), this, SLOT(signalLocation(double)));

	connect(m_mbRotX, SIGNAL(valueChanged(double)), this, SLOT(signalRotation(double)));
	connect(m_mbRotY, SIGNAL(valueChanged(double)), this, SLOT(signalRotation(double)));
	connect(m_mbRotZ, SIGNAL(valueChanged(double)), this, SLOT(signalRotation(double)));

}

void ModifyBox::mouseMoveEvent(QMouseEvent * event)
{
	move(mapToParent(event->pos()) - m_moveOffset);
}

void ModifyBox::mousePressEvent(QMouseEvent * event)
{
	m_moveOffset = event->pos();
}

void ModifyBox::mouseReleaseEvent(QMouseEvent * event)
{
	m_moveOffset = QPoint();
}

void ModifyBox::signalSize(double value)
{
	if (QObject::sender() == m_mbSizeX) {
		emit changeSize('x', (float) value);
	} else if (QObject::sender() == m_mbSizeY) {
		emit changeSize('y', (float) value);
	} else if (QObject::sender() == m_mbSizeZ) {
		emit changeSize('z', (float) value);
	}
}

void ModifyBox::signalLocation(double value)
{
	if (QObject::sender() == m_mbLocX) {
		emit changeLocation('x', (float) value);
	} else if (QObject::sender() == m_mbLocY) {
		emit changeLocation('y', (float) value);
	} else if (QObject::sender() == m_mbLocZ) {
		emit changeLocation('z', (float) value);
	}
}

void ModifyBox::signalRotation(double value)
{
	float deltaX, deltaY, deltaZ;
	deltaX = (float)m_mbRotX->value() - m_rx;
	deltaY = (float)m_mbRotY->value() - m_ry;
	deltaZ = (float)m_mbRotZ->value() - m_rz;
	m_rx = (float)m_mbRotX->value();
	m_ry = (float)m_mbRotY->value();
	m_rz = (float)m_mbRotZ->value();
	emit changeRotation(deltaX, deltaY, deltaZ);
}

void ModifyBox::updateData(const m3d::Mat4f* matrix)
{
	// TODO: this seems to trigger the signals of the spinners
	// we may need to set a boolean in this function that temporarily
	// disables the handling code in the slots above.
	if (matrix) {
		std::cout << matrix->_11 << " " << matrix->_22 << " "
				<< matrix->_33 << std::endl;
		m_mbSizeX->setValue(matrix->getX().len());
		m_mbSizeY->setValue(matrix->getY().len());
		m_mbSizeZ->setValue(matrix->getZ().len());
		std::cout << matrix->_41 << " " << matrix->_42 << " "
				<< matrix->_43 << std::endl;
		m_mbLocX->setValue(matrix->_41);
		m_mbLocY->setValue(matrix->_42);
		m_mbLocZ->setValue(matrix->_43);
		std::cout << matrix->_41 << " " << matrix->_42 << " "
				<< matrix->_43 << std::endl;
		m_mbRotX->setValue(matrix->eulerAngles().x);
		m_mbRotY->setValue(matrix->eulerAngles().y);
		m_mbRotZ->setValue(matrix->eulerAngles().z);
	}
}

void ModifyBox::updateData(bool selected)
{
	m_mbSizeX->setEnabled(selected);
	m_mbSizeY->setEnabled(selected);
	m_mbSizeZ->setEnabled(selected);

	m_mbLocX->setEnabled(selected);
	m_mbLocY->setEnabled(selected);
	m_mbLocZ->setEnabled(selected);

	m_mbRotX->setEnabled(selected);
	m_mbRotY->setEnabled(selected);
	m_mbRotZ->setEnabled(selected);
}
