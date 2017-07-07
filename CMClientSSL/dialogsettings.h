#ifndef DIALOGSETTINGS_H
#define DIALOGSETTINGS_H

#include <QDialog>

namespace Ui {
  class DialogSettings;
}

class DialogSettings : public QDialog
{
  Q_OBJECT

public:
  explicit DialogSettings(QWidget *parent = 0);
  ~DialogSettings();

  void setInputs(const QStringList inputs);

  QString getInput();
private slots:
  void on_buttonBox_accepted();

  void on_buttonBox_rejected();

private:
  Ui::DialogSettings *ui;
  QString mInput;
};

#endif // DIALOGSETTINGS_H
