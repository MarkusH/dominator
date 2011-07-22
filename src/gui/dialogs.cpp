/**
 * @author Markus Holtermann
 * @date Jun 7, 2011
 * @file gui/dialogs.cpp
 */

#include <gui/dialogs.hpp>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGridLayout>
#include <QtGui/QImage>
#include <QtGui/QLabel>
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
}

DataPage::DataPage(QWidget *parent) :
		ConfigurationPage(parent)
{
}

}
