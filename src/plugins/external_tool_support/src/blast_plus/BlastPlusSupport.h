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

#ifndef _U2_BLASTPLUS_SUPPORT_H
#define _U2_BLASTPLUS_SUPPORT_H

#include <U2Core/ExternalToolRegistry.h>
#include <U2Gui/ObjectViewModel.h>

namespace U2 {

class BlastPlusSupport : public ExternalTool {
    Q_OBJECT
public:
    BlastPlusSupport(const QString& id, const QString& name, const QString& path = "");

    static const QString ET_BLASTN;
    static const QString ET_BLASTN_ID;
    static const QString ET_BLASTP;
    static const QString ET_BLASTP_ID;
    static const QString ET_GPU_BLASTP;
    static const QString ET_GPU_BLASTP_ID;
    static const QString ET_BLASTX;
    static const QString ET_BLASTX_ID;
    static const QString ET_TBLASTN;
    static const QString ET_TBLASTN_ID;
    static const QString ET_TBLASTX;
    static const QString ET_TBLASTX_ID;
    static const QString ET_RPSBLAST;
    static const QString ET_RPSBLAST_ID;
    static const QString BLASTPLUS_TMP_DIR;
private slots:
    void sl_runWithExtFileSpecify();
    void sl_runAlign();

private:
    QString lastDBPath;
    QString lastDBName;
};

class BlastPlusSupportContext: public GObjectViewWindowContext {
    Q_OBJECT
public:
    BlastPlusSupportContext(QObject* p);

protected slots:
    void sl_showDialog();
    void sl_fetchSequenceById();

protected:
    virtual void initViewContext(GObjectView* view);
    virtual void buildMenu(GObjectView* view, QMenu* m);
private:
    QStringList toolIdList;
    QString lastDBPath;
    QString lastDBName;
    QString selectedId;
    QAction* fetchSequenceByIdAction;
};

}//namespace
#endif // _U2_BLASTPLUS_SUPPORT_H
