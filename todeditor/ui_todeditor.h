/********************************************************************************
** Form generated from reading UI file 'todeditor.ui'
**
** Created by: Qt User Interface Compiler version 5.9.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TODEDITOR_H
#define UI_TODEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TodEditor
{
public:
    QWidget *centralWidget;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuEdit;
    QMenu *menuTool;
    QMenu *menuHelp;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *TodEditor)
    {
        if (TodEditor->objectName().isEmpty())
            TodEditor->setObjectName(QStringLiteral("TodEditor"));
        TodEditor->resize(400, 300);
        centralWidget = new QWidget(TodEditor);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        TodEditor->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(TodEditor);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 400, 22));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuEdit = new QMenu(menuBar);
        menuEdit->setObjectName(QStringLiteral("menuEdit"));
        menuTool = new QMenu(menuBar);
        menuTool->setObjectName(QStringLiteral("menuTool"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        TodEditor->setMenuBar(menuBar);
        statusBar = new QStatusBar(TodEditor);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        TodEditor->setStatusBar(statusBar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuEdit->menuAction());
        menuBar->addAction(menuTool->menuAction());
        menuBar->addAction(menuHelp->menuAction());

        retranslateUi(TodEditor);

        QMetaObject::connectSlotsByName(TodEditor);
    } // setupUi

    void retranslateUi(QMainWindow *TodEditor)
    {
        TodEditor->setWindowTitle(QApplication::translate("TodEditor", "TodEditor", Q_NULLPTR));
        menuFile->setTitle(QApplication::translate("TodEditor", "File", Q_NULLPTR));
        menuEdit->setTitle(QApplication::translate("TodEditor", "Edit", Q_NULLPTR));
        menuTool->setTitle(QApplication::translate("TodEditor", "Tool", Q_NULLPTR));
        menuHelp->setTitle(QApplication::translate("TodEditor", "Help", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class TodEditor: public Ui_TodEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TODEDITOR_H
