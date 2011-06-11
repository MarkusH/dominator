/**
 * @author Markus Holtermann
 * @date May 25, 2011
 * @file gui/modifybox.hpp
 */

#ifndef MODIFYBOX_HPP_
#define MODIFYBOX_HPP_

#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QMouseEvent>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QLabel>
#include <QtCore/QPoint>
#include <QtCore/QObject>
#include <m3d/m3d.hpp>

class ModifyBox: public QWidget {
Q_OBJECT
public:
	ModifyBox(QWidget *parent = 0);

public slots:
	void updateData(const m3d::Mat4f*);
	void updateData(bool);

protected:
	void mouseMoveEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);

private:
	QPoint m_moveOffset;

	QDoubleSpinBox* m_mbSizeX;
	QDoubleSpinBox* m_mbSizeY;
	QDoubleSpinBox* m_mbSizeZ;
	QDoubleSpinBox* m_mbLocX;
	QDoubleSpinBox* m_mbLocY;
	QDoubleSpinBox* m_mbLocZ;
	QDoubleSpinBox* m_mbRotX;
	QDoubleSpinBox* m_mbRotY;
	QDoubleSpinBox* m_mbRotZ;

	float m_rx, m_ry, m_rz;

	static const int precision = 3;

private slots:
	void signalSize(double value);
	void signalLocation(double value);
	void signalRotation(double value);

signals:
	void changeSize(char, float);
	void changeLocation(char, float);
	void changeRotation(float, float, float);
};

#endif /* MODIFYBOX_HPP_ */
