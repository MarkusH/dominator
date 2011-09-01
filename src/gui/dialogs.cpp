/**
 * @author Markus Holtermann
 * @date Jun 7, 2011
 * @file gui/dialogs.cpp
 */

#include <gui/dialogs.hpp>
#include <util/config.hpp>

#include <QtCore/QString>
#include <QtGui/QCheckBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QImage>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QStackedWidget>

namespace gui {

const float GravityDialog::m_rangeLow = -100.0f;
const float GravityDialog::m_rangeHigh = 0.0f;

AboutDialog::AboutDialog(QWidget* parent) :
		QDialog(parent)
{
	QGridLayout* layout = new QGridLayout();
	QPixmap pixmap = QPixmap("data/splash.png");
	QLabel* icon = new QLabel();
	QString text;

	layout->setSizeConstraint(QLayout::SetFixedSize);
	layout->setVerticalSpacing(0);
	layout->setHorizontalSpacing(0);

	pixmap = pixmap.scaledToHeight(100, Qt::SmoothTransformation);
	icon->setPixmap(pixmap);
	layout->addWidget(icon, 0, 0, 2, 1, Qt::AlignTop);

	text = QString("\"DOMINATOR\" is a student's project at the\nDHBW Mannheim, Corporate State University.\n\n");
	layout->addWidget(new QLabel(text), 0, 1);

	text = QString("Authors:\n");
	text.append(" * Markus Döllinger\n");
	text.append(" * Robert Genz\n");
	text.append(" * Raffaela Heller\n");
	text.append(" * Markus Holtermann\n");
	text.append(" * Timo Hübner\n");
	text.append(" * Alexander Tenberge\n");
	text.append(" * Robert Waury\n");
	layout->addWidget(new QLabel(text), 1, 1);

	setWindowTitle("TUStudios - About");
	setLayout(layout);
}

GravityDialog::GravityDialog(const float gravity, QWidget* parent) :
		QDialog(parent)
{
	QGridLayout* m_layout = new QGridLayout();
	QDialogButtonBox* m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	m_gravity = new QDoubleSpinBox();

	// validate the given gravity
	if (gravity < m_rangeLow)
		m_value = m_rangeLow;
	else if (gravity > m_rangeHigh)
		m_value = m_rangeHigh;
	else
		m_value = gravity;

	m_layout->setSizeConstraint(QLayout::SetFixedSize);
	m_layout->setVerticalSpacing(0);
	m_layout->setHorizontalSpacing(0);

	m_gravity->setRange(0.0f, 25.0f);
	m_gravity->setValue(m_value * -0.25f);
	m_layout->addWidget(m_gravity, 0, 0);

	m_layout->addWidget(m_buttons, 1, 0);
	connect(m_buttons, SIGNAL(accepted()), this, SLOT(accept()));
	connect(m_buttons, SIGNAL(rejected()), this, SLOT(reject()));

	setWindowTitle("TUStudios - Simulation Gravity");
	setLayout(m_layout);
}

float GravityDialog::run()
{
	if (exec() == QDialog::Accepted) {
		return m_gravity->value() * -4.0f;
	} else {
		return m_value;
	}
}

MessageDialog::MessageDialog(const std::string title, const std::string message, const MessageType type) :
		QMessageBox()
{
	QMessageBox msgBox;
	msgBox.setText(QString::fromStdString(title));
	msgBox.setInformativeText(QString::fromStdString(message));
	switch (type) {
	case QINFO:
		msgBox.setIcon(QMessageBox::Information);
		break;
	case QWARNING:
		msgBox.setIcon(QMessageBox::Warning);
		break;
	case QERROR:
		msgBox.setIcon(QMessageBox::Critical);
		break;
	}
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.exec();
}

ConfigDialog::ConfigDialog(QWidget* parent) :
		QDialog(parent)
{
	contentsWidget = new QListWidget;
	contentsWidget->setViewMode(QListView::IconMode);
	contentsWidget->setIconSize(QSize(96, 84));
	contentsWidget->setMovement(QListView::Static);
	contentsWidget->setMaximumWidth(128);
	contentsWidget->setSpacing(12);

	pagesWidget = new QStackedWidget;
	pagesWidget->addWidget(new SettingsPage);
	pagesWidget->addWidget(new DataPage);

	QPushButton* closeButton = new QPushButton("Close");
	QPushButton* saveButton = new QPushButton("Save");

	createPages();
	contentsWidget->setCurrentRow(0);

	connect(closeButton, SIGNAL(clicked()), this, SLOT(reject()));
	connect(saveButton, SIGNAL(clicked()), this, SLOT(accept()));

	QHBoxLayout* horizontalLayout = new QHBoxLayout;
	horizontalLayout->addWidget(contentsWidget);
	horizontalLayout->addWidget(pagesWidget, 1);

	QHBoxLayout* buttonsLayout = new QHBoxLayout;
	buttonsLayout->addStretch(1);
	buttonsLayout->addWidget(closeButton);
	buttonsLayout->addWidget(saveButton);

	QVBoxLayout* mainLayout = new QVBoxLayout;
	mainLayout->addLayout(horizontalLayout);
	mainLayout->addStretch(1);
	mainLayout->addSpacing(12);
	mainLayout->addLayout(buttonsLayout);
	setLayout(mainLayout);

	setWindowTitle("Config Dialog");
}

void ConfigDialog::createPages()
{
	QListWidgetItem* settings = new QListWidgetItem(contentsWidget);
	settings->setText("Settings");
	settings->setTextAlignment(Qt::AlignHCenter);
	settings->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	QListWidgetItem* data = new QListWidgetItem(contentsWidget);
	data->setText("Data");
	data->setTextAlignment(Qt::AlignHCenter);
	data->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	connect(contentsWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(changePage(QListWidgetItem*,QListWidgetItem*)));
}

void ConfigDialog::changePage(QListWidgetItem *current, QListWidgetItem *previous)
{
	if (!current)
		current = previous;

	pagesWidget->setCurrentIndex(contentsWidget->row(current));
}

ConfigurationPage::ConfigurationPage(QWidget *parent) :
		QWidget(parent)
{
	m_layout = new QGridLayout();
	this->setLayout(m_layout);
}

SettingsPage::SettingsPage(QWidget *parent) :
		ConfigurationPage(parent)
{
	m_enableMusic = new QCheckBox("Enable Music");
	m_enableMusic->setChecked(util::Config::instance().get("enableMusic", false));
	m_layout->addWidget(m_enableMusic, 0, 0, 1, 3);
	connect(m_enableMusic, SIGNAL(stateChanged(int)), this, SLOT(onBooleanSettingChanged(int)));

	m_enableShadows = new QCheckBox("Enable Shadows");
	m_enableShadows->setChecked(util::Config::instance().get("enableShadows", false));
	m_enableShadows->setToolTip("Activating shadows reduces simulation speed dramatically!");
	m_layout->addWidget(m_enableShadows, 1, 0, 1, 3);
	connect(m_enableShadows, SIGNAL(stateChanged(int)), this, SLOT(onBooleanSettingChanged(int)));

	m_gravity = new QDoubleSpinBox();
	m_gravity->setRange(0.0f, 25.0f);
	m_gravity->setValue(util::Config::instance().get("gravity", 9.81));
	m_layout->addWidget(m_gravity, 2, 0);
	connect(m_gravity, SIGNAL(valueChanged(double)), this, SLOT(onDoubleSettingChanged(double)));
	m_layout->addWidget(new QLabel("Gravity"), 2, 1, 1, 2);

	m_useAF = new QCheckBox("Use anisotropic filtering");
	m_useAF->setChecked(util::Config::instance().get("useAF", false));
	m_useAF->setToolTip("Activating anisotropic texture filtering reduces simulation speed!");
	m_layout->addWidget(m_useAF, 3, 0, 1, 3);
	connect(m_useAF, SIGNAL(stateChanged(int)), this, SLOT(onBooleanSettingChanged(int)));
}

void SettingsPage::onBooleanSettingChanged(int state)
{
	QString key = "";
	if (QObject::sender() == m_enableMusic)
		key = "enableMusic";
	else if (QObject::sender() == m_enableShadows)
		key = "enableShadows";
	else if (QObject::sender() == m_useAF)
		key = "useAF";
	else
		return;
	util::Config::instance().set(key.toStdString(), state == Qt::Checked);
}

void SettingsPage::onDoubleSettingChanged(double d)
{
	QString key = "";
	if (QObject::sender() == m_gravity)
		key = "gravity";
	else
		return;
	util::Config::instance().set(key.toStdString(), d);
}

DataPage::DataPage(QWidget *parent) :
		ConfigurationPage(parent)
{
	m_layout->addWidget(new QLabel("Material Specifications:"));
	m_materialsXML = new QLineEdit();
	m_materialsXML->setText(QString::fromStdString(util::Config::instance().get<std::string>("materialsxml", "data/materials.xml")));
	m_layout->addWidget(m_materialsXML);
	connect(m_materialsXML, SIGNAL(textChanged(QString)), this, SLOT(onStringSettingChanged(QString)));

	m_layout->addWidget(new QLabel("Sounds:"));
	m_sounds = new QLineEdit();
	m_sounds->setText(QString::fromStdString(util::Config::instance().get<std::string>("sounds", "data/sounds/")));
	m_layout->addWidget(m_sounds);
	connect(m_sounds, SIGNAL(textChanged(QString)), this, SLOT(onStringSettingChanged(QString)));

	m_layout->addWidget(new QLabel("Background Music:"));
	m_music = new QLineEdit();
	m_music->setText(QString::fromStdString(util::Config::instance().get<std::string>("music", "data/music/")));
	m_layout->addWidget(m_music);
	connect(m_music, SIGNAL(textChanged(QString)), this, SLOT(onStringSettingChanged(QString)));

	m_layout->addWidget(new QLabel("Background Music:"));
	m_levels = new QLineEdit();
	m_levels->setText(QString::fromStdString(util::Config::instance().get<std::string>("levels", "data/levels/")));
	m_layout->addWidget(m_levels);
	connect(m_levels, SIGNAL(textChanged(QString)), this, SLOT(onStringSettingChanged(QString)));
}

void DataPage::onStringSettingChanged(QString text)
{
	QString key = "";
	if (QObject::sender() == m_materialsXML)
		key = "materialsxml";
	else if (QObject::sender() == m_sounds)
		key = "sounds";
	else if (QObject::sender() == m_music)
		key = "music";
	else if (QObject::sender() == m_levels)
		key = "levels";
	else
		return;
	util::Config::instance().set(key.toStdString(), text.toStdString());
}

}
