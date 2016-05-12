#include "plotraster.h"
#include "ui_plotraster.h"
#include "memory.h"

PlotRaster::PlotRaster(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PlotRaster)
{
    ui->setupUi(this);
}

PlotRaster::~PlotRaster()
{
    delete ui;
}

void PlotRaster::on_rbAmp_toggled(bool checked)
{
    Memory::set("rbAmp",checked);
}

void PlotRaster::on_rbPh_toggled(bool checked)
{
    Memory::set("rbPh",checked);
}
