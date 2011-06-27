/**
 * @author Markus Holtermann
 * @date May 25, 2011
 * @file gui/toolbox.hpp
 */

#ifndef TOOLBOX_HPP_
#define TOOLBOX_HPP_

#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include <gui/qutils.hpp>
#include <simulation/simulation.hpp>

namespace gui {

/**
 * The ToolBox holds all tools for adding and removing objects to the
 * sim::Simulation. Additionally one can select an
 * sim::Simulation::InteractionType
 */
class ToolBox: public QWidget {
Q_OBJECT
public:
	/**
	 * Constructor
	 *
	 * @param parent	the parent widget
	 */
	ToolBox(QWidget* parent = 0);
	/**
	 * We overwrite this method to add a widget to the layout from the outside
	 */
	void addWidget(QWidget* widget, int stretch = 0, Qt::Alignment alignment = 0);
	void loadMaterials(QString filename);
	void updateMaterials(QString filename = "");

public slots:
	/**
	 * This slot function is invoked by RenderWidget::objectSelected(m3d::Mat4f)
	 * and updates the data of each attribute:
	 * 		ToolBox::m_width, ToolBox::m_height, ToolBox::m_depth
	 * 		ToolBox::m_locationX, ToolBox::m_locationY, ToolBox::m_locationZ
	 * 		ToolBox::m_rotationX, ToolBox::m_rotationY, ToolBox::m_rotationZ
	 */
	void updateData(sim::Object object);
	void updateData();

private:
	void create_m_objects();
	void create_m_materials();
	void create_m_freezeState();
	void create_m_mass();
	void create_m_size();
	void create_m_location();
	void create_m_rotation();
	void create_m_buttonbox();

	QVBoxLayout* layout;
	QHBoxLayout* buttonLayout;
	QComboBox* m_materials;
	QPushButton* m_objects;
	QCheckBox* m_freezeState;
	QDoubleSpinBox* m_mass;

	QLabel* m_labelSize;
	QLabel* m_labelLocation;
	QLabel* m_labelRotation;

	/**
	 * The QDoubleSpinBox for the width of the object
	 */
	QDoubleSpinBox* m_width;
	/**
	 * The QDoubleSpinBox for the height of the object
	 */
	QDoubleSpinBox* m_height;
	/**
	 * The QDoubleSpinBox for the depth of the object
	 */
	QDoubleSpinBox* m_depth;

	/**
	 * The QDoubleSpinBox for the x position of the object
	 */
	QDoubleSpinBox* m_locationX;
	/**
	 * The QDoubleSpinBox for the y position of the object
	 */
	QDoubleSpinBox* m_locationY;
	/**
	 * The QDoubleSpinBox for the z position of the object
	 */
	QDoubleSpinBox* m_locationZ;

	/**
	 * The QDoubleSpinBox for the x rotation of the object
	 */
	QDoubleSpinBox* m_rotationX;
	/**
	 * The QDoubleSpinBox for the y rotation of the object
	 */
	QDoubleSpinBox* m_rotationY;
	/**
	 * The QDoubleSpinBox for the z rotation of the object
	 */
	QDoubleSpinBox* m_rotationZ;

	/**
	 * ModifyBox::precision defines the numer of fractional digits of all
	 * widgets
	 */
	static const int precision = 3;

	/**
	 * ToolBox::m_mouseinteraction is used for single selections of the toggle buttons
	 * ToolBox::m_moveH, ToolBox::m_moveV and ToolBox::m_rotate
	 */
	QButtonGroup* m_mouseinteraction;
	/**
	 * If activated, ToolBox::interactionSelected(sim::Simulation::InteractionType)
	 * is emitted with sim::Simulation::InteractionType::INT_MOVE_GROUND as parameter.
	 * If the button is deactivated, the parameter of the signal is
	 * sim::Simulation::InteractionType::INT_NONE
	 */
	QPushButton* m_moveH;
	/**
	 * If activated, ToolBox::interactionSelected(sim::Simulation::InteractionType)
	 * is emitted with sim::Simulation::InteractionType::INT_MOVE_BILLBOARD as parameter.
	 * If the button is deactivated, the parameter of the signal is
	 * sim::Simulation::InteractionType::INT_NONE
	 */
	QPushButton* m_moveV;
	/**
	 * If activated, ToolBox::interactionSelected(sim::Simulation::InteractionType)
	 * is emitted with sim::Simulation::InteractionType::INT_ROTATE as parameter.
	 * If the button is deactivated, the parameter of the signal is
	 * sim::Simulation::InteractionType::INT_NONE
	 */
	QPushButton* m_rotate;

	/**
	 * Temporary variable to make the "unselection" of buttons in
	 * ToolBox::m_mouseinteraction working
	 */
	sim::Simulation::InteractionType m_selectedInteraction;

	std::list<QWidget*> m_modifyWidgetsFull;
	std::list<QWidget*> m_modifyWidgetsDominos;
	std::list<QWidget*> m_modifyWidgets;
private slots:
	/**
	 * This slot function is invoked by clicking ToolBox::m_moveH,
	 * ToolBox::m_moveV and ToolBox::m_rotate and emits
	 * ToolBox::interactionSelected(sim::Simulation::InteractionType)
	 * as described at the buttons.
	 */
	void onInteractionPressed(int button);
	void addObject(QAction*);
	void materialSelected(int index);
	void freezeStateChanged(int state);
	void massChanged(double mass);

	/**
	 * This slot function is invoked by ToolBox::m_width,
	 * ToolBox::m_height and ToolBox::depth. It emits the specific
	 * ToolBox::sizeChanged(char, float) signal so that MainWindow can forward
	 * the data to RenderWidget::renderSize(char, float).
	 */
	void updateSize(double value);
	/**
	 * This slot function is invoked by ToolBox::m_locationX,
	 * ToolBox::m_locationY, ToolBox::m_locationZ. It emits the specific
	 * ToolBox::locationChanged(char, float) signal so that MainWindow can
	 * forward the data to RenderWidget::renderLocation(char, float).
	 */
	void updateLocation(double value);
	/**
	 * This slot function is invoked by ToolBox::m_rotationX,
	 * ToolBox::m_rotationY, ToolBox::m_rotationZ. It emits the
	 * ToolBox::rotationChanged(float, float, float) signal so that MainWindow
	 * can forward the data to RenderWidget::renderRotation(float, float, float).
	 */
	void updateRotation(double value);

signals:
	/**
	 * See ToolBox::m_moveH, ToolBox::m_moveV, ToolBox::m_rotate and
	 * ToolBox::onInteractionPressed(int)
	 */
	void interactionSelected(sim::Simulation::InteractionType);
};

}

#endif /* TOOLBOX_HPP_ */
