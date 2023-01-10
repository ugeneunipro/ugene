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

#ifndef _U2_SEARCH_QUALIFIER_DIALOG_H_
#define _U2_SEARCH_QUALIFIER_DIALOG_H_

#include <QDialog>
#include <QQueue>

#include <U2Core/global.h>

class Ui_SearchQualifierDialog;

namespace U2 {

class AnnotationsTreeView;
class AVAnnotationItem;
class AVItem;

class U2VIEW_EXPORT SearchQualifierDialog : public QDialog {
    Q_OBJECT
private:
    // Qualifier search settings.
    struct SearchQualifierSettings {
        SearchQualifierSettings(AVItem* groupToSearchIn, const QString& name, const QString& value, bool isExactMatch, bool searchAll, AVItem* prevAnnotation, int prevIndex);

        AVItem* groupToSearchIn;
        QString name;
        QString value;
        bool isExactMatch;
        bool searchAll;
        AVItem* prevAnnotation;  // Annotation containing the previously found qualifier.
        int prevIndex;  // Index in `prevAnnotation` of the previously found qualifier.
    };

    // Qualifier search algorithm. The class also expands found `AVItem`(s) in the Annotation Tree View.
    class SearchQualifier {
        AnnotationsTreeView* const treeView;  // This is needed to show the found qualifier(s).
        const QString name;
        const QString value;
        const bool isExactMatch;
        const bool searchAll;

        // Whether an qualifier was found. If true, then `resultAnotation`, `resultInd` are correct.
        bool foundResult;

        // Annotation containing the found qualifier. Equals to `nullptr` if no qualifier was found.
        // Outdated if `foundResult == false`.
        AVItem* resultAnnotation;

        // Index in `resultAnnotation` of the found qualifier. Equals to `-1` if no qualifier was found.
        // Outdated if `foundResult == false`.
        int resultInd;

        QQueue<AVItem*> toExpand;  // This queue is needed to expand items in main thread.
        QList<QPair<AVAnnotationItem*, int>> foundQuals;  // This is needed to set found items as selected.

        // Expands found `AVItem`(s) in the Annotation Tree View.
        void showQualifier() const;

        void searchInGroup(AVItem* group, bool& found);

        void searchInAnnotation(AVItem* annotation, bool& found);

        // The group index from which to start searching for the qualifier. Depends on the `resultAnnotation`.
        int getStartGroupIndex(AVItem* group);

        // The annotation index in group from which to start searching for the qualifier.
        // Depends on the `resultAnnotation`.
        int getStartAnnotationIndex(AVItem* annotation);

    public:
        SearchQualifier(AnnotationsTreeView* treeView, const SearchQualifierSettings& settings);

        // Whether an qualifier was found. If true, then `getResultAnnotation()`, `getIndexOfResult()` will return
        // corrected values. See `foundResult` filed.
        bool isFound() const;

        // Returns an annotation containing the found qualifier. Call `isFound()` before using.
        // See `resultAnnotation` filed.
        AVItem* getResultAnnotation() const;

        // Index of the found qualifier within the annotation. Call `isFound()` before using.
        // See `resultInd` filed.
        int getIndexOfResult() const;
    };

public:
    SearchQualifierDialog(QWidget* p, AnnotationsTreeView* treeView);
    ~SearchQualifierDialog();

protected:
    bool eventFilter(QObject* obj, QEvent* e);
    void clearPrevResults();

protected slots:
    void sl_searchNext();
    void sl_searchAll();

private slots:
    void sl_onSearchSettingsChanged();

private:
    void search(bool searchAll = false);
    void updateResultAndShowWarnings(const SearchQualifier& foundQualifier);

    AnnotationsTreeView* treeView;
    Ui_SearchQualifierDialog* ui;
    AVItem* groupToSearchIn;
    AVItem* parentAnnotationofPrevResult;
    int indexOfPrevResult;
};

}  // namespace U2

#endif  //_U2_SEARCH_QUALIFIER_DIALOG_H_
