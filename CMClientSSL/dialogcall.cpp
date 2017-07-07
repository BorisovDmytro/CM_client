#include "dialogcall.h"
#include "ui_dialogcall.h"

DialogCall::DialogCall(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DialogCall)
{
  ui->setupUi(this);
}

DialogCall::~DialogCall()
{
  delete ui;
}

void DialogCall::setFrom(const QString &name)
{
  ui->label_from_name->setText(name);
}

void DialogCall::on_btn_success_clicked()
{
  accept();
}

void DialogCall::on_btn_cansel_clicked()
{
  reject();
}
