#ifndef SERVICETABLEMODEL_H
#define SERVICETABLEMODEL_H

#include <QAbstractTableModel>
#include <QSettings>
#include <QProcess>
#include <QStyledItemDelegate>
#include <QTcpSocket>

#include "error.h"

class QPushButton;
class ServiceData;

class ServiceTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	enum ColumnDef
	{
		GROUP,
		NAME,
		PROCESS1,
		PROCESS2,
		PROCESS3,
	};

	static const int COLUMN_COUNT;

	ServiceTableModel(QObject *parent = NULL);
	~ServiceTableModel();

	int rowCount() const;
	int rowCount(const QModelIndex &parent) const;
	int columnCount(const QModelIndex &parent) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	void setModelData(QVector<ServiceData *> *data);
	void setResetModel(bool begin);
	void sort(int column, Qt::SortOrder order);

private:
	QVector<ServiceData *> *model_data;
};


class Process : public QProcess
{
	Q_OBJECT

public:
	Process(QObject *parent = NULL);
	~Process();

	const QString &getName() const;
	void setName(const QString &name);
	const QString &getCommand() const;
	void setCommand(const QString &command);

	void startCommand();
private:
	static const int TIMEOUT;

	QString name;
	QString command;
};

class ServiceData : public QSettings
{
	Q_OBJECT

signals:
    void finishedProcess(int process_num, int exit_code, QProcess::ExitStatus status);
    void errorProcess(int process_num, QProcess::ProcessError error);
    void readyProcessOutput(int process_num, const QString &text, bool error);

public:
	ServiceData(const QString &file_name, QObject *parent = NULL);
	~ServiceData();

	const QString &getKey() const;
	void setKey(const QString &key);
	const QString &getGroup() const;
	void setGroup(const QString &group);
	const QString &getName() const;
	void setName(const QString &name);
	Process *getProcess1();
	Process *getProcess2();
	Process *getProcess3();
    const QString &getHost();
    void setHost(const QString &host);
    const QString &getPort();
    void setPort(const QString &port);

	void restoreSettings();
	void storeSettings();
    void toggledProcess(int process_num);
    void terminateAllProcess();
private slots:
	void finishedProcess(int, QProcess::ExitStatus);
	void errorProcess(QProcess::ProcessError);
	void readProcessStdOutput();
	void readProcessStdError();

private:
	static const QString KEY_KEY;
	static const QString GROUP_KEY;
	static const QString NAME_KEY;
	static const QString PROCESS1_GROUP;
	static const QString PROCESS2_GROUP;
	static const QString PROCESS3_GROUP;
	static const QString COMMAND_KEY;
    static const QString HOST_KEY;
    static const QString PORT_KEY;

	QString key;
	QString group;
	QString name;
	Process process1;
	Process process2;
	Process process3;
    QString host;
    QString port;

    QTcpSocket *socket;

    int getProcessNum(const Process *process);
};

class ProcessDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	ProcessDelegate(QObject *parent = NULL);
	~ProcessDelegate();

	void paint(QPainter *painter, const QStyleOptionViewItem &option,
		const QModelIndex &index) const;
private:
	static const QString PLAY_ICON_PATH;
	static const QString STOP_ICON_PATH;

	const QPixmap *play_icon;
	const QPixmap *stop_icon;
	QPushButton *ref_button;
};

#endif // SERVICETABLEMODEL_H
