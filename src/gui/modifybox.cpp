/*
 * modifybox.cpp
 *
 *      Author: Markus Holtermann
 */

#include <gui/modifybox.hpp>

ModifyBox::ModifyBox(QWidget *parent) : QWidget(parent) {

	m_rx = m_ry = m_rz = 0;

	QVBoxLayout *layout = new QVBoxLayout();

	layout->addWidget(new QLabel("Size:"));

	m_mbSizeX = new QSpinBox();
	layout->addWidget(m_mbSizeX);

	m_mbSizeY = new QSpinBox();
	layout->addWidget(m_mbSizeY);

	m_mbSizeZ = new QSpinBox();
	layout->addWidget(m_mbSizeZ);

	layout->addWidget(new QLabel("Location:"));

	m_mbLocX = new QSpinBox();
	layout->addWidget(m_mbLocX);

	m_mbLocY = new QSpinBox();
	layout->addWidget(m_mbLocY);

	m_mbLocZ = new QSpinBox();
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

	connect(m_mbRotX, SIGNAL(valueChanged(int)), this, SLOT(signalRotation(int)));
	connect(m_mbRotY, SIGNAL(valueChanged(int)), this, SLOT(signalRotation(int)));
	connect(m_mbRotZ, SIGNAL(valueChanged(int)), this, SLOT(signalRotation(int)));

}

void ModifyBox::mouseMoveEvent(QMouseEvent * event) {
	move(mapToParent(event->pos()) - m_moveOffset);
}

void ModifyBox::mousePressEvent(QMouseEvent * event) {
	m_moveOffset = event->pos();
}

void ModifyBox::mouseReleaseEvent(QMouseEvent * event) {
	m_moveOffset = QPoint();
}

void ModifyBox::signalRotation(int value) {
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
