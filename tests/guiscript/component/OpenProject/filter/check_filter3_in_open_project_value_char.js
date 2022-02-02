//#include "qt.core"
//#include "qt.gui"
//#include "QTest"
//#separateThreads=false
//#exitOnException=true
//#exitOnEnding=true
print("Start testing...")
print("Test :Check item in docTreeWidget after set value in filter field")
Test_path = TestHelper.getEnvValue("TEST_PATH");
print(Test_path);
TestHelper.isDebug = true;
main_delay = 100;
filterText = "AY";
open_project_delay = 10000;
element_index_to_check = 0;
element_value_to_check = "[a] AY027935   standard features [Haemophilus.emb]";
obsalut_path = true;
project_names = new Array(//can't be empty
    "_common_data\\embl\\Haemophilus.emb",
    "_common_data\\genbank\\murine_sarcoma.gb"
);
//filterName="Group_by_document_action";
//filterName="Group_by_type_action";
filterName = "No_groups_action";

popupQMenu = function (menu) {
    print("popupQMenu: " + menu + " ; " + menu.objectName);
//TestHelper.findActionAt(menu,"gogi");
    AList = menu.actions();
    li = 0;
    ypos = 0;
    for (li in AList) {
        print("QAction" + li + ": " + AList[li] + " ; " + AList[li].objectName)
    }
    if (li == 0) {
        print("no Actions in menu");
        throw "no Actions in menu";
    }
    itemHeight = menu.height / (li + 1);
    for (li in AList) {
        if (AList[li].objectName == filterName) {
            ypos = (itemHeight * li) + 5;
        }
    }
    if (ypos == 0) {
        print("no Action " + filterName + " in menu");
        throw "no Action " + filterName + " in menu";
    }
    QTest.mouseMove(menu, new QPoint(10, ypos), main_delay);
    QTest.mouseClick(menu, 1, 0, new QPoint(10, ypos), main_delay);
}

for (ni in project_names) {
    file_to_open = project_names[ni];
    pos = TestHelper.findActionAt(main_window.mw_menu_bar, "mwmenu_file");
    print(pos);
    if (!pos) {
        print("file menu is not found");
        throw "file menu is not found";
    }
    QTest.mouseMove(main_window.mw_menu_bar, pos, main_delay);
    QTest.mouseClick(main_window.mw_menu_bar, 1, 0, pos, main_delay);

    menu_file = main_window.mw_menu_bar.actionAt(pos).menu();
    if (!menu_file.visible) {
        print("settins menu is not visible");
        throw "settins menu is not visible";
    }
    pos = 0;
    pos = TestHelper.findActionAt(menu_file, "action_projectsupport__open_project");//settings have no name
    if (!pos) {
        print("Action open_project is not found");
        throw "Action open_project is not found";
    }
    if (obsalut_path) {
        TestHelper.fileDialog(menu_file, Test_path + file_to_open, pos, 2000)
    } else {
        TestHelper.fileDialog(menu_file, file_to_open, pos, 2000)
    }
}
QTest.mouseMove(main_window.mw_menu_bar, pos, open_project_delay);
curSW = main_window.MDI_Area.currentSubWindow();
// children_one=curSW.children();
// for(i in children_one){
//    print("children "+i+" : "+children_one[i]+" "+children_one[i].objectName)
//}

goodResult = false;
getChildren(main_window);
for (i = 0; childrens_of_main_window[i]; i++) {
    if (childrens_of_main_window[i].objectName == "mw_docArea") {
        goodResult = true;
        break;
    }
}
if (!goodResult) {
    curBar = main_window.left_dock_bar;
    elem = curBar.doc_label__project_view;
    QTest.mouseMove(elem, new QPoint(0, 0), main_delay);
    QTest.mouseClick(elem, 1, 0, new QPoint(0, 0), main_delay);
}
//==================================================
curWE = main_window.mw_docArea.wrap_widget_project_view.project_view
//--------------------change filter
curModeButton = curWE.splitter.layoutWidget1.groupModeButton;
TestHelper.setPopupWidget("group_Mode_Menu", "popupQMenu", 700);
QTest.mouseMove(curModeButton, new QPoint(1, 1), main_delay);
QTest.mouseClick(curModeButton, 1, 0, new QPoint(1, 1), main_delay);
//--------------------

CurEdit = curWE.splitter.layoutWidget1.nameFilterEdit;
QTest.mouseMove(CurEdit, new QPoint(1, 1), main_delay);
QTest.mouseDClick(CurEdit, 1, 0, new QPoint(1, 1), main_delay);
QTest.keyClicks(CurEdit, filterText, 0, main_delay);

if (curWE.group_Mode_Menu.visible) {
    QTest.mouseMove(curWE.group_Mode_Menu, new QPoint(1, 1), main_delay);
}


curTree = curWE.splitter.layoutWidget1.documentTreeWidget;
treewidget_item = curTree.topLevelItem(element_index_to_check);
print("ss:" + treewidget_item.text(0) + ";")
if (treewidget_item.text(0) != element_value_to_check) {
    print(element_index_to_check + " element in documentTreeWidget is not correct");
    throw element_index_to_check + " element in documentTreeWidget is not correct";
}