/*
 * mainwindow.hpp
 *
 *      Author: Markus Holtermann
 */

#ifndef MAINWINDOW_HPP_
#define MAINWINDOW_HPP_

#include <QtCore/QTextCodec>
#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QtGui/QWidget>
#include <QtGui/QSplitter>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QAction>
#include <gui/toolbox.hpp>
#include <gui/modifybox.hpp>


class Render;

class MainWindow: public QMainWindow {
Q_OBJECT
public:
	MainWindow(QApplication *app);

private slots:
	void OnClosePressed();
	void updateFramesPerSecond(int frames);
	void updateObjectsCount(int count);

private:
	void createMenu();
	void createStatusBar();
	void initialize();

	Render *m_renderWindow;

	QMenu *m_menuFile;
	QAction *m_new;
	QAction *m_save;
	QAction *m_saveas;
	QAction *m_open;
	QAction *m_exit;

	QMenu *m_menuHelp;
	QAction *m_help;
	QAction *m_info;

	ToolBox *m_toolBox;

	ModifyBox *m_modifyBox;

	QSplitter *m_splitter;

	QLabel *m_framesPerSec;
	QLabel *m_objectsCount;
	QLabel *m_currentFilename;
};

#endif /* MAINWINDOW_HPP_ */
