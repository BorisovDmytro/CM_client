#include "diffhelmanprotocol.h"

#include <QDebug>

qint64 DiffHelmanProtocol::getModule() const
{
  return module;
}

qint64 DiffHelmanProtocol::getGenerator() const
{
  return generator;
}

DiffHelmanProtocol::DiffHelmanProtocol(bool generate)
{
  if (generate) {
    generator = getGenerator(getRandomInt(0, 2));
    module    = getRandomInt(111111111, 999999999);
  }

  privat = getRandomInt(2, 8);
}

qint64 DiffHelmanProtocol::getKey()
{
  return this->key;
}

void DiffHelmanProtocol::set(qint64 generator, qint64 module)
{
  this->generator = generator;
  this->module    = module;
}

qint64 DiffHelmanProtocol::getPrivate()
{
  return (qint64)pow(generator, privat) % module;
}

void DiffHelmanProtocol::setPublic(qint64 pub)
{
  key = fabs(((qint64)pow(pub, privat) % module));
}

qint64 DiffHelmanProtocol::getRandomInt(const qint64 min, const qint64 max)
{
  QTime midnight(0,0,0);
  qsrand(midnight.secsTo(QTime::currentTime()));
  return fabs(floor(qrand() * (max - min +1)) + min);
}

qint64 DiffHelmanProtocol::getGenerator(const qint64 gen)
{
  return 2 * gen + 2;
}

