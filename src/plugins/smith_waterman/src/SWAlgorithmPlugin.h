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

#ifndef _U2_SW_ALGORITHM_PLUGIN_H_
#define _U2_SW_ALGORITHM_PLUGIN_H_

#include <U2Core/PluginModel.h>
#include <U2Gui/ObjectViewModel.h>

#include <U2View/SmithWatermanDialog.h>
#include <U2Algorithm/PairwiseAlignmentTask.h>

namespace U2 {

class XMLTestFactory;
class SWAlgorithmADVContext;
class SWAlgorithmMSAContext;

class SWAlgorithmPlugin : public Plugin {
    Q_OBJECT
public:
    SWAlgorithmPlugin();

public slots:
        void regDependedIMPLFromOtherPlugins();

private:
    QList<XMLTestFactory*>  fs;
    SWAlgorithmADVContext * ctxADV;
    SWAlgorithmMSAContext * ctxMSA;
};


class SWAlgorithmTests {
public:
    static QList<XMLTestFactory*> createTestFactories();
};

class SWAlgorithmADVContext: public GObjectViewWindowContext {
    Q_OBJECT
public:
    SWAlgorithmADVContext(QObject* p);

    protected slots:
    void sl_search();
protected:
    virtual void initViewContext(GObjectView* view);
private:
    SWDialogConfig dialogConfig;
};

class SWPairwiseAlignmentAlgorithm : public AlignmentAlgorithm {
public:
    SWPairwiseAlignmentAlgorithm();
    bool checkAlphabet(const DNAAlphabet *alphabet) const;
};

} //namespace

#endif  //_U2_SW_ALGORITHM_PLUGIN_H_
