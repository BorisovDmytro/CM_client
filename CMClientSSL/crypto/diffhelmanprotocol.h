#ifndef DIFFHELMANPROTOCOL_H
#define DIFFHELMANPROTOCOL_H

#include <qmath.h>
#include "QTime"

class DiffHelmanProtocol
{
private:
  qint64 generator;
  qint64 module;
  qint64 privat;
  qint64 key;
public:
  DiffHelmanProtocol(bool generate = false);

  qint64 getPrivate();
  void setPublic(qint64 pub);

  qint64 getKey();
  void set(qint64 generator, qint64 module);


  qint64 getModule() const;
  qint64 getGenerator() const;

private:
  qint64 getRandomInt(const qint64 min, const qint64 max);
  qint64 getGenerator(const qint64 gen);
};

#endif // DIFFHELMANPROTOCOL_H

