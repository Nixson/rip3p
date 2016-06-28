#ifndef GR3B_H
#define GR3B_H

#include <QtDataVisualization/q3dbars.h>
#include <QtDataVisualization/qbardataproxy.h>
#include <QtDataVisualization/qabstract3dseries.h>

#include <QtGui/QFont>
#include <QtCore/QDebug>
#include <QtCore/QStringList>
#include <QtCore/QPointer>
#include <QtCore/QPropertyAnimation>

using namespace QtDataVisualization;

class Gr3B : public QObject
{
    Q_OBJECT
public:
    explicit Gr3B(Q3DBars *bargraph);
    ~Gr3B();

signals:

public slots:
    void changePresetCamera();
    void resetTemperatureData();

private:
    Q3DBars *m_graph;
    float m_xRotation;
    float m_yRotation;
    int m_fontSize;
    int m_segments;
    int m_subSegments;
    float m_minval;
    float m_maxval;
    QStringList m_months;
    QStringList m_years;
    QValue3DAxis *m_temperatureAxis;
    QCategory3DAxis *m_yearAxis;
    QCategory3DAxis *m_monthAxis;
    QBar3DSeries *m_primarySeries;
    QBar3DSeries *m_secondarySeries;
    QAbstract3DSeries::Mesh m_barMesh;
    bool m_smooth;
    QPropertyAnimation m_animationCameraX;
    QPropertyAnimation m_animationCameraY;
    QPropertyAnimation m_animationCameraZoom;
    QPropertyAnimation m_animationCameraTarget;
    float m_defaultAngleX;
    float m_defaultAngleY;
    float m_defaultZoom;
    QVector3D m_defaultTarget;

};

#endif // GR3B_H
