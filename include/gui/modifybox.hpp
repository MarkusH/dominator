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

namespace gui {

/**
 * The ModifyBox is a group of Qt widgets to set specific sizes, positions and
 * rotations to a selected object
 */
class ModifyBox: public QWidget {
Q_OBJECT
public:
	/**
	 * Constructor
	 *
	 * @param parent	the parent widget
	 */
	ModifyBox(QWidget *parent = 0);

public slots:
	/**
	 * This slot function is invoked by RenderWidget::objectSelected(m3d::Mat4f)
	 * and updates the data of each attribute:
	 * 		ModifyBox::m_mbSizeX, ModifyBox::m_mbSizeY, ModifyBox::m_mbSizeZ
	 * 		ModifyBox::m_mbLocX, ModifyBox::m_mbLocY, ModifyBox::m_mbLocZ
	 * 		ModifyBox::m_mbRotX, ModifyBox::m_mbRotY, ModifyBox::m_mbRotZ
	 */
	void updateData(const m3d::Mat4f*);
	/**
	 * This slot function is invoked by RenderWidget::objectSelected(bool)
	 * and enables or disables the widget.
	 */
	void updateData(bool);

private:
	/**
	 * The QDoubleSpinBox for the x size of the object
	 */
	QDoubleSpinBox* m_mbSizeX;
	/**
	 * The QDoubleSpinBox for the y size of the object
	 */
	QDoubleSpinBox* m_mbSizeY;
	/**
	 * The QDoubleSpinBox for the z size of the object
	 */
	QDoubleSpinBox* m_mbSizeZ;
	/**
	 * The QDoubleSpinBox for the x position of the object
	 */
	QDoubleSpinBox* m_mbLocX;
	/**
	 * The QDoubleSpinBox for the y position of the object
	 */
	QDoubleSpinBox* m_mbLocY;
	/**
	 * The QDoubleSpinBox for the z position of the object
	 */
	QDoubleSpinBox* m_mbLocZ;
	/**
	 * The QDoubleSpinBox for the x rotation of the object
	 */
	QDoubleSpinBox* m_mbRotX;
	/**
	 * The QDoubleSpinBox for the y rotation of the object
	 */
	QDoubleSpinBox* m_mbRotY;
	/**
	 * The QDoubleSpinBox for the z rotation of the object
	 */
	QDoubleSpinBox* m_mbRotZ;

	/**
	 * ModifyBox::precision defines the numer of fractional digits of all
	 * widgets
	 */
	static const int precision = 3;

private slots:
	/**
	 * This slot function is invoked by ModifyBox::m_mbSizeX,
	 * ModifyBox::m_mbSizeY and ModifyBox::m_mbSizeZ. It emits the specific
	 * ModifyBox::changeSize(char, float) signal so that MainWindow can forward
	 * the data to RenderWidget::renderSize(char, float).
	 */
	void signalSize(double value);
	/**
	 * This slot function is invoked by ModifyBox::m_mbLocX,
	 * ModifyBox::m_mbLocY, ModifyBox::m_mbLocZ. It emits the specific
	 * ModifyBox::changeLocation(char, float) signal so that MainWindow can
	 * forward the data to RenderWidget::renderLocation(char, float).
	 */
	void signalLocation(double value);
	/**
	 * This slot function is invoked by ModifyBox::m_mbRotX,
	 * ModifyBox::m_mbRotY, ModifyBox::m_mbRotZ. It emits the
	 * ModifyBox::changeRotation(float, float, float) signal so that MainWindow
	 * can forward the data to RenderWidget::renderRotation(float, float, float).
	 */
	void signalRotation(double value);

signals:
	/**
	 * Invoked by ModifyBox::signalSize(double) and connected to
	 * RenderWidget::renderSize(char, float)
	 */
	void changeSize(char, float);
	/**
	 * Invoked by ModifyBox::signalLocation(double) and connected to
	 * RenderWidget::renderLocation(char, float)
	 */
	void changeLocation(char, float);
	/**
	 * Invoked by ModifyBox::signalRotation(double) and connected to
	 * RenderWidget::renderRotation(float, float, float)
	 */
	void changeRotation(float, float, float);
};

}
#endif /* MODIFYBOX_HPP_ */
