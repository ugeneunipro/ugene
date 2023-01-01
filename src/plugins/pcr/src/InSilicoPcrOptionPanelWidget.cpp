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

#include "InSilicoPcrOptionPanelWidget.h"

#include <QMessageBox>

#include <U2Core/AppContext.h>
#include <U2Core/Counter.h>
#include <U2Core/DNAAlphabet.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/L10n.h>
#include <U2Core/MultiTask.h>
#include <U2Core/PrimerStatistics.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Theme.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/U2WidgetStateStorage.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

#include "ExtractProductTask.h"
#include "InSilicoPcrTask.h"
#include "PrimersDetailsDialog.h"

namespace U2 {

namespace {
const QString DETAILS_LINK = "details";
const QString FORWARD_SUBGROUP_ID = "forward";
const QString REVERSE_SUBGROUP_ID = "reverse";
const QString SETTINGS_SUBGROUP_ID = "settings";
}  // namespace

InSilicoPcrOptionPanelWidget::InSilicoPcrOptionPanelWidget(AnnotatedDNAView* annotatedDnaView)
    : QWidget(),
      annotatedDnaView(annotatedDnaView),
      pcrTask(nullptr),
      resultTableShown(false),
      savableWidget(this, GObjectViewUtils::findViewByName(annotatedDnaView->getName())) {
    GCOUNTER(cvar, "PCR options panel");
    setupUi(this);
    forwardPrimerBoxSubgroup->init(FORWARD_SUBGROUP_ID, tr("Forward primer"), forwardPrimerBox, true);
    reversePrimerBoxSubgroup->init(REVERSE_SUBGROUP_ID, tr("Reverse primer"), reversePrimerBox, true);
    settingsSubgroup->init(SETTINGS_SUBGROUP_ID, tr("Settings"), settingsWidget, true);
    annsComboBox->addItem(tr("Inner"), ExtractProductSettings::Inner);
    annsComboBox->addItem(tr("All intersected"), ExtractProductSettings::All);
    annsComboBox->addItem(tr("None"), ExtractProductSettings::None);
    annsComboBox->model()->sort(0);

    forwardPrimerBox->setAnnotatedDnaView(annotatedDnaView);
    reversePrimerBox->setAnnotatedDnaView(annotatedDnaView);

    connect(forwardPrimerBox, SIGNAL(si_primerChanged()), SLOT(sl_onPrimerChanged()));
    connect(reversePrimerBox, SIGNAL(si_primerChanged()), SLOT(sl_onPrimerChanged()));
    connect(findProductButton, SIGNAL(clicked()), SLOT(sl_findProduct()));
    connect(extractProductButton, SIGNAL(clicked()), SLOT(sl_extractProduct()));
    connect(annotatedDnaView, SIGNAL(si_sequenceModified(ADVSequenceObjectContext*)), SLOT(sl_onSequenceChanged(ADVSequenceObjectContext*)));
    connect(annotatedDnaView, SIGNAL(si_sequenceRemoved(ADVSequenceObjectContext*)), SLOT(sl_onSequenceChanged(ADVSequenceObjectContext*)));
    connect(annotatedDnaView, SIGNAL(si_activeSequenceWidgetChanged(ADVSequenceWidget*, ADVSequenceWidget*)), SLOT(sl_activeSequenceChanged()));
    connect(productsTable->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), SLOT(sl_onProductsSelectionChanged()));
    connect(productsTable, SIGNAL(doubleClicked(const QModelIndex&)), SLOT(sl_onProductDoubleClicked()));
    connect(detailsLinkLabel, SIGNAL(linkActivated(const QString&)), SLOT(sl_showDetails(const QString&)));

    static const QString linkText = QString("<a href=\"%1\" style=\"color: %2\">%3</a>").arg(DETAILS_LINK).arg(Theme::linkColorLabelStr()).arg(tr("Show primers details"));
    detailsLinkLabel->setText(linkText);
    warningLabel->setStyleSheet(warningLabel->styleSheet() + "color: " + Theme::errorColorLabelStr());
    warningLabel->setAlignment(Qt::AlignLeft);

    setResultTableShown(false);

    U2WidgetStateStorage::restoreWidgetState(savableWidget);

    sl_activeSequenceChanged();
    sl_onPrimerChanged();
}

InSilicoPcrOptionPanelWidget::~InSilicoPcrOptionPanelWidget() {
    if (nullptr != pcrTask) {
        pcrTask->cancel();
    }
}

AnnotatedDNAView* InSilicoPcrOptionPanelWidget::getDnaView() const {
    return annotatedDnaView;
}

void InSilicoPcrOptionPanelWidget::setResultTableShown(bool show) {
    resultTableShown = show;
    productsWidget->setVisible(show);
}

void InSilicoPcrOptionPanelWidget::sl_onPrimerChanged() {
    QByteArray forward = forwardPrimerBox->getPrimer();
    QByteArray reverse = reversePrimerBox->getPrimer();
    bool emptyPrimer = forward.isEmpty() || reverse.isEmpty();
    findProductButton->setDisabled(emptyPrimer);
    detailsLinkLabel->setVisible(!emptyPrimer);
    if (emptyPrimer) {
        warningLabel->hide();
        return;
    }

    bool isCriticalError = false;
    QString message = PrimerStatistics::checkPcrPrimersPair(forward, reverse, isCriticalError);
    detailsLinkLabel->setVisible(!isCriticalError);

    findProductButton->setEnabled(true);
    findProductButton->setText(tr("Find product(s)"));
    warningLabel->hide();
    if (!message.isEmpty()) {
        // The algorithm will not work with too short or too long primers: disable the "Run" button in this case.
        bool hasRunAnywayOption = PrimerStatistics::validatePrimerLength(forward) && PrimerStatistics::validatePrimerLength(reverse);
        findProductButton->setEnabled(hasRunAnywayOption);
        if (hasRunAnywayOption) {
            warningLabel->setText(tr("Warning: ") + message);
            findProductButton->setText(tr("Find product(s) anyway"));
        } else {
            warningLabel->setText(tr("Error: ") + message);
        }
        warningLabel->show();
    }
}

void InSilicoPcrOptionPanelWidget::sl_findProduct() {
    int maxProduct = productSizeSpinBox->value();
    SAFE_POINT(maxProduct > 0, "Non-positive product size", );
    int perfectMatch = perfectSpinBox->value();
    SAFE_POINT(perfectMatch >= 0, "Negative perfect match", );
    ADVSequenceObjectContext* sequenceContext = annotatedDnaView->getActiveSequenceContext();
    SAFE_POINT(nullptr != sequenceContext, L10N::nullPointerError("Sequence Context"), );
    U2SequenceObject* sequenceObject = sequenceContext->getSequenceObject();
    SAFE_POINT(nullptr != sequenceObject, L10N::nullPointerError("Sequence Object"), );

    InSilicoPcrTaskSettings settings;
    settings.forwardPrimer = forwardPrimerBox->getPrimer();
    settings.reversePrimer = reversePrimerBox->getPrimer();
    settings.forwardMismatches = forwardPrimerBox->getMismatches();
    settings.reverseMismatches = reversePrimerBox->getMismatches();
    settings.maxProductSize = uint(maxProduct);
    settings.perfectMatch = uint(perfectMatch);
    settings.useAmbiguousBases = useAmbiguousBasesCheckBox->isChecked();
    U2OpStatusImpl os;
    settings.sequence = sequenceObject->getWholeSequenceData(os);
    CHECK_OP_EXT(os, QMessageBox::critical(this, L10N::errorTitle(), os.getError()), );
    settings.sequenceObject = GObjectReference(sequenceObject);
    settings.isCircular = sequenceObject->isCircular();

    pcrTask = new InSilicoPcrTask(settings);
    connect(pcrTask, SIGNAL(si_stateChanged()), SLOT(sl_onFindTaskFinished()));
    AppContext::getTaskScheduler()->registerTopLevelTask(pcrTask);
    setDisabled(true);
    setResultTableShown(false);
}

void InSilicoPcrOptionPanelWidget::sl_onFindTaskFinished() {
    CHECK(sender() == pcrTask, );
    SAFE_POINT(nullptr != pcrTask, L10N::nullPointerError("InSilicoPcrTask"), );
    if (pcrTask->isCanceled() || pcrTask->hasError()) {
        disconnect(pcrTask, SIGNAL(si_stateChanged()));
        pcrTask = nullptr;
        setEnabled(true);
        return;
    }
    CHECK(pcrTask->isFinished(), );
    showResults(pcrTask);
    pcrTask = nullptr;
    setEnabled(true);
}

void InSilicoPcrOptionPanelWidget::showResults(InSilicoPcrTask* task) {
    ADVSequenceObjectContext* sequenceContext = annotatedDnaView->getSequenceContext(task->getSettings().sequenceObject);
    CHECK(nullptr != sequenceContext, );

    productsTable->showProducts(task->getResults(), sequenceContext);
    setResultTableShown(true);
}

void InSilicoPcrOptionPanelWidget::sl_extractProduct() {
    ADVSequenceObjectContext* sequenceContext = productsTable->productsContext();
    SAFE_POINT(nullptr != sequenceContext, L10N::nullPointerError("Sequence Context"), );
    U2SequenceObject* sequenceObject = sequenceContext->getSequenceObject();
    SAFE_POINT(nullptr != sequenceObject, L10N::nullPointerError("Sequence Object"), );
    ExtractProductSettings settings;
    settings.sequenceRef = sequenceContext->getSequenceRef();
    settings.annotationsExtraction = ExtractProductSettings::AnnotationsExtraction(annsComboBox->itemData(annsComboBox->currentIndex()).toInt());
    foreach (AnnotationTableObject* annsObject, sequenceContext->getAnnotationObjects()) {
        settings.annotationRefs << annsObject->getEntityRef();
    }

    QList<Task*> tasks;
    foreach (const InSilicoPcrProduct& product, productsTable->getSelectedProducts()) {
        tasks << new ExtractProductWrapperTask(product, sequenceObject->getSequenceName(), sequenceObject->getSequenceLength(), settings);
    }
    CHECK(!tasks.isEmpty(), );
    if (1 == tasks.size()) {
        AppContext::getTaskScheduler()->registerTopLevelTask(tasks.first());
    } else {
        AppContext::getTaskScheduler()->registerTopLevelTask(new MultiTask(tr("Extract In Silico PCR products"), tasks));
    }
}

void InSilicoPcrOptionPanelWidget::sl_onSequenceChanged(ADVSequenceObjectContext* sequenceContext) {
    bool tableChanged = productsTable->onSequenceChanged(sequenceContext);
    if (tableChanged) {
        setResultTableShown(false);
    }
    CHECK(nullptr != pcrTask, );
    bool taskChanged = GObjectReference(sequenceContext->getSequenceGObject()) == pcrTask->getSettings().sequenceObject;
    if (taskChanged) {
        pcrTask->cancel();
    }
}

bool InSilicoPcrOptionPanelWidget::isDnaSequence(ADVSequenceObjectContext* sequenceContext) {
    CHECK(nullptr != sequenceContext, false);
    const DNAAlphabet* alphabet = sequenceContext->getAlphabet();
    SAFE_POINT(alphabet != nullptr, L10N::nullPointerError("Alphabet"), false);
    return alphabet->isDNA();
}

void InSilicoPcrOptionPanelWidget::sl_activeSequenceChanged() {
    ADVSequenceObjectContext* sequenceContext = annotatedDnaView->getActiveSequenceContext();
    bool isDna = isDnaSequence(sequenceContext);
    runPcrWidget->setEnabled(isDna);
    algoWarningLabel->setVisible(!isDna);
}

void InSilicoPcrOptionPanelWidget::sl_onProductsSelectionChanged() {
    QList<InSilicoPcrProduct> products = productsTable->getSelectedProducts();
    extractProductButton->setEnabled(!products.isEmpty());
}

void InSilicoPcrOptionPanelWidget::sl_onProductDoubleClicked() {
    QList<InSilicoPcrProduct> products = productsTable->getSelectedProducts();
    if (1 == products.size()) {
        sl_extractProduct();
    }
}

void InSilicoPcrOptionPanelWidget::sl_showDetails(const QString& link) {
    SAFE_POINT(DETAILS_LINK == link, "Incorrect link", );
    PrimersPairStatistics calc(forwardPrimerBox->getPrimer(), reversePrimerBox->getPrimer());
    QObjectScopedPointer<PrimersDetailsDialog> dlg = new PrimersDetailsDialog(this, calc.generateReport());
    dlg->exec();
}

}  // namespace U2
