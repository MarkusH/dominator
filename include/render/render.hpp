/*
 * render/render.hpp
 *
 *      Author: Markus Holtermann
 */

#ifndef RENDER_RENDER_HPP_
#define RENDER_RENDER_HPP_

#include "model.hpp"
#include <QtOpenGL/QGLWidget>

class Render: public QGLWidget {
Q_OBJECT
public:
	Render(QWidget *parent = 0);
protected:
	virtual void initializeGL();
	virtual void resizeGL(int width, int height);
	virtual void paintGL();
private:
	Model3DS * monkey;
};

#endif /* RENDER_RENDER_HPP_ */
