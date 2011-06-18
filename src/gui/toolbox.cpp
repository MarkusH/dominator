/**
 * @author Markus Holtermann
 * @date May 25, 2011
 * @file gui/toolbox.cpp
 */

#include <gui/toolbox.hpp>
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QSizePolicy>
#include <simulation/material.hpp>
#include <simulation/object.hpp>
#include <iostream>
#include <set>

namespace gui {

ToolBox::ToolBox(QWidget *parent)
{
	QMenu* m_objectMenu = new QMenu();
	m_selectedInteraction = sim::Simulation::INT_NONE;

	QMenu* menu = new QMenu("Domino");
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

	m_objects = new QPushButton("Add an object");
	m_objects->setMenu(m_objectMenu);
	connect(m_objectMenu, SIGNAL(triggered(QAction*)), this, SLOT(addObject(QAction*)));

	/* just create the materials combobox. we connect the slot after
	 * loading the content to prevent crashes due to changed selection list
	 */
	m_materials = new QComboBox();

	m_freezeState = new QCheckBox("Freeze");
	connect(m_freezeState, SIGNAL(stateChanged(int)), this, SLOT(freezeStateChanged(int)));

	m_mass = new QDoubleSpinBox();
	m_mass->setDecimals(3);
	m_mass->setRange(0, 1000);
	m_mass->setSingleStep(1);
	connect(m_mass, SIGNAL(valueChanged(double)), this, SLOT(massChanged(double)));

	setMaximumWidth(250);
	layout = new QVBoxLayout();

	layout->addWidget(new QLabel("Object:"));
	layout->addWidget(m_objects);
	layout->addWidget(new QLabel("Material:"));
	layout->addWidget(m_materials);
	layout->addWidget(new QLabel("Freeze State:"));
	layout->addWidget(m_freezeState);
	layout->addWidget(new QLabel("Mass:"));
	layout->addWidget(m_mass);

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

void ToolBox::freezeStateChanged(int state)
{
	std::cout << state << std::endl;
	sim::Simulation::instance().setNewObjectFreezeState((state == Qt::Checked) ? true: false);
}

void ToolBox::massChanged(double mass)
{
	std::cout << mass << std::endl;
	sim::Simulation::instance().setNewObjectMass((float)mass);
}

}
