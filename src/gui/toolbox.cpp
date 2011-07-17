/**
 * @author Markus Holtermann
 * @date May 25, 2011
 * @file gui/toolbox.cpp
 */

#include <gui/toolbox.hpp>
#include <gui/dialogs.hpp>
#include <QtGui/QSizePolicy>
#include <simulation/material.hpp>
#include <simulation/object.hpp>
#include <iostream>
#include <set>
#include <QtCore/QDir>
#include <QtCore/QFileInfoList>

namespace gui {

ToolBox::ToolBox(QWidget *parent)
{
	setMaximumWidth(250);
	m_selectedInteraction = sim::Simulation::INT_NONE;

	layout = new QVBoxLayout();

	create_m_objects();
	create_m_materials();
	create_m_freezeState();
	create_m_mass();
	create_m_buttonbox();
	create_m_size();
	create_m_location();
	create_m_rotation();

	// add all default widgets to the layout
	layout->addWidget(new QLabel("Object:"));
	layout->addWidget(m_objects);
	layout->addWidget(new QLabel("Material:"));
	layout->addWidget(m_materials);
	layout->addWidget(new QLabel("Freeze State:"));
	layout->addWidget(m_freezeState);
	layout->addWidget(new QLabel("Mass:"));
	layout->addWidget(m_mass);

	layout->addLayout(buttonLayout);

	layout->addStretch(-1);

	setLayout(layout);
}

bool doUpdate = true;

void ToolBox::create_m_objects()
{
	// Create the menu for dominos
	QMenu* m_objectMenu = new QMenu();
	QMenu* menu = new QMenu("Domino");
	menu->addAction(new QObjectAction(sim::__Object::DOMINO_SMALL));
	menu->addAction(new QObjectAction(sim::__Object::DOMINO_MIDDLE));
	menu->addAction(new QObjectAction(sim::__Object::DOMINO_LARGE));
	m_objectMenu->addMenu(menu);

	// create the menu for primitive objects
	menu = new QMenu("Primitives");
	menu->addAction(new QObjectAction(sim::__Object::BOX));
	menu->addAction(new QObjectAction(sim::__Object::CAPSULE));
	menu->addAction(new QObjectAction(sim::__Object::CHAMFER_CYLINDER));
	menu->addAction(new QObjectAction(sim::__Object::CONE));
	menu->addAction(new QObjectAction(sim::__Object::CYLINDER));
	menu->addAction(new QObjectAction(sim::__Object::SPHERE));
	m_objectMenu->addMenu(menu);

	// create the menu for templates
	m_template_menu = new QMenu("Templates");
	loadTemplates("data/templates/");
	m_objectMenu->addMenu(m_template_menu);

	m_objects = new QPushButton("Add an object");
	m_objects->setMenu(m_objectMenu);
	connect(m_objectMenu, SIGNAL(triggered(QAction*)), this, SLOT(addObject(QAction*)));
}

void ToolBox::create_m_materials()
{
	// just create the materials QComboBox. we connect the slot after loading
	// the content to prevent crashes due to changed selection list
	m_materials = new QComboBox();
}

void ToolBox::create_m_freezeState()
{
	// build the QCheckBox for the object's freeze state
	m_freezeState = new QCheckBox("Freeze");
	connect(m_freezeState, SIGNAL(stateChanged(int)), this, SLOT(freezeStateChanged(int)));
}

void ToolBox::create_m_mass()
{
	// construct the spin box for the object's mass
	m_mass = new QDoubleSpinBox();
	m_mass->setDecimals(3);
	m_mass->setRange(0, 1000);
	m_mass->setSingleStep(1);
	connect(m_mass, SIGNAL(valueChanged(double)), this, SLOT(massChanged(double)));
}

void ToolBox::create_m_size()
{
	m_labelSize = new QLabel("Size:");

	m_width = new QDoubleSpinBox();
	m_width->setDecimals(3);
	m_width->setRange(0, 100);
	m_width->setSingleStep(1);
	connect(m_width, SIGNAL(valueChanged(double)), this, SLOT(updateSize(double)));
	m_height = new QDoubleSpinBox();
	m_height->setDecimals(3);
	m_height->setRange(0, 100);
	m_height->setSingleStep(1);
	connect(m_height, SIGNAL(valueChanged(double)), this, SLOT(updateSize(double)));
	m_depth = new QDoubleSpinBox();
	m_depth->setDecimals(3);
	m_depth->setRange(0, 100);
	m_depth->setSingleStep(1);
	connect(m_depth, SIGNAL(valueChanged(double)), this, SLOT(updateSize(double)));

	m_modifyWidgetsFull.push_back(m_labelSize);
	m_modifyWidgetsFull.push_back(m_width);
	m_modifyWidgetsFull.push_back(m_height);
	m_modifyWidgetsFull.push_back(m_depth);

	m_modifyWidgets.push_back(m_labelSize);
	m_modifyWidgets.push_back(m_width);
	m_modifyWidgets.push_back(m_height);
}

void ToolBox::create_m_location()
{
	m_labelLocation = new QLabel("Location:");

	m_locationX = new QDoubleSpinBox();
	m_locationX->setDecimals(precision);
	m_locationX->setRange(-100, 100);
	m_locationX->setSingleStep(0.1);
	connect(m_locationX, SIGNAL(valueChanged(double)), this, SLOT(updateLocation(double)));

	m_locationY = new QDoubleSpinBox();
	m_locationY->setDecimals(precision);
	m_locationY->setRange(-1000, 1000);
	m_locationY->setSingleStep(0.1);
	connect(m_locationY, SIGNAL(valueChanged(double)), this, SLOT(updateLocation(double)));

	m_locationZ = new QDoubleSpinBox();
	m_locationZ->setDecimals(precision);
	m_locationZ->setRange(-1000, 1000);
	m_locationZ->setSingleStep(0.1);
	connect(m_locationZ, SIGNAL(valueChanged(double)), this, SLOT(updateLocation(double)));

	m_modifyWidgetsFull.push_back(m_labelLocation);
	m_modifyWidgetsFull.push_back(m_locationX);
	m_modifyWidgetsFull.push_back(m_locationY);
	m_modifyWidgetsFull.push_back(m_locationZ);

	m_modifyWidgetsDominos.push_back(m_labelLocation);
	m_modifyWidgetsDominos.push_back(m_locationX);
	m_modifyWidgetsDominos.push_back(m_locationY);
	m_modifyWidgetsDominos.push_back(m_locationZ);

	m_modifyWidgets.push_back(m_labelLocation);
	m_modifyWidgets.push_back(m_locationX);
	m_modifyWidgets.push_back(m_locationY);
	m_modifyWidgets.push_back(m_locationZ);
}

void ToolBox::create_m_rotation()
{
	m_labelRotation = new QLabel("Rotation:");

	m_rotationX = new QDoubleSpinBox();
	m_rotationX->setDecimals(precision);
	m_rotationX->setRange(-360, 360);
	m_rotationX->setSuffix("°");
	m_rotationX->setSingleStep(0.1);
	m_rotationX->setWrapping(true);
	connect(m_rotationX, SIGNAL(valueChanged(double)), this, SLOT(updateRotation(double)));

	m_rotationY = new QDoubleSpinBox();
	m_rotationY->setDecimals(precision);
	m_rotationY->setRange(-360, 360);
	m_rotationY->setSuffix("°");
	m_rotationY->setSingleStep(0.1);
	m_rotationY->setWrapping(true);
	connect(m_rotationY, SIGNAL(valueChanged(double)), this, SLOT(updateRotation(double)));

	m_rotationZ = new QDoubleSpinBox();
	m_rotationZ->setDecimals(precision);
	m_rotationZ->setRange(-360, 360);
	m_rotationZ->setSuffix("°");
	m_rotationZ->setSingleStep(0.1);
	m_rotationZ->setWrapping(true);
	connect(m_rotationZ, SIGNAL(valueChanged(double)), this, SLOT(updateRotation(double)));

	m_modifyWidgetsFull.push_back(m_labelRotation);
	m_modifyWidgetsFull.push_back(m_rotationX);
	m_modifyWidgetsFull.push_back(m_rotationY);
	m_modifyWidgetsFull.push_back(m_rotationZ);

	m_modifyWidgetsDominos.push_back(m_labelRotation);
	m_modifyWidgetsDominos.push_back(m_rotationX);
	m_modifyWidgetsDominos.push_back(m_rotationY);
	m_modifyWidgetsDominos.push_back(m_rotationZ);

	m_modifyWidgets.push_back(m_labelRotation);
	m_modifyWidgets.push_back(m_rotationX);
	m_modifyWidgets.push_back(m_rotationY);
	m_modifyWidgets.push_back(m_rotationZ);
}

void ToolBox::create_m_buttonbox()
{
	// mouse interaction buttons
	buttonLayout = new QHBoxLayout();
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
	m_mouseinteraction->addButton(m_rotate, (int) sim::Simulation::INT_ROTATE_GROUND);
	buttonLayout->addWidget(m_rotate);

	m_mouseinteraction->setParent(this);
	connect(m_mouseinteraction, SIGNAL(buttonClicked(int)), this, SLOT(onInteractionPressed(int)));
}

void ToolBox::loadMaterials(QString filename)
{
	if (sim::MaterialMgr::instance().load(filename.toStdString())) {
		disconnect(m_materials, SIGNAL(currentIndexChanged(int)), this, SLOT(materialSelected(int)));
		std::set<std::string> materials;
		sim::MaterialMgr::instance().getMaterials(materials);
		for (std::set<std::string>::iterator itr = materials.begin(); itr != materials.end(); itr++) {
			m_materials->addItem(QString::fromStdString(*itr), QString::fromStdString(*itr));
		}
		connect(m_materials, SIGNAL(currentIndexChanged(int)), this, SLOT(materialSelected(int)));
	} // otherwise a catched exception message
}

void ToolBox::updateMaterials(QString filename)
{
	if (filename != "") {
		m_materials->clear();
		loadMaterials(filename);
	}
	materialSelected(0);
}

void ToolBox::loadTemplates(QString directory)
{
	m_template_menu->clear();
	QFileInfoList list = QDir(directory).entryInfoList(QStringList("*.xml"), QDir::Files | QDir::Readable);
	for (int i = 0; i < list.size(); ++i) {
		if (list.at(i).isFile()) {
			m_template_menu->addAction(new QObjectAction(list.at(i)));
		}
	}
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

void ToolBox::addObject(QAction *action)
{
	QObjectAction* a = (QObjectAction*) action;
	sim::Simulation::instance().setNewObjectType(a->getType());
	if (a->getType() == sim::__Object::COMPOUND) {
		sim::Simulation::instance().setNewObjectFilename(a->getFilename().toStdString());
	} else {
		sim::Simulation::instance().setNewObjectFilename("");
	}
	m_objects->setText(a->text());
	m_mass->setValue(a->getMass());
	m_freezeState->setCheckState((a->getFreezeState()) ? Qt::Checked : Qt::Unchecked);
	m_width->setValue(a->getSize().x);
	m_height->setValue(a->getSize().y);
	m_depth->setValue(a->getSize().z);
	m_selectedInteraction = sim::Simulation::INT_CREATE_OBJECT;
	emit interactionSelected(m_selectedInteraction);

	updateData();

	int position = layout->indexOf(m_mass) + 1;
	// todo
	switch (a->getType()) {
	case sim::__Object::BOX:
	case sim::__Object::SPHERE:
		layout->insertWidget(position, m_depth);
		layout->insertWidget(position, m_height);
		layout->insertWidget(position, m_width);
		layout->insertWidget(position, m_labelSize);
		break;
	case sim::__Object::CYLINDER:
	case sim::__Object::CONE:
	case sim::__Object::CAPSULE:
	case sim::__Object::CHAMFER_CYLINDER:
		layout->insertWidget(position, m_height);
		layout->insertWidget(position, m_width);
		layout->insertWidget(position, m_labelSize);
		break;
	default:
		break;
	}
}

void ToolBox::materialSelected(int index)
{
	std::string material = m_materials->itemText(index).toStdString();
	sim::Simulation::instance().setNewObjectMaterial(material);

	if (!doUpdate)
		return;

	if (sim::Simulation::instance().getSelectedObject()) {
		sim::Object obj = sim::Simulation::instance().getSelectedObject();
		obj->setMaterial(material);
		sim::Simulation::instance().updateObject(obj);
	}
}

void ToolBox::freezeStateChanged(int state)
{
	state = (state == Qt::Checked) ? true : false;
	sim::Simulation::instance().setNewObjectFreezeState(state);

	if (!doUpdate)
		return;

	if (sim::Simulation::instance().getSelectedObject()) {
		sim::Object obj = sim::Simulation::instance().getSelectedObject();
		obj->setFreezeState(state);
		sim::Simulation::instance().updateObject(obj);
	}
}

void ToolBox::massChanged(double mass)
{
	sim::Simulation::instance().setNewObjectMass((float) mass);

	if (!doUpdate)
		return;

	if (sim::Simulation::instance().getSelectedObject()) {
		sim::Object obj = sim::Simulation::instance().getSelectedObject();
		obj->setMass(mass);
		sim::Simulation::instance().updateObject(obj);
	}
}

void ToolBox::updateSize(double value)
{
	sim::Simulation::instance().setNewObjectSize(m3d::Vec3f(m_width->value(), m_height->value(), m_depth->value()));

	if (!doUpdate)
		return;

	if (sim::Simulation::instance().getSelectedObject()) {
		sim::Object obj = sim::Simulation::instance().getSelectedObject();
		m3d::Vec3f scale;
		if (obj->getType() == sim::__Object::BOX || obj->getType() == sim::__Object::SPHERE) {
			scale = m3d::Vec3f(m_width->value(), m_height->value(), m_depth->value());
		} else if (obj->getType() == sim::__Object::CYLINDER || obj->getType() == sim::__Object::CONE || obj->getType() == sim::__Object::CAPSULE
				|| obj->getType() == sim::__Object::CHAMFER_CYLINDER) {
			scale = m3d::Vec3f(m_width->value(), m_height->value(), 0);
		}
		obj->scale(scale);
		sim::Simulation::instance().updateObject(obj);
	}
}

void ToolBox::updateLocation(double value)
{
	if (!doUpdate)
		return;

	if (sim::Simulation::instance().getSelectedObject()) {
		m3d::Mat4f matrix = sim::Simulation::instance().getSelectedObject()->getMatrix();
		if (QObject::sender() == m_locationX) {
			matrix._41 = (float) value;
		} else if (QObject::sender() == m_locationY) {
			matrix._42 = (float) value;
		} else if (QObject::sender() == m_locationZ) {
			matrix._43 = (float) value;
		}
		sim::Simulation::instance().getSelectedObject()->setMatrix(matrix);
	}
}

void ToolBox::updateRotation(double value)
{
	if (!doUpdate)
		return;

	if (sim::Simulation::instance().getSelectedObject()) {
		sim::Object obj = sim::Simulation::instance().getSelectedObject();

		Mat4f matrix = Mat4f::rotZ(m_rotationZ->value() * PI / 180.0f) * Mat4f::rotX(m_rotationX->value() * PI / 180.0f) * Mat4f::rotY(
				m_rotationY->value() * PI / 180.0f) * Mat4f::translate(obj->getMatrix().getW());

		obj->setMatrix(matrix);
	}
}

void ToolBox::updateData(sim::Object object)
{
	// set a temporary variable to prevent widgets
	// from updating and sending their signals
	doUpdate = false;

	// this removes all scaling, positioning and roation widgets from the layout
	// we add them again later regarding the selected object
	updateData();

	const m3d::Mat4f* matrix = &object->getMatrix();
	if (matrix) {
		m_locationX->setValue(matrix->_41);
		m_locationY->setValue(matrix->_42);
		m_locationZ->setValue(matrix->_43);

		m_rotationX->setValue(matrix->eulerAngles().x * 180.0f / PI);
		m_rotationY->setValue(matrix->eulerAngles().y * 180.0f / PI);
		m_rotationZ->setValue(matrix->eulerAngles().z * 180.0f / PI);
	}

	if (object->getType() != sim::__Object::NONE) {
		int position = layout->indexOf(m_mass) + 1;

		m_width->setValue(object->getSize().x);
		m_height->setValue(object->getSize().y);
		m_depth->setValue(object->getSize().z);

		m_materials->setCurrentIndex(m_materials->findText(QString::fromStdString(object->getMaterial()), Qt::MatchExactly | Qt::MatchCaseSensitive));
		m_freezeState->setChecked(object->getFreezeState());
		m_mass->setValue(object->getMass());

		/**
		 * BOX: x = width, y = height, z = depth
		 * SPHERE: x = radiusX, y = radiusY, z = radiusZ
		 * CYLINDER, CONE, CAPSULE, CHAMFER_CYLINDER x = radius, y = height
		 */
		if (object->getType() == sim::__Object::DOMINO_SMALL || object->getType() == sim::__Object::DOMINO_MIDDLE || object->getType()
				== sim::__Object::DOMINO_LARGE) {
			for (std::list<QWidget*>::reverse_iterator rit = m_modifyWidgetsDominos.rbegin(); rit != m_modifyWidgetsDominos.rend(); rit++) {
				layout->insertWidget(position, (QWidget*) *rit);
			}

		} else if (object->getType() == sim::__Object::BOX || object->getType() == sim::__Object::SPHERE) {
			for (std::list<QWidget*>::reverse_iterator rit = m_modifyWidgetsFull.rbegin(); rit != m_modifyWidgetsFull.rend(); rit++) {
				layout->insertWidget(position, (QWidget*) *rit);
			}

		} else if (object->getType() == sim::__Object::CYLINDER || object->getType() == sim::__Object::CONE || object->getType() == sim::__Object::CAPSULE
				|| object->getType() == sim::__Object::CHAMFER_CYLINDER) {
			for (std::list<QWidget*>::reverse_iterator rit = m_modifyWidgets .rbegin(); rit != m_modifyWidgets.rend(); rit++) {
				layout->insertWidget(position, (QWidget*) *rit);
			}
		}
	}
	doUpdate = true;
}

void ToolBox::updateData()
{
	for (std::list<QWidget*>::reverse_iterator rit = m_modifyWidgetsFull.rbegin(); rit != m_modifyWidgetsFull.rend(); rit++) {
		layout->removeWidget((QWidget*) *rit);
		((QWidget*) *rit)->setParent(0);
	}
}

}
