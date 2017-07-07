#ifndef GUI_H
#define GUI_H

#include <QListWidgetItem>
#include <QMainWindow>
#include "cmclientengen.h"

namespace Ui {
  class GUI;
}

enum PagesGUI {
  AUTH_PAGE,
  MESSANGER_PAGE,
  OPTIONS_PAGE
};

typedef QList<MessageInformation>* StoragePtr;
typedef QMap<QString, StoragePtr>  History;

class GUI : public QMainWindow
{
  Q_OBJECT

public:
  explicit GUI(QWidget *parent = 0);
  ~GUI();

private slots:
  void onPushbuttonToggled(bool checked);

  void onBtnconnectClicked();

  void onConnectResualt(bool isConnected);

  void onAuthResualt(bool isConenct);

  void onLaodAccounts(QStringList list);

  void onNewMessage(QString recipient, QString autor, QString message, QString date, QString time);

  void onBntsendClicked();

  void onBtnCallClicked();

  void onSuccessCall(QString from);

  void onCanselCall(QString from);

  void onStartCall(QString from);

  void onEndCall();

  void on_btnCallCansel_clicked();

  void on_listWidget_itemClicked(QListWidgetItem *item);

  void on_actionshow_settings_triggered();

private:
  Ui::GUI *ui;
  CMClientEngene engen;
  History history;
  QString selectedUser;
};

#endif // GUI_H
