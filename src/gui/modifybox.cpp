/*
 * modifybox.cpp
 *
 *      Author: Markus Holtermann
 */

#include <gui/modifybox.hpp>

ModifyBox::ModifyBox(QWidget *parent) {
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
	layout->addWidget(m_mbRotX);

	m_mbRotY = new QSpinBox();
	layout->addWidget(m_mbRotY);

	m_mbRotZ = new QSpinBox();
	layout->addWidget(m_mbRotZ);

	layout->addStretch(-1);

	setLayout(layout);
	setMaximumWidth(150);
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
