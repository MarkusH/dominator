/**
 * @author Markus Holtermann
 * @date May 14, 2011
 * @file gui/renderwidget.hpp
 */

#ifndef RENDERWIDGET_HPP_
#define RENDERWIDGET_HPP_

#include <GL/glew.h>
#include <QtCore/QTimer>
#include <QtOpenGL/QGLWidget>
#include <QtCore/QString>
#include <opengl/Shader.hpp>
#include <m3d/m3d.hpp>
#include <util/Clock.hpp>
#include <util/InputAdapters.hpp>
#include <QtGui/QWheelEvent>

namespace gui {

/**
 * The RenderWidget is a <a href="http://doc.qt.nokia.com/qglwidget.html">QGLWidget</a>
 * that we overwrite with some specific functions
 */
class RenderWidget: public QGLWidget {
Q_OBJECT
public:
	/**
	 * Constructor
	 *
	 * @param parent	the parent widget
	 */
	RenderWidget(QWidget* parent = 0);

public slots:
	/**
	 * The slot function RenderWidget::renderSize will set the size of the
	 * currently selected object regarding to the parameters <i>axis</i>
	 * and <i>size</i>
	 *
	 * @param axis A character - <i>x</i>, <i>y</i> or <i>z</i>
	 * @param size The new size of the object regarding to the given <i>axis</i>
	 */
	void renderSize(char axis, float size);
	/**
	 * The slot function RenderWidget::renderLocation will set the position
	 * of the currently selected object regarding to the parameters <i>axis</i>
	 * and <i>position</i>
	 *
	 * @param axis A character - <i>x</i>, <i>y</i> or <i>z</i>
	 * @param position Set the <i>axis</i>-position of the object to the given
	 * 		  value
	 */
	void renderLocation(char axis, float position);
	/**
	 * The slot function RenderWidget::renderRotation will set the size of the
	 * currently selected object regarding to the parameters <i>x</i>, <i>y</i>
	 * and <i>z</i>
	 *
	 * @param x	Rotate the object around the x axis with the given degrees
	 * @param y	Rotate the object around the y axis with the given degrees
	 * @param z	Rotate the object around the z axis with the given degrees
	 */
	void renderRotation(float x, float y, float z);

protected:
	/**
	 * Within the RenderWidget::initializeGL function we initialize some
	 * environment data::
	 * 		* Shader
	 * 		* Textures
	 * 		* Materials
	 * Besides that we initialize the util::QtMouseAdapter and
	 * util::QtKeyAdapter. We use them for mouse and keyboard interactions.
	 */
	virtual void initializeGL();
	/**
	 * Overwritten function that correctly resizes the OpenGL viewpoint
	 *
	 * @param width		The new widget width
	 * @param height	The new widget height
	 */
	virtual void resizeGL(int width, int height);
	/**
	 * We overwrite QGLWidget::paintGL() to emit
	 * RenderWidget::framesPerSecondChanged(int) and
	 * RenderWidget::objectsCountChanged(int). The first one will be connected
	 * to MainWindow::updateFramesPerSecond(int), the latter to
	 * MainWindow::updateObjectsCount(int)
	 */
	virtual void paintGL();
	/**
	 * Overwritten to work with util::QtKeyAdapter. Therefore we call
	 * util::QtKeyAdapter::keyEvent(QKeyEvent) of the local variable
	 * RenderWidget::m_keyAdapter
	 *
	 * @param event QKeyEvent
	 */
	virtual void keyPressEvent(QKeyEvent* event);
	/**
	 * See RenderWidget::keyPressEvent(QKeyEvent)
	 *
	 * @param event QKeyEvent
	 */
	virtual void keyReleaseEvent(QKeyEvent* event);
	/**
	 * Overwritten to work with util::QtMouseAdapter. Therefore we call
	 * util::QtMouseAdapter::mouseEvent(QMouseEvent) of the local variable
	 * RenderWidget::m_mouseAdapter
	 *
	 * @param event QMouseEvent
	 */
	virtual void mouseMoveEvent(QMouseEvent* event);
	/**
	 * See RenderWidget::mouseMoveEvent(QMouseEvent)
	 *
	 * @param event QMouseEvent
	 */
	virtual void mousePressEvent(QMouseEvent* event);
	/**
	 * See RenderWidget::mouseMoveEvent(QMouseEvent)
	 *
	 * @param event QMouseEvent
	 */
	virtual void mouseReleaseEvent(QMouseEvent* event);
	/**
	 * Overwritten to work with util::QtMouseAdapter. Therefore we call
	 * util::QtMouseAdapter::mouseWheelEvent(QWheelEvent) of the local
	 * variable RenderWidget::m_mouseAdapter
	 *
	 * @param event QMouseEvent
	 */
	virtual void wheelEvent(QWheelEvent* event);
	/**
	 * See RenderWidget::mouseMoveEvent(QMouseEvent)
	 *
	 * @param event QMouseEvent
	 */
	virtual void mouseDoubleClickEvent(QMouseEvent* event);

private:
	/**
	 * RenderWidget::m_timer is used to update and repaint the display. The
	 * timeout() signal is connected to <a href="http://doc.qt.nokia.com/4.7/qglwidget.html#updateGL">
	 * QGLWidget::updateGL()</a>
	 */
	QTimer* m_timer;
	/**
	 * An instance of util::Clock to nicely get the frames per second and emit them to MainWindow
	 */
	util::Clock m_clock;
	/**
	 * An instance of a util::QtMouseAdapter for any mouse interaction with
	 * the sim:Simulation
	 */
	util::QtMouseAdapter m_mouseAdapter;
	/**
	 * An instance of a util::QtKeyAdapter for any key interaction with the
	 * sim:Simulation
	 */
	util::QtKeyAdapter m_keyAdapter;

signals:
	/**
	 * framesPerSecondChanged(int) is emitted every second by
	 * RenderWidget::m_clock and is connected to
	 * MainWindow::updateFramesPerSecond(int)
	 */
	void framesPerSecondChanged(int);
	/**
	 * objectsCountChanged(int) is emitted every second by
	 * RenderWidget::m_clock and is connected to
	 * MainWindow::updateFramesPerSecond(int)
	 */
	void objectsCountChanged(int);
	/**
	 * objectSelected(m3d::Mat4f) is emitted each time the user has selected
	 * an object. The signal is connected to ModifyBox::updateData(m3d::Mat4f)
	 */
	void objectSelected(const m3d::Mat4f*);
	/**
	 * objectSelected(bool) is emitted each time the user has selected
	 * an object. The signal is connected to ModifyBox::updateData(bool)
	 *
	 * @param if the parameter is true, the user selects an object if it is
	 * false, the user unselects an object
	 */
	void objectSelected(bool);
};

}
#endif /* RENDERWIDGET_HPP_ */
