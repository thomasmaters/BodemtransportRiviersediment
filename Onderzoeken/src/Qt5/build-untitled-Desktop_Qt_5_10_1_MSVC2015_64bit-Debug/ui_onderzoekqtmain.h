/********************************************************************************
** Form generated from reading UI file 'onderzoekqtmain.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ONDERZOEKQTMAIN_H
#define UI_ONDERZOEKQTMAIN_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_OnderzoekQtMain
{
public:
    QWidget *centralWidget;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QWidget *horizontalLayoutWidget_3;
    QHBoxLayout *horizontalLayout_3;
    QPushButton *pushButton;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *OnderzoekQtMain)
    {
        if (OnderzoekQtMain->objectName().isEmpty())
            OnderzoekQtMain->setObjectName(QStringLiteral("OnderzoekQtMain"));
        OnderzoekQtMain->resize(1347, 411);
        centralWidget = new QWidget(OnderzoekQtMain);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        horizontalLayoutWidget = new QWidget(centralWidget);
        horizontalLayoutWidget->setObjectName(QStringLiteral("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(0, 40, 1281, 281));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayoutWidget_2 = new QWidget(centralWidget);
        horizontalLayoutWidget_2->setObjectName(QStringLiteral("horizontalLayoutWidget_2"));
        horizontalLayoutWidget_2->setGeometry(QRect(0, 0, 1281, 41));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(horizontalLayoutWidget_2);
        label->setObjectName(QStringLiteral("label"));
        label->setAlignment(Qt::AlignCenter);

        horizontalLayout_2->addWidget(label);

        horizontalLayoutWidget_3 = new QWidget(centralWidget);
        horizontalLayoutWidget_3->setObjectName(QStringLiteral("horizontalLayoutWidget_3"));
        horizontalLayoutWidget_3->setGeometry(QRect(0, 320, 1281, 41));
        horizontalLayout_3 = new QHBoxLayout(horizontalLayoutWidget_3);
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(0, 0, 0, 0);
        pushButton = new QPushButton(horizontalLayoutWidget_3);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        horizontalLayout_3->addWidget(pushButton);

        OnderzoekQtMain->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(OnderzoekQtMain);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 1347, 21));
        OnderzoekQtMain->setMenuBar(menuBar);
        mainToolBar = new QToolBar(OnderzoekQtMain);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        OnderzoekQtMain->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(OnderzoekQtMain);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        OnderzoekQtMain->setStatusBar(statusBar);

        retranslateUi(OnderzoekQtMain);

        QMetaObject::connectSlotsByName(OnderzoekQtMain);
    } // setupUi

    void retranslateUi(QMainWindow *OnderzoekQtMain)
    {
        OnderzoekQtMain->setWindowTitle(QApplication::translate("OnderzoekQtMain", "OnderzoekQtMain", nullptr));
        label->setText(QApplication::translate("OnderzoekQtMain", "TEST", nullptr));
        pushButton->setText(QApplication::translate("OnderzoekQtMain", "TEST", nullptr));
    } // retranslateUi

};

namespace Ui {
    class OnderzoekQtMain: public Ui_OnderzoekQtMain {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ONDERZOEKQTMAIN_H
