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

#ifndef _U2_MRBAYES_SUPPORT_H_
#define _U2_MRBAYES_SUPPORT_H_

#include <U2Algorithm/PhyTreeGenerator.h>

#include <U2Core/ExternalToolRegistry.h>
#include <U2Core/PhyTree.h>

#include <U2View/CreatePhyTreeDialogController.h>

namespace U2 {

class MrBayesSupport : public ExternalTool {
    Q_OBJECT
public:
    MrBayesSupport(const QString& id, const QString& name, const QString& path = "");

    static const QString ET_MRBAYES;
    static const QString ET_MRBAYES_ID;
    static const QString MRBAYES_TMP_DIR;
};

class MrBayesAdapter : public PhyTreeGenerator{
public:
    Task * createCalculatePhyTreeTask(const MultipleSequenceAlignment &ma, const CreatePhyTreeSettings &s) ;
    CreatePhyTreeWidget *createPhyTreeSettingsWidget(const MultipleSequenceAlignment &ma, QWidget *parent = NULL);
};

class MrBayesModelTypes{
public:
    static QString poisson;
    static QString jones;
    static QString dayhoff;
    static QString mtrev;
    static QString mtmam;
    static QString wag;
    static QString rtrev;
    static QString cprev;
    static QString vt;
    static QString blosum;
    static QString equalin;
    static QStringList getAAModelTypes();

    static QString JC69;
    static QString HKY85;
    static QString GTR;
    static QStringList getSubstitutionModelTypes();
};

class MrBayesVariationTypes{
public:
    static QString equal;
    static QString gamma;
    static QString propinv;
    static QString invgamma;
    static QStringList getVariationTypes();
};

}   // namespace U2
#endif // _U2_MRBAYES_SUPPORT_H_
