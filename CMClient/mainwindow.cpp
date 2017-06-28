#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  apiClient = new CMClientEngene(this);

  connect(apiClient, SIGNAL(connectedDone()),
          this,      SLOT(connectedDone()));

  connect(apiClient, SIGNAL(authResualt(bool)),
          this,      SLOT(authResualt(bool)));

  connect(apiClient, SIGNAL(accountList(QStringList)),
          this,      SLOT(onLoadUserList(QStringList)));

  connect(apiClient, SIGNAL(newTextMessage(MessageInformation)),
          this,      SLOT(onNewMessage(MessageInformation)));

  connect(apiClient, SIGNAL(signalStartCall(QString)),
          this,      SLOT(onStartCall(QString)));

  connect(apiClient, SIGNAL(signalCanselCall(QString)),
          this,      SLOT(onCancelCall(QString)));

  connect(apiClient, SIGNAL(signalSuccessCall(QString)),
          this,      SLOT(onSuccessCall(QString)));
}

MainWindow::~MainWindow()
{
  apiClient->finilize();
  delete ui;
}

void MainWindow::on_btnCall_clicked()
{
  QString crntItm = ui->listWidget->currentItem()->text();

  apiClient->startCall(crntItm);
  ui->btnEndCall->setEnabled(true);
  ui->btnCall->setEnabled(false);
}

void MainWindow::on_btnEndCall_clicked()
{
  apiClient->endCall();
  ui->btnCall->setEnabled(true);
  ui->btnEndCall->setEnabled(false);
}

void MainWindow::connectedDone()
{
  apiClient->auth(ui->lineEdit->text(), ui->lineEdit_2->text());
}

void MainWindow::authResualt(bool res)
{
  qDebug() << "Auth resualt" << res;
  apiClient->loadAccountList();
}

void MainWindow::on_connectBtn_clicked()
{
  apiClient->connectToHost(ui->textBoxHost->text(), ui->textBoxPort->text().toInt());
  ui->tabWidget->setCurrentIndex(0);
}

void MainWindow::on_pushButton_clicked()
{
  QString msg = ui->lineEdit_3->text();
  if (msg.isEmpty() || currentUser.isEmpty())
    return;

  apiClient->sendMessage(currentUser, msg);

  QStringList* list = history.value(currentUser, NULL);
  if (list == NULL) {
    list = new QStringList();
    history.insert(currentUser, list);
  }
  if (apiClient->account() &&
      currentUser != apiClient->account()->name()) {
    list->append(msg);
    ui->textEdit->append(msg);
  }

  ui->lineEdit_3->clear();
}

void MainWindow::onLoadUserList(QStringList list)
{
  ui->listWidget->clear();
  ui->listWidget->addItems(list);
  users = list;
}

void MainWindow::onNewMessage(MessageInformation msg)
{
  if (msg.getAutor() == currentUser) {
    ui->textEdit->append(msg.getMessage());
  }

  if (history.keys().indexOf(msg.getAutor()) == -1) {
    QStringList *list = new QStringList();
    list->append(msg.getMessage());
    history.insert(msg.getAutor(), list);
  }
  QStringList* listHis = history.value(msg.getAutor());
  listHis->append(msg.getMessage());
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
  currentUser = item->text();
  ui->label->setText(currentUser);

  ui->textEdit->clear();

  QStringList* listHis = history.value(currentUser, NULL);
  if (listHis == NULL)
    return;

  for(int i = 0; i < listHis->size(); i++) {
    ui->textEdit->append(listHis->at(i));
  }
}

void MainWindow::on_pushButton_2_clicked()
{
  apiClient->loadAccountList();
}

void MainWindow::on_btnSuccess_clicked()
{
  apiClient->successCall();
}

void MainWindow::on_btnCansel_clicked()
{
  apiClient->canselCall();
}

void MainWindow::onStartCall(QString from)
{
  ui->labelFrom->setText(from);
}

void MainWindow::onSuccessCall(QString from)
{
  qDebug() << "SUCCESS CALL" << from;
}

void MainWindow::onCancelCall(QString from)
{
  qDebug() << "CANSEL CALL" << from;
  ui->btnCall->setEnabled(true);
  ui->btnCansel->setEnabled(false);
}

void MainWindow::on_pushButton_3_clicked()
{
  apiClient->offMicro();
}
