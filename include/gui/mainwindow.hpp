/**
 * @author Markus Holtermann
 * @date May 13, 2011
 * @file gui/mainwindow.hpp
 */

#ifndef MAINWINDOW_HPP_
#define MAINWINDOW_HPP_

#include <QtCore/QTextCodec>
#include <QtCore/QTemporaryFile>
#include <QtGui/QApplication>
#include <QtGui/QMainWindow>
#include <QtGui/QWidget>
#include <QtGui/QSplitter>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QAction>
#include <QtGui/QFileDialog>
#include <gui/renderwidget.hpp>
#include <gui/dialogs.hpp>
#include <gui/toolbox.hpp>
#include <simulation/simulation.hpp>

namespace gui {

class MainWindow: public QMainWindow {
Q_OBJECT
public:
	/**
	 * Constructor
	 * @param app 	the QApplication this QMainWindow belongs to
	 */
	MainWindow(QApplication* app);

private slots:
	/**
	 * The slot function updateFramesPerSecond(int) is used to update the fps
	 * count in the status bar. It is called by
	 * RenderWidget::framesPerSecondChanged(int).
	 *
	 * @param frames	The new number of frames per second
	 */
	void updateFramesPerSecond(int frames);
	/**
	 * The slot function updateObjectsCount(int) updates the total number of
	 * objects in the sim::Simulation environment. It is called by
	 * RenderWidget::objectsCountChanged(int).
	 *
	 * @param count	The total number of objects
	 */
	void updateObjectsCount(int count);
	/**
	 * The slot function selectInteraction(sim::Simulation::InteractionType)
	 * is executed each time the user clicks on an interaction button
	 * (ToolBox::m_moveH, ToolBox::m_moveV, ToolBox::m_rotate) in the ToolBox.
	 *
	 * @param type on of sim::Simulation::InteractionType
	 */
	void selectInteraction(sim::Simulation::InteractionType type);

	//File
	/**
	 * The slot function onNewPressed() is executed each time the user clicks
	 * on MainWindow::m_new
	 */
	void onNewPressed();
	/**
	 * The slot function onClosePressed() is executed each time the user
	 * clicks on MainWindow::m_exit
	 */
	void onClosePressed();
	/**
	 * The slot function onSavePressed() is executed each time the user clicks
	 * on MainWindow::m_save or MainWindow::m_saveas
	 */
	void onSavePressed();
	/**
	 * The slot function onOpenPressed() is executed each time the user clicks
	 * on MainWindow::m_open
	 */
	void onOpenPressed();

	//Simulation
	/**
	 * The slot function onSimulationControlsPressed() is executed each time
	 * the user clicks on MainWindow::m_play, MainWindow::m_stop or
	 * MainWindow::m_stop_no_reset
	 */
	void onSimulationControlsPressed();
	/**
	 * The slot function onGravityPressed() is executed each time the user
	 * clicks on MainWindow::m_gravity. A GravityDialog will then occur and
	 * the sim::Simulation gravity can be changed.
	 */
	void onGravityPressed();

	void onSoundControlsPressed();

	void onPreferencesPressed();

	//Help
	/**
	 * The slot function onHelpPressed() is executed each time the user clicks
	 * on MainWindow::m_help
	 */
	void onHelpPressed();
	/**
	 * The slot function onAboutPressed() is executed each time the user
	 * clicks on MainWindow::m_about. An AboutDialog will then occur.
	 */
	void onAboutPressed();

private:
	/**
	 * Initialize the GUI, such like default encoding and window title.
	 */
	void initialize();
	/**
	 * createMenu() builds the menu and connects the actions with their slots.
	 */
	void createMenu();
	/**
	 * createStatusBar() generates the status bar that holds the frames per
	 * second, the total number of object, the current filename, etc.
	 */
	void createStatusBar();
	/**
	 * isModified() returns true, if their are changes since the last saving.
	 * @return returns true, if there are changes
	 */
	bool isModified();

	/**
	 * The RenderWidget displays the 3D environment
	 */
	RenderWidget* m_renderWidget;

	/**
	 * The File menu. Holds MainWindow::m_new, MainWindow::m_save,
	 * MainWindow::m_saveas, MainWindow::m_open, MainWindow::m_exit
	 */
	QMenu* m_menuFile;
	/**
	 * When triggered MainWindow::onNewPressed() is executed
	 */
	QAction* m_new;
	/**
	 * When triggered MainWindow::onSavePressed() is executed
	 */
	QAction* m_save;
	/**
	 * When triggered MainWindow::onSavePressed() is executed
	 */
	QAction* m_saveas;
	/**
	 * When triggered MainWindow::onOpenPressed() is executed
	 */
	QAction* m_open;
	/**
	 * When triggered MainWindow::onClosePressed() is executed
	 */
	QAction* m_exit;

	/**
	 * The File menu. Holds MainWindow::m_play, MainWindow::m_stop,
	 * MainWindow::m_gravity
	 */
	QMenu* m_menuSimulation;
	/**
	 * When triggered MainWindow::onSimulationControlsPressed() is executed
	 */
	QAction* m_play;
	/**
	 * When triggered MainWindow::onSimulationControlsPressed() is executed
	 */
	QAction* m_stop;
	QAction* m_stop_no_reset;
	/**
	 * When triggered MainWindow::onGravityPressed() is executed
	 */
	QAction* m_gravity;

	QMenu* m_menuOptions;
	QAction* m_sound_play;
	QAction* m_sound_stop;
	QAction* m_preferences;

	/**
	 * The File menu. Holds MainWindow::m_help, MainWindow::m_about
	 */
	QMenu* m_menuHelp;
	/**
	 * When triggered MainWindow::onHelpPressed() is executed
	 */
	QAction* m_help;
	/**
	 * When triggered MainWindow::onAboutPressed() is executed
	 */
	QAction* m_about;

	/**
	 * MainWindow::m_toolBox is a ToolBox that holds e.g. the buttons for the
	 * sim::Simulation::InteractionType.
	 */
	ToolBox* m_toolBox;

	/**
	 * Splitter between the left side (ToolBox, ModifyBox) and the RenderWidget
	 */
	QSplitter* m_splitter;

	/**
	 * MainWindow::m_framesPerSec displays the current frames per second.
	 * Updated by updateFramesPerSecond(int)
	 */
	QLabel* m_framesPerSec;
	/**
	 * MainWindow::m_objectsCount displays the current total number of objects.
	 * Updated by updateObjectsCount(int)
	 */
	QLabel* m_objectsCount;
	/**
	 * MainWindow::m_simulationStatus displays the current simulation status.
	 * Either <i>Simulation started</i> or <i>Simulation stopped</i>.
	 * Updated by onSimulationControlsPressed()
	 */
	QLabel* m_simulationStatus;
	/**
	 * MainWindow::m_currentFilename displays the current filename.
	 * Updated by onSavePressed() and onOpenPressed()
	 */
	QLabel* m_currentFilename;

	/**
	 * MainWindow::m_filename holds the filename of the currently loaded
	 * sim::Simulation. It is an empty string at program start
	 * Updated by onSavePressed() and onOpenPressed()
	 */
	QString m_filename;
	QTemporaryFile* m_tmp_file;
	/**
	 * MainWindow::m_modified holds the modification status of the current sim::Simulation.
	 * Updated by onSavePressed() and onOpenPressed()
	 */
	bool m_modified;
};

inline bool MainWindow::isModified()
{
	return m_modified;
}

}

#endif /* MAINWINDOW_HPP_ */
