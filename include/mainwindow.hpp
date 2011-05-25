/*
 * mainwindow.hpp
 *
 *      Author: Markus Holtermann
 */

#ifndef MAINWINDOW_HPP_
#define MAINWINDOW_HPP_

#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QtGui/QWidget>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>
#include <QtGui/QSpinBox>
#include <QtGui/QComboBox>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QtCore/QTimer>

class Render;

class MainWindow: public QMainWindow {
Q_OBJECT
public:
	MainWindow(QApplication *app);

private slots:
	void OnClosePressed();

private:
	void createMenu();
	void createToolBox();
	void createModifyBox();

	QTimer *m_renderTimer;
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

	QWidget *m_toolBox;
	QComboBox *m_tb3DOjects;
	QComboBox *m_tbStones;
	QComboBox *m_tbTemplate;
	QComboBox *m_tbTexture;

	QWidget *m_modifyBox;
	QSpinBox *m_mbSizeX;
	QSpinBox *m_mbSizeY;
	QSpinBox *m_mbSizeZ;
	QSpinBox *m_mbLocX;
	QSpinBox *m_mbLocY;
	QSpinBox *m_mbLocZ;
	QSpinBox *m_mbRotX;
	QSpinBox *m_mbRotY;
	QSpinBox *m_mbRotZ;

	QSplitter *m_splitter;
};

#endif /* MAINWINDOW_HPP_ */
