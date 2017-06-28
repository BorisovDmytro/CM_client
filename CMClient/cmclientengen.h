#ifndef CMAPICLIENT_H
#define CMAPICLIENT_H
#define QT_Version QDataStream::Qt_5_9

#include <QObject>
#include <QAudioRecorder>
#include <QMediaPlayer>
#include <QAudioProbe>
#include <QAudioOutput>
#include <QTcpSocket>

#include "messageinformation.h"
#include "account.h"

class CMClientEngene : public QObject
{
  Q_OBJECT
private:
  QAudioRecorder *mAudioRecord;
  QAudioRecorder *mRecord;
  QAudioProbe    *mProbe;
  QAudioOutput   *mAudioOut;
  QIODevice      *mAudioOutDevice;
  QTcpSocket     *mSocket;
  Account        *mAccount;

  quint64 mLastVoiceFrameIndex;
  quint64 mExpectedVoiceFrameIndex;
  quint16 m_nNextBlockSize;
  void playAudio(const char* data, int size);
public:
  enum MessageType {
    Undefined = -1,
    CallFrame = 0,
    StartCall = 1,
    SuccessCall = 2,
    CanselCall = 3,
    EndCall = 4,
    TextMessage = 5,
    Auth = 6,
    GetUserList = 7
  };

  explicit CMClientEngene(QObject *parent = 0);

  Account *account() const;

private:
  void readCallFrame(QDataStream &stream);

private slots:
  void audioBufferProbed(const QAudioBuffer &buffer);
  void connected();
  void readyRead();
  void slotError(QAbstractSocket::SocketError err);


  void sendData(QByteArray &arr);


signals:
  /**
   */
  void error(QString msg);
  void newTextMessage(QString recipient, QString autor, QString message, QString date, QString time);
  void authResualt(bool isSuccess);
  void connectedDone();
  void accountList(QStringList list);
  void signalSuccessCall(QString from);
  void signalCanselCall(QString from);
  void signalStartCall(QString from);
  void signalEndCall();

public slots:
  void auth(const QString &strName, const QString &strPassword);
  void sendMessage(const QString &recipient, const QString &text);
  void initialize();
  void successCall();
  void canselCall();
  void startCall(const QString &recipient);
  void endCall();
  void connectToHost(QString host, int port);
  void finilize();
  void loadAccountList();
  void offMicro();

};

#endif // CMAPICLIENT_H
