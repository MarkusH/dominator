/*
 * toolbox.hpp
 *
 *      Author: Markus Holtermann
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
#include <simulation/Simulation.hpp>

class ToolBox: public QWidget {
Q_OBJECT
public:
	ToolBox(QWidget* parent = 0);
	void addWidget(QWidget* widget, int stretch = 0, Qt::Alignment alignment = 0);

private:
	QVBoxLayout* layout;

	QComboBox* m_tb3DOjects;
	QComboBox* m_tbStones;
	QComboBox* m_tbTemplate;
	QComboBox* m_tbTexture;

	QButtonGroup* m_mouseinteraction;
	QPushButton* m_moveH;
	QPushButton* m_moveV;
	QPushButton* m_rotate;

	sim::Simulation::InteractionType m_selectedInteraction;
private slots:
	void onInteractionPressed(int button);

signals:
	void interactionSelected(sim::Simulation::InteractionType);
};

#endif /* TOOLBOX_HPP_ */
