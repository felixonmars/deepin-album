#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "mainwindow.h"

#include <QTestEventList>

TEST(MainWindow, BtnGroupClick)
{
    MainWindow *w = dApp->getMainWindow();
    QTestEventList event;
    event.addMouseClick(Qt::MouseButton::LeftButton);
    event.simulate(w->getButG()->button(1));
    event.simulate(w->getButG()->button(2));
    event.clear();

}