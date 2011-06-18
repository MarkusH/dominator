/**
 * @author Markus Holtermann
 * @date May 25, 2011
 * @file gui/toolbox.cpp
 */

#include <gui/toolbox.hpp>
#include <QtGui/QSizePolicy>
#include <iostream>
#include <simulation/material.hpp>
#include <simulation/object.hpp>
#include <set>

namespace gui {

ToolBox::ToolBox(QWidget *parent)
{
	QMenu* menu;

	m_selectedInteraction = sim::Simulation::INT_NONE;
	m_objectMenu = new QMenu();

	menu = new QMenu("Domino");
	menu->addAction(new QObjectAction("Small", sim::__Object::DOMINO_SMALL));
	menu->addAction(new QObjectAction("Middle", sim::__Object::DOMINO_MIDDLE));
	menu->addAction(new QObjectAction("Large", sim::__Object::DOMINO_LARGE));
	m_objectMenu->addMenu(menu);

	menu = new QMenu("Primitives");
	menu->addAction(new QObjectAction("Box", sim::__Object::BOX));
	menu->addAction(new QObjectAction("Capsule", sim::__Object::CAPSULE));
	menu->addAction(new QObjectAction("Chamfer Cylinder", sim::__Object::CHAMFER_CYLINER));
	menu->addAction(new QObjectAction("Cone", sim::__Object::CONE));
	menu->addAction(new QObjectAction("Cylinder", sim::__Object::CYLINDER));
	menu->addAction(new QObjectAction("Sphere", sim::__Object::SPHERE));
	m_objectMenu->addMenu(menu);

	menu = new QMenu("Templates");
	m_objectMenu->addMenu(menu);

	m_lbMaterials = new QLabel("Material:");
	m_lbObjects = new QLabel("Object:");

	m_materials = new QComboBox();

	m_objects = new QPushButton("Add an object");
	m_objects->setMenu(m_objectMenu);
	connect(m_objectMenu, SIGNAL(triggered(QAction*)), this, SLOT(addObject(QAction*)));

	setMaximumWidth(250);
	layout = new QVBoxLayout();

	layout->addWidget(m_lbObjects);
	layout->addWidget(m_objects);
	layout->addWidget(m_lbMaterials);
	layout->addWidget(m_materials);

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

void ToolBox::loadMaterials(QString filename)
{
	sim::MaterialMgr::instance().load(filename.toStdString().c_str());
	std::set<std::string> materials;
	sim::MaterialMgr::instance().getMaterials(materials);
	for (std::set<std::string>::iterator itr = materials.begin(); itr != materials.end(); itr++) {
		m_materials->addItem(QString::fromStdString(*itr), QString::fromStdString(*itr));
	}
	connect(m_materials, SIGNAL(currentIndexChanged(int)), this, SLOT(materialSelected(int)));
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
		emit
		interactionSelected(m_selectedInteraction);
		return;
	}
	m_selectedInteraction = (sim::Simulation::InteractionType) m_mouseinteraction->checkedId();
	emit interactionSelected(m_selectedInteraction);
}

void ToolBox::addObject(QAction* action)
{
	sim::Simulation::instance().setNewObjectType(((QObjectAction*) action)->getType());
	m_objects->setText(action->text());
}

void ToolBox::materialSelected(int index)
{
	sim::Simulation::instance().setNewObjectMaterial(m_materials->itemText(index).toStdString());
}

}
