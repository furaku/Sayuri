#ifndef SERVICESETTINGSDIALOG_H
#define SERVICESETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class ServiceSettingsDialog;
}

class ServiceSettingsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ServiceSettingsDialog(QWidget *parent = 0);
	~ServiceSettingsDialog();

	const QString &getKey() const;
	void setKey(const QString &key);
	const QString &getGroup() const;
	void setGroup(const QString &group);
	const QString &getName() const;
	void setName(const QString &name);
	const QString &getProcess1Name() const;
	void setProcess1Name(const QString &process1Name);
	const QString &getProcess1Command() const;
	void setProcess1Command(const QString &process1Command);
	const QString &getProcess2Name() const;
	void setProcess2Name(const QString &process2Name);
	const QString &getProcess2Command() const;
	void setProcess2Command(const QString &process2Command);
	const QString &getProcess3Name() const;
	void setProcess3Name(const QString &process3Name);
	const QString &getProcess3Command() const;
	void setProcess3Command(const QString &process3Command);

private:
	Ui::ServiceSettingsDialog *ui;

	QString key;
};

#endif // SERVICESETTINGSDIALOG_H
