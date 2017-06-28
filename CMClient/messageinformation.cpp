#include "messageinformation.h"

QString MessageInformation::getRecipient() const
{
  return recipient;
}

void MessageInformation::setRecipient(const QString &value)
{
  recipient = value;
}

MessageInformation::MessageInformation()
{

}

MessageInformation::MessageInformation(QString recipient, QString autor, QString message, QDate date, QTime time)
{
  this->recipient = recipient;
  this->autor = autor;
  this->message = message;
  this->date = date;
  this->time = time;
}

MessageInformation::MessageInformation(QDataStream &stream)
{
  stream >> recipient;
  stream >> autor;
  stream >> message;
  stream >> date;
  stream >> time;
}

QString MessageInformation::getAutor() const
{
  return autor;
}

void MessageInformation::setAutor(const QString &value)
{
  autor = value;
}

QString MessageInformation::getMessage() const
{
  return message;
}

void MessageInformation::setMessage(const QString &value)
{
  message = value;
}

QDate MessageInformation::getDate() const
{
  return date;
}

void MessageInformation::setDate(const QDate &value)
{
  date = value;
}

QTime MessageInformation::getTime() const
{
  return time;
}

void MessageInformation::setTime(const QTime &value)
{
  time = value;
}

void MessageInformation::saveToStream(QDataStream &stream)
{
  stream << recipient;
  stream << autor;
  stream << message;
  stream << date;
  stream << time;
}
