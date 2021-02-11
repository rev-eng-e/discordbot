/********************************************************************************
** Form generated from reading UI file 'botsetupui.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BOTSETUPUI_H
#define UI_BOTSETUPUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTreeWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_botsetupui
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout;
    QLabel *botsFileLabel;
    QLineEdit *botsFileEdit;
    QToolButton *botsFileBrowse;
    QLabel *botNameLabel;
    QLineEdit *botNameEdit;
    QLabel *botKeyLabel;
    QLineEdit *botKeyEdit;
    QCheckBox *tts;
    QToolButton *botAddButton;
    QToolButton *botRemoveButton;
    QTreeWidget *bots;

    void setupUi(QMainWindow *botsetupui)
    {
        if (botsetupui->objectName().isEmpty())
            botsetupui->setObjectName(QString::fromUtf8("botsetupui"));
        botsetupui->resize(782, 465);
        centralwidget = new QWidget(botsetupui);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout = new QVBoxLayout(centralwidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        botsFileLabel = new QLabel(centralwidget);
        botsFileLabel->setObjectName(QString::fromUtf8("botsFileLabel"));

        verticalLayout->addWidget(botsFileLabel);

        botsFileEdit = new QLineEdit(centralwidget);
        botsFileEdit->setObjectName(QString::fromUtf8("botsFileEdit"));

        verticalLayout->addWidget(botsFileEdit);

        botsFileBrowse = new QToolButton(centralwidget);
        botsFileBrowse->setObjectName(QString::fromUtf8("botsFileBrowse"));

        verticalLayout->addWidget(botsFileBrowse);

        botNameLabel = new QLabel(centralwidget);
        botNameLabel->setObjectName(QString::fromUtf8("botNameLabel"));

        verticalLayout->addWidget(botNameLabel);

        botNameEdit = new QLineEdit(centralwidget);
        botNameEdit->setObjectName(QString::fromUtf8("botNameEdit"));

        verticalLayout->addWidget(botNameEdit);

        botKeyLabel = new QLabel(centralwidget);
        botKeyLabel->setObjectName(QString::fromUtf8("botKeyLabel"));

        verticalLayout->addWidget(botKeyLabel);

        botKeyEdit = new QLineEdit(centralwidget);
        botKeyEdit->setObjectName(QString::fromUtf8("botKeyEdit"));

        verticalLayout->addWidget(botKeyEdit);

        tts = new QCheckBox(centralwidget);
        tts->setObjectName(QString::fromUtf8("tts"));

        verticalLayout->addWidget(tts);

        botAddButton = new QToolButton(centralwidget);
        botAddButton->setObjectName(QString::fromUtf8("botAddButton"));

        verticalLayout->addWidget(botAddButton);

        botRemoveButton = new QToolButton(centralwidget);
        botRemoveButton->setObjectName(QString::fromUtf8("botRemoveButton"));

        verticalLayout->addWidget(botRemoveButton);

        bots = new QTreeWidget(centralwidget);
        bots->setObjectName(QString::fromUtf8("bots"));
        bots->setIndentation(0);
        bots->header()->setMinimumSectionSize(200);
        bots->header()->setDefaultSectionSize(200);

        verticalLayout->addWidget(bots);

        botsetupui->setCentralWidget(centralwidget);

        retranslateUi(botsetupui);

        QMetaObject::connectSlotsByName(botsetupui);
    } // setupUi

    void retranslateUi(QMainWindow *botsetupui)
    {
        botsetupui->setWindowTitle(QCoreApplication::translate("botsetupui", "Bot Setup", nullptr));
        botsFileLabel->setText(QCoreApplication::translate("botsetupui", "Bots File:", nullptr));
        botsFileBrowse->setText(QCoreApplication::translate("botsetupui", "Browse", nullptr));
        botNameLabel->setText(QCoreApplication::translate("botsetupui", "Bot Name:", nullptr));
        botKeyLabel->setText(QCoreApplication::translate("botsetupui", "Bot Key:", nullptr));
        tts->setText(QCoreApplication::translate("botsetupui", "TTS (Text To Speech)", nullptr));
        botAddButton->setText(QCoreApplication::translate("botsetupui", "Add/Update", nullptr));
        botRemoveButton->setText(QCoreApplication::translate("botsetupui", "Remove Selected", nullptr));
        QTreeWidgetItem *___qtreewidgetitem = bots->headerItem();
        ___qtreewidgetitem->setText(2, QCoreApplication::translate("botsetupui", "TTS", nullptr));
        ___qtreewidgetitem->setText(1, QCoreApplication::translate("botsetupui", "Bot Key", nullptr));
        ___qtreewidgetitem->setText(0, QCoreApplication::translate("botsetupui", "Bot Name", nullptr));
    } // retranslateUi

};

namespace Ui {
    class botsetupui: public Ui_botsetupui {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BOTSETUPUI_H
