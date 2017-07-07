#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>

class Account
{

private:
  QString mName;
  QString mPassword;

public:
  Account(QString name, QString password);
  Account(QDataStream &stream);


  void serialize(QDataStream &stream);

  QString name() const;
  void setName(const QString &name);
  QString password() const;
  void setPassword(const QString &password);
};

#endif // ACCOUNT_H
