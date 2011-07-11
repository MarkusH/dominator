/**
 * @author Markus Holtermann
 * @date May 12, 2011
 * @file main.cpp
 */

#include <iostream>
#include <QtGui/QApplication>
#include <gui/mainwindow.hpp>
#include <sound/SoundMgr.hpp>//test
int main(int argc, char **argv) {
	std::cout << "Totally Unrelated Studios proudly presents:" << std::endl
			  << "\tDOMINATOR" << std::endl << std::endl;

int i=0;

	snd::SoundMgr::instance();//test
	i= snd::SoundMgr::instance().LoadSound("data/sounds");
	snd::SoundMgr::instance().LoadMusic("data/music");

    float pos[3] = { -0.0f, -0.0f, -1.0f };
    float vel[3] = {   0.0f,  0.0f, 0.0f };
    float up[3] = {   0.0f,  1.0f, 0.0f };
    float forw[3] = {   0.0f,  0.0f, 1.0f };
    snd::SoundMgr::instance().SetListenerPos(pos,forw,up,vel);

    float spos[3] = { -0.0f, -0.0f, 0.0f };
    float svel[3] = {   0.0f,  0.0f, 0.0f };



	snd::SoundMgr::instance().PlayMusic(100);

	std::cout << "Test:" << i << std::endl;
float bla = 0.0f;
	while(0)
	{
		snd::SoundMgr::instance().SoundUpdate();
		pos[0] = sin(bla) * 5.0f;
		pos[2] = tan(bla) * 5.0f;
			bla  += 0.000001f;
		// snd::SoundMgr::instance().SetListenerPos(pos,forw,up,vel);

	}

	QApplication app(argc, argv);

	gui::MainWindow mainwindow(&app);

	return app.exec();
}
