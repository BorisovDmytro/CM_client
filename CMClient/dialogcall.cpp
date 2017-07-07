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
