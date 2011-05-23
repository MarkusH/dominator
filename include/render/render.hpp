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

class Render: public QGLWidget {
Q_OBJECT
public:
	Render(QString *filename = 0, QWidget *parent = 0);
	void load(QString *filename);
	void rotate();
public slots:
	void setRotationX(int value);
	void setRotationY(int value);
	void setRotationZ(int value);
protected:
	virtual void initializeGL();
	virtual void resizeGL(int width, int height);
	virtual void paintGL();
private:
	Model3DS * model;
	float rotate_x, rotate_y, rotate_z;
};

#endif /* RENDER_RENDER_HPP_ */
