#ifndef CONSOLEWIDGET_H
#define CONSOLEWIDGET_H

#include <QWidget>

namespace Ui {
class ConsoleWidget;
}

class ConsoleWidget : public QWidget
{
	Q_OBJECT

public:
	explicit ConsoleWidget(QWidget *parent = 0);
	~ConsoleWidget();

	const QString &getKey() const;
	void setKey(const QString & key);

public slots:
	void write(const QString &text, bool error);

private:
	static const int MAX_BLOCKS;
	static const QColor NO_ERROR_COROR;
	static const QColor ERROR_COROR;

	Ui::ConsoleWidget *ui;

	QString key;
};

#endif // CONSOLEWIDGET_H
