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

#include "SearchQualifierDialog.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>

#include <U2View/AnnotationsTreeView.h>

#include "ui_SearchQualifierDialog.h"

namespace U2 {

/////////////////////////////////////////////////SearchQualifierSettings////////////////////////////////////////////////

SearchQualifierDialog::SearchQualifierSettings::SearchQualifierSettings(AVItem* const groupToSearchIn,
                                                                        const QString& name,
                                                                        const QString& value,
                                                                        const bool isExactMatch,
                                                                        const bool searchAll,
                                                                        AVItem* const prevAnnotation,
                                                                        const int prevIndex)
    : groupToSearchIn(groupToSearchIn), name(name), value(value), isExactMatch(isExactMatch), searchAll(searchAll),
      prevAnnotation(prevAnnotation), prevIndex(prevIndex) {
}

/////////////////////////////////////////////////////SearchQualifier////////////////////////////////////////////////////

SearchQualifierDialog::SearchQualifier::SearchQualifier(AnnotationsTreeView* treeView,
                                                        const SearchQualifierSettings& settings)
    : treeView(treeView), name(settings.name), value(settings.value), isExactMatch(settings.isExactMatch),
      searchAll(settings.searchAll), foundResult(false), resultAnnotation(settings.prevAnnotation),
      resultInd(settings.prevIndex) {
    AVItem* rootGroup = settings.groupToSearchIn;

    int childCount = rootGroup->childCount();
    for (int i = getStartGroupIndex(rootGroup); i < childCount; i++) {
        bool found = false;
        AVItem* child = static_cast<AVItem*>(rootGroup->child(i));
        if (child->type == AVItemType_Annotation) {
            searchInAnnotation(child, found);
        } else if (child->type == AVItemType_Group) {
            searchInGroup(child, found);
        }
        if (!foundQuals.isEmpty()) {
            if (!rootGroup->isExpanded()) {
                if (!toExpand.contains(rootGroup)) {
                    toExpand.enqueue(rootGroup);
                }
            }
            if (!searchAll) {
                break;
            }
        }
    }
    foundResult = !foundQuals.isEmpty();

    showQualifier();
}

bool SearchQualifierDialog::SearchQualifier::isFound() const {
    return foundResult;
}

AVItem* SearchQualifierDialog::SearchQualifier::getResultAnnotation() const {
    return resultAnnotation;
}

int SearchQualifierDialog::SearchQualifier::getIndexOfResult() const {
    return resultInd;
}

void SearchQualifierDialog::SearchQualifier::showQualifier() const {
    AVItem* qual = nullptr;
    int qualsSize = foundQuals.size();
    if (qualsSize > 0) {
        treeView->getTreeWidget()->clearSelection();
    }

    for (const auto& p : qAsConst(foundQuals)) {
        AVAnnotationItem* ai = p.first;
        SAFE_POINT(ai != nullptr, L10N::nullPointerError("annotation item"), );
        if (!ai->isExpanded()) {
            treeView->getTreeWidget()->expandItem(ai);
            treeView->sl_itemExpanded(ai);
        }

        if (foundResult) {
            const U2Qualifier u2qual = ai->annotation->getQualifiers().at(p.second);
            qual = ai->findQualifierItem(u2qual.name, u2qual.value);
            if (qual == nullptr) {
                continue;
            }

            qual->setSelected(true);
            qual->parent()->setSelected(true);
        }
    }

    for (AVItem* item : qAsConst(toExpand)) {
        treeView->getTreeWidget()->expandItem(item);
    }

    if (qual != nullptr && qualsSize == 1) {
        treeView->getTreeWidget()->scrollToItem(qual);
    }
}

void SearchQualifierDialog::SearchQualifier::searchInGroup(AVItem* group, bool& found) {
    for (int i = getStartGroupIndex(group); i < group->childCount(); i++) {
        found = false;
        AVItem* child = static_cast<AVItem*>(group->child(i));
        if (child->type == AVItemType_Annotation) {
            searchInAnnotation(child, found);
        } else if (child->type == AVItemType_Group) {
            searchInGroup(child, found);
        }
        if (found) {
            if (!group->isExpanded()) {
                if (!toExpand.contains(group)) {
                    toExpand.enqueue(group);
                }
            }
            if (!searchAll) {
                break;
            }
        }
    }
}

void SearchQualifierDialog::SearchQualifier::searchInAnnotation(AVItem* annotation, bool& found) {
    auto matchWords = [](const QString& expected, const QString& current, const bool isExactMatch) {
        if (expected.isEmpty()) {
            return true;
        }
        return isExactMatch ? current.compare(expected, Qt::CaseInsensitive) == 0 : current.contains(expected, Qt::CaseInsensitive);
    };

    AVAnnotationItem* ai = static_cast<AVAnnotationItem*>(annotation);
    const QVector<U2Qualifier>& quals = ai->annotation->getQualifiers();
    int startIdx = getStartAnnotationIndex(ai);
    for (int j = startIdx; j < quals.size(); j++) {
        const U2Qualifier& qual = quals.at(j);
        QString simplifiedValue = AVQualifierItem::simplifyText(qual.value);
        bool matchName = matchWords(name, qual.name, isExactMatch);
        bool matchValue = matchWords(value, simplifiedValue, isExactMatch);
        bool match = matchName && matchValue;
        if (match) {
            // Matched annotation is always first in the queue.
            // toExpand.enqueue(annotation);
            found = true;
            resultAnnotation = annotation;
            resultInd = j;

            foundQuals.append(QPair<AVAnnotationItem*, int>(ai, resultInd));

            if (!searchAll) {
                break;
            }
        }
    }
}

int SearchQualifierDialog::SearchQualifier::getStartGroupIndex(AVItem* group) {
    int result = 0;
    if (resultAnnotation == nullptr) {
        return result;
    }

    if (AVItem* parentGroup = dynamic_cast<AVItem*>(resultAnnotation->parent())) {
        AVItem* groupToSearchChild = parentGroup != group
                                         ? parentGroup  // If parent group is a subgroup of group seek to its index.
                                         : resultAnnotation;  // If annotation is in the same group seek to its idx.
        int idx = group->indexOfChild(groupToSearchChild);
        if (idx != -1) {
            result = idx;
        }
    }
    return result;
}

int SearchQualifierDialog::SearchQualifier::getStartAnnotationIndex(AVItem* annotation) {
    int result = 0;
    if (resultAnnotation != nullptr && resultAnnotation == annotation) {
        result = resultInd + 1;  // Start from the next qualifier in the annotation.
    }
    return result;
}

//////////////////////////////////////////////////SearchQualifierDialog/////////////////////////////////////////////////

SearchQualifierDialog::SearchQualifierDialog(QWidget* p, AnnotationsTreeView* treeView)
    : QDialog(p),
      treeView(treeView),
      ui(new Ui_SearchQualifierDialog),
      groupToSearchIn(nullptr),
      parentAnnotationofPrevResult(nullptr),
      indexOfPrevResult(-1) {
    ui->setupUi(this);
    new HelpButton(this, ui->buttonBox, "65929489");
    ui->buttonBox->button(QDialogButtonBox::Yes)->setText(tr("Select all"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Next"));
    ui->buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));

    ui->valueEdit->installEventFilter(this);

    SAFE_POINT(treeView != nullptr, "Tree Veiw is NULL", );

    clearPrevResults();

    AVItem* currentItem = static_cast<AVItem*>(treeView->tree->currentItem());
    switch (currentItem->type) {
        case AVItemType_Group: {
            groupToSearchIn = currentItem;
            break;
        }
        case AVItemType_Annotation: {
            parentAnnotationofPrevResult = currentItem;
            groupToSearchIn = treeView->findGroupItem(currentItem->getAnnotationGroup());
            break;
        }
        case AVItemType_Qualifier: {
            AVItem* annotation = dynamic_cast<AVItem*>(currentItem->parent());
            if (annotation && annotation->type == AVItemType_Annotation) {
                parentAnnotationofPrevResult = annotation;
            }
            groupToSearchIn = treeView->findGroupItem(currentItem->getAnnotationGroup());
            break;
        }
    }
    QString groupName = groupToSearchIn->getAnnotationGroup()->getName();
    if (groupName == AnnotationGroup::ROOT_GROUP_NAME) {
        AnnotationTableObject* aObj = groupToSearchIn->getAnnotationTableObject();
        groupName = aObj->getGObjectName();
    }
    ui->groupLabel->setText(groupName);

    connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), SLOT(sl_searchNext()));
    connect(ui->buttonBox->button(QDialogButtonBox::Yes), SIGNAL(clicked()), SLOT(sl_searchAll()));
    connect(ui->valueEdit, SIGNAL(textChanged(const QString&)), SLOT(sl_onSearchSettingsChanged()));
    connect(ui->nameEdit, SIGNAL(textChanged(const QString&)), SLOT(sl_onSearchSettingsChanged()));
    sl_onSearchSettingsChanged();
}

bool SearchQualifierDialog::eventFilter(QObject* obj, QEvent* e) {
    Q_UNUSED(obj);
    QEvent::Type t = e->type();
    if (t == QEvent::KeyPress) {
        QKeyEvent* ke = (QKeyEvent*)e;
        int key = ke->key();
        if (key == Qt::Key_Tab) {
            ui->nameEdit->setFocus();
            return true;
        }
        if (key == Qt::Key_Enter || key == Qt::Key_Return) {
            accept();
            return true;
        }
    }
    return false;
}

SearchQualifierDialog::~SearchQualifierDialog() {
    clearPrevResults();
    delete ui;
}

void SearchQualifierDialog::updateResultAndShowWarnings(const SearchQualifier& foundQualifier) {
    if (foundQualifier.isFound()) {
        parentAnnotationofPrevResult = foundQualifier.getResultAnnotation();
        indexOfPrevResult = foundQualifier.getIndexOfResult();
    } else if (indexOfPrevResult != -1) {
        int result = QMessageBox::question(this,
                                           tr("Search Complete"),
                                           tr("The end of the annotation tree has been reached. Would you like to start the search from the beginning?"),
                                           QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::Yes) {
            clearPrevResults();
            search();
        }
    } else {
        QMessageBox::information(this,
                                 tr("Search Complete"),
                                 tr("No results found"));
    }
}

void SearchQualifierDialog::sl_searchNext() {
    search();
}

void SearchQualifierDialog::clearPrevResults() {
    parentAnnotationofPrevResult = nullptr;
    indexOfPrevResult = -1;
}

void SearchQualifierDialog::search(bool searchAll /* = false*/) {
    QString name = AVQualifierItem::simplifyText(ui->nameEdit->text());
    QString val = AVQualifierItem::simplifyText(ui->valueEdit->text());
    if (!(name.length() < 20 && TextUtils::fits(TextUtils::QUALIFIER_NAME_CHARS, name.toLatin1().data(), name.length()))) {
        QMessageBox::critical(this, tr("Error!"), tr("Illegal qualifier name"));
        return;
    }
    if (!Annotation::isValidQualifierValue(val)) {
        QMessageBox::critical(this, tr("Error!"), tr("Illegal qualifier value"));
        return;
    }
    if (searchAll) {
        clearPrevResults();
    }

    SearchQualifierSettings settings(groupToSearchIn, name, val, ui->exactButton->isChecked(), searchAll, parentAnnotationofPrevResult, indexOfPrevResult);
    updateResultAndShowWarnings(SearchQualifier(treeView, settings));
}

void SearchQualifierDialog::sl_searchAll() {
    search(true);
}

void SearchQualifierDialog::sl_onSearchSettingsChanged() {
    bool searchTextIsEmpty = ui->valueEdit->text().isEmpty() && ui->nameEdit->text().isEmpty();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!searchTextIsEmpty);
    ui->buttonBox->button(QDialogButtonBox::Yes)->setEnabled(!searchTextIsEmpty);
}

}  // namespace U2
