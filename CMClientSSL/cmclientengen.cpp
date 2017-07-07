#include "cmclientengen.h"

#include <QDebug>
#include <QDataStream>
#include <QAudioDeviceInfo>


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

  QStringList inputs = mAudioRecord->audioInputs();
  QString selectedInput;
  foreach (QString input, inputs) {
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
  qDebug () << "Start record";

  QByteArray  arr;
  QDataStream out(&arr, QIODevice::WriteOnly);
  out << quint16(0);

  out << (int) StartCall;
  out << recipient;
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
      if (lengthRead > 0)
        playAudio(data, lengthExpected);
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

  QByteArray  arr;
  QDataStream out(&arr, QIODevice::WriteOnly);
  out.setVersion(QT_Version);
  out << quint16(0);
  int type = MessageType::CallFrame;

  out << type;
  out << mLastVoiceFrameIndex;
  out << count;

  out.writeBytes((const char*)buffer.data(), count);
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
          //QString recipient, QString autor, QString message, QDate date, QTime time
          emit newTextMessage(msg.getRecipient(),
                              msg.getAutor(),
                              msg.getMessage(),
                              msg.getDate().toString("dd.MM.yyyy"),
                              msg.getTime().toString("hh:mm:ss"));
        } break;
      case StartCall: {
          QString fromName;
          stream >> fromName;

          emit signalStartCall(fromName);
        } break;
      case SuccessCall: {
          qDebug() << "success call";
          QString fromName;
          stream >> fromName;

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
  QByteArray  arr;
  QDataStream out(&arr, QIODevice::WriteOnly);
  out.setVersion(QT_Version);

  out << quint16(0);

  out << (int)Auth;
  out << strName;
  out << strPassword;

  out.device()->seek(0);
  out << quint16(arr.size() - sizeof(quint16));

  mAccount = new Account(strName, strPassword);
  // mSocket->write(arrBlock);
  sendData(arr);
}

void CMClientEngene::sendMessage(const QString &recipient, const QString &text)
{
  QByteArray arr;
  QDataStream out(&arr, QIODevice::WriteOnly);

  QDate date = QDate::currentDate();
  QTime time = QTime::currentTime();

  if (mAccount) {
      MessageInformation msg(recipient, mAccount->name(), text, date, time);
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
