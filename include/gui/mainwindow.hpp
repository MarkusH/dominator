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
#include <QtGui/QFileDialog>
#include <gui/toolbox.hpp>
#include <gui/modifybox.hpp>
#include <simulation/Simulation.hpp>

class Render;

class MainWindow: public QMainWindow {
Q_OBJECT
public:
	MainWindow(QApplication* app);

private slots:
	void updateFramesPerSecond(int frames);
	void updateObjectsCount(int count);
	void selectInteraction(sim::Simulation::InteractionType type);
	//File
	void onNewPressed();
	void onClosePressed();
	void onSavePressed();
	void onOpenPressed();
	//Simulation
	void onSimulationControlsPressed();
	void onGravityPressed();
	//Help
	void onHelpPressed();
	void onAboutPressed();

private:
	void initialize();
	void createMenu();
	void createStatusBar();
	bool isModified();

	Render* m_renderWindow;

	QMenu* m_menuFile;
	QAction* m_new;
	QAction* m_save;
	QAction* m_saveas;
	QAction* m_open;
	QAction* m_exit;

	QMenu* m_menuSimulation;
	QAction* m_play;
	QAction* m_stop;
	QAction* m_gravity;

	QMenu* m_menuHelp;
	QAction* m_help;
	QAction* m_about;

	ToolBox* m_toolBox;

	ModifyBox* m_modifyBox;

	QSplitter* m_splitter;

	QLabel* m_framesPerSec;
	QLabel* m_objectsCount;
	QLabel* m_simulationStatus;
	QLabel* m_currentFilename;

	QString m_filename;
	bool m_modified;
};

inline bool MainWindow::isModified()
{
	return m_modified;
}

#endif /* MAINWINDOW_HPP_ */
