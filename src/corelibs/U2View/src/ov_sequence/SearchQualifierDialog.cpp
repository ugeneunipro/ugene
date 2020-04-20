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

#include <QKeyEvent>
#include <QMessageBox>
#include <QPushButton>

#include <U2Core/AnnotationTableObject.h>
#include <U2Core/AppContext.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>

#include <U2View/AnnotationsTreeView.h>

#include "SearchQualifierDialog.h"
#include "ui_SearchQualifierDialog.h"

namespace U2 {

SearchQualifierDialog::SearchQualifierDialog(QWidget* p, AnnotationsTreeView *treeView) :
    QDialog(p),
    treeView(treeView),
    ui(new Ui_SearchQualifierDialog),
    groupToSearchIn(NULL),
    parentAnnotationofPrevResult(NULL),
    indexOfPrevResult(-1)
{
     ui->setupUi(this);
     new HelpButton(this, ui->buttonBox, "24748793");
     ui->buttonBox->button(QDialogButtonBox::Yes)->setText(tr("Select all"));
     ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Next"));
     ui->buttonBox->button(QDialogButtonBox::Close)->setText(tr("Close"));

     ui->valueEdit->installEventFilter(this);

     SAFE_POINT(treeView != NULL, "Tree Veiw is NULL", );

     clearPrevResults();

     AVItem* currentItem = static_cast<AVItem*>(treeView->tree->currentItem());
     switch (currentItem->type){
         case AVItemType_Group:
             {
                 groupToSearchIn = currentItem;
                 break;
             }
         case AVItemType_Annotation:
             {
                 parentAnnotationofPrevResult = currentItem;
                 groupToSearchIn = treeView->findGroupItem(currentItem->getAnnotationGroup());
                 break;
             }
         case AVItemType_Qualifier:
             {
                 AVItem* annotation = dynamic_cast<AVItem*>(currentItem->parent());
                 if (annotation && annotation->type == AVItemType_Annotation){
                     parentAnnotationofPrevResult = annotation;
                 }
                 groupToSearchIn = treeView->findGroupItem(currentItem->getAnnotationGroup());
                 break;
             }
     }
     QString groupName = groupToSearchIn->getAnnotationGroup()->getName();
     if (groupName == AnnotationGroup::ROOT_GROUP_NAME) {
         AnnotationTableObject *aObj = groupToSearchIn->getAnnotationTableObject();
         groupName = aObj->getGObjectName();
     }
     ui->groupLabel->setText(groupName);

     connect(ui->buttonBox->button(QDialogButtonBox::Ok), SIGNAL(clicked()), SLOT(sl_searchNext()));
     connect(ui->buttonBox->button(QDialogButtonBox::Yes), SIGNAL(clicked()), SLOT(sl_searchAll()));
     connect(ui->valueEdit, SIGNAL(textChanged(const QString&)), SLOT(sl_onSearchSettingsChanged()));
     connect(ui->nameEdit, SIGNAL(textChanged(const QString&)), SLOT(sl_onSearchSettingsChanged()));
     sl_onSearchSettingsChanged();
}

bool SearchQualifierDialog::eventFilter(QObject *obj, QEvent *e) {
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

SearchQualifierDialog::~SearchQualifierDialog( ) {
    clearPrevResults();
    delete ui;
}

void SearchQualifierDialog::sl_searchTaskStateChanged( ) {
    FindQualifierTask* task = qobject_cast<FindQualifierTask *>(sender());
    if(!task || task->isCanceled() || !task->isFinished()){
        return;
    }

    if (task->isFound()) {
        parentAnnotationofPrevResult = task->getResultAnnotation();
        indexOfPrevResult = task->getIndexOfResult();
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

void SearchQualifierDialog::clearPrevResults(){
    parentAnnotationofPrevResult = NULL;
    indexOfPrevResult = -1;
}

void SearchQualifierDialog::search( bool searchAll /* = false*/ ){
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
    if(searchAll){
        clearPrevResults();
    }

    FindQualifierTaskSettings settings(groupToSearchIn, name, val, ui->exactButton->isChecked(), searchAll, parentAnnotationofPrevResult, indexOfPrevResult);


    FindQualifierTask* findTask = new FindQualifierTask(treeView, settings);
    connect(findTask, SIGNAL( si_stateChanged() ), SLOT( sl_searchTaskStateChanged() ));
    TaskScheduler* s = AppContext::getTaskScheduler();
    s->registerTopLevelTask(findTask);
}

void SearchQualifierDialog::sl_searchAll() {
    search(true);
}

void SearchQualifierDialog::sl_onSearchSettingsChanged() {
    bool searchTextIsEmpty = ui->valueEdit->text().isEmpty() && ui->nameEdit->text().isEmpty();
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!searchTextIsEmpty);
    ui->buttonBox->button(QDialogButtonBox::Yes)->setEnabled(!searchTextIsEmpty);
}

}   // namespace U2
