#include "plot3b.h"
#include "ui_plot3b.h"
#include "memory.h"

Plot3B::Plot3B(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Plot3B)
{
    ui->setupUi(this);
/*    pgl = new PlotGl();
    connect(pgl,&PlotGl::xRotationChanged,this,&Plot3B::xRotationChanged);
    connect(pgl,&PlotGl::yRotationChanged,this,&Plot3B::yRotationChanged);
    connect(pgl,&PlotGl::zRotationChanged,this,&Plot3B::zRotationChanged);*/
}

void Plot3B::setType(QString typeName){
    tName = typeName;
//    ui->controlBlock->addWidget(pgl);
    ui->SlideScale->setValue(Memory::get(tName+"Scale3b",0).toInt());
    ui->SlideX->setValue(Memory::get(tName+"SLideX3b",0).toInt());
    ui->SlideY->setValue(Memory::get(tName+"SLideY3b",0).toInt());
    ui->SlideZ->setValue(Memory::get(tName+"SLideZ3b",0).toInt());
}


Plot3B::~Plot3B()
{
    delete ui;
}

void Plot3B::on_SlideScale_valueChanged(int value)
{
    Memory::set(tName+"Scale3b",value);
    emit sync();
}

void Plot3B::on_SlideX_valueChanged(int value)
{
    Memory::set(tName+"SLideX3b",value);
    emit sync();
}

void Plot3B::on_SlideY_valueChanged(int value)
{
    Memory::set(tName+"SLideY3b",value);
    emit sync();
}

void Plot3B::on_SlideZ_valueChanged(int value)
{
    Memory::set(tName+"SLideZ3b",value);
    emit sync();
}
void Plot3B::xRotationChanged(int val){
    ui->SlideX->setValue(val);
    Memory::set(tName+"SLideX3b",val);
    emit sync();
}
void Plot3B::yRotationChanged(int val){
    ui->SlideY->setValue(val);
    Memory::set(tName+"SLideY3b",val);
    emit sync();
}
void Plot3B::zRotationChanged(int val){
    ui->SlideZ->setValue(val);
    Memory::set(tName+"SLideZ3b",val);
    emit sync();
}
