/**
 * @author Markus Holtermann
 * @date May 25, 2011
 * @file gui/toolbox.hpp
 */

#ifndef TOOLBOX_HPP_
#define TOOLBOX_HPP_

#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QButtonGroup>
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

private:
	QVBoxLayout* layout;

	QComboBox* m_objects;
	QComboBox* m_stones;
	QComboBox* m_templates;
	QComboBox* m_materials;

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
private slots:
	/**
	 * This slot function is invoked by clicking ToolBox::m_moveH,
	 * ToolBox::m_moveV and ToolBox::m_rotate and emits
	 * ToolBox::interactionSelected(sim::Simulation::InteractionType)
	 * as described at the buttons.
	 */
	void onInteractionPressed(int button);

signals:
	/**
	 * See ToolBox::m_moveH, ToolBox::m_moveV, ToolBox::m_rotate and
	 * ToolBox::onInteractionPressed(int)
	 */
	void interactionSelected(sim::Simulation::InteractionType);
};

}

#endif /* TOOLBOX_HPP_ */
