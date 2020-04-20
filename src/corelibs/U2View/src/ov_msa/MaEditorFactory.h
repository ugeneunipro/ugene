/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
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

#ifndef _U2_MA_EDITOR_FACTORY_H_
#define _U2_MA_EDITOR_FACTORY_H_

#include <U2Core/GObjectTypes.h>
#include <U2Gui/ObjectViewModel.h>

namespace U2 {

class MaEditor;
class MultipleAlignmentObject;
class OpenMaEditorTask;
class UnloadedObject;

/************************************************************************/
/* MaEditorFactory */
/************************************************************************/
class U2VIEW_EXPORT MaEditorFactory : public GObjectViewFactory {
    Q_OBJECT
public:
    MaEditorFactory(GObjectType type, GObjectViewFactoryId id);

    virtual bool canCreateView(const MultiGSelection& multiSelection);

    virtual Task* createViewTask(const MultiGSelection& multiSelection, bool single = false);

    virtual bool isStateInSelection(const MultiGSelection& multiSelection, const QVariantMap& stateData);

    virtual Task* createViewTask(const QString& viewName, const QVariantMap& stateData);

    virtual bool supportsSavedStates() const;

    virtual MaEditor* getEditor(const QString &viewName, GObject *obj) = 0;

protected:
    virtual OpenMaEditorTask* getOpenMaEditorTask(MultipleAlignmentObject* obj) = 0;
    virtual OpenMaEditorTask* getOpenMaEditorTask(UnloadedObject* obj) = 0;
    virtual OpenMaEditorTask* getOpenMaEditorTask(Document* doc) = 0;

    GObjectType type;
};

/************************************************************************/
/* MsaEditorFactory */
/************************************************************************/
class U2VIEW_EXPORT MsaEditorFactory : public MaEditorFactory {
    Q_OBJECT
public:
    MsaEditorFactory();

    MaEditor* getEditor(const QString &viewName, GObject *obj);

    static const GObjectViewFactoryId ID;
private:
    OpenMaEditorTask* getOpenMaEditorTask(MultipleAlignmentObject* obj);
    OpenMaEditorTask* getOpenMaEditorTask(UnloadedObject* obj);
    OpenMaEditorTask* getOpenMaEditorTask(Document* doc);

};

/************************************************************************/
/* McaEditorFactory */
/************************************************************************/
class U2VIEW_EXPORT McaEditorFactory : public MaEditorFactory {
    Q_OBJECT
public:
    McaEditorFactory();

    MaEditor* getEditor(const QString &viewName, GObject *obj);

    static const GObjectViewFactoryId ID;
private:
    OpenMaEditorTask* getOpenMaEditorTask(MultipleAlignmentObject* obj);
    OpenMaEditorTask* getOpenMaEditorTask(UnloadedObject* obj);
    OpenMaEditorTask* getOpenMaEditorTask(Document* doc);

};

} // namespace

#endif // _U2_MA_EDITOR_FACTORY_H_

