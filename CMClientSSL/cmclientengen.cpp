#include "cmclientengen.h"

#include <QDebug>
#include <QDataStream>
#include <QAudioDeviceInfo>
#include "aes/aes256.hpp"

#include <iostream>

CMClientEngene::CMClientEngene(QObject *parent) : QObject(parent)
{
  initialize();
  mAccount = NULL;
  m_nNextBlockSize = 0;
}

void CMClientEngene::initialize()
{
  QAudioEncoderSettings setting;
  setting.setCodec("audio/aac");
  setting.setQuality(QMultimedia::HighQuality);

  qDebug() << "AUDIO OPTINS";
  qDebug() << setting.channelCount();
  qDebug() << setting.sampleRate();

  mAudioRecord = new QAudioRecorder(this);
  mAudioRecord->setEncodingSettings(setting);

  mProbe = new QAudioProbe(this);

  mAudioInputsDevices = mAudioRecord->audioInputs();
  QString selectedInput;
  foreach (QString input, mAudioInputsDevices) {
      QString description = mAudioRecord->audioInputDescription(input);
      selectedInput = input;
      qDebug() << selectedInput << description;
    }

  QAudioDeviceInfo qauid;

  qDebug () << "Support codec" << mAudioRecord->supportedAudioCodecs();
  qDebug () << "Support codec" << qauid.supportedCodecs();


  mAudioRecord->setAudioInput(selectedInput);
  mProbe->setSource(mAudioRecord);

  connect(mProbe, SIGNAL(audioBufferProbed(QAudioBuffer)),
          this,   SLOT(audioBufferProbed(QAudioBuffer)));

  QAudioFormat format;
  format.setChannelCount(1);
  format.setCodec("audio/pcm");
  format.setSampleRate(48000);
  format.setSampleSize(16);
  format.setByteOrder(QAudioFormat::LittleEndian);
  format.setSampleType(QAudioFormat::SignedInt);

  mAudioOut       = new QAudioOutput(format, this);
  mAudioOutDevice = mAudioOut->start();
}

void CMClientEngene::successCall()
{
  qDebug () << "Start record";
  QByteArray  arr;
  QDataStream out(&arr, QIODevice::WriteOnly);
  out << quint16(0);

  out << (int) SuccessCall;
  out << (qint64)mDiffHelman.getPrivate();

  out.device()->seek(0);
  out << quint16(arr.size() - sizeof(quint16));
  sendData(arr);
}

void CMClientEngene::canselCall()
{
  qDebug () << "Start record";

  QByteArray  arr;
  QDataStream out(&arr, QIODevice::WriteOnly);
  out << quint16(0);

  out << (int) CanselCall;
  out << mAccount->name();

  out.device()->seek(0);
  out << quint16(arr.size() - sizeof(quint16));
  sendData(arr);
}

void CMClientEngene::startCall(const QString &recipient)
{
  qDebug () << "Start record " << recipient;

  QByteArray  arr;
  QDataStream out(&arr, QIODevice::WriteOnly);
  out << quint16(0);

  out << (int) StartCall;
  out << recipient;

  mDiffHelman = DiffHelmanProtocol(true);

  out << mDiffHelman.getGenerator(); // long
  out << mDiffHelman.getModule(); // long
  out << mDiffHelman.getPrivate(); // long

  sendData(arr);
}

void CMClientEngene::endCall()
{
  qDebug () << "Stop record";
  mAudioRecord->stop();

  QByteArray arr;
  QDataStream out(&arr, QIODevice::WriteOnly);

  out << quint16(0);

  out << (int) EndCall;

  out.device()->seek(0);
  out << quint16(arr.size() - sizeof(quint16));
  sendData(arr);
}

void CMClientEngene::connectToHost(QString host, int port)
{
  QUrl url("ws://"+host+":"+QString::number(port));

  qDebug() << "connect to server at: " << host  << " : "<< port;
  mSocket = new QWebSocket();

  connect(mSocket, SIGNAL(connected()),
          this, SLOT(connected()));

  connect(mSocket, SIGNAL(binaryMessageReceived(QByteArray)),
          this, SLOT(readyRead(QByteArray)));

  connect(mSocket, SIGNAL(sslErrors(QList<QSslError>)),
          this, SLOT(sslErrors(QList<QSslError>)));

  mSocket->open(url);
}

void CMClientEngene::finilize()
{
  mAudioRecord->stop();
  if (mAudioOut)
    mAudioOut->stop();

  if (mSocket)
    delete mSocket;

  if (mAccount)
    delete mAccount;
}

void CMClientEngene::loadAccountList()
{
  QByteArray  arr;
  QDataStream out(&arr, QIODevice::WriteOnly);
  out.setVersion(QT_Version);

  out << quint16(0);

  int type = MessageType::GetUserList;
  out << type;

  out.device()->seek(0);
  out << quint16(arr.size() - sizeof(quint16));
  sendData(arr);
}

void CMClientEngene::offMicro() {
  mAudioRecord->stop();
}

QStringList CMClientEngene::getAudioInputsDevices()
{
  return mAudioInputsDevices;
}

void CMClientEngene::setAudioInput(const QString &input)
{
  mAudioRecord->setAudioInput(input);
}

void CMClientEngene::readCallFrame(QDataStream &stream)
{
  uint    lengthRead;
  quint64 voiceFrameIndex;
  int     lengthExpected;
  char*   data;

  stream >> voiceFrameIndex;
  stream >> lengthExpected;

  if (mExpectedVoiceFrameIndex <= voiceFrameIndex) {
      stream.readBytes(data, lengthRead);
      /*std::vector<unsigned char> key = { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
                                           0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4 };*/
      ByteArray dec;
      Aes256::decrypt(mKey, (unsigned char*) data, lengthRead, dec);

      if (lengthRead > 0)
        playAudio((const char *)dec.data(), dec.size());
    } else {
      stream.readBytes(data, lengthRead);
    }

  mExpectedVoiceFrameIndex = voiceFrameIndex++;
}

void CMClientEngene::audioBufferProbed(const QAudioBuffer& buffer)
{
  int count = buffer.byteCount();
  if (count == 0)
    return;

 /*std::vector<unsigned char> key = { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
                                       0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4 };*/
  QByteArray  arr;
  QDataStream out(&arr, QIODevice::WriteOnly);
  out.setVersion(QT_Version);
  out << quint16(0);
  int type = MessageType::CallFrame;

  out << type;
  out << mLastVoiceFrameIndex;
  out << count;

  ByteArray enc;
  Aes256::encrypt(mKey, (unsigned char*)buffer.data(), count, enc);

  out.writeBytes((const char*)enc.data(), enc.size());
  out.device()->seek(0);
  out << quint16(arr.size() - sizeof(quint16));
  mLastVoiceFrameIndex ++;
  sendData(arr);
}

void CMClientEngene::connected()
{
  qDebug() << "CMClientEngene::connected()";
  emit connectToHostResualt(true);
}

void CMClientEngene::readyRead(QByteArray in)
{
  QDataStream stream(in);
  stream.setVersion(QT_Version);

  int type;

  try {
    stream >> m_nNextBlockSize;
    stream >> type;
    qDebug() << "TYPE:" << type;
    switch (type) {
      case CallFrame: {
          readCallFrame(stream);
        } break;
      case Auth: {
          bool isSuccess;
          stream >> isSuccess;
          qDebug() << "Auth resualt :" << isSuccess;
          if (!isSuccess)
            delete mAccount;
          else {
              qint64 pub = 0;
              stream >> pub;
              mDiffHelmanAuth.setPublic(pub);
              qint64 tmpKey = mDiffHelmanAuth.getKey();
              qDebug() << "Auth key:" << tmpKey;

              std::string strKey = QString::number(tmpKey).toStdString();

              std::vector<unsigned char> keyArr(32);
              for (int i = 0; i < 32; i++) {
                  if (strKey.size() > i) {
                      keyArr.push_back(strKey.at(i));
                    } else {
                      keyArr.push_back(0xff);
                    }
                }

              mKeyAuth = keyArr;
            }

          emit authResualt(isSuccess);
        } break;
      case GetUserList: {
          int         count;
          QString     buffer;
          QStringList list;

          stream >> count;

          for(; count != 0; count --) {
              stream >> buffer;
              list << buffer;
            }
          qDebug() << "ACCOUNTS:" << list;
          emit accountList(list);
        } break;
      case TextMessage: {
          MessageInformation msg(stream);

          //Aes256::encrypt(mKey, (unsigned char*)buffer.data(), count, enc);

          ByteArray enc = msg.getMessage();
          ByteArray dec;
          Aes256::decrypt(mKeyAuth, enc, dec);

          std::string str((const char*)dec.data(), dec.size());
          QString decText(str.c_str());
          qDebug() << "DEC_TEXT:" << decText;
          //QString recipient, QString autor, QString message, QDate date, QTime time
          emit newTextMessage(msg.getRecipient(),
                              msg.getAutor(),
                              decText,
                              msg.getDate().toString("dd.MM.yyyy"),
                              msg.getTime().toString("hh:mm:ss"));
        } break;
      case StartCall: {
          QString fromName;
          qint64 gen  = 0;
          qint64 mod  = 0;
          qint64 priv = 0;
          stream >> fromName;

          stream >> gen;
          stream >> mod;
          stream >> priv;

          mDiffHelman = DiffHelmanProtocol(false);
          mDiffHelman.set(gen, mod);
          mDiffHelman.setPublic(priv);

          qDebug() << "StartCall call: " << mDiffHelman.getKey();

          std::string strKey = QString::number(mDiffHelman.getKey()).toStdString();

          std::vector<unsigned char> key(32);
          for (int i = 0; i < 32; i++) {
              if (strKey.size() > i) {
                  key.push_back(strKey.at(i));
                } else {
                  key.push_back(0xff);
                }
            }

          mKey = key;

          emit signalStartCall(fromName);
        } break;
      case SuccessCall: {
          QString fromName;
          qint64 priv = 0;
          stream >> fromName;
          stream >> priv;

          mDiffHelman.setPublic(priv);
          qDebug() << "success call: " << mDiffHelman.getKey();

          std::string strKey = QString::number(mDiffHelman.getKey()).toStdString();

          std::vector<unsigned char> key(32);
          for (int i = 0; i < 32; i++) {
              if (strKey.size() > i) {
                  key.push_back(strKey.at(i));
                } else {
                  key.push_back(0xff);
                }
            }

          mKey = key;

          mLastVoiceFrameIndex     = 0;
          mExpectedVoiceFrameIndex = 0;
          mAudioRecord->record();

          emit signalSuccessCall(fromName);
        } break;
      case EndCall: {
          mAudioRecord->stop();
          emit signalEndCall();
        }
      case CanselCall: {
          QString fromName;
          stream >> fromName;
          emit signalCanselCall(fromName);
        }
      default:
        break;
      }
  } catch (...) {
    qDebug () << "Error read chunck";
  }
  //qDebug() << "Byte availabel:" << (int)mSocket->bytesAvailable();
}

void CMClientEngene::slotError(QAbstractSocket::SocketError err)
{
  qDebug() << "Socket error :" << mSocket->errorString();
  emit connectToHostResualt(false);
}

void CMClientEngene::sendData(QByteArray &arr) {
  mSocket->sendBinaryMessage(arr);
}

void CMClientEngene::sslErrors(QList<QSslError> errs)
{
  qDebug() << errs;
}

void CMClientEngene::auth(const QString &strName, const QString &strPassword)
{
  mDiffHelmanAuth = DiffHelmanProtocol(true);

  QByteArray  arr;
  QDataStream out(&arr, QIODevice::WriteOnly);
  out.setVersion(QT_Version);

  out << quint16(0);

  out << (int)Auth;
  out << strName;
  out << strPassword;

  out << mDiffHelmanAuth.getGenerator(); // long
  out << mDiffHelmanAuth.getModule();    // long
  out << mDiffHelmanAuth.getPrivate();   // long

  out.device()->seek(0);
  out << quint16(arr.size() - sizeof(quint16));

  mAccount = new Account(strName, strPassword);
  sendData(arr);
}

void CMClientEngene::sendMessage(const QString &recipient, const QString &text)
{
  QByteArray arr;
  QDataStream out(&arr, QIODevice::WriteOnly);

  QDate date = QDate::currentDate();
  QTime time = QTime::currentTime();

  if (mAccount) {
      ByteArray encr;
      Aes256::encrypt(mKeyAuth, (unsigned char*)text.toStdString().c_str(), text.toStdString().size(), encr);

      qDebug() << "ENCSIZE:" << encr.size();

      MessageInformation msg(recipient, mAccount->name(), encr, date, time);
      out << quint16(0);

      out << (int) MessageType::TextMessage;
      msg.saveToStream(out);

      out.device()->seek(0);
      out << quint16(arr.size() - sizeof(quint16));
      sendData(arr);
    } else {
      emit error("Auth error try auth");
    }
}

Account *CMClientEngene::account() const
{
  return mAccount;
}

void CMClientEngene::playAudio(const char* data, int size)
{
  mAudioOutDevice->write(data, size);
}
