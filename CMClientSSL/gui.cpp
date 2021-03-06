#include "gui.h"
#include "ui_gui.h"

#include <QMessageBox>
#include "dialogcall.h"
#include "dialogsettings.h"

GUI::GUI(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::GUI)
{
  ui->setupUi(this);
  ui->btnCallCansel->setVisible(false);

  ui->wConnectSetting->setVisible(false);

  connect(&engen, SIGNAL(authResualt(bool)),
          this, SLOT(onAuthResualt(bool)));

  connect(&engen, SIGNAL(connectToHostResualt(bool)),
          this,   SLOT(onConnectResualt(bool)));

  connect(&engen, SIGNAL(accountList(QStringList)),
          this,   SLOT(onLaodAccounts(QStringList)));

  connect(&engen, SIGNAL(newTextMessage(QString,QString,QString,QString,QString)),
          this,   SLOT(onNewMessage(QString,QString,QString,QString,QString)));

  connect(&engen, SIGNAL(signalStartCall(QString)),
          this,   SLOT(onStartCall(QString)));

  connect(&engen, SIGNAL(signalEndCall()),
          this,   SLOT(onEndCall()));

  connect(&engen, SIGNAL(signalSuccessCall(QString)),
          this,   SLOT(onSuccessCall(QString)));

  connect(&engen, SIGNAL(signalCanselCall(QString)),
          this,   SLOT(onCanselCall(QString)));

  connect(ui->pushButton, SIGNAL(toggled(bool)),
          this,           SLOT(onPushbuttonToggled(bool)));

  connect(ui->btnConnect, SIGNAL(clicked(bool)),
          this,           SLOT(onBtnconnectClicked()));

  connect(ui->bntSend, SIGNAL(clicked(bool)),
          this,        SLOT(onBntsendClicked()));

  connect(ui->btn_call, SIGNAL(clicked(bool)),
          this        , SLOT(onBtnCallClicked()));

  connect(ui->btnCallCansel, SIGNAL(clicked(bool)),
          this,              SLOT(on_btnCallCansel_clicked()));
}

GUI::~GUI()
{
  delete ui;
}

void GUI::onPushbuttonToggled(bool checked)
{
  ui->wConnectSetting->setVisible(checked);
}

void GUI::onBtnconnectClicked()
{
  engen.connectToHost(ui->boxIp->text(), ui->boxPort->text().toInt());
}

void GUI::onConnectResualt(bool isConnected)
{
  if (isConnected) {
      const QString name     = ui->boxName->text();
      const QString password = ui->boxPassword->text();

      if (name.isEmpty() || password.isEmpty()) {
          QMessageBox::warning(this, "Input error", "Empty folder");
        } else {
          engen.auth(name, password);
        }
    } else {
      QMessageBox::warning(this, "Connect error", "Error conenct to server ");
    }
}

void GUI::onAuthResualt(bool isConenct)
{
  if (isConenct) {
      ui->stackedWidget->setCurrentIndex(MESSANGER_PAGE);
      engen.loadAccountList();
    } else {
      QMessageBox::warning(this, "Auth error", "Error auth to server");
    }
}

void GUI::onLaodAccounts(QStringList list)
{
  ui->listWidget->clear();
  qDebug () << "load accounts : " << list;
  foreach (auto var, list) {
      history.insert(var, new QList<MessageData>());
      ui->listWidget->addItem(var);
    }
}


void GUI::onNewMessage(QString recipient, QString autor, QString message, QString date, QString time)
{
  StoragePtr storage = history.value(autor, NULL);
  if (storage) {
      storage->append(MessageData(recipient, autor, message, QDate::fromString(date), QTime::fromString(time)));
    }

  if (selectedUser == autor) {
      ui->textEdit->append(message);
    }
}

void GUI::onBntsendClicked()
{
  if (selectedUser.isEmpty())
    return;

  const QString msg = ui->textEdit_2->document()->toPlainText();

  if (msg.isEmpty())
    return;

  engen.sendMessage(selectedUser, msg);
}

void GUI::onBtnCallClicked()
{
  qDebug() << "void GUI::onBtnCallClicked()";
  ui->btn_call->setVisible(false);
  ui->btnCallCansel->setVisible(true);
  engen.startCall(selectedUser);
}

void GUI::onSuccessCall(QString from)
{
  ui->textEdit->append("Start call " + from);
}

void GUI::onCanselCall(QString from)
{
    ui->textEdit->append("Call cansel " + from);
}

void GUI::onStartCall(QString from)
{
  qDebug() << "onStartCall";
  DialogCall dlg(this);
  dlg.setFrom(from);
  if (dlg.exec() == DialogCall::Accepted) {
      engen.successCall();
    } else {
       engen.canselCall();
    }
}

void GUI::onEndCall()
{
  ui->textEdit->append("Call end");
}

void GUI::on_btnCallCansel_clicked()
{
  ui->btn_call->setVisible(true);
  ui->btnCallCansel->setVisible(false);
  engen.canselCall();
}

void GUI::on_listWidget_itemClicked(QListWidgetItem *item)
{
  selectedUser = item->text();
  ui->label_slect_account->setText(selectedUser);
  StoragePtr storage = history.value(selectedUser, NULL);

  ui->textEdit->clear();
  foreach (MessageData var, *storage) {
      ui->textEdit->append(var.message);
    }
}

void GUI::on_actionshow_settings_triggered()
{
  DialogSettings dlg(this);
  dlg.setInputs(engen.getAudioInputsDevices());

  if (dlg.exec() == DialogSettings::Accepted) {
      engen.setAudioInput(dlg.getInput());
    }
}
