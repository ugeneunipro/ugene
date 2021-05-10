//#include "qt.core"
//#include "qt.gui"
//#include "QTest"
//#separateThreads=false
//#exitOnException=true
//#exitOnEnding=true
print("Start testing...")
print("Test:in workflow designer setting menu check Box save value after close")
Test_path = TestHelper.getEnvValue("TEST_PATH");
print(Test_path);
TestHelper.isDebug = true;
main_delay = 100;

chf1 = 0;
chf2 = 0;
chf3 = 0;

CheckCheckBox = function (currentCheckBox) {
    for (i = 0; i != 1; i++) {
        if (currentCheckBox.checkState() == 2) {
            QTest.mouseMove(currentCheckBox, new QPoint(1, 1), main_delay);
            QTest.mouseClick(currentCheckBox, 1, 0, new QPoint(1, 1), main_delay);
            if (currentCheckBox.checkState() == 2) {
                print(currentCheckBox.objectName + " cheked");
                throw currentCheckBox.objectName + " cheked";
            }
        } else {
            QTest.mouseMove(currentCheckBox, new QPoint(1, 1), main_delay);
            QTest.mouseClick(currentCheckBox, 1, 0, new QPoint(1, 1), main_delay);
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
            //settingBox.settingsBox.LogSettingsWidget
            //settingBox.settingsBox.UserApplicationSettingsWidget
            //settingBox.settingsBox.ResourceSettingsWidget
            //settingBox.settingsBox.WorkflowSettingsWidget<------
            if (settingBox.settingsBox.WorkflowSettingsWidget) {
                print("WorkflowSettingsWidget is Visible");
                normalEnd = 1;
                break;
            } else {
                QTest.keyClick(treeW, Qt.Key_Tab, Qt.ControlModifier, main_delay);
            }
            ;
        }
        if (!normalEnd) {
            print("WorkflowSettingsWidget is not find");
            throw "WorkflowSettingsWidget is not find";
        }
//test of form's elements-----------------------------------------------------------------
        WorkflowSettingsW = settingBox.settingsBox.WorkflowSettingsWidget;

        currentCheckBox = WorkflowSettingsW.groupBox.gridBox;
        if (chf1 == 0) {
            if (currentCheckBox.checkState() == 2) {
                chf1 = 1;
            } else {
                chf1 = 2;
            }
        } else {
            if (currentCheckBox.checkState() == 2 && chf1 == 1) {
                print(currentCheckBox.objectName + " is checked expected not checked");
                throw currentCheckBox.objectName + " is checked expected not checked";
            }
            if (currentCheckBox.checkState() != 2 && chf1 == 2) {
                print(currentCheckBox.objectName + " is not checked expected checked");
                throw currentCheckBox.objectName + " is not checked expected checked";
            }
        }
        CheckCheckBox(currentCheckBox);
        currentCheckBox = WorkflowSettingsW.groupBox.lockBox;
        if (chf2 == 0) {
            if (currentCheckBox.checkState() == 2) {
                chf2 = 1;
            } else {
                chf2 = 2;
            }
        } else {
            if (currentCheckBox.checkState() == 2 && chf2 == 1) {
                print(currentCheckBox.objectName + " is checked expected not checked");
                throw currentCheckBox.objectName + " is checked expected not checked";
            }
            if (currentCheckBox.checkState() != 2 && chf2 == 2) {
                print(currentCheckBox.objectName + " is not checked expected checked");
                throw currentCheckBox.objectName + " is not checked expected checked";
            }
        }
        CheckCheckBox(currentCheckBox);
        currentCheckBox = WorkflowSettingsW.groupBox.snapBox;
        if (chf3 == 0) {
            if (currentCheckBox.checkState() == 2) {
                chf3 = 1;
            } else {
                chf3 = 2;
            }
        } else {
            if (currentCheckBox.checkState() == 2 && chf3 == 1) {
                print(currentCheckBox.objectName + " is checked expected not checked");
                throw currentCheckBox.objectName + " is checked expected not checked";
            }
            if (currentCheckBox.checkState() != 2 && chf3 == 2) {
                print(currentCheckBox.objectName + " is not checked expected checked");
                throw currentCheckBox.objectName + " is not checked expected checked";
            }
        }
        CheckCheckBox(currentCheckBox);
//-------------------------------------------------------------------------------------------
        QTest.mouseMove(settingBox.okButton, new QPoint(0, 0), main_delay);
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