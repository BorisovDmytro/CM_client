#include "dialogsettings.h"
#include "ui_dialogsettings.h"

DialogSettings::DialogSettings(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::DialogSettings)
{
  ui->setupUi(this);
}

DialogSettings::~DialogSettings()
{
  delete ui;
}

void DialogSettings::setInputs(const QStringList inputs)
{
  ui->comboBox->addItems(inputs);
}

QString DialogSettings::getInput()
{
  return mInput;
}

void DialogSettings::on_buttonBox_accepted()
{
  mInput = ui->comboBox->currentText();
  accept();
}

void DialogSettings::on_buttonBox_rejected()
{
  reject();
}
