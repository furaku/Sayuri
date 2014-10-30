#include "consolewidget.h"
#include "ui_consolewidget.h"

#include <QScrollBar>

const int ConsoleWidget::MAX_BLOCKS = 1000;
const QColor ConsoleWidget::NO_ERROR_COROR = QColor("black");
const QColor ConsoleWidget::ERROR_COROR = QColor("red");

ConsoleWidget::ConsoleWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ConsoleWidget)
{
	ui->setupUi(this);
	ui->textEdit->document()->setMaximumBlockCount(MAX_BLOCKS);

	connect(ui->clearButton, SIGNAL(clicked()), ui->textEdit, SLOT(clear()));
}

ConsoleWidget::~ConsoleWidget()
{
	delete ui;
}

const QString &ConsoleWidget::getKey() const
{
	return key;
}

void ConsoleWidget::setKey(const QString &key)
{
	this->key = key;
}

void ConsoleWidget::write(const QString &text, bool error)
{
	QTextEdit *edit = ui->textEdit;
	if (error)
	{
		edit->setTextColor(ERROR_COROR);
	} else
	{
		edit->setTextColor(NO_ERROR_COROR);
	}
    edit->append(text);
	QScrollBar *scrollBar = edit->verticalScrollBar();
	scrollBar->setValue(scrollBar->maximum());
}
