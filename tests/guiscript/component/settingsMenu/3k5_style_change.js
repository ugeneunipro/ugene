//#include "qt.core"
//#include "qt.gui"
//#include "QTest"
//#separateThreads=false
//#exitOnException=true
//#exitOnEnding=true
print("Start testing...")
print("Test : settings -> User settings, save style value, after close settings")
Test_path = TestHelper.getEnvValue("TEST_PATH");
print(Test_path);
TestHelper.isDebug = true;
main_delay = 100;
valueToFind = "CDE";

oldOneValue = 0;
findValInComboBox = function (curComboBox, valToFind) {
    if (!curComboBox) {
        print("no widget");
        throw "no widget";
    }
    oldValue = curComboBox.currentText;
    if (curElem.currentText == valToFind) return;
    predIndex = -1;
    curIndex = -2;
    while (true) {
        QTest.keyClick(curComboBox, Qt.Key_Down, 0, main_delay);
        curIndex = curComboBox.currentIndex;
        if (valToFind == curComboBox.currentText) {
            return;
        }
        if (predIndex == curIndex) {
            break;
        }
        predIndex = curIndex;
    }
    while (true) {
        QTest.keyClick(curComboBox, Qt.Key_Up, 0, main_delay);
        curIndex = curComboBox.currentIndex;
        if (valToFind == curComboBox.currentText) {
            return;
        }
        if (predIndex == curIndex) {
            break;
        }
        predIndex = curIndex;
    }
    findValInComboBox(curComboBox, oldValue);
    print(valToFind + " not found in ComboBox: " + curComboBox.objectName);
    throw valToFind + " not found in ComboBox: " + curComboBox.objectName;
}

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
            //settingBox.settingsBox.LogSettingsWidget
            //settingBox.settingsBox.UserApplicationSettingsWidget<------
            //settingBox.settingsBox.ResourceSettingsWidget
            //settingBox.settingsBox.WorkflowSettingsWidget
            if (settingBox.settingsBox.UserApplicationsSettingsWidget) {
                print("UserApplicationSettingsWidget is Visible");
                normalEnd = 1;
                break;
            } else {
                QTest.keyClick(treeW, Qt.Key_Tab, Qt.ControlModifier, main_delay);
            }
            ;
        }
        if (!normalEnd) {
            print("UserApplicationSettingsWidget is not find");
            throw "UserApplicationSettingsWidget is not find";
        }
//test of form's elements-----------------------------------------------------------------
        UserApplicationSettingsW = settingBox.settingsBox.UserApplicationsSettingsWidget;
        curElem = UserApplicationSettingsW.groupBox_2.styleCombo;
        if (ni == 0) {
            oldOneValue = curElem.currentText;
            findValInComboBox(curElem, valueToFind);
        } else {
            findValInComboBox(curElem, oldOneValue);
        }
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