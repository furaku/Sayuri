#include "servicesettingsdialog.h"
#include "ui_servicesettingsdialog.h"

ServiceSettingsDialog::ServiceSettingsDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ServiceSettingsDialog)
{
	ui->setupUi(this);
}

ServiceSettingsDialog::~ServiceSettingsDialog()
{
	delete ui;
}

const QString ServiceSettingsDialog::getKey() const
{
	return key;
}

void ServiceSettingsDialog::setKey(const QString &key)
{
	this->key = key;
}

const QString ServiceSettingsDialog::getGroup() const
{
	QString aa = ui->groupEdit->text();
	return ui->groupEdit->text();
}

void ServiceSettingsDialog::setGroup(const QString &group)
{
	ui->groupEdit->setText(group);
}

const QString ServiceSettingsDialog::getName() const
{
	return ui->nameEdit->text();
}

void ServiceSettingsDialog::setName(const QString &name)
{
	ui->nameEdit->setText(name);
}

const QString ServiceSettingsDialog::getProcess1Name() const
{
	return ui->process1NameEdit->text();
}

void ServiceSettingsDialog::setProcess1Name(const QString &process1Name)
{
	ui->process1NameEdit->setText(process1Name);
}

const QString ServiceSettingsDialog::getProcess1Command() const
{
	return ui->process1CommandEdit->text();
}

void ServiceSettingsDialog::setProcess1Command(const QString &process1Command)
{
	ui->process1CommandEdit->setText(process1Command);
}

const QString ServiceSettingsDialog::getProcess2Name() const
{
	return ui->process2NameEdit->text();
}

void ServiceSettingsDialog::setProcess2Name(const QString &process2Name)
{
	ui->process2NameEdit->setText(process2Name);
}

const QString ServiceSettingsDialog::getProcess2Command() const
{
	return ui->process2CommandEdit->text();
}

void ServiceSettingsDialog::setProcess2Command(const QString &process2Command)
{
	ui->process2CommandEdit->setText(process2Command);
}

const QString ServiceSettingsDialog::getProcess3Name() const
{
	return ui->process3NameEdit->text();
}

void ServiceSettingsDialog::setProcess3Name(const QString &process3Name)
{
	ui->process3NameEdit->setText(process3Name);
}

const QString ServiceSettingsDialog::getProcess3Command() const
{
	return ui->process3CommandEdit->text();
}

void ServiceSettingsDialog::setProcess3Command(const QString &process3Command)
{
	ui->process3CommandEdit->setText(process3Command);
}
