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

	m_mbSizeX = new QSpinBox();
	m_mbSizeX->setRange(0, 2000);
	layout->addWidget(m_mbSizeX);

	m_mbSizeY = new QSpinBox();
	m_mbSizeY->setRange(0, 2000);
	layout->addWidget(m_mbSizeY);

	m_mbSizeZ = new QSpinBox();
	m_mbSizeZ->setRange(0, 2000);
	layout->addWidget(m_mbSizeZ);

	layout->addWidget(new QLabel("Location:"));

	m_mbLocX = new QSpinBox();
	m_mbLocX->setRange(-1000, 1000);
	layout->addWidget(m_mbLocX);

	m_mbLocY = new QSpinBox();
	m_mbLocY->setRange(-1000, 1000);
	layout->addWidget(m_mbLocY);

	m_mbLocZ = new QSpinBox();
	m_mbLocZ->setRange(-1000, 1000);
	layout->addWidget(m_mbLocZ);

	layout->addWidget(new QLabel("Rotation:"));

	m_mbRotX = new QSpinBox();
	m_mbRotX->setRange(-360, 360);
	m_mbRotX->setSuffix("°");
	layout->addWidget(m_mbRotX);

	m_mbRotY = new QSpinBox();
	m_mbRotY->setRange(-360, 360);
	m_mbRotY->setSuffix("°");
	layout->addWidget(m_mbRotY);

	m_mbRotZ = new QSpinBox();
	m_mbRotZ->setRange(-360, 360);
	m_mbRotZ->setSuffix("°");
	layout->addWidget(m_mbRotZ);

	layout->addStretch(-1);

	setLayout(layout);
	setMaximumWidth(150);

	updateData(false);

	connect(m_mbSizeX, SIGNAL(valueChanged(int)), this, SLOT(signalSize(int)));
	connect(m_mbSizeY, SIGNAL(valueChanged(int)), this, SLOT(signalSize(int)));
	connect(m_mbSizeZ, SIGNAL(valueChanged(int)), this, SLOT(signalSize(int)));

	connect(m_mbLocX, SIGNAL(valueChanged(int)), this,
			SLOT(signalLocation(int)));
	connect(m_mbLocY, SIGNAL(valueChanged(int)), this,
			SLOT(signalLocation(int)));
	connect(m_mbLocZ, SIGNAL(valueChanged(int)), this,
			SLOT(signalLocation(int)));

	connect(m_mbRotX, SIGNAL(valueChanged(int)), this,
			SLOT(signalRotation(int)));
	connect(m_mbRotY, SIGNAL(valueChanged(int)), this,
			SLOT(signalRotation(int)));
	connect(m_mbRotZ, SIGNAL(valueChanged(int)), this,
			SLOT(signalRotation(int)));

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

void ModifyBox::signalSize(int value)
{
	if (QObject::sender() == m_mbSizeX) {
		emit changeSize('x', value);
	} else if (QObject::sender() == m_mbSizeY) {
		emit changeSize('y', value);
	} else if (QObject::sender() == m_mbSizeZ) {
		emit changeSize('z', value);
	}
}

void ModifyBox::signalLocation(int value)
{
	if (QObject::sender() == m_mbLocX) {
		emit changeLocation('x', value);
	} else if (QObject::sender() == m_mbLocY) {
		emit changeLocation('y', value);
	} else if (QObject::sender() == m_mbLocZ) {
		emit changeLocation('z', value);
	}
}

void ModifyBox::signalRotation(int value)
{
	int delta;
	if (QObject::sender() == m_mbRotX) {
		delta = value - m_rx;
		m_rx = value;
		emit changeRotation('x', delta);
	} else if (QObject::sender() == m_mbRotY) {
		delta = value - m_ry;
		m_ry = value;
		emit changeRotation('y', delta);
	} else if (QObject::sender() == m_mbRotZ) {
		delta = value - m_rz;
		m_rz = value;
		emit changeRotation('z', delta);
	}
}

void ModifyBox::updateData(const m3d::Mat4f* matrix)
{
	if (matrix) {
		std::cout << (int) matrix->_11 << " " << (int) matrix->_22 << " "
				<< (int) matrix->_33 << std::endl;
		m_mbSizeX->setValue((int) matrix->_11 * 10);
		m_mbSizeY->setValue((int) matrix->_22 * 10);
		m_mbSizeZ->setValue((int) matrix->_33 * 10);
		std::cout << (int) matrix->_41 << " " << (int) matrix->_42 << " "
				<< (int) matrix->_43 << std::endl;
		m_mbLocX->setValue((int) matrix->_41 * 10);
		m_mbLocY->setValue((int) matrix->_42 * 10);
		m_mbLocZ->setValue((int) matrix->_43 * 10);
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
