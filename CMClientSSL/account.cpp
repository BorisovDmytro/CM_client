#include "account.h"
Account::Account(QString name, QString password)
{
  mName = name;
  mPassword = password;
}

Account::Account(QDataStream &stream)
{
  stream >> mName;
  stream >> mPassword;
}

void Account::serialize(QDataStream &stream)
{
  stream << mName;
  stream << mPassword;
}

QString Account::password() const
{
  return mPassword;
}

void Account::setPassword(const QString &password)
{
  mPassword = password;
}

QString Account::name() const
{
  return mName;
}

void Account::setName(const QString &name)
{
  mName = name;
}
