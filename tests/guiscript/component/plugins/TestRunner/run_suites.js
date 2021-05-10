//#include "qt.core"
//#include "qt.gui"
//#include "QTest"
//#separateThreads=false
//#exitOnException=true
//#exitOnEnding=true

Test_suite_name = TestHelper.getEnvValue("TEST_SUITE");
print(Test_suite_name);
if (Test_suite_name == null) {
    suite_names = new Array(//can't be empty
        "All.list"
    );
} else {
    suite_names = new Array(Test_suite_name);
}

env_names = new Array(//can't be empty
    "NUM_THREADS", "4",
    "TIME_OUT_VAR", "3600"
);
main_delay = 100;
//TestHelper.isDebug=true;

absolute_path = true;//use TEST_PATH when load suites
//Test_path="C:\\work\\trunk\\test\\";
Test_path = TestHelper.getEnvValue("TEST_PATH");
print(Test_path);
path_to_save_report = Test_path;

//set env if it need
EnvTime = function (setEnvEditBox) {
    if (setEnvEditBox) {
        print("setEnveditBox: " + setEnvEditBox);
        listOfC = setEnvEditBox.children();
        temp_end = 0;
        for (itemp in env_names) {
            for (i in listOfC) {
                if (listOfC[i].text == (env_names[itemp] + ":")) {
                    QTest.mouseDClick(listOfC[i - 1], 1);//select old value
                    QTest.keyClicks(listOfC[i - 1], env_names[itemp + 1]);//set new value
                    temp_end = i - 1;
                }
            }
            itemp++;
        }
        QTest.keyClick(listOfC[temp_end], Qt.Key_Enter);

    } else {
        throw "EnvEditBox is undefined"
    }

}

print("Start testing...")
print("Test :run all suites in test runner");
pos = 0;
pos = TestHelper.findActionAt(main_window.mw_menu_bar, "mwmenu_tools");
if (!pos) {
    print("Tools menu is not found");
    throw "Tools menu is not found";
}
QTest.mouseMove(main_window.mw_menu_bar, pos, main_delay);
QTest.mouseClick(main_window.mw_menu_bar, 1, 0, pos, main_delay);
menu_tools = main_window.mw_menu_bar.actionAt(pos).menu();
if (menu_tools.visible) {
    pos = 0;
    pos = TestHelper.findActionAt(menu_tools, "action__testrunner");
    if (!pos) {
        print("Test runner in Tools menu is not found");
        throw "Test runner in Tools menu is not found";
    }
    QTest.mouseMove(menu_tools, pos);
    QTest.mouseClick(menu_tools, 1, 0, pos, main_delay);
} else {
    print("Tools menu is not visible");
    throw "Tools menu is not visible";
}

//select all suites-------------------------------------
pos = 0;
pos = TestHelper.findActionAt(main_window.mw_menu_bar, "mwmenu_actions");
if (!pos) {
    print("Action menu is not found");
    throw "Action menu is not found";
}
QTest.mouseMove(main_window.mw_menu_bar, pos, main_delay);
QTest.mouseClick(main_window.mw_menu_bar, 1, 0, pos, main_delay);
menu_actions = main_window.mw_menu_bar.actionAt(pos).menu();
if (menu_actions.visible) {
    pos = 0;
    pos = TestHelper.findActionAt(menu_actions, "action_select_all");
    if (!pos) {
        print("select all in action menu is not found");
        throw "select all in action menu is not found";
    }
    QTest.mouseMove(menu_actions, pos);
    QTest.mouseClick(menu_actions, 1, 0, pos, main_delay);
} else {
    print("Action menu is not visible");
    throw "Action menu is not visible";
}
//get current MDI window(TestRunner)------------------------------------------
QTest.qWait(main_delay);
TestRunnerMDIWindow = main_window.MDI_Area.currentSubWindow();
if (TestRunnerMDIWindow.objectName != "TestView_SubWindow") {
    print("currentSubWindow is not TestView_SubWindow");
    throw "currentSubWindow is not TestView_SubWindow";
}
//remove all suites-------------------------------------

if (TestRunnerMDIWindow.TestView.splitter.tree.topLevelItemCount) {
    pos = 0;
    pos = TestHelper.findActionAt(main_window.mw_menu_bar, "mwmenu_actions");
    if (!pos) {
        print("Action menu is not found");
        throw "Action menu is not found";
    }
    QTest.mouseMove(main_window.mw_menu_bar, pos, main_delay);
    QTest.mouseClick(main_window.mw_menu_bar, 1, 0, pos, main_delay);
    menu_actions = main_window.mw_menu_bar.actionAt(pos).menu();
    if (menu_actions.visible) {
        pos = 0;
        pos = TestHelper.findActionAt(menu_actions, "action_remove_suite");
        if (!pos) {
            print("remove suite in action menu is not found");
            throw "remove suite in action menu is not found";
        }
        QTest.mouseMove(menu_actions, pos);
        QTest.mouseClick(menu_actions, 1, 0, pos, main_delay);
    } else {
        pos = 0;
        pos = TestHelper.findActionAt(main_window.mw_menu_bar, "mwmenu_actions");
        // QTest.mouseClick(main_window.mw_menu_bar,1,0,pos,main_delay);    
    }
}
//load test suites-------------------------------------
for (i in suite_names) {
    suite_name = suite_names[i];
    pos = 0;
    pos = TestHelper.findActionAt(main_window.mw_menu_bar, "mwmenu_actions");
    if (!pos) {
        print("Action menu is not found");
        throw "Action menu is not found";
    }
    QTest.mouseMove(main_window.mw_menu_bar, pos, main_delay);
    QTest.mouseClick(main_window.mw_menu_bar, 1, 0, pos, main_delay);
    menu_actions = main_window.mw_menu_bar.actionAt(pos).menu();
    if (menu_actions.visible) {
        pos = 0;
        pos = TestHelper.findActionAt(menu_actions, "action_add_suite");
        if (!pos) {
            print("add suite in action menu is not found");
            throw "add suite in action menu is not found";
        }
        if (absolute_path) {
            TestHelper.fileDialog(menu_actions, Test_path + suite_name, pos, 500);
        } else {
            TestHelper.fileDialog(menu_actions, suite_name, pos, 500);
        }
    } else {
        print("Action menu is not visible");
        throw "Action menu is not visible";
    }
}
//setup enviroment if it's need-------------------------------------

/* TODO: WTF?
    pos=0;
    pos=TestHelper.findActionAt(main_window.mw_menu_bar,"mwmenu_actions");
    if (!pos){
        print("Action menu is not found");
        throw "Action menu is not found";
    }   
    QTest.mouseMove(main_window.mw_menu_bar,pos,main_delay);
    QTest.mouseClick(main_window.mw_menu_bar,1,0,pos,main_delay);
    menu_actions=main_window.mw_menu_bar.actionAt(pos).menu();
    if(menu_actions.visible){
         pos=0;
         pos=TestHelper.findActionAt(menu_actions,"action_setup_environment");
         if (!pos){
            print("select all in action menu is not found");
            throw "select all in action menu is not found";
         }
         print("press set Env");
         TestHelper.setWindowProc("DialogName","EnvTime",2000)
         QTest.mouseMove(menu_actions,pos);
         QTest.mouseClick(menu_actions,1,0,pos,main_delay);
    }else{
        print("Action menu is not visible");
        throw "Action menu is not visible";
    }
*/

//run all test suites-------------------------------------
pos = 0;
pos = TestHelper.findActionAt(main_window.mw_menu_bar, "mwmenu_actions");
if (!pos) {
    print("Action menu is not found");
    throw "Action menu is not found";
}
QTest.mouseMove(main_window.mw_menu_bar, pos, main_delay);
QTest.mouseClick(main_window.mw_menu_bar, 1, 0, pos, main_delay);
menu_actions = main_window.mw_menu_bar.actionAt(pos).menu();
if (menu_actions.visible) {
    pos = 0;
    pos = TestHelper.findActionAt(menu_actions, "action_run_all_tests");
    if (!pos) {
        print("run all tests in action menu is not found");
        throw "run all tests in action menu is not found";
    }
    QTest.mouseMove(menu_actions, pos);
    QTest.mouseClick(menu_actions, 1, 0, pos, main_delay);
} else {
    print("File menu is not visible");
    throw "File menu is not visible";
}
//get tree widget and it's last elem. index-----------------------------------

treeW = TestRunnerMDIWindow.TestView.splitter.tree;
treewidget_last = treeW.topLevelItem(treeW.topLevelItemCount - 1);//index of item

//waiting for end------------------------------------------
for (end = false; end != true;) {
    QTest.qWait(5000);
    test_run_info = treewidget_last.text(1);
    index_numbers_of_idle_tests = test_run_info.indexOf("N") + 2;
    end = test_run_info.charAt(index_numbers_of_idle_tests) == "0";
}
//get report------------------------------------------
pos = 0;
pos = TestHelper.findActionAt(main_window.mw_menu_bar, "mwmenu_actions");
if (!pos) {
    print("Action menu is not found");
    throw "Action menu is not found";
}
QTest.mouseMove(main_window.mw_menu_bar, pos, main_delay);
QTest.mouseClick(main_window.mw_menu_bar, 1, 0, pos, main_delay);
menu_actions = main_window.mw_menu_bar.actionAt(pos).menu();
if (menu_actions.visible) {
    pos = 0;
    pos = TestHelper.findActionAt(menu_actions, "action_generate_report");
    if (!pos) {
        print("select all in action menu is not found");
        throw "select all in action menu is not found";
    }
    QTest.mouseMove(menu_actions, pos);
    QTest.mouseClick(menu_actions, 1, 0, pos, main_delay);
} else {
    print("Action menu is not visible");
    throw "Action menu is not visible";
}

//get current MDI window(Reporter)------------------------------------------
QTest.qWait(5000);
TestRunnerMDIWindow = main_window.MDI_Area.currentSubWindow();
if (TestRunnerMDIWindow.objectName != "Reporter_SubWindow") {
    print("currentSubWindow is not Reporter_SubWindow");
    throw "currentSubWindow is not Reporter_SubWindow";
}
//save report------------------------------------------
pos = 0;
pos = TestHelper.findActionAt(main_window.mw_menu_bar, "mwmenu_actions");
if (!pos) {
    print("Action menu is not found");
    throw "Action menu is not found";
}
QTest.mouseMove(main_window.mw_menu_bar, pos, main_delay);
QTest.mouseClick(main_window.mw_menu_bar, 1, 0, pos, main_delay);
menu_actions = main_window.mw_menu_bar.actionAt(pos).menu();
if (menu_actions.visible) {
    pos = 0;
    pos = TestHelper.findActionAt(menu_actions, "action_save");
    if (!pos) {
        print("save in action menu is not found");
        throw "save in action menu is not found";
    }
    time_one = new Date();
    //full_report_path=path_to_save_report+"report_"+time_one.getYear()+"_"+time_one.getMonth()+"_"+time_one.getDay()+"_"+time_one.getHours()+"_"+time_one.getMinutes()+"_"+time_one.getSeconds()+".html";
    full_report_path = path_to_save_report + "test_report.html";
    TestHelper.fileDialog(menu_actions, full_report_path, pos, 2000);
} else {
    print("Action menu is not visible");
    throw "Action menu is not visible";
}
//close report window------------------------------------------
QTest.keyClick(main_window, Qt.Key_W, Qt.ControlModifier, main_delay);
//open reporter
pos = 0;
pos = TestHelper.findActionAt(main_window.mw_menu_bar, "mwmenu_tools");
if (!pos) {
    print("Tools menu is not found");
    throw "Tools menu is not found";
}
QTest.mouseMove(main_window.mw_menu_bar, pos, main_delay);
QTest.mouseClick(main_window.mw_menu_bar, 1, 0, pos, main_delay);
menu_tools = main_window.mw_menu_bar.actionAt(pos).menu();
if (menu_tools.visible) {
    pos = 0;
    pos = TestHelper.findActionAt(menu_tools, "action__testrunner");
    if (!pos) {
        print("Test runner in Tools menu is not found");
        throw "Test runner in Tools menu is not found";
    }
    QTest.mouseMove(menu_tools, pos);
    QTest.mouseClick(menu_tools, 1, 0, pos, main_delay);
} else {
    print("Tools menu is not visible");
    throw "Tools menu is not visible";
}
//get current MDI window(TestRunner)------------------------------------------
QTest.qWait(5000);
TestRunnerMDIWindow = main_window.MDI_Area.currentSubWindow();
if (TestRunnerMDIWindow.objectName != "TestView_SubWindow") {
    print("currentSubWindow is not TestView_SubWindow");
    throw "currentSubWindow is not TestView_SubWindow";
}
//get tree widget and it's last elem. index-----------------------------------

treeW = TestRunnerMDIWindow.TestView.splitter.tree;
treewidget_last = treeW.topLevelItem(treeW.topLevelItemCount - 1);//index of item

//find falled tests------------------------------------------
for (i = 0; i != treeW.topLevelItemCount; i++) {
    treewidget_last = treeW.topLevelItem(i);
    test_run_info = treewidget_last.text(1);
    index_numbers_of_idle_tests = test_run_info.indexOf("F") + 2;
    if (test_run_info.charAt(index_numbers_of_idle_tests) != "0") {
        print("one or more failed tests");
        throw "one or more failed tests";
    }
}
