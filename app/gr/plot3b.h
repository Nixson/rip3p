#ifndef PLOT3B_H
#define PLOT3B_H

#include <QWidget>
#include "typelist.h"

namespace Ui {
class Plot3B;
}

class Plot3B : public QWidget
{
    Q_OBJECT

public:
    explicit Plot3B(QWidget *parent = 0);
    ~Plot3B();
    void setType(QString type);
   // void setBuf(Clowd &Ar, Clowd &Ph);
   // void shared(int);
signals:
    void sync();
public slots:
    void xRotationChanged(int);
    void yRotationChanged(int);
    void zRotationChanged(int);
private slots:
    void on_SlideScale_valueChanged(int value);

    void on_SlideX_valueChanged(int value);

    void on_SlideY_valueChanged(int value);

    void on_SlideZ_valueChanged(int value);

private:
    Ui::Plot3B *ui;

    QString tName;
};

#endif // PLOT3B_H
