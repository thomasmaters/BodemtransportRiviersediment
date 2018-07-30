#ifndef ONDERZOEK3DCHART_H
#define ONDERZOEK3DCHART_H

#include <QWidget>
#include <QtDataVisualization/QtDataVisualization>
#include <QtDataVisualization/Q3DSurface>
#include <QtDataVisualization/QValue3DAxis>

class Onderzoek3Dchart : public QWidget
{
    Q_OBJECT
public:
    explicit Onderzoek3Dchart(QWidget *parent = nullptr);
    QtDataVisualization::Q3DSurface *surface;
    QWidget *container;

signals:

public slots:
};

#endif // ONDERZOEK3DCHART_H
