/********************************************************************************
** Form generated from reading UI file 'mainapplication.ui'
**
** Created by: Qt User Interface Compiler version 5.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINAPPLICATION_H
#define UI_MAINAPPLICATION_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>
#include "qcustomplot.h"

QT_BEGIN_NAMESPACE

class Ui_MainApplication
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout;
    QCustomPlot *customplot;
    QCustomPlot *customplot2;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *MainApplication)
    {
        if (MainApplication->objectName().isEmpty())
            MainApplication->setObjectName(QStringLiteral("MainApplication"));
        MainApplication->resize(960, 593);
        centralWidget = new QWidget(MainApplication);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(centralWidget->sizePolicy().hasHeightForWidth());
        centralWidget->setSizePolicy(sizePolicy);
        centralWidget->setMinimumSize(QSize(960, 540));
        centralWidget->setMaximumSize(QSize(1920, 1080));
#ifndef QT_NO_WHATSTHIS
        centralWidget->setWhatsThis(QStringLiteral(""));
#endif // QT_NO_WHATSTHIS
        centralWidget->setLayoutDirection(Qt::LeftToRight);
        gridLayout_2 = new QGridLayout(centralWidget);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        customplot = new QCustomPlot(centralWidget);
        customplot->setObjectName(QStringLiteral("customplot"));
        customplot->setMinimumSize(QSize(0, 150));

        horizontalLayout->addWidget(customplot);


        gridLayout_2->addLayout(horizontalLayout, 0, 0, 1, 1);

        customplot2 = new QCustomPlot(centralWidget);
        customplot2->setObjectName(QStringLiteral("customplot2"));
        customplot2->setMinimumSize(QSize(0, 150));

        gridLayout_2->addWidget(customplot2, 1, 0, 1, 1);

        MainApplication->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(MainApplication);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 960, 21));
        MainApplication->setMenuBar(menuBar);
        mainToolBar = new QToolBar(MainApplication);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        MainApplication->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(MainApplication);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        MainApplication->setStatusBar(statusBar);

        retranslateUi(MainApplication);

        QMetaObject::connectSlotsByName(MainApplication);
    } // setupUi

    void retranslateUi(QMainWindow *MainApplication)
    {
        MainApplication->setWindowTitle(QApplication::translate("MainApplication", "MainApplication", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainApplication: public Ui_MainApplication {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINAPPLICATION_H
