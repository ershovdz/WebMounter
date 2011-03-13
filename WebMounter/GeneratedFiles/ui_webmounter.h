/********************************************************************************
** Form generated from reading UI file 'webmounter.ui'
**
** Created: Sun 13. Mar 15:17:48 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WEBMOUNTER_H
#define UI_WEBMOUNTER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WebDiskClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *WebDiskClass)
    {
        if (WebDiskClass->objectName().isEmpty())
            WebDiskClass->setObjectName(QString::fromUtf8("WebDiskClass"));
        WebDiskClass->resize(600, 400);
        menuBar = new QMenuBar(WebDiskClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        WebDiskClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(WebDiskClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        WebDiskClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(WebDiskClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        WebDiskClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(WebDiskClass);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        WebDiskClass->setStatusBar(statusBar);

        retranslateUi(WebDiskClass);

        QMetaObject::connectSlotsByName(WebDiskClass);
    } // setupUi

    void retranslateUi(QMainWindow *WebDiskClass)
    {
        WebDiskClass->setWindowTitle(QApplication::translate("WebDiskClass", "WebDisk", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class WebDiskClass: public Ui_WebDiskClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WEBMOUNTER_H
