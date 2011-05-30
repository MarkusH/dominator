/*
 * render/render.hpp
 *
 *      Author: Markus Holtermann
 */

#ifndef RENDER_RENDER_HPP_
#define RENDER_RENDER_HPP_

#include <QtCore/QTimer>
#include <render/model.hpp>
#include <QtOpenGL/QGLWidget>
#include <QtCore/QString>
#include <opengl/Shader.hpp>
#include <m3d/m3d.hpp>
#include <util/Clock.hpp>
#include <util/InputAdapters.hpp>
#include <QtGui/QWheelEvent>

class Render: public QGLWidget {
Q_OBJECT
public:
	Render(QWidget *parent = 0);

public slots:
	void renderSize(char axis, float size);
	void renderLocation(char axis, float position);
	void renderRotation(float angleX, float angleY, float angleZ);

protected:
	virtual void initializeGL();
	virtual void resizeGL(int width, int height);
	virtual void paintGL();
	virtual void keyPressEvent(QKeyEvent* event);
	virtual void keyReleaseEvent(QKeyEvent* event);
	virtual void mouseMoveEvent(QMouseEvent* event);
	virtual void mousePressEvent(QMouseEvent* event);
	virtual void mouseReleaseEvent(QMouseEvent* event);
	virtual void wheelEvent(QWheelEvent* event);
	virtual void mouseDoubleClickEvent(QMouseEvent* event);

private:
	QTimer *m_timer;
	m3d::Mat4f m_matrix;
	util::Clock m_clock;
	util::QtMouseAdapter m_mouseAdapter;
	util::QtKeyAdapter m_keyAdapter;

signals:
	void framesPerSecondChanged(int);
	void objectsCountChanged(int);
	void objectSelected(const m3d::Mat4f*);
	void objectSelected(bool);
};

#endif /* RENDER_RENDER_HPP_ */
