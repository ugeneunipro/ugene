/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2025 UniPro <ugene@unipro.ru>
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

#pragma once

#include <QAction>

#include <U2Core/U2Region.h>

namespace U2 {

class AnnotatedDNAView;
class LRegionsSelection;
class Annotation;
class AnnotationSelection;
class ADVSequenceObjectContext;
class ADVSequenceWidget;

class U2VIEW_EXPORT ADVClipboard : public QObject {
    Q_OBJECT
public:
    ADVClipboard(AnnotatedDNAView* ctx);

    QAction* getCopySequenceAction() const;
    QAction* getCopyComplementAction() const;
    QAction* getCopyTranslationAction() const;
    QAction* getCopyComplementTranslationAction() const;

    QAction* getCopyAnnotationSequenceAction() const;
    QAction* getCopyAnnotationSequenceTranslationAction() const;

    QAction* getCopyQualifierAction() const;
    QAction* getPasteSequenceAction() const;

    ADVSequenceObjectContext* getSequenceContext() const;

    void addCopyMenu(QMenu* m);

    void connectSequence(ADVSequenceObjectContext* s);

    static QAction* createPasteSequenceAction(QObject* parent);

public slots:

    void sl_onDNASelectionChanged(LRegionsSelection* s, const QVector<U2Region>& added, const QVector<U2Region>& removed);
    void sl_onAnnotationSelectionChanged(AnnotationSelection* s, const QList<Annotation*>& added, const QList<Annotation*>& removed);

    /** Calls 'updateState' to handle new active sequence correctly. */
    void sl_onActiveSequenceChanged();

    void sl_copySequence();
    void sl_copyComplementSequence();
    void sl_copyTranslation();
    void sl_copyComplementTranslation();

    void sl_copyAnnotationSequence();
    void sl_copyAnnotationSequenceTranslation();

    void sl_setCopyQualifierActionStatus(bool isEnabled, QString text);

private:
    void updateActions();
    void copySequenceSelection(bool complement, bool amino);
    void copyAnnotationSelection(bool amino);
    void putIntoClipboard(const QString& data);

    AnnotatedDNAView* ctx;
    QAction* copySequenceAction;
    QAction* copyComplementSequenceAction;
    QAction* copyTranslationAction;
    QAction* copyComplementTranslationAction;

    QAction* copyAnnotationSequenceAction;
    QAction* copyAnnotationSequenceTranslationAction;

    QAction* copyQualifierAction;

    QAction* pasteSequenceAction;

    static const QString COPY_FAILED_MESSAGE;
};

}  // namespace U2
