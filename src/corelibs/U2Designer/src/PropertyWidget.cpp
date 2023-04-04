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

#include "PropertyWidget.h"

#include <QKeyEvent>
#include <QLayout>
#include <QListView>
#include <QMessageBox>
#include <QStandardItemModel>

#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/EditConnectionDialog.h>
#include <U2Gui/LastUsedDirHelper.h>
#include <U2Gui/U2FileDialog.h>

#include <U2Lang/SchemaConfig.h>
#include <U2Lang/SharedDbUrlUtils.h>
#include <U2Lang/URLContainer.h>
#include <U2Lang/WorkflowUtils.h>

#include "OutputFileDialog.h"

namespace U2 {

/************************************************************************/
/* AbstractDefaultPropertyWidget */
/************************************************************************/
BaseDefaultPropertyWidget::BaseDefaultPropertyWidget(QWidget* parent)
    : PropertyWidget(parent) {
}

QVariant BaseDefaultPropertyWidget::value() {
    return lineEdit->text();
}

void BaseDefaultPropertyWidget::setValue(const QVariant& value) {
    lineEdit->setText(value.toString());
}

void BaseDefaultPropertyWidget::setRequired() {
    if (lineEdit->placeholderText().isEmpty()) {
        lineEdit->setPlaceholderText(L10N::required());
    }
}

void BaseDefaultPropertyWidget::sl_valueChanged(const QString& value) {
    emit si_valueChanged(value);
}

void BaseDefaultPropertyWidget::configureLineEdit(const int maxLength) {
    if (maxLength >= 0) {
        lineEdit->setMaxLength(maxLength);
    }
    addMainWidget(lineEdit);
    connect(lineEdit, SIGNAL(textChanged(const QString&)), SLOT(sl_valueChanged(const QString&)));
}

/************************************************************************/
/* DefaultPropertyWidget */
/************************************************************************/

DefaultPropertyWidget::DefaultPropertyWidget(int maxLength, QWidget* parent)
    : BaseDefaultPropertyWidget(parent) {
    lineEdit = new QLineEdit(this);
    configureLineEdit(maxLength);
}

/************************************************************************/
/* IgnoreUpDownPropertyWidget */
/************************************************************************/

IgnoreUpDownPropertyWidget::IgnoreUpDownPropertyWidget(int maxLength, QWidget* parent)
    : BaseDefaultPropertyWidget(parent) {
    lineEdit = new LineEditIgnoreUpDown(this);
    configureLineEdit(maxLength);
}

/************************************************************************/
/* LineEditIgnoreUpDown */
/************************************************************************/

LineEditIgnoreUpDown::LineEditIgnoreUpDown(QWidget* parent)
    : QLineEdit(parent) {
}

void LineEditIgnoreUpDown::keyPressEvent(QKeyEvent* e) {
    if ((e->key() == Qt::Key_Up) || (e->key() == Qt::Key_Down)) {
        e->ignore();
        return;
    }

    QLineEdit::keyPressEvent(e);
}

/************************************************************************/
/* SpinBoxWidget */
/************************************************************************/
SpinBoxWidget::SpinBoxWidget(const QVariantMap& spinProperties, QWidget* parent)
    : PropertyWidget(parent) {
    spinBox = new QSpinBox(this);
    WorkflowUtils::setQObjectProperties(*spinBox, spinProperties);
    addMainWidget(spinBox);

    connect(spinBox, SIGNAL(valueChanged(int)), SLOT(sl_valueChanged(int)));
}

QVariant SpinBoxWidget::value() {
    spinBox->interpretText();
    return spinBox->value();
}

void SpinBoxWidget::setValue(const QVariant& value) {
    spinBox->setValue(value.toInt());
}

void SpinBoxWidget::processDelegateTags() {
    CHECK(_tags != nullptr, );

    if (_tags->get("minimum") != QVariant()) {
        spinBox->setMinimum(_tags->get("minimum").toInt());
    }
    if (_tags->get("maximum") != QVariant()) {
        spinBox->setMaximum(_tags->get("maximum").toInt());
    }
}

bool SpinBoxWidget::setProperty(const char* name, const QVariant& value) {
    return spinBox->setProperty(name, value);
}

void SpinBoxWidget::sl_valueChanged(int value) {
    emit valueChanged(value);
    emit si_valueChanged(value);
}

/************************************************************************/
/* DoubleSpinBoxWidget */
/************************************************************************/
DoubleSpinBoxWidget::DoubleSpinBoxWidget(const QVariantMap& spinProperties, QWidget* parent)
    : PropertyWidget(parent) {
    spinBox = new QDoubleSpinBox(this);
    spinBox->setMinimumWidth(1);
    WorkflowUtils::setQObjectProperties(*spinBox, spinProperties);
    addMainWidget(spinBox);

    connect(spinBox, SIGNAL(valueChanged(double)), SLOT(sl_valueChanged(double)));
}

QVariant DoubleSpinBoxWidget::value() {
    spinBox->interpretText();
    return spinBox->value();
}

void DoubleSpinBoxWidget::setValue(const QVariant& value) {
    spinBox->setValue(value.toDouble());
}

void DoubleSpinBoxWidget::sl_valueChanged(double value) {
    emit si_valueChanged(value);
}

////////////////////////////////////
//// ComboBoxWidgetBase

ComboBoxWidgetBase::ComboBoxWidgetBase(QWidget* parent, const QSharedPointer<StringFormatter>& _formatter, bool _isSorted)
    : PropertyWidget(parent), formatter(_formatter), isSorted(_isSorted) {
}

QString ComboBoxWidgetBase::getFormattedItemText(const QString& itemKey) const {
    return formatter.isNull() ? itemKey : formatter->format(itemKey);
}

void ComboBoxWidgetBase::sortComboItemsByName(QList<ComboItem>& itemList) {
    std::stable_sort(itemList.begin(), itemList.end(), [](auto& i1, auto& i2) {
        return QString::compare(i1.first, i2.first, Qt::CaseInsensitive) < 0;
    });
}

/************************************************************************/
/* ComboBoxWidget */
/************************************************************************/
ComboBoxWidget::ComboBoxWidget(const QList<ComboItem>& items, QWidget* parent, const QSharedPointer<StringFormatter>& formatter, bool isSorted)
    : ComboBoxWidgetBase(parent, formatter, isSorted) {
    comboBox = new QComboBox(this);
    addMainWidget(comboBox);

    QList<ComboItem> sortedItems;
    for (const ComboItem& item : qAsConst(items)) {
        sortedItems.append({getFormattedItemText(item.first), item.second});
    }
    if (isSorted) {
        sortComboItemsByName(sortedItems);
    }
    for (const ComboItem& item : qAsConst(sortedItems)) {
        comboBox->addItem(item.first, item.second);
    }
    connect(comboBox, SIGNAL(activated(const QString&)), this, SIGNAL(valueChanged(const QString&)));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(sl_valueChanged(int)));
}

QVariant ComboBoxWidget::value() {
    return comboBox->itemData(comboBox->currentIndex());
}

void ComboBoxWidget::setValue(const QVariant& value) {
    int idx = comboBox->findData(value);
    comboBox->setCurrentIndex(idx);
}

void ComboBoxWidget::sl_valueChanged(int) {
    emit valueChanged(value().toString());
    emit si_valueChanged(value());
}

ComboBoxWidget* ComboBoxWidget::createBooleanWidget(QWidget* parent) {
    QList<ComboItem> values;
    values.append(qMakePair(ComboBoxWidget::tr("False"), false));
    values.append(qMakePair(ComboBoxWidget::tr("True"), true));
    return new ComboBoxWidget(values, parent);
}

/************************************************************************/
/* ComboBoxEditableWidget */
/************************************************************************/
ComboBoxEditableWidget::ComboBoxEditableWidget(const QVariantMap& items, QWidget* parent)
    : PropertyWidget(parent), customIdx(-1) {
    comboBox = new QComboBox(this);
    comboBox->setEditable(true);
    addMainWidget(comboBox);

    foreach (const QString& key, items.keys()) {
        comboBox->addItem(key, items[key]);
    }

    connect(comboBox, SIGNAL(editTextChanged(const QString&)), this, SLOT(sl_edit(const QString&)));

    connect(comboBox, SIGNAL(activated(const QString&)), this, SIGNAL(valueChanged(const QString&)));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(sl_valueChanged(int)));
}

QVariant ComboBoxEditableWidget::value() {
    return comboBox->itemData(comboBox->currentIndex());
}

void ComboBoxEditableWidget::setValue(const QVariant& value) {
    int idx = comboBox->findData(value);
    if (idx == -1) {
        // try by text
        idx = comboBox->findText(value.toString());
        if (idx == -1) {
            if (customIdx == -1) {
                comboBox->addItem(value.toString(), value);
                customIdx = comboBox->findData(value);
            } else {
                comboBox->setItemText(customIdx, value.toString());
                comboBox->setItemData(customIdx, value);
            }
            comboBox->setCurrentIndex(customIdx);
        } else {
            comboBox->setCurrentIndex(idx);
        }
    } else {
        comboBox->setCurrentIndex(idx);
    }
    sl_valueChanged(0);
}

void ComboBoxEditableWidget::sl_valueChanged(int) {
    emit si_valueChanged(value());
}

void ComboBoxEditableWidget::sl_edit(const QString& val) {
    if (!val.isEmpty()) {
        setValue(val);
        comboBox->setFocus();
    }
}

/************************************************************************/
/* ComboBoxWithUrlWidget */
/************************************************************************/
ComboBoxWithUrlWidget::ComboBoxWithUrlWidget(const QVariantMap& items, bool _isPath, QWidget* parent)
    : PropertyWidget(parent), customIdx(-1), isPath(_isPath) {
    comboBox = new QComboBox(this);
    addMainWidget(comboBox);

    foreach (const QString& key, items.keys()) {
        comboBox->addItem(key, items[key]);
    }

    QToolButton* toolButton = new QToolButton(this);
    toolButton->setObjectName("browsePathBtn");
    toolButton->setText("...");
    toolButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(toolButton, SIGNAL(clicked()), SLOT(sl_browse()));
    layout()->addWidget(toolButton);

#ifdef Q_OS_DARWIN
    toolButton->setMinimumHeight(18);
    QString style = "QComboBox {"
                    "min-height: 19px;"
                    "max-height: 24px;"
                    "}";
    comboBox->setStyleSheet(style);
#endif

    connect(comboBox, SIGNAL(activated(const QString&)), this, SIGNAL(valueChanged(const QString&)));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(sl_valueChanged(int)));
}

QVariant ComboBoxWithUrlWidget::value() {
    return comboBox->itemData(comboBox->currentIndex());
}

void ComboBoxWithUrlWidget::setValue(const QVariant& value) {
    int idx = comboBox->findData(value);
    if (idx == -1) {
        if (customIdx == -1) {
            comboBox->addItem(value.toString(), value);
            customIdx = comboBox->findData(value);
        } else {
            comboBox->setItemText(customIdx, value.toString());
            comboBox->setItemData(customIdx, value);
        }
        comboBox->setCurrentIndex(customIdx);
    } else {
        idx = comboBox->findData(value);
        comboBox->setCurrentIndex(idx);
    }
    sl_valueChanged(0);
}

void ComboBoxWithUrlWidget::sl_valueChanged(int) {
    emit si_valueChanged(value());
}

void ComboBoxWithUrlWidget::sl_browse() {
    LastUsedDirHelper lod("UrlCombo");
    QString lastDir = lod.dir;

    QString name;
    if (isPath) {
        lod.dir = name = U2FileDialog::getExistingDirectory(nullptr, tr("Select a folder"), lastDir);
        if (!name.isEmpty()) {
            setValue(name);
        }
    } else {
        lod.url = name = U2FileDialog::getOpenFileName(nullptr, tr("Select a file"), lastDir);
        if (!name.isEmpty()) {
            setValue(name);
        }
    }

    comboBox->setFocus();
}

/************************************************************************/
/* ComboBoxWithChecksWidget */
/************************************************************************/
ComboBoxWithChecksWidget::ComboBoxWithChecksWidget(const QVariantMap& _items, QWidget* parent, const QSharedPointer<StringFormatter>& formatter, bool isSorted)
    : ComboBoxWidgetBase(parent, formatter, isSorted), cm(nullptr), items(_items) {
    comboBox = new QComboBox(this);
    addMainWidget(comboBox);
    initModelView();

    connect(cm, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(sl_itemChanged(QStandardItem*)));
    connect(comboBox, SIGNAL(activated(const QString&)), this, SIGNAL(valueChanged(const QString&)));
    connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(sl_valueChanged(int)));
}

QVariant ComboBoxWithChecksWidget::value() {
    QStringList sList;
    const QList<QString>& keys = items.keys();
    foreach (const QString& key, keys) {
        if (items[key].toBool()) {
            sList << key;
        }
    }
    return sList.join(",");
}

void ComboBoxWithChecksWidget::setValue(const QVariant& value) {
    QStringList curList = value.toString().split(',', QString::SkipEmptyParts);
    // 0-item is a `ghostItem` with the result of all currently checked checkboxes. That's why we start with 1.
    for (int i = 1; i < cm->rowCount(); i++) {
        QStandardItem* item = cm->item(i);
        QString key = item->data().toString();
        bool checked = curList.contains(key, Qt::CaseInsensitive);
        items[key] = checked;
        item->setCheckState((checked) ? Qt::Checked : Qt::Unchecked);
    }
}

void ComboBoxWithChecksWidget::sl_valueChanged(int) {
    comboBox->setCurrentIndex(0);
    emit si_valueChanged(value());
}

void ComboBoxWithChecksWidget::sl_itemChanged(QStandardItem* item) {
    QStandardItem* standardItem = item;
    QString key = standardItem->data().toString();

    if (items.contains(key)) {
        bool newCheckState = standardItem->checkState() == Qt::Checked;
        if (items.value(key).toBool() != newCheckState) {
            items[key] = newCheckState;
            sl_valueChanged(0);
        }
    }

    comboBox->setItemText(0, getFormattedValue());
}

QString ComboBoxWithChecksWidget::getFormattedValue() {
    QStringList selectedValues = value().toString().split(",");
    QStringList formattedValues;
    for (const QString& value : qAsConst(selectedValues)) {
        formattedValues << getFormattedItemText(value);
    }
    if (isSorted) {
        formattedValues.sort(Qt::CaseInsensitive);
    }
    return formattedValues.join(",");
}

void ComboBoxWithChecksWidget::initModelView() {
    cm = new QStandardItemModel(comboBox);

    auto ghostItem = new QStandardItem(getFormattedValue());
    cm->appendRow(ghostItem);

    QList<QStandardItem*> standardItems;
    for (auto it = items.begin(); it != items.end(); ++it) {
        QString formattedValue = getFormattedItemText(it.key());
        auto item = new QStandardItem(formattedValue);
        item->setCheckable(true);
        item->setEditable(false);
        item->setSelectable(false);
        item->setCheckState(it.value().toBool() ? Qt::Checked : Qt::Unchecked);
        item->setData(it.key());
        standardItems << item;
    }
    if (isSorted) {
        std::stable_sort(standardItems.begin(), standardItems.end(), [](auto i1, auto i2) {
            return QString::compare(i1->text(), i2->text(), Qt::CaseInsensitive) < 0;
        });
    }
    std::for_each(standardItems.begin(), standardItems.end(), [&](auto item) -> void { cm->appendRow(item); });
    comboBox->setModel(cm);

    auto vw = new QListView(comboBox);
    vw->setModel(cm);
    vw->setRowHidden(0, true);

    comboBox->setView(vw);
}

/************************************************************************/
/* URLWidget */
/************************************************************************/
URLWidget::URLWidget(const QString& type, bool multi, bool isPath, bool saveFile, DelegateTags* tags, QWidget* parent)
    : PropertyWidget(parent, tags) {
    setAutoFillBackground(true);

    urlLine = new URLLineEdit(type, multi, isPath, saveFile, this);
    urlLine->setObjectName("urlLine");
    urlLine->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    connect(urlLine, SIGNAL(si_finished()), SLOT(sl_finished()));
    connect(urlLine, SIGNAL(textChanged(const QString&)), SLOT(sl_textChanged(const QString&)));
    addMainWidget(urlLine);

    browseButton = new QToolButton(this);
    browseButton->setObjectName("browseButton");
    browseButton->setText("...");
    browseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    connect(browseButton, SIGNAL(clicked()), SLOT(sl_browse()));
    layout()->addWidget(browseButton);

    if (urlLine->isMulti()) {
        addButton = new QToolButton(this);
        addButton->setObjectName("addButton");
        addButton->setVisible(!urlLine->text().isEmpty());
        addButton->setText(tr("add"));
        addButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
        connect(addButton, SIGNAL(clicked()), urlLine, SLOT(sl_onBrowseWithAdding()));
        layout()->addWidget(addButton);
    }
    setObjectName("URLWidget");
}

void URLWidget::sl_textChanged(const QString& text) {
    if (!urlLine->isMulti()) {
        return;
    }

    addButton->setVisible(!text.isEmpty());
}

QVariant URLWidget::value() {
    return urlLine->text();
}

void URLWidget::setValue(const QVariant& value) {
    if (value.canConvert<QList<Dataset>>()) {
        QStringList urls;
        foreach (const Dataset& set, value.value<QList<Dataset>>()) {
            QList<URLContainer*> containers = set.getUrls();
            for (URLContainer* c : qAsConst(containers)) {
                urls << c->getUrl();
            }
        }
        initialValue = urls.join(";");
    } else {
        initialValue = value.toString();
    }
    urlLine->setText(initialValue);
}

void URLWidget::setRequired() {
    if (urlLine->placeholderText().isEmpty()) {
        urlLine->setPlaceholderText(L10N::required());
    }
}

void URLWidget::activate() {
    if (urlLine->text().isEmpty()) {
        sl_browse();
    }
}

void URLWidget::sl_browse() {
    if (!urlLine->saveFile) {
        urlLine->sl_onBrowse();
        return;
    }
    RunFileSystem* rfs = getRFS();
    if (rfs == nullptr) {
        urlLine->sl_onBrowse();
    } else {
        QObjectScopedPointer<OutputFileDialog> d = new OutputFileDialog(rfs, urlLine->isPath, urlLine->getCompletionFillerInstance(), this);
        const int dialogResult = d->exec();
        CHECK(!d.isNull(), );

        if (QDialog::Accepted == dialogResult) {
            urlLine->setText(d->getResult());
        } else if (d->isSaveToFileSystem()) {
            urlLine->sl_onBrowse();
        }
        urlLine->setFocus();
    }
}

void URLWidget::sl_finished() {
    RunFileSystem* rfs = getRFS();
    if (rfs != nullptr) {
        QString result = urlLine->text();
        if ((result != initialValue) && RFSUtils::isCorrectUrl(result)) {
            if (rfs->canAdd(result, urlLine->isPath)) {
                U2OpStatusImpl os;
                rfs->addItem(result, urlLine->isPath, os);
            } else {
                urlLine->setText(initialValue);
            }
        }
    }
    urlLine->setText(finalyze(urlLine->text()));
    emit si_valueChanged(urlLine->text());
    emit finished();
}

RunFileSystem* URLWidget::getRFS() {
    CHECK(schemaConfig != nullptr, nullptr);
    return schemaConfig->getRFS();
}

QString URLWidget::finalyze(const QString& url) {
    return url;
}

/************************************************************************/
/* NoFileURLWidget */
/************************************************************************/
NoFileURLWidget::NoFileURLWidget(const QString& type, bool multi, bool isPath, bool saveFile, DelegateTags* tags, QWidget* parent)
    : URLWidget(type, multi, isPath, saveFile, tags, parent) {
}

static const QString FILE_TAG("file");

QString NoFileURLWidget::finalyze(const QString& url, DelegateTags* tags) {
    QFileInfo info(url);
    if (url.isEmpty() || info.isDir() || info.isRelative() || !info.exists()) {
        if (tags != nullptr) {
            tags->set(FILE_TAG, "");
        }
        return url;
    }

    if (tags != nullptr) {
        tags->set(FILE_TAG, info.fileName());
    }
    return info.dir().absolutePath();
}

QString NoFileURLWidget::finalyze(const QString& url) {
    return finalyze(url, const_cast<DelegateTags*>(tags()));
}

}  // namespace U2
