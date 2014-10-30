#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QDebug>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	const QString translator_file_name = "ServiceUtilities_" + QLocale::system().name();
	QTranslator translator;
	if (!translator.load(translator_file_name))
	{
		qDebug() << "can't open " << translator_file_name;
	}
	a.installTranslator(&translator);

	MainWindow w;
	w.show();

	return a.exec();
}
