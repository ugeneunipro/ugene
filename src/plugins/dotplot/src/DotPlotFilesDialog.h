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

#ifndef _U2_DOT_PLOT_FILES_DIALOG_H_
#define _U2_DOT_PLOT_FILES_DIALOG_H_

#include <ui_DotPlotFilesDialog.h>

namespace U2 {
class ADVSequenceObjectContext;

class DotPlotFilesDialog : public QDialog, public Ui_DotPlotFilesDialog{
    Q_OBJECT
public:
    DotPlotFilesDialog(QWidget *parent);

    virtual void accept();

    QString getFirstFileName() const {return firstFileName;}
    QString getSecondFileName() const {return secondFileName;}

    int getFirstGap() const;
    int getSecondGap() const;

protected slots:
    void sl_openFirstFile();
    void sl_openSecondFile();

    void sl_oneSequence();

    void sl_mergeFirst();
    void sl_mergeSecond();
private:
    QString firstFileName;
    QString secondFileName;

    QString filter;
};

} //namespace

#endif
