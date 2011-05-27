/*
 * render/render.hpp
 *
 *      Author: Markus Holtermann
 */

#ifndef RENDER_RENDER_HPP_
#define RENDER_RENDER_HPP_

#include <render/model.hpp>
#include <QtOpenGL/QGLWidget>
#include <QtCore/QString>
#include <opengl/Shader.hpp>
#include <m3d/m3d.hpp>
#include <util/Clock.hpp>

class Render: public QGLWidget {
Q_OBJECT
public:
	Render(QString *filename = 0, QWidget *parent = 0);
	void load(QString *filename);

public slots:
	void setRotationX(float x = 4.0f);
	void setRotationY(float x = 4.0f);
	void setRotationZ(float x = 4.0f);

protected:
	virtual void initializeGL();
	virtual void resizeGL(int width, int height);
	virtual void paintGL();

private:
	Model3DS * model;
	m3d::Mat4f m_matrix;
	util::Clock m_clock;

signals:
	void framesPerSecondChanged(int);
};

#endif /* RENDER_RENDER_HPP_ */
