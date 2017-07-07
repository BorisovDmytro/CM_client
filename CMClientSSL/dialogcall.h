#ifndef DIALOGCALL_H
#define DIALOGCALL_H

#include <QDialog>

namespace Ui {
  class DialogCall;
}

class DialogCall : public QDialog
{
  Q_OBJECT

public:
  explicit DialogCall(QWidget *parent = 0);
  ~DialogCall();

  void setFrom(const QString& name);
private slots:
  void on_btn_success_clicked();

  void on_btn_cansel_clicked();

private:
  Ui::DialogCall *ui;
};

#endif // DIALOGCALL_H
