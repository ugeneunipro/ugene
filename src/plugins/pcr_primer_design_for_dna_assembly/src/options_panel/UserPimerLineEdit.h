/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2021 UniPro <ugene@unipro.ru>
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

#ifndef _U2_USER_PRIMER_LINE_EDIT_H_
#define _U2_USER_PRIMER_LINE_EDIT_H_

#include <U2Gui/PrimerLineEdit.h>

namespace U2 {

/**
 * The @UserPimerLineEditResult struct
 * @text the whole text data from the @UserPimerLineEdit
 * Size of the left (red) text
 * Size of the right (green) text
 */
struct UserPimerLineEditResult {
    QString text;
    int leftEndSize = 0;
    int rightEndSize = 0;
};

/**
 * The @UserPimerLineEdit class is aimed to visualize primers on the "PCR Priming design" tab
 * This Line edits can hightlight in red and green left end right ends of the inserted text
 */
class UserPimerLineEdit : public PrimerLineEdit {
    Q_OBJECT
public:
    UserPimerLineEdit(QWidget* parent);

    /**
     * Set left (the red one) end of the text to the @left5End string
     */
    void setLeftEnd(const QString& left5End);
    /**
     * Set right (the green one) end of the text to the @right3End string
     */
    void setRightEnd(const QString& right3End);

    /**
     * Return @UserPimerLineEditResult of the object
     */
    UserPimerLineEditResult getData() const;
    /**
     * Set @text, @leftEndSize, @rightEndSize to the data, stored in @data
     * Update the view and colors
     */
    void setData(const UserPimerLineEditResult& data);

private slots:
    /**
     * Called then the selection has been changed
     */
    void sl_selectionChanged();
    /**
     * Called then the text has been edited (manually or by calling the @setText function)
     * @text has been set
     */
    void sl_textEdited(const QString& text);
    /**
     * Called then the text has been edited (manually only)
     * @text has been set
     */
    void sl_textChanged(const QString& text);

private:
    /**
     * Set colors of the text to the @colors
     * Each element from the @colors vector is one letter
     * That means, that if @colors has five elements, then five letters are colored
     */
    void setCharColors(const QList<QColor>& colors);
    /**
     * Return the color depends on a position af a letter - red, black or green
     * @pos the letter position
     */
    QColor getColorForInsertedText(int pos) const;
    /**
     * Update colors corresponding to the current @text, @leftEndSize and @rightEndSize values
     */
    void updateColors();

    QList<QColor> colors;
    int lastTextSize = 0;
    int lastSelectedTextSize = 0;
    int lastSelectionStart = -1;
    int leftEndSize = 0;
    int rightEndSize = 0;
    int lastLeftEndSize = 0;
    int lastRightEndSize = 0;
    bool changeLeftEndColor = false;
    bool changeRightEndColor = false;
};


}

#endif
