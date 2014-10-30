#ifndef ERROR_H
#define ERROR_H

#include <QObject>

class Error
{
public:
	virtual ~Error();
};

class MakeDirError : public QObject, public Error
{
	Q_OBJECT

public:
	explicit MakeDirError(const QString &dir_path, QObject *parent = 0);
	~MakeDirError();

	const QString &getDirPath() const;
private:
	QString dir_path;
};
/*
class MakeDirError : public QObject, public Error
{
	Q_OBJECT

public:
	explicit MakeDirError(const QString &dir_path, QObject *parent = 0);
	~MakeDirError();

	const QString &getDirPath() const;
private:
	QString dir_path;
};
*/
#endif // ERROR_H
