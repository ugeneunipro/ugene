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

#ifndef _U2_PHYTREE_OBJECT_H_
#define _U2_PHYTREE_OBJECT_H_

#include <U2Core/GObject.h>
#include <U2Core/PhyTree.h>
#include <U2Core/U2RawData.h>

namespace U2 {

class U2CORE_EXPORT U2PhyTree : public U2RawData {
public:
    U2PhyTree();
    U2PhyTree(const U2DbiRef &dbiRef);

    U2DataType getType() const;
};

class U2CORE_EXPORT PhyTreeObject : public GObject {
    Q_OBJECT
public:
    static PhyTreeObject * createInstance(const PhyTree &tree, const QString &objectName, const U2DbiRef &dbiRef, U2OpStatus &os, const QVariantMap &hintsMap = QVariantMap());

    PhyTreeObject(const QString &objectName, const U2EntityRef &treeRef, const QVariantMap &hintsMap = QVariantMap());

    virtual const PhyTree& getTree() const;
    void setTree(const PhyTree& _tree);

    void onTreeChanged();

    void rerootPhyTree(PhyNode* node);
    // Warning!
    // PhyBranches can be accessed and modified!
    // TODO: move branches to private data, add getters and setters
    const PhyNode* findPhyNodeByName(const QString& name);

    virtual GObject* clone(const U2DbiRef &dstDbiRef, U2OpStatus &os, const QVariantMap &hints = QVariantMap()) const;

    // Utility functions
    // Compares number of nodes and nodes with names (how many nodes etc.)
    static bool treesAreAlike(const PhyTree& tree1, const PhyTree& tree2);

    bool haveNodeLabels() const;

signals:
    void si_phyTreeChanged();

protected:
    void loadDataCore(U2OpStatus &os);

private:
    PhyTreeObject(const PhyTree &tree, const QString &objectName, const U2EntityRef &treeRef, const QVariantMap &hintsMap);
    void commit();
    static void commit(const PhyTree &tree, const U2EntityRef &treeRef, U2OpStatus &os);
    static void commit(const PhyTree &tree, const U2EntityRef &treeRef);

    PhyTree tree;
};


}//namespace


#endif
