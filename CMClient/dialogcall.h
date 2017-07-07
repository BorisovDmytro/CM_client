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

private:
  Ui::DialogCall *ui;
};

#endif // DIALOGCALL_H
