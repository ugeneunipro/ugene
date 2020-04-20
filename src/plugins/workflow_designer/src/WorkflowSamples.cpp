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

#include <QAbstractItemModel>
#include <QApplication>
#include <QContextMenuEvent>
#include <QDir>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPainter>
#include <QStyle>
#include <QStyledItemDelegate>
#include <QTextDocument>
#include <QTextStream>
#include <QToolButton>
#include <QTreeView>
#include <QUrl>
#include <QVBoxLayout>

#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/Settings.h>
#include <U2Core/U2SafePoints.h>

#include <U2Designer/WorkflowGUIUtils.h>

#include <U2Lang/HRSchemaSerializer.h>
#include <U2Lang/WorkflowSettings.h>
#include <U2Lang/WorkflowUtils.h>

#include "WorkflowSamples.h"
#include "WorkflowViewController.h"
#include "util/SaveSchemaImageUtils.h"

Q_DECLARE_METATYPE(QTextDocument*)

namespace U2 {

const QString SamplesWidget::MIME_TYPE("application/x-ugene-sample-id");
QList<SampleCategory> SampleRegistry::data;

#define DATA_ROLE Qt::UserRole
#define INFO_ROLE Qt::UserRole + 1
#define ICON_ROLE Qt::UserRole + 2
#define DOC_ROLE Qt::UserRole + 3
#define ID_ROLE Qt::UserRole + 4

class SampleDelegate : public QStyledItemDelegate {
public:
    SampleDelegate(QObject* parent = 0) : QStyledItemDelegate(parent){}
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
    {
        QVariant value = index.data(Qt::SizeHintRole);
        if (value.isValid())
            return qvariant_cast<QSize>(value);

        QStyleOptionViewItem opt = option;
        initStyleOption(&opt, index);
        const QWidget *widget = qobject_cast<QWidget*>(parent());//QStyledItemDelegatePrivate::widget(option);
        QStyle *style = widget ? widget->style() : QApplication::style();
        opt.rect.setSize(widget->size());
        return style->sizeFromContents(QStyle::CT_ItemViewItem, &opt, QSize(), widget);
    }
};

SamplesWidget::SamplesWidget(WorkflowScene *scene, QWidget *parent) : QTreeWidget(parent) {
    setColumnCount(1);
    setHeaderHidden(true);
    setItemDelegate(new SampleDelegate(this));
    setWordWrap(true);

    foreach(const SampleCategory& cat, SampleRegistry::getCategories()) {
        addCategory(cat);
    }

    expandAll();

    glass = new SamplePane(scene);

    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), SLOT(handleTreeItem(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), SLOT(activateItem(QTreeWidgetItem*)));
    connect(glass, SIGNAL(itemActivated(QTreeWidgetItem*)), SLOT(activateItem(QTreeWidgetItem*)));
    connect(glass, SIGNAL(cancel()), SLOT(cancelItem()));
    connect(WorkflowSettings::watcher, SIGNAL(changed()), this, SLOT(sl_refreshSampesItems()));
}

QTreeWidgetItem * SamplesWidget::getSampleItem(const QString &category, const QString &id) {
    QList<QTreeWidgetItem*> items = findItems(category, Qt::MatchExactly);
    CHECK(1 == items.size(), NULL);

    for (int i=0; i<items.first()->childCount(); i++) {
        QTreeWidgetItem *sampleItem = items.first()->child(i);
        const QString sampleId = sampleItem->data(0, ID_ROLE).toString();
        if (sampleId == id) {
            return sampleItem;
        }
    }

    return NULL;
}

void SamplesWidget::activateSample(const QString &category, const QString &id) {
    QTreeWidgetItem *sampleItem = getSampleItem(category, id);
    CHECK(NULL != sampleItem, );

    scrollToItem(sampleItem);
    setCurrentItem(sampleItem);
    return;
}

void SamplesWidget::loadSample(const QString &category, const QString &id) {
    QTreeWidgetItem *sampleItem = getSampleItem(category, id);
    CHECK(NULL != sampleItem, );

    activateItem(sampleItem);
    return;
}

void SamplesWidget::activateItem(QTreeWidgetItem * item) {
    if (item && item->data(0, DATA_ROLE).isValid()) {
        emit sampleSelected(item->data(0, DATA_ROLE).toString());
    }
}

void SamplesWidget::handleTreeItem(QTreeWidgetItem * item) {
     if (item && !item->data(0, DATA_ROLE).isValid()) {
         item = NULL;
     }

    glass->setItem(item);
    emit setupGlass(glass);
}

void SamplesWidget::cancelItem() {
     selectionModel()->clear();
     if (isHidden()) {
         emit setupGlass(NULL);
         glass->setItem(NULL);
     } else {
         emit setupGlass(glass);
     }
}

void SamplesWidget::sl_nameFilterChanged(const QString &nameFilter) {
    revisible(nameFilter);
}

void SamplesWidget::revisible(const QString &nameFilter) {
    setMouseTracking(false);
    for (int catIdx=0; catIdx<topLevelItemCount(); catIdx++) {
        QTreeWidgetItem *category = topLevelItem(catIdx);
        bool hasVisibleSamples = false;
        QString catName = category->text(0);
        for (int childIdx=0; childIdx<category->childCount(); childIdx++) {
            QTreeWidgetItem *sample = category->child(childIdx);
            QString name = sample->text(0);
            if (!NameFilterLayout::filterMatched(nameFilter, name) &&
                !NameFilterLayout::filterMatched(nameFilter, catName)) {
                sample->setHidden(true);
            } else {
                sample->setHidden(false);
                hasVisibleSamples = true;
            }
        }
        category->setHidden(!hasVisibleSamples);
        category->setExpanded(hasVisibleSamples);
    }
    setMouseTracking(true);
}

void SamplesWidget::addCategory( const SampleCategory& cat )
{
    QTreeWidgetItem* ci = new QTreeWidgetItem(this, QStringList(cat.d.getDisplayName()));
    ci->setFlags(Qt::ItemIsEnabled);
    QFont cf;
    cf.setBold(true);
    ci->setData(0, Qt::FontRole, cf);
    ci->setData(0, Qt::BackgroundRole, QColor(255,255,160, 127));

    foreach(const Sample& item, cat.items) {
        QTreeWidgetItem* ib = new QTreeWidgetItem(ci, QStringList(item.d.getDisplayName()));
        ib->setData(0, DATA_ROLE, item.content);
        ib->setData(0, ID_ROLE, item.id);
        QTextDocument* doc = new QTextDocument(this);
        ib->setData(0, DOC_ROLE, qVariantFromValue<QTextDocument*>(doc));
        Descriptor d = item.d;
        QIcon ico = item.ico;
        if (ico.isNull()) {
            const QPixmap pixmap = SaveSchemaImageUtils::generateSchemaSnapshot(item.content.toUtf8());
            if (!pixmap.isNull()) {
                ico.addPixmap(pixmap);
            }
        }
        DesignerGUIUtils::setupSamplesDocument(d, ico, doc);
    }
}

void SamplesWidget::sl_refreshSampesItems(){
    clear();
    foreach(const SampleCategory& cat, SampleRegistry::getCategories()) {
        addCategory(cat);
    }
    expandAll();
}

void SamplePane::mouseDoubleClickEvent( QMouseEvent *e) {
    if (!item) {
        return;
    }

    QTextDocument* doc = item->data(0, DOC_ROLE).value<QTextDocument*>();
    int pageWidth = qMax(width() - 100, 100);
    int pageHeight = qMax(height() - 100, 100);
    if (pageWidth != doc->pageSize().width()) {
        doc->setPageSize(QSize(pageWidth, pageHeight));
    }

    QSize ts = doc->size().toSize();
    QRect textRect(width() / 2- pageWidth / 2,
        height() / 2 - pageHeight / 2,
        pageWidth,
        pageHeight);
    textRect.setSize(ts);

    QPoint position = e->pos();
    if(textRect.contains(position)) {
        emit itemActivated(item);
    }
    else {
        item = NULL;
        scene->update();
    }
}

void SamplePane::keyPressEvent(QKeyEvent * event ){
    if (event->key() == Qt::Key_Escape) {
        emit cancel();
    } else if (event->key() == Qt::Key_Enter) {
        emit itemActivated(item);
    }
}

SamplePane::SamplePane(WorkflowScene *_scene) : item(NULL), scene(_scene) {
    m_document = new QTextDocument(this);
}

void SamplePane::paint(QPainter* painter)
{
    const WorkflowView *ctrl = scene->getController();
    SAFE_POINT(NULL != ctrl, "NULL workflow controller", );
    if (!item && ctrl->isShowSamplesHint()) {
        DesignerGUIUtils::paintSamplesArrow(painter);
        return;
    }

    if(item) {
        QTextDocument* doc = item->data(0, DOC_ROLE).value<QTextDocument*>();
        DesignerGUIUtils::paintSamplesDocument(painter, doc, width(), height(), palette());
    }
}

const int LoadSamplesTask::maxDepth = 1;

LoadSamplesTask::LoadSamplesTask( const QStringList& lst)
: Task(tr("Load workflow samples"), TaskFlag_None), dirs(lst) {}

void LoadSamplesTask::run() {
    foreach(const QString& s, dirs) {
        scanDir(s);
    }
}

void LoadSamplesTask::scanDir( const QString& s, int depth) {
    QDir dir(s);
    if (!dir.exists()) {
        ioLog.error(tr("Sample dir does not exist: %1").arg(s));
        return;
    }
    SampleCategory category(s,dir.dirName());
    QStringList names;
    foreach(const QString & ext, WorkflowUtils::WD_FILE_EXTENSIONS) {
        names << "*." + ext;
    }

    foreach(const QFileInfo& fi, dir.entryInfoList(names, QDir::Files|QDir::NoSymLinks)) {
        QFile f(fi.absoluteFilePath());
        if(!f.open(QIODevice::ReadOnly)) {
            ioLog.error(tr("Failed to load sample: %1").arg(L10N::errorOpeningFileRead(fi.absoluteFilePath())));
            continue;
        }

        QTextStream in(&f);
        in.setCodec("UTF-8");
        Sample sample;
        sample.content = in.readAll();

        Metadata meta;
        QString err = HRSchemaSerializer::string2Schema(sample.content, NULL, &meta);
        if(!err.isEmpty()) {
            coreLog.error(tr("Failed to load sample: %1").arg(err));
            continue;
        }
        sample.d = Descriptor(fi.absoluteFilePath(), meta.name.isEmpty()? fi.baseName() : meta.name, meta.comment);

        QString icoName = dir.absoluteFilePath(fi.baseName() + ".png");
        if (QFile::exists(icoName)) {
            sample.ico.addFile(icoName);
        }
        sample.id = fi.fileName();
        category.items << sample;
    }
    if (!category.items.isEmpty()) {
        result << category;
    }
    if (depth < maxDepth) {
        foreach(const QFileInfo& fi, dir.entryInfoList(QStringList(), QDir::AllDirs|QDir::NoSymLinks|QDir::NoDotAndDotDot)) {
            scanDir(fi.absoluteFilePath(), depth + 1);
        }
    }
}

Task::ReportResult LoadSamplesTask::report()
{
    SampleRegistry::data = result;
    return ReportResult_Finished;
}

Task* SampleRegistry::init( const QStringList& lst) {
    return new LoadSamplesTask(lst);
}

/************************************************************************/
/* NameFilterLayout */
/************************************************************************/
NameFilterLayout::NameFilterLayout(QWidget *parent)
: QHBoxLayout(parent)
{
    setContentsMargins(0, 0, 0, 0);
    setSpacing(6);
    nameEdit = new QLineEdit();
    nameEdit->setObjectName("nameFilterLineEdit");
    nameEdit->setPlaceholderText(tr("Type to filter by name..."));

    QLabel *label = new QLabel(tr("Name filter:"));
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    nameEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    addWidget(label);
    addWidget(nameEdit);

    delTextAction = new QAction(this);
    delTextAction->setShortcut(QKeySequence(tr("Esc")));
    nameEdit->addAction(delTextAction);

    connect(delTextAction, SIGNAL(triggered()), nameEdit, SLOT(clear()));
}

QLineEdit * NameFilterLayout::getNameEdit() const {
    return nameEdit;
}

bool NameFilterLayout::filterMatched(const QString &nameFilter, const QString &name) {
    static QRegExp spaces("\\s");
    QStringList filterWords = nameFilter.split(spaces);
    foreach (const QString &word, filterWords) {
        if (!name.contains(word, Qt::CaseInsensitive)) {
            return false;
        }
    }
    return true;
}

/************************************************************************/
/* SamplesWrapper */
/************************************************************************/
SamplesWrapper::SamplesWrapper(SamplesWidget *samples, QWidget *parent)
: QWidget(parent)
{
    QVBoxLayout *vl = new QVBoxLayout(this);
    vl->setContentsMargins(0, 3, 0, 0);
    vl->setSpacing(3);
    NameFilterLayout *hl = new NameFilterLayout(NULL);
    vl->addLayout(hl);
    vl->addWidget(samples);

    connect(hl->getNameEdit(), SIGNAL(textChanged(const QString &)), samples, SLOT(sl_nameFilterChanged(const QString &)));
    setFocusProxy(hl->getNameEdit());
}

} //namespace
