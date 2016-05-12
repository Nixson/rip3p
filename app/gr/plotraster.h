#ifndef PLOTRASTER_H
#define PLOTRASTER_H

#include <QWidget>

namespace Ui {
class PlotRaster;
}

class PlotRaster : public QWidget
{
    Q_OBJECT

public:
    explicit PlotRaster(QWidget *parent = 0);
    ~PlotRaster();

private slots:
    void on_rbAmp_toggled(bool checked);

    void on_rbPh_toggled(bool checked);

private:
    Ui::PlotRaster *ui;
};

#endif // PLOTRASTER_H
