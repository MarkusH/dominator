/*
 * toolbox.hpp
 *
 *      Author: Markus Holtermann
 */

#ifndef TOOLBOX_HPP_
#define TOOLBOX_HPP_

#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>

class ToolBox: public QWidget {
Q_OBJECT
public:
	ToolBox(QWidget* parent = 0);
	void addWidget(QWidget* widget, int stretch = 0, Qt::Alignment alignment = 0);

private:
	QComboBox* m_tb3DOjects;
	QComboBox* m_tbStones;
	QComboBox* m_tbTemplate;
	QComboBox* m_tbTexture;

	QVBoxLayout* layout;
};

#endif /* TOOLBOX_HPP_ */
