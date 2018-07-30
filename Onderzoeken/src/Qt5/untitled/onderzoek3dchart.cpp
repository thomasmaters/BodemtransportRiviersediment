#include "onderzoek3dchart.h"

using namespace QtDataVisualization;

Onderzoek3Dchart::Onderzoek3Dchart(QWidget *parent) : QWidget(parent)
{
    surface = new Q3DSurface();
    container = QWidget::createWindowContainer(surface);
    surface->setAxisX(new QValue3DAxis);
    surface->setAxisY(new QValue3DAxis);
    surface->setAxisZ(new QValue3DAxis);

    QSurfaceDataProxy* dataProxy = new QSurfaceDataProxy();
    QSurface3DSeries* dataSeries = new QSurface3DSeries(dataProxy);
    QSurfaceDataArray *dataArray = new QSurfaceDataArray;

    dataArray->reserve(100);
    for (int i = 0 ; i < 100 ; i++) {
       QSurfaceDataRow *newRow = new QSurfaceDataRow(100);
       // Keep values within range bounds, since just adding step can cause minor drift due
       // to the rounding errors.
       float z = i;
       int index = 0;
       for (int j = 0; j < 100; j++) {
           float x = j;
           float R = qSqrt(z * z + x * x) + 0.01f;
           float y = (qSin(R) / R + 0.24f) * 1.61f;
           (*newRow)[index++].setPosition(QVector3D(x, y, z));
       }
       *dataArray << newRow;
    }

    dataProxy->resetArray(dataArray);

    surface->addSeries(dataSeries);
}
