//#include "qt.core"
//#include "qt.gui"
//#include "QTest"
//#separateThreads=false
//#exitOnException=true
//#exitOnEnding=true
print("Start testing...")
print("Test 1:")
Test_path = TestHelper.getEnvValue("TEST_PATH");
print(Test_path);
TestHelper.isDebug = true;
main_delay = 100;

newCPUValue = 2;
newMemValue = 600;
newThreadValue = 5;

oldCPUValue = 0;
oldMemValue = 0;
oldThreadValue = 0;

clearVal = function (curElem1) {
    QTest.mouseMove(curElem1, new QPoint(3, 3), main_delay);
    QTest.mouseDClick(curElem1, 1, 0, new QPoint(3, 3), main_delay);
    i = 0
    while (curElem1.value) {
        i++
        QTest.keyClick(curElem1, Qt.Key_Backspace, 0, main_delay);
        QTest.keyClick(curElem1, Qt.Key_Delete, 0, main_delay);
        if (i == 10) break;
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
            //settingBox.settingsBox.ResourceSettingsWidget<------
            //settingBox.settingsBox.WorkflowSettingsWidget
            if (settingBox.settingsBox.ResourceSettingsWidget) {
                print("ResourceSettingsWidget is Visible");
                normalEnd = 1;
                break;
            } else {
                QTest.keyClick(treeW, Qt.Key_Tab, Qt.ControlModifier, main_delay);
            }
            ;
        }
        if (!normalEnd) {
            print("ResourceSettingsWidget is not find");
            throw "ResourceSettingsWidget is not find";
        }
//test of form's elements-----------------------------------------------------------------
        ResourceSettingsW = settingBox.settingsBox.ResourceSettingsWidget;

        curElem = ResourceSettingsW.groupBox.cpuBox;
        if (!curElem.enabled) {
            print(curElem.objectName + " desabled");
            throw curElem.objectName + " desabled";
        }
        if (!oldCPUValue) {
            oldCPUValue = curElem.value;
            clearVal(curElem);
            QTest.keyClicks(curElem, newCPUValue, 0, main_delay);
        } else {
            if (curElem.value != newCPUValue) {
                print(curElem.objectName + " wrong value");
                throw curElem.objectName + " wrong value";
            }
            clearVal(curElem);
            QTest.keyClicks(curElem, oldCPUValue, 0, main_delay);
        }

        curElem = ResourceSettingsW.groupBox.memBox;
        if (!curElem.enabled) {
            print(curElem.objectName + " desabled");
            throw curElem.objectName + " desabled";
        }

        if (!oldMemValue) {
            oldMemValue = curElem.value;
            clearVal(curElem);
            QTest.keyClicks(curElem, newMemValue, 0, main_delay);
        } else {
            if (curElem.value != newMemValue) {
                print(curElem.objectName + " wrong value");
                throw curElem.objectName + " wrong value";
            }
            clearVal(curElem);
            QTest.keyClicks(curElem, oldMemValue, 0, main_delay);
        }
        curElem = ResourceSettingsW.groupBox.threadBox;
        if (!curElem.enabled) {
            print(curElem.objectName + " desabled");
            throw curElem.objectName + " desabled";
        }
        if (!oldThreadValue) {
            oldThreadValue = curElem.value;
            clearVal(curElem);
            QTest.keyClicks(curElem, newThreadValue, 0, main_delay);
        } else {
            if (curElem.value != newThreadValue) {
                print(curElem.objectName + " wrong value");
                throw curElem.objectName + " wrong value";
            }
            clearVal(curElem);
            QTest.keyClicks(curElem, oldThreadValue, 0, main_delay);
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