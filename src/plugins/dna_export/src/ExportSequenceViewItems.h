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

#ifndef _U2_EXPORT_SEQUENCE_VIEW_ITEMS_H_
#define _U2_EXPORT_SEQUENCE_VIEW_ITEMS_H_

#include <U2Core/MultipleSequenceAlignment.h>
#include <U2Core/U2Region.h>

#include <U2Gui/ObjectViewModel.h>

namespace U2 {

class ADVSequenceObjectContext;
class AnnotatedDNAView;
class AnnotationSelection;
class Annotation;
class LRegionsSelection;
class U2OpStatus;

class ExportSequenceViewItemsController : public GObjectViewWindowContext {
    Q_OBJECT
public:
    ExportSequenceViewItemsController(QObject* p);
    void init();


protected:
    virtual void initViewContext(GObjectView* view);
    virtual void buildMenu(GObjectView* v, QMenu* m);
private:
    AnnotatedDNAView* av;
};

class ADVExportContext : public QObject {
    Q_OBJECT;
public:
    ADVExportContext(AnnotatedDNAView* v);
    void buildMenu(QMenu* m);

protected slots:
    void sl_saveSelectedSequences();
    void sl_saveSelectedAnnotationsSequence();
    void sl_saveSelectedAnnotations();
    void sl_saveSelectedAnnotationsToAlignment();
    void sl_saveSelectedAnnotationsToAlignmentWithTranslation();
    void sl_saveSelectedSequenceToAlignment();
    void sl_saveSelectedSequenceToAlignmentWithTranslation();
    void sl_getSequenceByDBXref();
    void sl_getSequenceByAccession();
    void sl_getSequenceById();

    void sl_onSequenceContextAdded(ADVSequenceObjectContext* c);
    void sl_onSequenceContextRemoved(ADVSequenceObjectContext* c);
    void sl_exportBlastResultToAlignment();

    void updateActions();

private:
    void prepareMAFromBlastAnnotations(MultipleSequenceAlignment& ma, const QString& nameQualId, bool includeRef, U2OpStatus& os);
    void prepareMAFromAnnotations(MultipleSequenceAlignment& ma, bool translate, U2OpStatus& os);
    void prepareMAFromSequences(MultipleSequenceAlignment& ma, bool translate, U2OpStatus& os);
    void fetchSequencesFromRemoteDB(const QString & listId);
    void selectionToAlignment(const QString& title, bool annotations, bool translate);

    AnnotatedDNAView*   view;

    QAction*  sequence2SequenceAction;
    QAction*  annotations2SequenceAction;
    QAction*  annotations2CSVAction;
    QAction*  annotationsToAlignmentAction;
    QAction*  annotationsToAlignmentWithTranslatedAction;
    QAction*  sequenceToAlignmentAction;
    QAction*  sequenceToAlignmentWithTranslationAction;
    QAction*  sequenceById;
    QAction*  sequenceByAccession;
    QAction*  sequenceByDBXref;
    QAction*  blastResultToAlignmentAction;

};

}//namespace

#endif
