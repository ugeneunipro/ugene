//#include "qt.core"
//#include "qt.gui"
//#include "QTest"
//#separateThreads=false
//#exitOnException=true
//#exitOnEnding=true
print("Start testing...")
print("Test: setting menu -> User settings, on click radio button -EditLine is enabled or desabled")
Test_path = TestHelper.getEnvValue("TEST_PATH");
print(Test_path);
TestHelper.isDebug = true;
main_delay = 100;

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
        for (i = 0; i != 2; i++) {
            if (UserApplicationSettingsW.groupBox.defaultWebBrowser.checked) {
                if (UserApplicationSettingsW.groupBox.webBrowserEdit.enabled) {
                    print("webBrowserEdit enabled");
                    throw "webBrowserEdit enabled";
                }
                if (UserApplicationSettingsW.groupBox.webBrowserButton.enabled) {
                    print("webBrowserButton enabled");
                    throw "webBrowserButton enabled";
                }
                QTest.mouseMove(UserApplicationSettingsW.groupBox.customWebBrowser, new QPoint(1, 1), main_delay);
                QTest.mouseClick(UserApplicationSettingsW.groupBox.customWebBrowser, 1, 0, new QPoint(1, 1), main_delay);
                if (!UserApplicationSettingsW.groupBox.webBrowserEdit.enabled) {
                    print("webBrowserEdit desabled");
                    throw "webBrowserEdit desabled";
                }
                if (!UserApplicationSettingsW.groupBox.webBrowserButton.enabled) {
                    print("webBrowserButton desabled");
                    throw "webBrowserButton desabled";
                }
            } else {
                if (!UserApplicationSettingsW.groupBox.webBrowserEdit.enabled) {
                    print("webBrowserEdit desabled");
                    throw "webBrowserEdit desabled";
                }
                if (!UserApplicationSettingsW.groupBox.webBrowserButton.enabled) {
                    print("webBrowserButton desabled");
                    throw "webBrowserButton desabled";
                }
                QTest.mouseMove(UserApplicationSettingsW.groupBox.defaultWebBrowser, new QPoint(1, 1), main_delay);
                QTest.mouseClick(UserApplicationSettingsW.groupBox.defaultWebBrowser, 1, 0, new QPoint(1, 1), main_delay);
                if (UserApplicationSettingsW.groupBox.webBrowserEdit.enabled) {
                    print("webBrowserEdit enabled");
                    throw "webBrowserEdit enabled";
                }
                if (UserApplicationSettingsW.groupBox.webBrowserButton.enabled) {
                    print("webBrowserButton enabled");
                    throw "webBrowserButton enabled";
                }
            }
        }
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