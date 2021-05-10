//#include "qt.core"
//#include "qt.gui"
//#include "QTest"
//#separateThreads=false
//#exitOnException=true
//#exitOnEnding=true

print("Start testing...")
print("Test :Testin of project view: add annotation to the begining of gb file")
Test_path = TestHelper.getEnvValue("TEST_PATH");
print(Test_path);
TestHelper.isDebug = false;

main_delay = 100;
open_project_delay = 10000;
obsalut_path = true;
project_names = new Array(//can't be empty
    "_common_data\\genbank\\murine_sarcoma.gb"
);


findWidgetAtGlobalPosition = function (globalPoint) {
    fullPath = 0;
    element = 0;
    QTest.mouseMove(main_window, globalPoint, 200);
    for (element = QApplication.widgetAt(globalPoint); element != main_window; element = element.parent()) {
        if (!element) {
            break;
        }
        if (element.objectName) {
            print("[ " + element + " ; " + element.objectName + "]");
        } else {
            print("[ " + element + " ; " + "Noname" + "]");
        }
        if (fullPath) {
            if (element.objectName) {
                fullPath = element.objectName + "." + fullPath;
            } else {
                fullPath = "[ " + element + " Noname]." + fullPath;
            }
        } else {
            if (element.objectName) {
                fullPath = element.objectName;
            } else {
                fullPath = "[ " + element + " Noname]";
            }
        }
    }
    if (!element) {
        fullPath = "[NoParent]" + "." + fullPath;
    } else {
        if (fullPath) {
            fullPath = element.objectName + "." + fullPath;
        } else {
            fullPath = element.objectName;
        }
    }
    print("total: " + fullPath);
}
regionSelect = function (modalWid) {
    if (modalWid.objectName != "range_selection_dialog") {
        print("modal dialog is not found");
        throw "modal dialog is not found";
    }

    pos = new QPoint(1, 1);
    curW = modalWid.range_selector.start_edit_line;
    QTest.mouseMove(curW, pos, main_delay);
    QTest.mouseDClick(curW, 1, 0, pos, main_delay);
    QTest.keyClicks(curW, "1", 0, main_delay);

    curW = modalWid.range_selector.end_edit_line;
    QTest.mouseMove(curW, pos, main_delay);
    QTest.mouseDClick(curW, 1, 0, pos, main_delay);
    QTest.keyClicks(curW, "50", 0, main_delay);

    QTest.mouseMove(modalWid.ok_button, pos, main_delay);
    QTest.mouseClick(modalWid.ok_button, 1, 0, pos, main_delay);

    return;
}

createNewAnnotation = function (modalWid) {

    pos = new QPoint(1, 1);
    curW = modalWid.CreateAnnotationWidget.groupBox_2.groupNameEdit;
    QTest.mouseMove(curW, pos, main_delay);
    QTest.mouseDClick(curW, 1, 0, pos, main_delay);
    QTest.keyClick(curW, Qt.Key_Backspace, 0, main_delay);
    QTest.mouseDClick(curW, 1, 0, pos, main_delay);
    QTest.keyClicks(curW, "test_group", 0, main_delay);

    curW = modalWid.CreateAnnotationWidget.groupBox_2.annotationNameEdit;
    QTest.mouseMove(curW, pos, main_delay);
    QTest.mouseDClick(curW, 1, 0, pos, main_delay);
    QTest.keyClicks(curW, "test_annotation_1", 0, main_delay);

    if (modalWid.CreateAnnotationWidget.groupBox_2.locationEdit.text != "1..50") {
        print("wrong location when create new annotation");
        throw "wrong location when create new annotation";
    }

    QTest.mouseMove(modalWid.create_button, pos, main_delay);
    QTest.mouseClick(modalWid.create_button, 1, 0, pos, main_delay);
    return;
}

addAnotation = function (popupMenu) {
    actionToFind = "create_annotation_action";
    print("popupMenu: " + popupMenu);
    ss = popupMenu.actions();
    vtf = -1;
    li = 0;
    action1 = 0;
    separ1 = 0;
    totalHeight = 0;
    for (li in ss) {
        tempname = ss[li];
        if (!tempname.isSeparator()) {
            action1++;
        } else {
            separ1++;
        }
    }
    itemHeight = (popupMenu.height - (separ1 * 4)) / (action1);
    for (li in ss) {
        //print("ac "+li+": "+ss[li]+" name: "+ss[li].objectName);//actions in menu
        tempname = ss[li];
        if (ss[li].objectName == actionToFind) {
            totalHeight = totalHeight + 10;
            break;
        }
        if (tempname.isSeparator()) {
            totalHeight = totalHeight + 3;
        } else {
            totalHeight = totalHeight + itemHeight
        }
    }
    if (totalHeight == 0) {
        print("no action " + actionToFind + " in popup menu");
        throw "no action " + actionToFind + " in popup menu";
    }
    QTest.mouseMove(popupMenu, new QPoint(10, totalHeight), main_delay);
    TestHelper.setWindowProc("new_annotation_dialog", "createNewAnnotation", 2000);
    QTest.mouseClick(popupMenu, 1, 0, new QPoint(10, totalHeight), main_delay);
    return;
}

////////////////////////////////////////////////////////////////////////////////////////

for (ni in project_names) {
    file_to_open = project_names[ni];
    pos = TestHelper.findActionAt(main_window.mw_menu_bar, "mwmenu_file");
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
//project open and ready for work
//prVA=curSW.project_view_area.annotated_DNA_splitter.annotated_DNA_scrollarea.qt_scrollarea_viewport.scrolled_widget_layout.ADV_single_sequence_widget.det_view.render_area;
prVA = curSW.project_view_area.annotated_DNA_splitter.annotated_DNA_scrollarea.qt_scrollarea_viewport.scrolled_widget_layout.ADV_single_sequence_widget_0;
pos = new QPoint(100, 10);

QTest.mouseMove(prVA, pos, main_delay);
TestHelper.setWindowProc("range_selection_dialog", "regionSelect", 2000);
QTest.keyClick(prVA, Qt.Key_A, Qt.ControlModifier, main_delay);
//region selected and ready

pos = new QPoint((prVA.width / 2), prVA.height - 20);
prVA = curSW.project_view_area.annotated_DNA_splitter;
QTest.mouseMove(prVA, pos, main_delay);
TestHelper.setPopupWidget(prVA, "addAnotation", 2000);
prVA.customContextMenuRequested(pos);
//QTest.keyClick(prVA,Qt.Key_N,Qt.ControlModifier,main_delay);

//check QTreeWidget Value
QTest.mouseMove(curSW.project_view_area.annotated_DNA_splitter.annotations_tree_view, pos, main_delay);

curTree = curSW.project_view_area.annotated_DNA_splitter.annotations_tree_view.tree_widget;

readyflag = false;
for (i = 0; curTree.topLevelItemCount != i; i++) {
    treewidget_item = curTree.topLevelItem(i);
    if (treewidget_item.text(0).indexOf("Annotations") != -1) {
        for (j = 0; treewidget_item.childCount() != j; j++) {
            treewidget_item1 = treewidget_item.child(j);
            if (treewidget_item1.text(0).indexOf("test_group") != -1) {
                for (k = 0; treewidget_item1.childCount() != k; k++) {
                    treewidget_item2 = treewidget_item1.child(k);
                    if (treewidget_item2.text(0).indexOf("test_annotation_1") != -1) {
                        readyflag = true;
                    }
                }
            }
        }
    }
}
if (!readyflag) {
    print("wrong Annotations");
    throw "wrong Annotations";
} else {
    print("all right");
}













