/*
 * toolbox.cpp
 *
 *      Author: Markus Holtermann
 */

#include <gui/toolbox.hpp>
#include <QtGui/QSizePolicy>

ToolBox::ToolBox(QWidget *parent) {
	setMaximumWidth(250);
	layout = new QVBoxLayout();

	layout->addWidget(new QLabel("3D objects:"));

	m_tb3DOjects = new QComboBox();
	layout->addWidget(m_tb3DOjects);

	layout->addWidget(new QLabel("Stones:"));

	m_tbStones = new QComboBox();
	layout->addWidget(m_tbStones);

	layout->addWidget(new QLabel("Templates:"));

	m_tbTemplate = new QComboBox();
	layout->addWidget(m_tbTemplate);

	layout->addWidget(new QLabel("Textures:"));

	m_tbTexture = new QComboBox();
	layout->addWidget(m_tbTexture);

	layout->addStretch(-1);

	setLayout(layout);
}

void ToolBox::addWidget(QWidget * widget, int stretch, Qt::Alignment alignment) {
	layout->addWidget(widget, stretch, alignment);
	setLayout(layout);
}
