/********************************************************************************
** Form generated from reading UI file 'gigapixelapp.ui'
**
** Created by: Qt User Interface Compiler version 5.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GIGAPIXELAPP_H
#define UI_GIGAPIXELAPP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_gigapixelappClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *gigapixelappClass)
    {
        if (gigapixelappClass->objectName().isEmpty())
            gigapixelappClass->setObjectName(QStringLiteral("gigapixelappClass"));
        gigapixelappClass->resize(600, 400);
        menuBar = new QMenuBar(gigapixelappClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        gigapixelappClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(gigapixelappClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        gigapixelappClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(gigapixelappClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gigapixelappClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(gigapixelappClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        gigapixelappClass->setStatusBar(statusBar);

        retranslateUi(gigapixelappClass);

        QMetaObject::connectSlotsByName(gigapixelappClass);
    } // setupUi

    void retranslateUi(QMainWindow *gigapixelappClass)
    {
        gigapixelappClass->setWindowTitle(QApplication::translate("gigapixelappClass", "gigapixelapp", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class gigapixelappClass: public Ui_gigapixelappClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GIGAPIXELAPP_H
