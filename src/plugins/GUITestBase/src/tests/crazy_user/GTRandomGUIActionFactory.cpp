/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2023 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include "GTRandomGUIActionFactory.h"
#include <base_dialogs/GTFileDialog.h>
#include <core/GUITest.h>
#include <harness/UGUITestBase.h>
#include <primitives/GTWidget.h>

#include <QDirIterator>
#include <QFileDialog>

#include <U2Core/U2SafePoints.h>

namespace U2 {

namespace GUITest_crazy_user {

int randInt(int low, int high) {
    return qrand() % ((high + 1) - low) + low;
}

GTAbstractGUIAction* GTRandomGUIActionFactory::create(QObject* obj) {
    SAFE_POINT(nullptr != obj, "", nullptr);

    GTAbstractGUIAction* action = nullptr;
    for (const QMetaObject* metaObj = obj->metaObject(); metaObj != nullptr; metaObj = metaObj->superClass()) {
        QString className = metaObj->className();
        uiLog.trace(QString("Searching GTAbstractGUIAction for %1").arg(className));

        GTAbstractGUIActionMap::const_iterator mappedIterator = actionMap.constFind(className);
        const GTAbstractGUIAction* mappedAction = actionMap.constEnd() == mappedIterator ? nullptr : actionMap[className];
        if (nullptr != mappedAction) {
            uiLog.trace(QString("Found GUIAction for %1").arg(className));
            action = mappedAction->clone();
            action->init(obj);
            break;
        }
    }
    if (action == nullptr) {
        uiLog.trace("Not found GUIAction");
    }
    uiLog.trace("-----------------------");
    return action;
}

QMap<QString, const GTAbstractGUIAction*> GTRandomGUIActionFactory::actionMap;

class GTAbstractGUIAction_QWidget : public GTAbstractGUIAction {
public:
    GTAbstractGUIAction_QWidget(Priority priority)
        : GTAbstractGUIAction(priority) {
    }
    virtual void run() {
        auto objCasted = qobject_cast<QWidget*>(obj);
        SAFE_POINT(nullptr != objCasted, "", );

        GTWidget::click(os, objCasted);
    }

    virtual GTAbstractGUIAction_QWidget* clone() const {
        return new GTAbstractGUIAction_QWidget(*this);
    }
};
GTAbstractGUIActionMap::const_iterator it_QWidget = GTRandomGUIActionFactory::actionMap.insert("QWidget", new GTAbstractGUIAction_QWidget(GTAbstractGUIAction::Priority_Normal));

class GTAbstractGUIAction_QFileDialog : public GTAbstractGUIAction {
public:
    GTAbstractGUIAction_QFileDialog(Priority priority)
        : GTAbstractGUIAction(priority) {
    }
    virtual void run() {
        auto objCasted = qobject_cast<QFileDialog*>(obj);
        SAFE_POINT(nullptr != objCasted, "", );

        QString findPath = UGUITest::dataDir;
        QDir dir(findPath);

        QStringList files;
        QDirIterator iterator(dir.absolutePath(), QDirIterator::Subdirectories);
        while (iterator.hasNext()) {
            iterator.next();
            if (!iterator.fileInfo().isDir()) {
                files.append(iterator.filePath());
            }
        }

        int filesListId = randInt(0, files.size() - 1);
        QString randomFilePath = files[filesListId];

        GTFileDialogUtils* u = new GTFileDialogUtils(os, randomFilePath);
        u->run();
    }

    virtual GTAbstractGUIAction_QFileDialog* clone() const {
        return new GTAbstractGUIAction_QFileDialog(*this);
    }
};
GTAbstractGUIActionMap::const_iterator it_QFileDialog = GTRandomGUIActionFactory::actionMap.insert("QFileDialog", new GTAbstractGUIAction_QFileDialog(GTAbstractGUIAction::Priority_High));

}  // namespace GUITest_crazy_user

}  // namespace U2
