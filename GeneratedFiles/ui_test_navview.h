/********************************************************************************
** Form generated from reading UI file 'test_navview.ui'
**
** Created: Mon Jan 26 10:25:58 2015
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEST_NAVVIEW_H
#define UI_TEST_NAVVIEW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>
#include "navview.h"

QT_BEGIN_NAMESPACE

class Ui_test_NavViewClass
{
public:
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    NavView *listView;
    QFrame *frame;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;

    void setupUi(QMainWindow *test_NavViewClass)
    {
        if (test_NavViewClass->objectName().isEmpty())
            test_NavViewClass->setObjectName(QString::fromUtf8("test_NavViewClass"));
        test_NavViewClass->resize(930, 650);
        centralWidget = new QWidget(test_NavViewClass);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        listView = new NavView(centralWidget);
        listView->setObjectName(QString::fromUtf8("listView"));
        listView->setMaximumSize(QSize(200, 16777215));
        listView->setFrameShape(QFrame::NoFrame);

        horizontalLayout->addWidget(listView);

        frame = new QFrame(centralWidget);
        frame->setObjectName(QString::fromUtf8("frame"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(frame->sizePolicy().hasHeightForWidth());
        frame->setSizePolicy(sizePolicy);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);

        horizontalLayout->addWidget(frame);

        test_NavViewClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(test_NavViewClass);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 930, 23));
        test_NavViewClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(test_NavViewClass);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        test_NavViewClass->addToolBar(Qt::TopToolBarArea, mainToolBar);

        retranslateUi(test_NavViewClass);

        QMetaObject::connectSlotsByName(test_NavViewClass);
    } // setupUi

    void retranslateUi(QMainWindow *test_NavViewClass)
    {
        test_NavViewClass->setWindowTitle(QApplication::translate("test_NavViewClass", "test_NavView", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class test_NavViewClass: public Ui_test_NavViewClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEST_NAVVIEW_H
