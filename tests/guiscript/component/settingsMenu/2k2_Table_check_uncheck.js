//#include "qt.core"
//#include "qt.gui"
//#include "QTest"
//#separateThreads=false
//#exitOnException=true
//#exitOnEnding=true
print("Start testing...")
print("Test : setting menu->log settins,check Boxs in Qtable, save it's value after close")
Test_path = TestHelper.getEnvValue("TEST_PATH");
print(Test_path);
TestHelper.isDebug = true;
main_delay = 100;

ni = 0;
oldMainValue = new Array(4);
oldValues_trace = 0;
oldValues_details = 0;
oldValues_info = 0;
oldValues_errors = 0;
CheckCheckBox = function (currentCheckBox) {
    for (i = 0; i != 2; i++) {
        if (currentCheckBox.checkState() == 2) {
            QTest.mouseMove(currentCheckBox, new QPoint(0, 0), main_delay);
            QTest.mouseClick(currentCheckBox, 1, 0, 0, main_delay);
            if (currentCheckBox.checkState() == 2) {
                print(currentCheckBox.objectName + " cheked");
                throw currentCheckBox.objectName + " cheked";
            }
        } else {
            QTest.mouseMove(currentCheckBox, new QPoint(0, 0), main_delay);
            QTest.mouseClick(currentCheckBox, 1, 0, 0, main_delay);
            if (currentCheckBox.checkState() != 2) {
                print(currentCheckBox.objectName + " not cheked");
                throw currentCheckBox.objectName + " not cheked";
            }
        }
    }
}

SettingsTime = function (settingBox) {
    if (settingBox) {
        print("current Dialog Box: " + settingBox + " " + settingBox.objectName);

        //children_one=settingBox.children();
        // for(i in children_one){
        //    print("children "+i+" : "+children_one[i]+" "+children_one[i].objectName)
        // }
        treeW = settingBox.tree;
        normalEnd = 0;
        for (i = 0; i != treeW.topLevelItemCount; i++) {
            treewidget_last = treeW.topLevelItem(i);
            print("elem:" + treewidget_last.text(0) + ":");
            //settingBox.settingsBox.NetworkSettingsWidget
            //settingBox.settingsBox.LogSettingsWidget<------
            //settingBox.settingsBox.UserApplicationSettingsWidget
            //settingBox.settingsBox.ResourceSettingsWidget
            //settingBox.settingsBox.WorkflowSettingsWidget
            if (settingBox.settingsBox.LogSettingsWidget) {
                print("LogSettingsWidget is Visible");
                normalEnd = 1;
                break;
            } else {
                QTest.keyClick(treeW, Qt.Key_Tab, Qt.ControlModifier, main_delay);
            }
            ;
        }
        if (!normalEnd) {
            print("LogSettingsWidget is not find");
            throw "LogSettingsWidget is not find";
        }
//test of form's elements-----------------------------------------------------------------
        LogSettingsW = settingBox.settingsBox.LogSettingsWidget;

//check box table testing
        treeW = LogSettingsW.tableWidget;
        viewArea = treeW.qt_scrollarea_viewport;

        if (ni == 0) {
            oldValues_trace = new Array(treeW.rowCount - 1);
            oldValues_details = new Array(treeW.rowCount - 1);
            oldValues_info = new Array(treeW.rowCount - 1);
            oldValues_errors = new Array(treeW.rowCount - 1);
        } else {
            oldValueMassiv = 0
            for (column_i = 1; column_i != treeW.columnCount; column_i++) {
                if (column_i == 1) {
                    oldValueMassiv = oldValues_trace
                }
                if (column_i == 2) {
                    oldValueMassiv = oldValues_details
                }
                if (column_i == 3) {
                    oldValueMassiv = oldValues_info
                }
                if (column_i == 4) {
                    oldValueMassiv = oldValues_errors
                }
                for (row_i = 1; row_i != treeW.rowCount; row_i++) {
                    item_last = treeW.item(row_i, column_i);

                    if (item_last.checkState() == 2) {
                        if (oldValueMassiv[row_i - 1] == 1) {
                            print("wrong value of item: " + row_i + "," + column_i);
                            throw "wrong value of item: " + row_i + "," + column_i;
                        }
                    } else {
                        if (oldValueMassiv[row_i - 1] == 0) {
                            print("wrong value of item: " + row_i + "," + column_i);
                            throw "wrong value of item: " + row_i + "," + column_i;
                        }
                    }
                    Rect = treeW.visualItemRect(item_last);
                    pointOne = new QPoint(Rect.x() + 15, Rect.y() + 10);
                    QTest.mouseMove(viewArea, pointOne, main_delay);
                    QTest.mouseClick(viewArea, 1, 0, pointOne, main_delay);
                }
            }
        }
//========main check box checked===========
        for (di = 1; di != treeW.columnCount; di++) {
            curW = treeW.cellWidget(0, di);
            childrenOfCurEl = curW.children();
            chekrigik = childrenOfCurEl[0];
            print("chekrigik: " + chekrigik)
            if (ni == 0) {
                if (!chekrigik.checked) {
                    oldMainValue[di] = 0;
                    QTest.mouseMove(chekrigik, new QPoint(1, 1), main_delay);
                    QTest.mouseClick(chekrigik, 1, 0, new QPoint(1, 1), main_delay);
                } else {
                    oldMainValue[di] = 1;
                }
            } else {
                if (!chekrigik.checked) {
                    print("item 0," + di + " not checked");
                    throw "item 0," + di + " not checked";
                }
                if (oldMainValue[di] == 0) {
                    QTest.mouseMove(chekrigik, new QPoint(1, 1), main_delay);
                    QTest.mouseClick(chekrigik, 1, 0, new QPoint(1, 1), main_delay);
                }
            }
        }
//=================================
        if (ni == 0) {
            oldValueMassiv = 0;
            for (column_i = 1; column_i != treeW.columnCount; column_i++) {
                if (column_i == 1) {
                    oldValueMassiv = oldValues_trace
                }
                if (column_i == 2) {
                    oldValueMassiv = oldValues_details
                }
                if (column_i == 3) {
                    oldValueMassiv = oldValues_info
                }
                if (column_i == 4) {
                    oldValueMassiv = oldValues_errors
                }
                for (row_i = 1; row_i != treeW.rowCount; row_i++) {
                    item_last = treeW.item(row_i, column_i);

                    if (item_last.checkState() == 2) {
                        oldValueMassiv[row_i - 1] = 1;
                    } else {
                        oldValueMassiv[row_i - 1] = 0;
                    }
                    Rect = treeW.visualItemRect(item_last);
                    pointOne = new QPoint(Rect.x() + 15, Rect.y() + 10);
                    QTest.mouseMove(viewArea, pointOne, main_delay);
                    QTest.mouseClick(viewArea, 1, 0, pointOne, main_delay);
                }
            }
        }
//-------------------------------------------------------------------------------------------
        QTest.mouseMove(settingBox.okButton, new QPoint(0, 0), 1000);
        QTest.mouseClick(settingBox.okButton, 1, 0, 0, main_delay);

    } else {
        throw "Object is undefined"
    }

}
for (ni = 0; ni != 2; ni++) {
    pos = TestHelper.findActionAt(main_window.mw_menu_bar, "mwmenu_settings");
    print(pos);
    if (!pos) {
        print("settings menu is not found");
        throw "settings menu is not found";
    }
    QTest.mouseMove(main_window.mw_menu_bar, pos, main_delay);
    QTest.mouseClick(main_window.mw_menu_bar, 1, 0, pos, main_delay);
    menu_sett = main_window.mw_menu_bar.actionAt(pos).menu();
    if (menu_sett.visible) {
        pos = 0;
        pos = TestHelper.findActionAt(menu_sett, "action__settings");//settings have no name
        if (!pos) {
            print("Action settungs is not found");
            throw "Action settungs is not found";
        }
        TestHelper.setWindowProc("AppSettingDialog", "SettingsTime", 2000)
        QTest.mouseMove(menu_sett, pos, main_delay);
        QTest.mouseClick(menu_sett, 1, 0, pos, main_delay);

        //QTest.qWait(2000);


    } else {
        print("settins menu is not visible");
        throw "settins menu is not visible";
    }
}