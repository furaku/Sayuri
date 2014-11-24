#include "mainwindow.h"

#include <QDir>
#include <QDateTime>
#include <QCryptographicHash>
#include <QMessageBox>

const QString ServiceUtilitiesSettings::FILE_NAME("settings.ini");
const QString ServiceUtilitiesSettings::WIDGET("Widget");
const QString ServiceUtilitiesSettings::GEOMETRY("geometry");
const QString ServiceUtilitiesSettings::STATE("state");
const QString ServiceUtilitiesSettings::DOCK_AREA("dock_area");
const QString ServiceUtilitiesSettings::DOCK_VISIBLE("dock_visible");
const QString ServiceUtilitiesSettings::COLUMN_WIDTH("column_width_%1");
const int ServiceUtilitiesSettings::DEFAULT_COLUMN_WIDTH = 100;

ServiceUtilitiesSettings::ServiceUtilitiesSettings(QObject *parent)
	: QSettings(FILE_NAME, QSettings::IniFormat, parent)
{
	column_width = new int [ServiceTableModel::COLUMN_COUNT];
	for (int column = 0 ; column < ServiceTableModel::COLUMN_COUNT; column++)
	{
		column_width[column] = DEFAULT_COLUMN_WIDTH;
	}
}

ServiceUtilitiesSettings::~ServiceUtilitiesSettings()
{
	delete[] column_width;
}


const QByteArray &ServiceUtilitiesSettings::getGeometry() const
{
	return geometry;
}

void ServiceUtilitiesSettings::setGeometry(const QByteArray &geometry)
{
	this->geometry = geometry;
}

const QByteArray &ServiceUtilitiesSettings::getState() const
{
	return state;
}

void ServiceUtilitiesSettings::setState(const QByteArray &state)
{
	this->state = state;
}

Qt::DockWidgetArea ServiceUtilitiesSettings::getDockArea() const
{
	return dock_area;
}

void ServiceUtilitiesSettings::setDockArea(Qt::DockWidgetArea dock_area)
{
	this->dock_area = dock_area;
}

bool ServiceUtilitiesSettings::getDockVisible() const
{
	return dock_visible;
}

void ServiceUtilitiesSettings::setDockVisible(bool dock_visible)
{
	this->dock_visible = dock_visible;
}

int ServiceUtilitiesSettings::getColumnWidth(int column) const
{
	return column_width[column];
}

void ServiceUtilitiesSettings::setColumnWidth(int column, int width)
{
	column_width[column] = width;
}

void ServiceUtilitiesSettings::restore()
{
	beginGroup(WIDGET);
	geometry = value(GEOMETRY).toByteArray();
	state = value(STATE).toByteArray();
	dock_area = (Qt::DockWidgetArea)value(DOCK_AREA).toInt();
	dock_visible = value(DOCK_VISIBLE).toBool();
	for (int column = 0; column < ServiceTableModel::COLUMN_COUNT; column++) {
		column_width[column] =
			value(COLUMN_WIDTH.arg(column), QVariant(DEFAULT_COLUMN_WIDTH)).toInt();
	}
	endGroup();
}

void ServiceUtilitiesSettings::store()
{
	beginGroup(WIDGET);
	setValue(GEOMETRY, geometry);
	setValue(STATE, state);
	setValue(DOCK_AREA, dock_area);
	setValue(DOCK_VISIBLE, dock_visible);
	for (int column = 0; column < ServiceTableModel::COLUMN_COUNT; column++) {
		setValue(COLUMN_WIDTH.arg(column), column_width[column]);
	}
	endGroup();
}

const QString MainWindow::PROCESSES_DIR_NAME("processes");

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	table_model.setModelData(&model_data);
	ui->tableView->setModel(&table_model);
	ui->tableView->setItemDelegateForColumn(ServiceTableModel::PROCESS1, new ProcessDelegate());
	ui->tableView->setItemDelegateForColumn(ServiceTableModel::PROCESS2, new ProcessDelegate());
	ui->tableView->setItemDelegateForColumn(ServiceTableModel::PROCESS3, new ProcessDelegate());

	service_setting_dialog = new ServiceSettingsDialog(this);

	connect(ui->tableView, SIGNAL(clicked(const QModelIndex &)), this, SLOT(clickedTable(const QModelIndex &)));
	connect(ui->newButton, SIGNAL(clicked()), this, SLOT(newServieAction()));
	connect(ui->editButton, SIGNAL(clicked()), this, SLOT(editServiceAction()));
	connect(ui->removeButton, SIGNAL(clicked()), this, SLOT(removeServiceAction()));
	connect(ui->reloadButton, SIGNAL(clicked()), this, SLOT(reload()));
	connect(ui->outputButton, SIGNAL(toggled(bool)), this, SLOT(showOutput(bool)));
	connect(ui->dockWidget, SIGNAL(visibilityChanged(bool)), this, SLOT(changedOutputVisible(bool)));
	connect(ui->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(tabCloseRequested(int)));
	connect(service_setting_dialog, SIGNAL(accepted()), this, SLOT(acceptedService()));

	table_model.setResetModel(true);
	try {
		load();
	} catch (MakeDirError *error)
	{
		QMessageBox msg_box;
		msg_box.setText(tr("fail to make the directory:%1").arg(error->getDirPath()));
		msg_box.exec();
		delete error;
	}
	table_model.setResetModel(false);

	settings = new ServiceUtilitiesSettings();
	restoreSettings();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::closeEvent(QCloseEvent * event)
{
	terminateAllProcesses();
	storeSettings();
	QMainWindow::closeEvent(event);
}

void MainWindow::clickedTable(const QModelIndex &index) {
	if (index.isValid())
	{
		switch (index.column()) {
		case ServiceTableModel::PROCESS1:
			model_data[index.row()]->toggledProcess(1);
			break;
		case ServiceTableModel::PROCESS2:
			model_data[index.row()]->toggledProcess(2);
			break;
		case ServiceTableModel::PROCESS3:
			model_data[index.row()]->toggledProcess(3);
			break;
		}
	}
}

void MainWindow::newServieAction()
{
	showServiceSettingsDialog();
}

void MainWindow::editServiceAction()
{
	QModelIndexList indexes = ui->tableView->selectionModel()->selectedIndexes();
	if (!indexes.empty())
	{
		showServiceSettingsDialog(indexes[0].row());
	}
}

void MainWindow::removeServiceAction()
{
	QModelIndexList indexes = ui->tableView->selectionModel()->selectedIndexes();
	if (!indexes.empty())
	{
		int row = indexes[0].row();
		ServiceData *data = model_data.at(row);
		QMessageBox msg_box;
		msg_box.setText(tr("Are you sure you want to remove this service?\nGroup: %1\nName: %2").arg(data->getGroup(), data->getName()));
		msg_box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
		msg_box.setDefaultButton(QMessageBox::Cancel);
		if (msg_box.exec() == QMessageBox::Ok) {
			removeService(row);
		}
	}
}

void MainWindow::reload()
{
	terminateAllProcesses();
	table_model.setResetModel(true);
	model_data.clear();
	try {
		load();
	} catch (MakeDirError *error)
	{
		QMessageBox msg_box;
		msg_box.setText(tr("fail to make the directory:%1").arg(error->getDirPath()));
		msg_box.exec();
		delete error;
	}
	table_model.setResetModel(false);
}

void MainWindow::showOutput(bool on)
{
	if (on)
	{
		ui->dockWidget->show();
	} else
	{
		ui->dockWidget->hide();
	}
}

void MainWindow::acceptedService()
{
	ServiceSettingsDialog *dialog = (ServiceSettingsDialog *) sender();
	ServiceData *data = NULL;
	table_model.setResetModel(true);
	if (dialog->getKey().isEmpty()) {
		QString key;
		key.append(QString::number(QDateTime::currentMSecsSinceEpoch()));
		key.append(QString(dialog->getGroup()));
		key.append(QString(dialog->getName()));
		key = QCryptographicHash::hash(key.toLocal8Bit(), QCryptographicHash::Md5).toHex();
		data = new ServiceData(QString(PROCESSES_DIR_NAME + QDir::separator() + "%1.ini").arg(key));
		data->setKey(key);
		connect(data, SIGNAL(finishedProcess(int, int, QProcess::ExitStatus)), this, SLOT(finishedProcess(int, int, QProcess::ExitStatus)));
		connect(data, SIGNAL(errorProcess(int, QProcess::ProcessError)), this, SLOT(errorProcess(int, QProcess::ProcessError)));
		connect(data, SIGNAL(readyProcessOutput(int, const QString &, bool)), this, SLOT(readyProcessOutput(int, const QString &, bool)));
		model_data.append(data);
	} else
	{
		foreach (ServiceData *current_data, model_data)
		{
			if (current_data->getKey() == dialog->getKey())
			{
				data = current_data;
				break;
			}
		}
	}
	data->setGroup(dialog->getGroup());
	data->setName(dialog->getName());
	data->getProcess1()->setName(dialog->getProcess1Name());
	data->getProcess1()->setCommand(dialog->getProcess1Command());
	data->getProcess2()->setName(dialog->getProcess2Name());
	data->getProcess2()->setCommand(dialog->getProcess2Command());
	data->getProcess3()->setName(dialog->getProcess3Name());
	data->getProcess3()->setCommand(dialog->getProcess3Command());
	table_model.setResetModel(false);

	try {
		makeProcessesDir();
		data->storeSettings();
	} catch (MakeDirError *error)
	{
		QMessageBox msg_box;
		msg_box.setText(tr("fail to make the directory:%1").arg(error->getDirPath()));
		msg_box.exec();
		delete error;
	}
}

void MainWindow::load() throw (const MakeDirError *)
{
	makeProcessesDir();
	QDir processes_dir(PROCESSES_DIR_NAME);

	table_model.setResetModel(true);
	QStringList filter;
	filter += "*.ini";
	foreach (QString file_name, processes_dir.entryList(filter, QDir::Files, QDir::Name))
	{
		ServiceData *data = new ServiceData(PROCESSES_DIR_NAME + QDir::separator() + file_name);
		data->restoreSettings();
		connect(data, SIGNAL(finishedProcess(int, int, QProcess::ExitStatus)), this, SLOT(finishedProcess(int, int, QProcess::ExitStatus)));
		connect(data, SIGNAL(errorProcess(int, QProcess::ProcessError)), this, SLOT(errorProcess(int, QProcess::ProcessError)));
		connect(data, SIGNAL(readyProcessOutput(int, const QString &, bool)), this, SLOT(readyProcessOutput(int, const QString &, bool)));
		model_data.append(data);
	}
}

void MainWindow::changedOutputVisible(bool visible)
{
	ui->outputButton->setChecked(visible);
}

void MainWindow::tabCloseRequested(int index)
{
	QTabWidget *tab = ui->tabWidget;
	QWidget *widget = tab->widget(index);
	tab->removeTab(index);
	delete widget;
}

void MainWindow::finishedProcess(int process_num, int/* exit_code*/, QProcess::ExitStatus/* status*/)
{
	int row = model_data.indexOf((ServiceData *) sender());
	if (row >= 0)
	{
		QModelIndex model_index = table_model.index(row, process_num);
		table_model.dataChanged(model_index, model_index);
	}
}

void MainWindow::errorProcess(int process_num, QProcess::ProcessError error)
{
	QMessageBox msg_box;
	QString error_text;
	switch (error) {
	case QProcess::FailedToStart:
		error_text.append(tr("Fail to start process:%1"));
		break;
	case QProcess::Crashed:
		error_text.append(tr("Crashed process:%1"));
		break;
	case QProcess::Timedout:
		error_text.append(tr("Timeout process:%1"));
		break;
	case QProcess::ReadError:
		error_text.append(tr("Read error process:%1"));
		break;
	case QProcess::WriteError:
		error_text.append(tr("Write error process:%1"));
		break;
	case QProcess::UnknownError:
		error_text.append(tr("Unknown error process:%1"));
		break;
	}
	QString name;
	ServiceData *data = (ServiceData *) sender();
	switch (process_num) {
	case 1:
		name = data->getProcess1()->getName();
		break;
	case 2:
		name = data->getProcess2()->getName();
		break;
	case 3:
		name = data->getProcess3()->getName();
		break;
	}

	msg_box.setText(error_text.arg(name));
	msg_box.exec();
}

void MainWindow::readyProcessOutput(int/* process_num*/, const QString &text, bool error)
{
	ServiceData *data = (ServiceData *) sender();
	QTabWidget *tab = ui->tabWidget;
	ConsoleWidget *widget = NULL;
	for (int i = 0; i < tab->count(); i++)
	{
		ConsoleWidget *current_widget =  (ConsoleWidget *) tab->widget(i);
		if (data->getKey() == current_widget->getKey())
		{
			widget = current_widget;
			break;
		}
	}
	if (widget == NULL)
	{
		widget = new ConsoleWidget(this);
		widget->setKey(data->getKey());
		tab->addTab(widget, data->getName());
	}
	tab->setCurrentWidget(widget);
	widget->write(text, error);
}

void MainWindow::restoreSettings()
{
	settings->restore();
	restoreGeometry(settings->getGeometry());
	restoreState(settings->getState());
	restoreDockWidget(ui->dockWidget);
	if (settings->getDockArea() != Qt::NoDockWidgetArea)
	{
		addDockWidget(settings->getDockArea(), ui->dockWidget);
	}
	ui->outputButton->setChecked(settings->getDockVisible());
	for (int column = 0; column < ServiceTableModel::COLUMN_COUNT; column++)
	{
		ui->tableView->setColumnWidth(column, settings->getColumnWidth(column));
	}
}

void MainWindow::storeSettings()
{
	settings->setGeometry(saveGeometry());
	settings->setState(saveState());
	settings->setDockArea(dockWidgetArea(ui->dockWidget));
	settings->setDockVisible(ui->dockWidget->isVisible());
	for (int column = 0; column < ServiceTableModel::COLUMN_COUNT; column++) {
		settings->setColumnWidth(column, ui->tableView->columnWidth(column));
	}
	settings->store();
}

void MainWindow::makeProcessesDir() const throw (const MakeDirError *)
{
	QDir dir;
	if (!dir.exists(PROCESSES_DIR_NAME) || !QFileInfo(PROCESSES_DIR_NAME).isDir()) {
		if (!dir.mkdir(PROCESSES_DIR_NAME))
		{
			throw new MakeDirError(PROCESSES_DIR_NAME);
		}
	}
}

void MainWindow::showServiceSettingsDialog(int index)
{
	if (index >= 0)
	{
		ServiceData *data = model_data[index];
		service_setting_dialog->setKey(data->getKey());
		service_setting_dialog->setGroup(data->getGroup());
		service_setting_dialog->setName(data->getName());
		service_setting_dialog->setProcess1Name(data->getProcess1()->getName());
		service_setting_dialog->setProcess1Command(data->getProcess1()->getCommand());
		service_setting_dialog->setProcess2Name(data->getProcess2()->getName());
		service_setting_dialog->setProcess2Command(data->getProcess2()->getCommand());
		service_setting_dialog->setProcess3Name(data->getProcess3()->getName());
		service_setting_dialog->setProcess3Command(data->getProcess3()->getCommand());
	} else {
		service_setting_dialog->setKey("");
		service_setting_dialog->setGroup("");
		service_setting_dialog->setName("");
		service_setting_dialog->setProcess1Name("");
		service_setting_dialog->setProcess1Command("");
		service_setting_dialog->setProcess2Name("");
		service_setting_dialog->setProcess2Command("");
		service_setting_dialog->setProcess3Name("");
		service_setting_dialog->setProcess3Command("");
	}
	service_setting_dialog->show();
}

void MainWindow::removeService(int index)
{
	ServiceData *data = model_data[index];
	table_model.setResetModel(true);
	model_data.removeAt(index);
	table_model.setResetModel(false);
	QFile(data->fileName()).remove();
	delete data;
}

void MainWindow::terminateAllProcesses()
{
	foreach(ServiceData *data, model_data)
	{
		data->terminateAllProcess();
	}
}
