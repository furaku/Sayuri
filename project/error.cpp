#include "error.h"

Error::~Error()
{

}

MakeDirError::MakeDirError(const QString &dir_path, QObject *parent)
	: QObject(parent)
{
	this->dir_path = dir_path;
}

MakeDirError::~MakeDirError()
{

}

const QString &MakeDirError::getDirPath() const
{
	return dir_path;
}
