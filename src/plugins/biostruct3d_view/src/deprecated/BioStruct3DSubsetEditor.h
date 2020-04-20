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

#ifndef _U2_BIOSTRUCT3D_SUBSET_EDITOR_H_
#define _U2_BIOSTRUCT3D_SUBSET_EDITOR_H_

#include "ui_BioStruct3DSubsetEditor.h"

#include <QWidget>

namespace U2 {

class U2Region;
class BioStruct3DObject;
class BioStruct3DReference;

class BioStruct3DSubsetEditor : public QWidget, public Ui_BioStruct3DSubsetEditor
{
    Q_OBJECT

public:
    /** @param biostructs List of biostruct objects in which user can select
      * @param selected Pre selected biostruct
      * @param selectedModel Pre selected model
      */
    BioStruct3DSubsetEditor(const QList<BioStruct3DObject*> &biostructs, const BioStruct3DObject *selected = 0, int selectedModel = -1,  QWidget *parent = 0);

    /** Fill model ComboBox respectively with current biostruct */
    void fillModelCombo();

    /** Fill chain ComboBox respectively with current biostruct */
    void fillChainCombo();

    /** Fill region Edit respectively with current chain */
    void fillRegionEdit();

    /** Validate the subset.
      * @returns "" on 0k, error message on fail
      */
    QString validate() const;

    /** Fill and @return choosen subset */
    BioStruct3DReference getSubset();

    void setBiostructDisabled();
    void setModelDisabled();

private:
    /** Convert text from region Edit to U2Region */
    U2Region getRegion() const;

    /** Set text in region Edit from U2Region */
    void setRegion(const U2Region &region);

private slots:
    void sl_onBiostructChanged(int);
    void sl_onChainChanged(int);

};

}   // namespace U2

#endif  // #ifndef _U2_BIOSTRUCT3D_SUBSET_EDITOR_H_
