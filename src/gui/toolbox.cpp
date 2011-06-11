/**
 * @author Markus Holtermann
 * @date May 25, 2011
 * @file gui/toolbox.cpp
 */

#include <gui/toolbox.hpp>
#include <QtGui/QSizePolicy>
#include <iostream>

ToolBox::ToolBox(QWidget *parent)
{
	m_selectedInteraction = sim::Simulation::INT_NONE;

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

	// mouse interaction buttons
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	m_mouseinteraction = new QButtonGroup();

	m_moveH = new QPushButton("Move &Ground");
	m_moveH->setCheckable(true);
	m_mouseinteraction->addButton(m_moveH, (int) sim::Simulation::INT_MOVE_GROUND);
	buttonLayout->addWidget(m_moveH);

	m_moveV = new QPushButton("Move Billboard");
	m_moveV->setCheckable(true);
	m_mouseinteraction->addButton(m_moveV, (int) sim::Simulation::INT_MOVE_BILLBOARD);
	buttonLayout->addWidget(m_moveV);

	m_rotate = new QPushButton("Rotate");
	m_rotate->setCheckable(true);
	m_mouseinteraction->addButton(m_rotate, (int) sim::Simulation::INT_ROTATE);
	buttonLayout->addWidget(m_rotate);

	m_mouseinteraction->setParent(this);
	connect(m_mouseinteraction, SIGNAL(buttonClicked(int)), this, SLOT(onInteractionPressed(int)));
	layout->addLayout(buttonLayout);

	layout->addStretch(-1);

	setLayout(layout);
}

void ToolBox::addWidget(QWidget* widget, int stretch, Qt::Alignment alignment)
{
	layout->addWidget(widget, stretch, alignment);
	setLayout(layout);
}

void ToolBox::onInteractionPressed(int button)
{
	if (m_mouseinteraction->checkedId() == m_selectedInteraction) {
		m_mouseinteraction->setExclusive(false);
		m_mouseinteraction->checkedButton()->setChecked(false);
		m_mouseinteraction->setExclusive(true);
		m_selectedInteraction = sim::Simulation::INT_NONE;
		emit interactionSelected(m_selectedInteraction);
		return;
	}
	m_selectedInteraction = (sim::Simulation::InteractionType) m_mouseinteraction->checkedId();
	emit interactionSelected(m_selectedInteraction);
}
