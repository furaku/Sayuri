#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ui_mainwindow.h"
#include "servicesettingsdialog.h"
#include "consolewidget.h"
#include "servicetablemodel.h"

namespace Ui {
	class MainWindow;
}

class ServiceUtilitiesSettings : public QSettings
{
	Q_OBJECT

public:
	ServiceUtilitiesSettings(QObject *parent = 0);
	~ServiceUtilitiesSettings();

	const QByteArray &getGeometry() const;
	void setGeometry(const QByteArray &geometry);
	const QByteArray &getState() const;
	void setState(const QByteArray &state);
	Qt::DockWidgetArea getDockArea() const;
	void setDockArea(Qt::DockWidgetArea dock_area);
	bool getDockVisible() const;
	void setDockVisible(bool dock_visible);
	int getColumnWidth(int column) const;
	void setColumnWidth(int column, int width);

	void restore();
	void store();
private:
	static const QString FILE_NAME;
	static const QString WIDGET;
	static const QString SETTINGS;
	static const QString GEOMETRY;
	static const QString STATE;
	static const QString DOCK_AREA;
	static const QString DOCK_VISIBLE;
	static const QString COLUMN_WIDTH;
	static const int DEFAULT_COLUMN_WIDTH;

	QByteArray geometry;
	QByteArray state;
	Qt::DockWidgetArea dock_area;
	bool dock_visible;
	int *column_width;
};

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

protected:
	void closeEvent(QCloseEvent * event);

private slots:
	void clickedTable(const QModelIndex &);
	void newServieAction();
	void editServiceAction();
	void removeServiceAction();
	void reload();
	void showOutput(bool);
	void acceptedService();
	void changedOutputVisible(bool);
	void tabCloseRequested(int);
    void finishedProcess(int, int, QProcess::ExitStatus);
    void errorProcess(int, QProcess::ProcessError);
    void readyProcessOutput(int, const QString &, bool);

private:
	static const QString PROCESSES_DIR_NAME;

	Ui::MainWindow *ui;
	QVector<ServiceData *> model_data;
	ServiceTableModel table_model;
	ServiceSettingsDialog *service_setting_dialog;
	ServiceUtilitiesSettings *settings;

	void load() throw (const MakeDirError *);
	void restoreSettings();
	void storeSettings();
	void makeProcessesDir() const throw (const MakeDirError *);
	void showServiceSettingsDialog(int index = -1);
	void removeService(int index);
	void terminateAllProcesses();
};

#endif // MAINWINDOW_H
