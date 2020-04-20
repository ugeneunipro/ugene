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

#ifndef _U2_MSA_COMBO_BOX_CONTROLLER_H_
#define _U2_MSA_COMBO_BOX_CONTROLLER_H_

#include <U2Core/DNAAlphabet.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/GroupedComboBoxDelegate.h>

#include <U2View/MSAEditor.h>

#include <QComboBox>

class QStandardItemModel;

namespace U2 {

class ComboBoxSignalHandler : public QObject {
    Q_OBJECT
public:
    ComboBoxSignalHandler(QWidget *parent = NULL) : QObject(parent) {
        comboBox = new QComboBox(parent);
        comboBox->setItemDelegate(new GroupedComboBoxDelegate(comboBox));
        connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(sl_indexChanged(int)));
    }
    QComboBox* getComboBox() {
        return comboBox;
    }
signals:
    void si_dataChanged(const QString &newScheme);
private slots:
    void sl_indexChanged(int index) {
        emit si_dataChanged(comboBox->itemData(index).toString());
    }
protected:
    QComboBox *comboBox;
};

template <class Factory, class Registry>
class MsaSchemeComboBoxController : public ComboBoxSignalHandler {
public:
    MsaSchemeComboBoxController(MSAEditor *msa, Registry *registry, QWidget *parent = NULL);
    void init();
    void setCurrentItemById(const QString& id);
private:
    void fillCbWithGrouping();
    void createAndFillGroup(QList<Factory *> rawSchemesFactories, const QString& groupName);

    MSAEditor *msa;
    Registry *registry;
};

template <class Factory, class Registry>
MsaSchemeComboBoxController<Factory, Registry>::MsaSchemeComboBoxController(MSAEditor *_msa, Registry *_registry, QWidget *parent /*= NULL*/)
    : ComboBoxSignalHandler(parent), msa(_msa), registry(_registry) {
    init();
}

template <class Factory, class Registry>
void MsaSchemeComboBoxController<Factory, Registry>::init() {
    CHECK(registry != NULL, );

    bool isAlphabetRaw = msa->getMaObject()->getAlphabet()->getType() == DNAAlphabet_RAW;

    comboBox->blockSignals(true);
    comboBox->clear();
    if (isAlphabetRaw) {
        fillCbWithGrouping();
    } else {
        CHECK(msa->getMaObject(), );
        CHECK(msa->getMaObject()->getAlphabet(), );
        DNAAlphabetType alphabetType = msa->getMaObject()->getAlphabet()->getType();
        QList<Factory *> schemesFactories = registry->getAllSchemes(alphabetType);
        Factory* emptySchemeFactory = registry->getEmptySchemeFactory();
        schemesFactories.removeAll(emptySchemeFactory);
        schemesFactories.prepend(emptySchemeFactory);
        foreach(Factory *factory, schemesFactories) {
            comboBox->addItem(factory->getName(), factory->getId());
        }
    }
    comboBox->blockSignals(false);
}

template <class Factory, class Registry>
void MsaSchemeComboBoxController<Factory, Registry>::setCurrentItemById(const QString& id) {
    comboBox->setCurrentIndex(comboBox->findData(id));
}

template <class Factory, class Registry>
void MsaSchemeComboBoxController<Factory, Registry>::fillCbWithGrouping() {
    QMap<AlphabetFlags, QList<Factory*> > schemesFactories = registry->getAllSchemesGrouped();
    Factory *emptySchemeFactory = registry->getEmptySchemeFactory();

    QList<Factory *> commonSchemesFactories = schemesFactories[DNAAlphabet_RAW | DNAAlphabet_AMINO | DNAAlphabet_NUCL];
    QList<Factory *> aminoSchemesFactories = schemesFactories[DNAAlphabet_RAW | DNAAlphabet_AMINO];
    QList<Factory *> nucleotideSchemesFactories = schemesFactories[DNAAlphabet_RAW | DNAAlphabet_NUCL];

    commonSchemesFactories.removeAll(emptySchemeFactory);
    commonSchemesFactories.prepend(emptySchemeFactory);

    createAndFillGroup(commonSchemesFactories, tr("All alphabets"));
    createAndFillGroup(aminoSchemesFactories, tr("Amino acid alphabet"));
    createAndFillGroup(nucleotideSchemesFactories, tr("Nucleotide alphabet"));
}

template <class Factory, class Registry>
void MsaSchemeComboBoxController<Factory, Registry>::createAndFillGroup(QList<Factory *> rawSchemesFactories, const QString& groupName) {
    CHECK(!rawSchemesFactories.isEmpty(), );
    GroupedComboBoxDelegate *schemeDelegate = qobject_cast<GroupedComboBoxDelegate*>(comboBox->itemDelegate());
    QStandardItemModel *schemeModel = qobject_cast<QStandardItemModel*>(comboBox->model());
    CHECK(schemeDelegate != NULL, );
    CHECK(schemeModel != NULL, );
    schemeDelegate->addParentItem(schemeModel, groupName);
    foreach(Factory *factory, rawSchemesFactories) {
        schemeDelegate->addChildItem(schemeModel, factory->getName(), factory->getId());
    }
}

}

#endif
