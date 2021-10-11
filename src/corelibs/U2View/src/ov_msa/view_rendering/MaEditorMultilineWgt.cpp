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

#include "MaEditorMultilineWgt.h"

#include <QGridLayout>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/GObjectTypes.h>

#include <U2View/MSAEditor.h>
#include <U2View/MSAEditorConsensusArea.h>
#include <U2View/MSAEditorOffsetsView.h>
#include <U2View/MSAEditorMultilineOverviewArea.h>
#include <U2View/MSAEditorSequenceArea.h>
#include <U2View/MaEditorNameList.h>
#include <U2View/MaEditorStatusBar.h>
#include <U2View/UndoRedoFramework.h>

#include "MaEditorUtils.h"
#include "SequenceAreaRenderer.h"
#include "ov_msa/helpers/MultilineBaseWidthController.h"
#include "ov_msa/helpers/DrawHelper.h"
#include "ov_msa/helpers/MultilineScrollController.h"

namespace U2 {

/************************************************************************/
/* MaEditorMultilineWgt */
/************************************************************************/
MaEditorMultilineWgt::MaEditorMultilineWgt(MaEditor *_editor)
    : editor(_editor),
      overviewArea(nullptr),
      statusBar(nullptr),
      enableCollapsingOfSingleRowGroups(false),
      scrollController(new MultilineScrollController(editor, this)),
      baseWidthController(new MultilineBaseWidthController(this)),
      rowHeightController(nullptr) {
    SAFE_POINT(editor != nullptr, "MaEditor is null!", );
    setFocusPolicy(Qt::ClickFocus);
}

MaEditorStatusBar *MaEditorMultilineWgt::getStatusBar() const {
    return statusBar;
}

void MaEditorMultilineWgt::initWidgets() {
    setContextMenuPolicy(Qt::CustomContextMenu);
    setMinimumSize(300, 200);

    setWindowIcon(GObjectTypes::getTypeInfo(GObjectTypes::MULTIPLE_SEQUENCE_ALIGNMENT).icon);

    GScrollBar *shBar = new GScrollBar(Qt::Horizontal);
    shBar->setObjectName("horizontal_sequence_scroll");
    shBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QScrollBar *nameListHorizontalScrollBar = new QScrollBar(Qt::Horizontal);
    nameListHorizontalScrollBar->setObjectName("horizontal_names_scroll");
    GScrollBar *cvBar = new GScrollBar(Qt::Vertical);
    cvBar->setObjectName("vertical_sequence_scroll");

    scrollController->init(shBar, cvBar);

    initOverviewArea();
    initStatusBar();

    QVBoxLayout *maContainerLayout = new QVBoxLayout();
    maContainerLayout->setContentsMargins(0, 0, 0, 0);
    maContainerLayout->setSpacing(0);

    maContainerLayout->setStretch(0, 1);
    maContainerLayout->addWidget(statusBar);

    QWidget *maContainer = new QWidget(this);
    maContainer->setLayout(maContainerLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);

    QSplitter *mainSplitter = new QSplitter(Qt::Vertical, this);
    mainSplitter->addWidget(maContainer);
    mainSplitter->setStretchFactor(0, 2);

    maContainerLayout->addWidget(overviewArea);
    mainLayout->addWidget(mainSplitter);
    setLayout(mainLayout);

    connect(editor, SIGNAL(si_zoomOperationPerformed(bool)), scrollController, SLOT(sl_zoomScrollBars()));
}

void MaEditorMultilineWgt::initActions() {
}

MaEditor *MaEditorMultilineWgt::getEditor() const {
    return editor;
}

}  // namespace U2
