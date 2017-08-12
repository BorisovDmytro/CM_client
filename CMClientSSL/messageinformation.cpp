#include "messageinformation.h"
#include <QDebug>

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

MessageInformation::MessageInformation(QString recipient, QString autor, ByteArray message, QDate date, QTime time)
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
  stream >> date;
  stream >> time;

  uint rb = 0;
  char *buffer;
  stream.readBytes(buffer, rb);

  qDebug() << "ENCRP_SIZE" << rb;

  message = ByteArray(rb);
  for (int i = 0; i < rb; i ++) {
      message[i] = buffer[i];
    }
}

QString MessageInformation::getAutor() const
{
  return autor;
}

void MessageInformation::setAutor(const QString &value)
{
  autor = value;
}

ByteArray MessageInformation::getMessage() const
{
  return message;
}

void MessageInformation::setMessage(ByteArray &arr)
{
  message = arr;
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
  stream << date;
  stream << time;
  stream.writeBytes((const char*)message.data(), message.size());
}
