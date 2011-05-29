/*
 * modifybox.hpp
 *
 *      Author: Markus Holtermann
 */

#ifndef MODIFYBOX_HPP_
#define MODIFYBOX_HPP_

#include <QtGui/QWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QMouseEvent>
#include <QtGui/QSpinBox>
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

	QSpinBox* m_mbSizeX;
	QSpinBox* m_mbSizeY;
	QSpinBox* m_mbSizeZ;
	QSpinBox* m_mbLocX;
	QSpinBox* m_mbLocY;
	QSpinBox* m_mbLocZ;
	QSpinBox* m_mbRotX;
	QSpinBox* m_mbRotY;
	QSpinBox* m_mbRotZ;

	int m_rx, m_ry, m_rz;

private slots:
	void signalSize(int value);
	void signalLocation(int value);
	void signalRotation(int value);

signals:
	void changeSize(char, int);
	void changeLocation(char, int);
	void changeRotation(char, int);
};

#endif /* MODIFYBOX_HPP_ */
