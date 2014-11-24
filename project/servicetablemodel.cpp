#include "servicetablemodel.h"

#include <QPainter>
#include <QPushButton>
#include <QSet>

const int ServiceTableModel::COLUMN_COUNT = 5;

ServiceTableModel::ServiceTableModel(QObject *parent)
	: QAbstractTableModel(parent)
{
	model_data = NULL;
}

ServiceTableModel::~ServiceTableModel()
{
}

int ServiceTableModel::rowCount() const
{
	int count;
	if (model_data != NULL) {
		count = model_data->count();
	} else {
		count = 0;
	}
	return count;
}

int ServiceTableModel::rowCount(const QModelIndex &/* parent */) const
{
	return rowCount();
}

int ServiceTableModel::columnCount(const QModelIndex &/* parent */) const
{
	return COLUMN_COUNT;
}

QVariant ServiceTableModel::data(const QModelIndex &index, int role) const
{
	QVariant empty;
	QVariant &ret = empty;

	if (index.isValid() && model_data != NULL) {
		if (role == Qt::DisplayRole) {
			ServiceData *data = model_data->at(index.row());
			switch(index.column()) {
			case GROUP:
				ret = data->getGroup();
				break;
			case NAME:
				ret = data->getName();
				break;
			case PROCESS1:
				ret = QVariant::fromValue(data->getProcess1());
				break;
			case PROCESS2:
				ret = QVariant::fromValue(data->getProcess2());
				break;
			case PROCESS3:
				ret = QVariant::fromValue(data->getProcess3());
				break;
			}
		}
	}

	return ret;
}

QVariant ServiceTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	QVariant empty;
	QVariant &ret = empty;

	if (role == Qt::DisplayRole) {
		if (orientation == Qt::Horizontal) {
			switch (section) {
			case GROUP:
				ret = tr("Group");
				break;
			case NAME:
				ret = tr("Name");
				break;
			case PROCESS1:
				ret = tr("Process1");
				break;
			case PROCESS2:
				ret = tr("Process2");
				break;
			case PROCESS3:
				ret = tr("Process3");
				break;
			}
		}
	}

	return ret;
}

void ServiceTableModel::setModelData(QVector<ServiceData *> *data)
{
	model_data = data;
}

void ServiceTableModel::setResetModel(bool begin)
{
	if (begin) {
		beginResetModel();
	} else {
		endResetModel();
	}
}

void ServiceTableModel::sort(int column, Qt::SortOrder order)
{
	QMap<QString, QList<ServiceData *> *> map;
	switch (column) {
	case GROUP:
		foreach (ServiceData *data, *model_data) {
			QList<ServiceData *> *list = map[data->getGroup()];
			if (list == NULL) {
				list = new QList<ServiceData *>();
				map[data->getGroup()] = list;
			}
			list->append(data);
		}
		break;
	case NAME:
		foreach (ServiceData *data, *model_data) {
			QList<ServiceData *> *list = map[data->getName()];
			if (list == NULL) {
				list = new QList<ServiceData *>();
				map[data->getName()] = list;
			}
			list->append(data);
		}
		break;
	case PROCESS1:
		foreach (ServiceData *data, *model_data) {
			QList<ServiceData *> *list = map[data->getProcess1()->getName()];
			if (list == NULL) {
				list = new QList<ServiceData *>();
				map[data->getProcess1()->getName()] = list;
			}
			list->append(data);
		}
		break;
	case PROCESS2:
		foreach (ServiceData *data, *model_data) {
			QList<ServiceData *> *list = map[data->getProcess2()->getName()];
			if (list == NULL) {
				list = new QList<ServiceData *>();
				map[data->getProcess2()->getName()] = list;
			}
			list->append(data);
		}
		break;
	case PROCESS3:
		foreach (ServiceData *data, *model_data) {
			QList<ServiceData *> *list = map[data->getProcess3()->getName()];
			if (list == NULL) {
				list = new QList<ServiceData *>();
				map[data->getProcess3()->getName()] = list;
			}
			list->append(data);
		}
		break;
	}
	QList<QString> keys = map.keys();
	qSort(keys);
	setResetModel(true);
	model_data->clear();
	if (order == Qt::DescendingOrder)
	{
		QList<QString>::iterator it = keys.end();
		for (it--; it != keys.begin()-1; it--)
		{
			QList<ServiceData *> *list = map[*it];
			QList<ServiceData *>::iterator it2 = list->end();
			for(it2--; it2 != list->begin()-1; it2--)
			{
				model_data->append(*it2);
			}
		}

	} else
	{
		foreach (QString key, keys)
		{
			foreach(ServiceData *data, *map[key])
			{
				model_data->append(data);
			}
		}
	}
	setResetModel(false);

}


const QString ServiceData::KEY_KEY("KEY");
const QString ServiceData::GROUP_KEY("GROUP");
const QString ServiceData::NAME_KEY("NAME");
const QString ServiceData::PROCESS1_GROUP("PROCESS1");
const QString ServiceData::PROCESS2_GROUP("PROCESS2");
const QString ServiceData::PROCESS3_GROUP("PROCESS3");
const QString ServiceData::COMMAND_KEY("COMMAND");
const QString ServiceData::HOST_KEY("HOST");
const QString ServiceData::PORT_KEY("PORT");

ServiceData::ServiceData(const QString &file_name, QObject *parent)
	: QSettings(file_name, QSettings::IniFormat, parent)
{
	socket = NULL;

	connect(&process1, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishedProcess(int, QProcess::ExitStatus)));
	connect(&process2, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishedProcess(int, QProcess::ExitStatus)));
	connect(&process3, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishedProcess(int, QProcess::ExitStatus)));
	connect(&process1, SIGNAL(error(QProcess::ProcessError)), this, SLOT(errorProcess(QProcess::ProcessError)));
	connect(&process2, SIGNAL(error(QProcess::ProcessError)), this, SLOT(errorProcess(QProcess::ProcessError)));
	connect(&process3, SIGNAL(error(QProcess::ProcessError)), this, SLOT(errorProcess(QProcess::ProcessError)));
	connect(&process1, SIGNAL(readyReadStandardOutput()), this, SLOT(readProcessStdOutput()));
	connect(&process2, SIGNAL(readyReadStandardOutput()), this, SLOT(readProcessStdOutput()));
	connect(&process3, SIGNAL(readyReadStandardOutput()), this, SLOT(readProcessStdOutput()));
	connect(&process1, SIGNAL(readyReadStandardError()), this, SLOT(readProcessStdError()));
	connect(&process2, SIGNAL(readyReadStandardError()), this, SLOT(readProcessStdError()));
	connect(&process3, SIGNAL(readyReadStandardError()), this, SLOT(readProcessStdError()));
}

ServiceData::~ServiceData()
{
}

const QString &ServiceData::getKey() const
{
	return key;
}

void ServiceData::setKey(const QString &key)
{
	this->key = key;
}

const QString &ServiceData::getGroup() const
{
	return group;
}

void ServiceData::setGroup(const QString &group)
{
	this->group = group;
}

const QString &ServiceData::getName() const
{
	return name;
}

void ServiceData::setName(const QString &name)
{
	this->name = name;
}

Process *ServiceData::getProcess1()
{
	return &process1;
}

Process *ServiceData::getProcess2()
{
	return &process2;
}

Process *ServiceData::getProcess3()
{
	return &process3;
}

const QString &ServiceData::getHost()
{
	return host;
}

void ServiceData::setHost(const QString &host)
{
	this->host = host;
}

const QString &ServiceData::getPort()
{
	return port;
}

void ServiceData::setPort(const QString &port)
{
	this->port = port;
}

void ServiceData::restoreSettings()
{
	key = value(KEY_KEY).toString();
	group = value(GROUP_KEY).toString();
	name = value(NAME_KEY).toString();
	beginGroup(PROCESS1_GROUP);
	process1.setCommand(value(COMMAND_KEY).toString());
	process1.setName(value(NAME_KEY).toString());
	endGroup();
	beginGroup(PROCESS2_GROUP);
	process2.setCommand(value(COMMAND_KEY).toString());
	process2.setName(value(NAME_KEY).toString());
	endGroup();
	beginGroup(PROCESS3_GROUP);
	process3.setCommand(value(COMMAND_KEY).toString());
	process3.setName(value(NAME_KEY).toString());
	endGroup();
	host = value(HOST_KEY).toString();
	port = value(PORT_KEY).toString();
}

void ServiceData::storeSettings()
{
	setValue(KEY_KEY, key);
	setValue(GROUP_KEY, group);
	setValue(NAME_KEY, name);
	beginGroup(PROCESS1_GROUP);
	setValue(COMMAND_KEY, process1.getCommand());
	setValue(NAME_KEY, process1.getName());
	endGroup();
	beginGroup(PROCESS2_GROUP);
	setValue(COMMAND_KEY, process2.getCommand());
	setValue(NAME_KEY, process2.getName());
	endGroup();
	beginGroup(PROCESS3_GROUP);
	setValue(COMMAND_KEY, process3.getCommand());
	setValue(NAME_KEY, process3.getName());
	endGroup();
	setValue(HOST_KEY, host);
	setValue(PORT_KEY, port);
}

void ServiceData::toggledProcess(int process_num)
{
	Process *process = NULL;
	switch (process_num) {
	case 1:
		process = &process1;
		break;
	case 2:
		process = &process2;
		break;
	case 3:
		process = &process3;
		break;
	}

	if (process != NULL) {
		if (process->state() == Process::Running) {
			process->terminate();
		} else {
			process->startCommand();
		}
	}
}

void ServiceData::terminateAllProcess()
{
	process1.terminate();
	process2.terminate();
	process3.terminate();
}

void ServiceData::finishedProcess(int exit_code, QProcess::ExitStatus status)
{
	Process *process = (Process *) sender();
	emit finishedProcess(getProcessNum(process), exit_code, status);
}

void ServiceData::errorProcess(QProcess::ProcessError error)
{
	Process *process = (Process *) sender();
	emit errorProcess(getProcessNum(process), error);
}

void ServiceData::readProcessStdOutput()
{
	Process *process = (Process *) sender();
	emit readyProcessOutput(getProcessNum(process), QString(process->readAllStandardOutput()), false);
}

void ServiceData::readProcessStdError()
{
	Process *process = (Process *) sender();
	emit readyProcessOutput(getProcessNum(process), QString(process->readAllStandardError()), true);
}

int ServiceData::getProcessNum(const Process *process)
{
	int process_num = -1;
	if (process == &process1) {
		process_num = 1;
	} else if (process == &process2) {
		process_num = 2;
	} else if (process == &process3){
		process_num = 3;
	}
	return process_num;
}

const int Process::TIMEOUT = 10000;

Process::Process(QObject *parent)
	: QProcess(parent)
{
}

Process::~Process()
{
}

void Process::startCommand()
{
	start(command, QIODevice::ReadOnly);
}

const QString &Process::getCommand() const
{
	return command;
}

void Process::setCommand(const QString &command)
{
	this->command = command;
}

const QString &Process::getName() const
{
	return name;
}

void Process::setName(const QString &name)
{
	this->name = name;
}

const QString ProcessDelegate::PLAY_ICON_PATH(":/icon/play.png");
const QString ProcessDelegate::STOP_ICON_PATH(":/icon/stop.png");

ProcessDelegate::ProcessDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{
	play_icon = new QPixmap(PLAY_ICON_PATH);
	stop_icon = new QPixmap(STOP_ICON_PATH);
	ref_button = new QPushButton();
}

ProcessDelegate::~ProcessDelegate()
{
}

void ProcessDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
	const QModelIndex &index) const
{
	Process *process = index.data(Qt::DisplayRole).value<Process *>();
	ref_button->setEnabled(!process->getName().isEmpty());
	if (process->state() == Process::Running) {
		ref_button->setIcon(*stop_icon);
	} else {
		ref_button->setIcon(*play_icon);
	}
	ref_button->setText(process->getName());
	ref_button->setGeometry(QRect(QPoint(), option.rect.size()));
	painter->drawPixmap(option.rect, ref_button->grab());
}
