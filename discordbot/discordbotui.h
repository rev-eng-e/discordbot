#ifndef DISCORDBOTUI_H
#define DISCORDBOTUI_H

#include <QMainWindow>
#include "botsetupui.h"
#include "eventviewerui.h"

QT_BEGIN_NAMESPACE
namespace Ui { class discordbotui; }
QT_END_NAMESPACE

class discordbotui : public QMainWindow
{
    Q_OBJECT
public:
    inline static ptr<discordbotui> botui=nullptr;
    ptr<botsetupui> botsetup=nullptr;
    ptr<eventviewerui> eventviewer=nullptr;
    discordbotui(QWidget *parent=nullptr);
    ~discordbotui();

private slots:
    void on_buttonSettings_clicked();
    void on_botSelect_currentIndexChanged(int index);
    void on_buttonEvents_clicked();

private:
    Ui::discordbotui *ui;
    void showBotSetup();
    void showEventViewer();
};
#endif // DISCORDBOTUI_H
