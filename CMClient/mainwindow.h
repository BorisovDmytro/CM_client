#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>

#include "cmclientengen.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
  void on_connectBtn_clicked();

  void on_btnCall_clicked();

  void on_btnEndCall_clicked();

  void connectedDone();

  void authResualt(bool res);

  void on_pushButton_clicked();

  void onLoadUserList(QStringList list);

  void onNewMessage(MessageInformation msg);

  void on_listWidget_itemClicked(QListWidgetItem *item);

  void on_pushButton_2_clicked();

  void on_btnSuccess_clicked();

  void on_btnCansel_clicked();

  void onStartCall(QString from);
  void onSuccessCall(QString from);
  void onCancelCall(QString from);
  void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
    CMClientEngene *apiClient;
    QStringList users;
    QString currentUser;

    QMap<QString, QStringList*> history;
};

#endif // MAINWINDOW_H
