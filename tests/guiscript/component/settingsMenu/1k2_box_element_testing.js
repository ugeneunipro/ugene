//#include "qt.core"
//#include "qt.gui"
//#include "QTest"
//#separateThreads=false
//#exitOnException=true
//#exitOnEnding=true
print("Start testing...")
print("Test:settings menu-> NetworkSettings, change Proxy Exceptions list")
Test_path = TestHelper.getEnvValue("TEST_PATH");
print(Test_path);
TestHelper.isDebug = true;
main_delay = 100;
httpProxyValue = "www.googl.com";

checkedFlag = 0;
ProxyOldValue = 0;
ni = 0

SettingsTime = function (settingBox) {
    if (settingBox) {
        print("current Dialog Box: " + settingBox + " " + settingBox.objectName);

        //children_one=settingBox.children();
        //for(i in children_one){
        //    print("children "+i+" : "+children_one[i]+" "+children_one[i].objectName)
        //}
        treeW = settingBox.tree;
        normalEnd = 0;
        for (i = 0; i != treeW.topLevelItemCount; i++) {
            treewidget_last = treeW.topLevelItem(i);
            print("elem:" + treewidget_last.text(0) + ":");
            //settingBox.settingsBox.NetworkSettingsWidget<------
            //settingBox.settingsBox.LogSettingsWidget
            //settingBox.settingsBox.UserApplicationSettingsWidget
            //settingBox.settingsBox.ResourceSettingsWidget
            //settingBox.settingsBox.WorkflowSettingsWidget
            if (settingBox.settingsBox.NetworkSettingsWidget) {
                print("NetworkSettingsWidget is Visible");
                normalEnd = 1;
                break;
            } else {
                QTest.keyClick(treeW, Qt.Key_Tab, Qt.ControlModifier, main_delay);
            }
            ;
        }
        if (!normalEnd) {
            print("NetworkSettingsWidget is not find");
            throw "NetworkSettingsWidget is not find";
        }
//test of form's elements-----------------------------------------------------------------
        NetworkSettingsW = settingBox.settingsBox.NetworkSettingsWidget;
        print("Box: " + NetworkSettingsW.proxyGroup.proxyExceptionsCheck);
//press ProxyExc checkbox
        if (checkedFlag == 0) {
            if (NetworkSettingsW.proxyGroup.proxyExceptionsCheck.checkState() != 2) {
                checkedFlag = 1;
                QTest.mouseMove(NetworkSettingsW.proxyGroup.proxyExceptionsCheck, new QPoint(0, 0), main_delay);
                QTest.mouseClick(NetworkSettingsW.proxyGroup.proxyExceptionsCheck, 1, 0, 0, main_delay);
                if (NetworkSettingsW.proxyGroup.proxyExceptionsCheck.checkState() != 2) {
                    print("proxyExceptionsCheck not cheked");
                    throw "proxyExceptionsCheck not cheked";
                }
            } else {
                checkedFlag = 2;
            }
        }
//set value prxy Exc
        sameString = 0;
        if (!ProxyOldValue) {
            sameString = "www.googl.com";
            ProxyOldValue = new String(NetworkSettingsW.proxyGroup.proxyExceptionsEdit.plainText)
        } else {
            sameString = ProxyOldValue;
        }
        curEditLine = NetworkSettingsW.proxyGroup.proxyExceptionsEdit;
        if (curEditLine.visible) {
            if (curEditLine.enabled) {
                QTest.mouseDClick(curEditLine, 1, 0, 0, main_delay);
                while (curEditLine.plainText) {
                    QTest.keyClick(curEditLine, Qt.Key_Backspace, 0, main_delay);
                    QTest.keyClick(curEditLine, Qt.Key_Delete, 0, main_delay);
                }
                QTest.keyClicks(curEditLine, sameString, 0, main_delay);
                if (curEditLine.plainText != sameString) {
                    print("proxyExceptionsEdit text is not correct");
                    throw "proxyExceptionsEdit text is not correct";
                }
            } else {
                print("proxyExceptionsEdit is not enabled");
                throw "proxyExceptionsEdit is not enabled";
            }
        } else {
            print("proxyExceptionsEdit is not visible");
            throw "proxyExceptionsEdit is not visible";
        }
        if (checkedFlag != 0) {
            if (NetworkSettingsW.proxyGroup.proxyExceptionsCheck.checkState() != 2) {
                print("proxyExceptionsCheck not cheked");
                throw "proxyExceptionsCheck not cheked";
            }
            if (checkedFlag == 1 && ni) {
                QTest.mouseMove(NetworkSettingsW.proxyGroup.proxyExceptionsCheck, new QPoint(0, 0), main_delay);
                QTest.mouseClick(NetworkSettingsW.proxyGroup.proxyExceptionsCheck, 1, 0, 0, main_delay);
                if (NetworkSettingsW.proxyGroup.proxyExceptionsCheck.checkState() == 2) {
                    print("proxyExceptionsCheck cheked");
                    throw "proxyExceptionsCheck cheked";
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
