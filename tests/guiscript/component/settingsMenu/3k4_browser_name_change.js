//#include "qt.core"
//#include "qt.gui"
//#include "QTest"
//#separateThreads=false
//#exitOnException=true
//#exitOnEnding=true
print("Start testing...")
print("Test: in settings menu find User settings, change path to browser in it. After test end change path to old value.")
Test_path = TestHelper.getEnvValue("TEST_PATH");
print(Test_path);
TestHelper.isDebug = true;
main_delay = 100;
pathToBrowser = "C:\\Program Files\\Internet Explorer\\IEXPLORE.EXE";

defaultBrowser = 0;
tempBrValue = 0;

modalWindowClose = function (modaW) {
    QTest.keyClick(modaW, Qt.Key_Enter, 0, main_delay);
}

EndTime = function (settingBox) {
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
        UserApplicationSettingsW = settingBox.settingsBox.UserApplicationsSettingsWidget;
        if (!UserApplicationSettingsW.groupBox.webBrowserEdit.enabled) {
            print("webBrowserEdit desabled");
            throw "webBrowserEdit desabled";
        }
        if (UserApplicationSettingsW.groupBox.webBrowserEdit.text != pathToBrowser) {
            print("wrong webBrowserEdit value");
            throw "wrong webBrowserEdit value";
        }
        if (tempBrValue) {
            curEditLine = UserApplicationSettingsW.groupBox.webBrowserEdit;
            while (curEditLine.text) {
                QTest.keyClick(curEditLine, Qt.Key_Backspace, 0, main_delay);
                QTest.keyClick(curEditLine, Qt.Key_Delete, 0, main_delay);
            }
            QTest.keyClicks(curEditLine, tempBrValue, 0, main_delay);
        }
        if (defaultBrowser) {
            QTest.mouseMove(UserApplicationSettingsW.groupBox.defaultWebBrowser, new QPoint(1, 1), main_delay);
            QTest.mouseClick(UserApplicationSettingsW.groupBox.defaultWebBrowser, 1, 0, new QPoint(1, 1), main_delay);
        }
    } else {
        throw "Object is undefined"
    }
    QTest.mouseMove(settingBox.okButton, new QPoint(0, 0), main_delay);
    QTest.mouseClick(settingBox.okButton, 1, 0, 0, main_delay);
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
//UserApplicationSettingsW.groupBox.defaultWebBrowser
//UserApplicationSettingsW.groupBox.customWebBrowser
//UserApplicationSettingsW.groupBox.webBrowserEdit
//UserApplicationSettingsW.groupBox.webBrowserButton
        wrongPath = "ggggg";
        if (!UserApplicationSettingsW.groupBox.webBrowserEdit.enabled) {
            defaultBrowser = 1;
            QTest.mouseMove(UserApplicationSettingsW.groupBox.customWebBrowser, new QPoint(1, 1), main_delay);
            QTest.mouseClick(UserApplicationSettingsW.groupBox.customWebBrowser, 1, 0, new QPoint(1, 1), main_delay);
            if (!UserApplicationSettingsW.groupBox.webBrowserEdit.enabled) {
                print("webBrowserEdit desabled");
                throw "webBrowserEdit desabled";
            }
        }
        if (UserApplicationSettingsW.groupBox.webBrowserEdit) {
            tempBrValue = new String(UserApplicationSettingsW.groupBox.webBrowserEdit.text);
            curEditLine = UserApplicationSettingsW.groupBox.webBrowserEdit;
            QTest.mouseDClick(curEditLine, 1, 0, 0, main_delay);
            while (curEditLine.text) {
                QTest.keyClick(curEditLine, Qt.Key_Backspace, 0, main_delay);
                QTest.keyClick(curEditLine, Qt.Key_Delete, 0, main_delay);
            }
        }
        QTest.keyClicks(curEditLine, wrongPath, 0, main_delay);
        QTest.mouseMove(settingBox.okButton, new QPoint(0, 0), main_delay);
        TestHelper.setWindowProc("modalWindow", "modalWindowClose", 1000)
        QTest.mouseClick(settingBox.okButton, 1, 0, 0, main_delay);
        QTest.mouseDClick(curEditLine, 1, 0, 0, main_delay);
        while (curEditLine.text) {
            QTest.keyClick(curEditLine, Qt.Key_Backspace, 0, main_delay);
            QTest.keyClick(curEditLine, Qt.Key_Delete, 0, main_delay);
        }
        QTest.keyClicks(curEditLine, pathToBrowser, 0, main_delay);
//-------------------------------------------------------------------------------------------
        QTest.mouseMove(settingBox.okButton, new QPoint(0, 0), main_delay);
        QTest.mouseClick(settingBox.okButton, 1, 0, 0, main_delay);

    } else {
        throw "Object is undefined"
    }

}

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
    TestHelper.setWindowProc("AppSettingDialog", "EndTime", 2000)
    QTest.mouseMove(menu_sett, pos, main_delay);
    QTest.mouseClick(menu_sett, 1, 0, pos, main_delay);

    //QTest.qWait(2000);


} else {
    print("settins menu is not visible");
    throw "settins menu is not visible";
}