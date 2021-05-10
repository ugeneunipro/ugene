//#include "qt.core"
//#include "qt.gui"
//#include "QTest"
//#separateThreads=false
//#exitOnException=true
//#exitOnEnding=true
print("Start testing...")
print("Test: settings menu-> NetworkSettings, proxy Edit area enabled after check Box Checked")
Test_path = TestHelper.getEnvValue("TEST_PATH");
print(Test_path);
TestHelper.isDebug = true;
main_delay = 100;

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
        print("Box: " + NetworkSettingsW.proxyGroup.httpProxyCheck);
//press httpProxy checkbox
        for (i = 0; i != 2; i++) {
            if (NetworkSettingsW.proxyGroup.httpProxyCheck.checkState() == 2) {
                QTest.mouseMove(NetworkSettingsW.proxyGroup.httpProxyCheck, new QPoint(0, 0), main_delay);
                QTest.mouseClick(NetworkSettingsW.proxyGroup.httpProxyCheck, 1, 0, 0, main_delay);
                if (NetworkSettingsW.proxyGroup.httpProxyCheck.checkState() == 2) {
                    print("httpProxyCheck cheked");
                    throw "httpProxyCheck cheked";
                }
                if (NetworkSettingsW.proxyGroup.httpProxyPortEdit.enabled) {
                    print("httpProxyCheck enabled after check");
                    throw "httpProxyCheck enabled after check";
                }
            } else {
                QTest.mouseMove(NetworkSettingsW.proxyGroup.httpProxyCheck, new QPoint(0, 0), main_delay);
                QTest.mouseClick(NetworkSettingsW.proxyGroup.httpProxyCheck, 1, 0, 0, main_delay);
                if (NetworkSettingsW.proxyGroup.httpProxyCheck.checkState() != 2) {
                    print("httpProxyCheck not cheked");
                    throw "httpProxyCheck not  cheked";
                }
                if (!NetworkSettingsW.proxyGroup.httpProxyPortEdit.enabled) {
                    print("httpProxyCheck desabled after check");
                    throw "httpProxyCheck desabled after check";
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