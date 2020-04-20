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

#include <QCheckBox>
#include <QMessageBox>
#include <QRegExp>
#include <QStringList>

#include <U2Algorithm/SmithWatermanReportCallback.h>

#include <U2Core/AppContext.h>
#include <U2Core/BaseDocumentFormats.h>
#include <U2Core/DNASequenceObject.h>
#include <U2Core/DNASequenceSelection.h>
#include <U2Core/L10n.h>
#include <U2Core/Log.h>
#include <U2Core/ProjectModel.h>
#include <U2Core/Settings.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/CreateAnnotationWidgetController.h>
#include <U2Gui/DialogUtils.h>
#include <U2Gui/HelpButton.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Gui/U2FileDialog.h>

#include <U2View/ADVSequenceObjectContext.h>
#include <U2View/AnnotatedDNAView.h>

#include "SmithWatermanDialogImpl.h"
#include "SubstMatrixDialog.h"

const QString SETTINGS_LASTDIR = "save_align_files/last_dir";

extern const char OPEN_SQUARE_BRACKET;
extern const char CLOSE_SQUARE_BRACKET;
extern const char SHORTHAND_AND_LABEL_SEPARATOR;
extern const qint32 STRING_HAS_NO_KEY_MESSAGE;

const quint8 COUNT_OF_TEMPLATE_BUTTONS_IN_ROW = 2;
const char * RUN_BUTTON_ANNOT_RESULT_LABEL = "Search";
const char * RUN_BUTTON_MA_RESULT_LABEL = "Align";
const char * RESULT_DIR_NOT_FOUND_MESSAGE = "Folder you have chosen for alignment files does not exist";
const QString DEFAULT_PATTERN_SEQUENCE_NAME = "P";
const QChar DEFAULT_SHORTHANDS_SEPARATOR = '_';

namespace U2 {

SmithWatermanDialog::SmithWatermanDialog(QWidget* w, ADVSequenceObjectContext* ctx, SWDialogConfig* _dialogConfig)
    : QDialog(w), substMatrixRegistry(0), swTaskFactoryRegistry(0)
{
    ctxSeq = ctx;
    dialogConfig = _dialogConfig;

    setupUi(this);
    new HelpButton(this, buttonBox, "24748978");
    buttonBox->button(QDialogButtonBox::Yes)->setText(tr("Remote run"));
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Search"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    remoteRunPushButton = buttonBox->button(QDialogButtonBox::Yes);
    bttnRun = buttonBox->button(QDialogButtonBox::Ok);
    bttnCancel = buttonBox->button(QDialogButtonBox::Cancel);

    tabWidget->setCurrentIndex(0);
    regionSelector = new RegionSelector(this, ctx->getSequenceLength(), true, ctx->getSequenceSelection());
    rangeSelectorLayout->addWidget(regionSelector);
    setTabOrder(radioComplement, regionSelector);
    setTabOrder(regionSelector, comboRealization);

    substMatrixRegistry = AppContext::getSubstMatrixRegistry();
    if (0 == substMatrixRegistry) {
        coreLog.error(tr("No substitution matrices found."));
        QMessageBox::critical(this, windowTitle(), tr("No substitution matrices found."));
        QDialog::done(-1);
        return;
    }

    swResultFilterRegistry = AppContext::getSWResultFilterRegistry();
    if (0 == swResultFilterRegistry) {
        coreLog.error(tr("No filter registry found.")); //FIXME should be optional?
        QDialog::done(-1);                    //No.
        return;
    }

    tagsRegistry = AppContext::getSWMulAlignResultNamesTagsRegistry();
    if(NULL == tagsRegistry) {
        coreLog.error(tr("No result names tag registry found."));
        QDialog::done(-1);
        return;
    }

    swTaskFactoryRegistry = AppContext::getSmithWatermanTaskFactoryRegistry();
    if (0 == swTaskFactoryRegistry) {
        coreLog.error(tr("No algorithm registry found."));
        QDialog::done(-1);
        return;
    }
    fillTemplateButtonsList();
    addAnnotationWidget();
    setParameters();
    connectGUI();
    initResultDirPath();
    clearAll();
    loadDialogConfig();
    updatePatternFieldVisualState();

    teditPattern->setFocus();

    //! FIXME:
    remoteRunPushButton->setVisible(false);
}

void SmithWatermanDialog::connectGUI()
{
    connect(bttnViewMatrix, SIGNAL(clicked()), SLOT(sl_bttnViewMatrix()));
    connect(bttnRun, SIGNAL(clicked()), SLOT(sl_bttnRun()));
    connect(bttnCancel, SIGNAL(clicked()), SLOT(sl_cancelButton()));
    connect(radioTranslation, SIGNAL(toggled(bool)), SLOT(sl_translationToggled(bool)));
    //connect( remoteRunPushButton, SIGNAL( clicked() ), SLOT( sl_remoteRunButtonClicked() ) );

    connect(teditPattern, SIGNAL(textChanged()), SLOT(sl_patternChanged()));
    connect(resultViewVariants, SIGNAL(currentIndexChanged(const QString &)), SLOT(sl_resultViewChanged(const QString &)));
    connect(browseResultDirBtn, SIGNAL(clicked()), SLOT(sl_browseAlignFilesDir()));

    connectTemplateButtonsGui();
}

void SmithWatermanDialog::connectTemplateButtonsGui()
{
    foreach(QPushButton * button, *templateButtons)
        connect(button, SIGNAL(clicked()), SLOT(sl_templateButtonPressed()));
}

SmithWatermanDialog::~SmithWatermanDialog()
{
    delete templateButtons;
    delete templateButtonsApplicability;
}

bool SmithWatermanDialog::eventFilter(QObject * object, QEvent * event)
{
    if(object == mObjectNameTmpl || object == refSubseqNameTmpl || object == patternSubseqNameTmpl)
    {
        if(QEvent::FocusIn == event->type()) {
            templateEditInFocus();
        } else if(QEvent::FocusOut == event->type()) {
            if(advOptions != QApplication::focusWidget()
                || (Qt::BacktabFocusReason == dynamic_cast<QFocusEvent *>(event)->reason())) {
                templateEditLostFocus();
            } else {
                qobject_cast<QLineEdit *>(object)->setFocus();
                return true;
            }
        }
        return false;
    }
    return QDialog::eventFilter(object, event);
}

void SmithWatermanDialog::changeResultSavingWidgets(const QString & currentText)
{
    if(SmithWatermanSettings::getResultViewNames()[SmithWatermanSettings::ANNOTATIONS] == currentText) {
        annotationsWidget->show();
        alignmentParams->hide();
        bttnRun->setText(tr(RUN_BUTTON_ANNOT_RESULT_LABEL));
    } else if(SmithWatermanSettings::getResultViewNames()[SmithWatermanSettings::MULTIPLE_ALIGNMENT] == currentText) {
        annotationsWidget->hide();
        alignmentParams->show();
        bttnRun->setText(tr(RUN_BUTTON_MA_RESULT_LABEL));
    } else
        assert(0);

    annotationsWidget->layout()->update();
}

void SmithWatermanDialog::sl_resultViewChanged(const QString & text)
{
    changeResultSavingWidgets(text);
}

void SmithWatermanDialog::sl_patternChanged()
{
    updatePatternFieldVisualState();
}

void SmithWatermanDialog::sl_templateButtonPressed()
{
    const QPushButton * senderButton = qobject_cast<QPushButton *>(sender());
    const QString buttonText = senderButton->text();
    QLineEdit * editInFocus = dynamic_cast<QLineEdit *>(QApplication::focusWidget());

    assert(NULL != senderButton && NULL != editInFocus);

    const QString buttonTag = buttonText.left(buttonText.indexOf(SHORTHAND_AND_LABEL_SEPARATOR));
    editInFocus->setText(editInFocus->text().append(buttonTag));
}

bool SmithWatermanDialog::checkTemplateButtonName(const QString & name)
{
    if(OPEN_SQUARE_BRACKET != name[0])
        return false;

    quint32 closeBracketIndex = 0;

    for(qint32 i = 1; i < name.length(); ++i) {
        if(!name[i].isLetterOrNumber() && !name[i].isSpace() && CLOSE_SQUARE_BRACKET != name[i])
            return false;

        if(0 == closeBracketIndex) {
            if(CLOSE_SQUARE_BRACKET == name[i] && SHORTHAND_AND_LABEL_SEPARATOR == name[i + 1]) {
                if(0 != i - 1) {
                    closeBracketIndex = i;
                    ++i;
                } else
                    return false;

            } else if(name[i].isSpace())
                return false;
        }
        else if(0 != closeBracketIndex && name[i].isLetterOrNumber()
                && SHORTHAND_AND_LABEL_SEPARATOR == name[i - 1])
            return true;
    }

    return false;
}

void SmithWatermanDialog::fillTemplateNamesFieldsByDefault()
{
    const QStringList * defaultMobjectTags = tagsRegistry->getDefaultTagsForMobjectsNames();
    const QString defaultMobjectTagsString = defaultMobjectTags->join(QString(CLOSE_SQUARE_BRACKET)
                                                                      + DEFAULT_SHORTHANDS_SEPARATOR
                                                                      + OPEN_SQUARE_BRACKET)
                                             .append(CLOSE_SQUARE_BRACKET)
                                             .prepend(OPEN_SQUARE_BRACKET);
    mObjectNameTmpl->setText(defaultMobjectTagsString);
    delete defaultMobjectTags;

    const QStringList * defaultRefSubseqTags = tagsRegistry->getDefaultTagsForRefSubseqNames();
    const QString defaultRefSubseqTagsString = defaultRefSubseqTags->join(QString(CLOSE_SQUARE_BRACKET)
                                                                          + DEFAULT_SHORTHANDS_SEPARATOR
                                                                          + OPEN_SQUARE_BRACKET)
                                               .append(CLOSE_SQUARE_BRACKET)
                                               .prepend(OPEN_SQUARE_BRACKET);
    refSubseqNameTmpl->setText(defaultRefSubseqTagsString);
    delete defaultRefSubseqTags;

    const QStringList * defaultPtrnSubseqTags = tagsRegistry->getDefaultTagsForPtrnSubseqNames();
    const QString defaultPtrnSubseqTagsString = defaultPtrnSubseqTags->join(QString(CLOSE_SQUARE_BRACKET)
                                                                            + DEFAULT_SHORTHANDS_SEPARATOR
                                                                            + OPEN_SQUARE_BRACKET)
                                                .append(CLOSE_SQUARE_BRACKET)
                                                .prepend(OPEN_SQUARE_BRACKET);
    patternSubseqNameTmpl->setText(defaultPtrnSubseqTagsString);
    delete defaultPtrnSubseqTags;

    patternSequenceName->setText(DEFAULT_PATTERN_SEQUENCE_NAME + QString::number(dialogConfig->countOfLaunchesAlgorithm));
}

void SmithWatermanDialog::updatePatternFieldVisualState()
{
    int patternLen = teditPattern->toPlainText().length();
    if (patternLen == 0)
        lblPattern->setText(tr("Enter pattern here"));
    else
        lblPattern->setText(tr("Pattern length: %1").arg(patternLen));
}

void SmithWatermanDialog::fillTemplateButtonsList()
{
    templateButtons = tagsRegistry->getTagsButtons();
    templateButtonsApplicability = tagsRegistry->getBitmapOfTagsApplicability();

    quint8 buttonRow = 0;
    quint8 buttonColumn = 0;

    for(QList<QPushButton *>::const_iterator it = templateButtons->constBegin(); it != templateButtons->constEnd(); ++it, ++buttonColumn) {
        if(COUNT_OF_TEMPLATE_BUTTONS_IN_ROW == buttonColumn) {
            buttonColumn = 0;
            ++buttonRow;
        }
        tmplButtonsLayout->addWidget(*it, buttonRow, buttonColumn);
        (*it)->setEnabled(false);
        (*it)->setFocusPolicy(Qt::NoFocus);
        assert(checkTemplateButtonName((*it)->text()));
    }
}

void SmithWatermanDialog::addAnnotationWidget()
{
    U2SequenceObject *dnaso = qobject_cast<U2SequenceObject*>(ctxSeq->getSequenceGObject());
    CreateAnnotationModel acm;

    acm.sequenceObjectRef = GObjectReference(dnaso);
    acm.hideLocation = true;
    acm.useAminoAnnotationTypes = ctxSeq->getAlphabet()->isAmino();
    acm.sequenceLen = dnaso->getSequenceLength();
    annotationController = new CreateAnnotationWidgetController(acm, this);
    QWidget* caw = annotationController->getWidget();
    QVBoxLayout* l = new QVBoxLayout();
    l->setMargin(0);
    l->addWidget(caw);

    addPatternContentQualifier = new QCheckBox(
        tr( "Add qualifier with corresponding pattern subsequences to result annotations" ),
        annotationsWidget );
    addPatternContentQualifier->setObjectName("addPatternContentQualifierCheck");
    l->addWidget( addPatternContentQualifier );

    annotationsWidget->setLayout(l);
    annotationsWidget->setMinimumSize(caw->layout()->minimumSize());
}

void SmithWatermanDialog::setParameters()
{
    const DNAAlphabet* alphabet = ctxSeq->getAlphabet();
    QStringList matrixList = substMatrixRegistry->selectMatrixNamesByAlphabet(alphabet);
    if (!matrixList.isEmpty())
        bttnViewMatrix->setEnabled(true);

    comboMatrix->addItems(matrixList);

    QStringList alg_lst = swTaskFactoryRegistry->getListFactoryNames();
    comboRealization->addItems(alg_lst);

    QStringList filterIds = swResultFilterRegistry->getFiltersIds(); //TODO: use localized names!
    comboResultFilter->addItems(filterIds);
    int defaultFilterIndex = filterIds.indexOf(swResultFilterRegistry->getDefaultFilterId());
    comboResultFilter->setCurrentIndex(defaultFilterIndex);

    radioSequence->setEnabled(true);
    radioSequence->setChecked(true);
    if (0 != ctxSeq->getAminoTT())
        radioTranslation->setEnabled(true);

    radioDirect->setEnabled(true);
    if (ctxSeq->getComplementTT() != NULL) {
        radioComplement->setEnabled(true);
        radioBoth->setEnabled(true);
        radioBoth->setChecked(true);
    } else
        radioDirect->setChecked(true);

    spinScorePercent->setValue(DEF_PERCENT_OF_SCORE);
    spinGapOpen->setValue(DEF_GAP_OPEN_SCORE);
    spinGapExtd->setValue(DEF_GAP_EXTD_SCORE);

    resultViewVariants->addItem(tr(SmithWatermanSettings::getResultViewNames()[SmithWatermanSettings::MULTIPLE_ALIGNMENT]));
    resultViewVariants->addItem(tr(SmithWatermanSettings::getResultViewNames()[SmithWatermanSettings::ANNOTATIONS]));
    resultViewVariants->setCurrentIndex(0);

    mObjectNameTmpl->installEventFilter(this);
    refSubseqNameTmpl->installEventFilter(this);
    patternSubseqNameTmpl->installEventFilter(this);
}

void SmithWatermanDialog::initResultDirPath()
{
    QString lastDir = AppContext::getSettings()->getValue(SETTINGS_LASTDIR, QString(""), true).toString();
    if (lastDir.isEmpty() || !QDir(lastDir).exists()) {
        lastDir = QDir::homePath();
        const Project * curProject = AppContext::getProject();
        if (curProject != NULL) {
            const QString & curProjectUrl = curProject->getProjectURL();
            if (!curProjectUrl.isEmpty()) {
                QFileInfo fi(curProjectUrl);
                const QDir & curProjectDir = fi.absoluteDir();
                lastDir = curProjectDir.absolutePath();
            }
        }
    }
    alignmentFilesPath->setText(lastDir + '/');
}

void SmithWatermanDialog::templateEditInFocus()
{
    quint8 counter = 0;
    foreach(QPushButton * button, *templateButtons) {
        if((!templateButtonsApplicability->at(counter)
            && mObjectNameTmpl == QApplication::focusWidget())
            || mObjectNameTmpl != QApplication::focusWidget()) {
            button->setEnabled(true);
        }
        ++counter;
    }
}

void SmithWatermanDialog::templateEditLostFocus()
{
    foreach(QPushButton * button, *templateButtons) {
        if(button->isEnabled()) {
            button->setDisabled(true);
        }
    }
}

void SmithWatermanDialog::sl_browseAlignFilesDir()
{
    const QString openUrl = QFileInfo(alignmentFilesPath->text()).absoluteDir().absolutePath();
    const QString name = U2FileDialog::getExistingDirectory(this, tr("Choose folder"), openUrl);
    if (!name.isEmpty()) {
        alignmentFilesPath->setText(name + '/');
        AppContext::getSettings()->setValue(SETTINGS_LASTDIR, name, true);
    }
}

void SmithWatermanDialog::sl_bttnViewMatrix()
{
    QString strSelectedMatrix = comboMatrix->currentText();
    SMatrix mtx = substMatrixRegistry->getMatrix(strSelectedMatrix);
    if (mtx.isEmpty()) {
        QMessageBox::critical(this, windowTitle(), tr("Matrix not found."));
        return;
    }
    QObjectScopedPointer<SubstMatrixDialog> smDialog = new SubstMatrixDialog(mtx, this);
    smDialog->exec();
}

void SmithWatermanDialog::sl_translationToggled(bool checked)
{
    const DNAAlphabet* alphabet = 0;
    if (checked) {
        DNATranslation* aminoTT = ctxSeq->getAminoTT();
        alphabet = aminoTT->getDstAlphabet();
    } else
        alphabet = ctxSeq->getAlphabet();

    QStringList matrixList = substMatrixRegistry->selectMatrixNamesByAlphabet(alphabet);
    if (matrixList.isEmpty())
        bttnViewMatrix->setEnabled(false);
    else
        bttnViewMatrix->setEnabled(true);

    comboMatrix->clear();
    comboMatrix->addItems(matrixList);
}

QString SmithWatermanDialog::validateResultDirPath() const
{
    QString error = QDir(alignmentFilesPath->text()).exists() ? QString() : tr(RESULT_DIR_NOT_FOUND_MESSAGE);
    return error;
}

void SmithWatermanDialog::sl_cancelButton()
{
    saveDialogConfig();
}

void SmithWatermanDialog::sl_bttnRun()
{
    QString err = annotationController->validate();
    if(err.isEmpty())
        err = validateResultDirPath();

    if (!err.isEmpty()) {
        QMessageBox::critical(this, L10N::errorTitle(), err);
        return;
    }
    if (readParameters()) {
        if(SmithWatermanSettings::ANNOTATIONS == config.resultView) {
            bool objectPrepared = annotationController->prepareAnnotationObject();
            if (!objectPrepared){
                QMessageBox::critical(this, L10N::errorTitle(), tr("Cannot create an annotation object. Please check settings."));
                return;
            }

            const CreateAnnotationModel& m = annotationController->getModel();
            AnnotationTableObject *obj = m.getAnnotationObject();
            U2FeatureType annotationType = m.data->type;
            QString annotationName = m.data->name;
            QString annotationGroup = m.groupName;
            ctxSeq->getAnnotatedDNAView()->tryAddObject(obj);

            config.resultCallback = new SmithWatermanReportCallbackAnnotImpl(obj, annotationType, annotationName,
                annotationGroup, m.description, addPatternContentQualifier->isChecked());
            config.includePatternContent = addPatternContentQualifier->isChecked();
        } else if (SmithWatermanSettings::MULTIPLE_ALIGNMENT == config.resultView) {
            const U2SequenceObject * sequence = ctxSeq->getSequenceObject();
            U2OpStatusImpl os;
            QByteArray seqData = sequence->getWholeSequenceData(os);
            CHECK_OP_EXT(os, QMessageBox::critical(this, L10N::errorTitle(), os.getError()), );
            config.resultCallback = new SmithWatermanReportCallbackMAImpl(alignmentFilesPath->text(), mObjectNameTmpl->text(),
                                                                        refSubseqNameTmpl->text(), patternSubseqNameTmpl->text(),
                                                                        seqData, config.ptrn,
                                                                        sequence->getSequenceName(), patternSequenceName->text(),
                                                                        sequence->getAlphabet(), config.aminoTT);
        }
        config.resultListener = new SmithWatermanResultListener;

        Task* task = realization->getTaskInstance(config, tr("SmithWatermanTask") );
        AppContext::getTaskScheduler()->registerTopLevelTask(task);
        saveDialogConfig();
        QDialog::accept();
    } else {
        clearAll();
    }
}

bool SmithWatermanDialog::readParameters()
{
    clearAll();

    U2OpStatusImpl os;
    config.sqnc = ctxSeq->getSequenceObject()->getWholeSequenceData(os);
    CHECK_OP_EXT(os, QMessageBox::critical(this, L10N::errorTitle(), os.getError()), false);
    config.searchCircular = ctxSeq->getSequenceObject()->isCircular();

    DNATranslation* aminoTT = 0;
    bool isTranslation = radioTranslation->isChecked();
    if (isTranslation)
        aminoTT = ctxSeq->getAminoTT();
    else
        aminoTT = 0;

    if (!readSubstMatrix())
        return false;

    if (!readPattern(aminoTT))
        return false;

    config.aminoTT = aminoTT;

    if (!readRegion() || !readGapModel() || !readResultFilter() || !readRealization())
        return false;

    qint32 resultView = config.getResultViewKeyForString(resultViewVariants->currentText());
    if(STRING_HAS_NO_KEY_MESSAGE != resultView) {
        config.resultView = static_cast<SmithWatermanSettings::SWResultView>(resultView);

        if(mObjectNameTmpl->text().isEmpty() || refSubseqNameTmpl->text().isEmpty()
            || patternSubseqNameTmpl->text().isEmpty() || patternSequenceName->text().isEmpty()) {
            QMessageBox::critical(this, windowTitle(),
                                  tr("Names of result alignment files or names of result "
                                  "subsequences cannot be empty."));
            return false;
        }
    } else
        assert(0);

    if (radioDirect->isChecked())
        config.strand = StrandOption_DirectOnly;
    else if (radioComplement->isChecked())
        config.strand = StrandOption_ComplementOnly;
    else if (radioBoth->isChecked())
        config.strand = StrandOption_Both;
    else
        assert(0);

    config.complTT = ctxSeq->getComplementTT();
    if (0 == config.complTT && isComplement(config.strand)) {
        QMessageBox::critical(this, windowTitle(), tr("Complement translation is not found."));
        return false;
    }

    return true;
}

bool SmithWatermanDialog::readRealization()
{
    QString strSelectedRealization = comboRealization->currentText();
    SmithWatermanTaskFactory* rlz = swTaskFactoryRegistry->getFactory(strSelectedRealization);
    if (0 == rlz) {
        QMessageBox::critical(this, windowTitle(), tr("Algorithm is not found."));
        return false;
    }
    realization = rlz;
    return true;
}

bool SmithWatermanDialog::readSubstMatrix()
{
    QString strSelectedMatrix = comboMatrix->currentText();
    SMatrix mtx = substMatrixRegistry->getMatrix(strSelectedMatrix);
    if (mtx.isEmpty()) {
        QMessageBox::critical(this, windowTitle(),
                              tr("Matrix %1 is not found.").arg(strSelectedMatrix));
        return false;
    }
    config.pSm = mtx;
    return true;
}

bool SmithWatermanDialog::readRegion()
{
    bool isRegionOk=false;
    config.globalRegion = regionSelector->getRegion(&isRegionOk);
    return isRegionOk;
}

bool SmithWatermanDialog::readGapModel()
{
    float scoreGapOpen = spinGapOpen->value();
    config.gapModel.scoreGapOpen = scoreGapOpen;

    float scoreGapExtd = spinGapExtd->value();
    config.gapModel.scoreGapExtd = scoreGapExtd;

    return true;
}

bool SmithWatermanDialog::readResultFilter()
{
    int percentOfScore = spinScorePercent->value();
    config.percentOfScore = percentOfScore;

    QString strSelectedFilter = comboResultFilter->currentText();
    SmithWatermanResultFilter* filter = swResultFilterRegistry->getFilter(strSelectedFilter);
    if (0 == filter) {
        QMessageBox::critical(this, windowTitle(), tr("Filter is not found."));
        return false;
    }
    config.resultFilter = filter;
    return true;
}

bool SmithWatermanDialog::readPattern(DNATranslation* aminoTT)
{
    const DNAAlphabet* al = 0;
    if (0 == aminoTT) {
        assert(config.pSm.getAlphabet() != NULL);
        al = config.pSm.getAlphabet();
    } else
        al = aminoTT->getDstAlphabet();

    if (0 == al) {
        QMessageBox::critical(this, windowTitle(), tr("Internal error"));
        return false;
    }

    QString inputPattern = teditPattern->toPlainText();
    stripFormatSymbolsFromPattern(inputPattern);

    if (inputPattern.isEmpty()) {
        QMessageBox::critical(this, windowTitle(),  tr("Pattern is empty"));
        return false;
    }

    QByteArray pattern;
    if (!al->isCaseSensitive()) {
        QString upperPattern = inputPattern.toUpper();
        pattern = upperPattern.toLocal8Bit();
    } else
        pattern = inputPattern.toLocal8Bit();

    if (!TextUtils::fits(al->getMap(), pattern.constData(), pattern.length())) {
        QMessageBox::critical(this, windowTitle(),  tr("Pattern contains unknown symbol"));
        return false;
    }

    config.ptrn = pattern;
    return true;
}

void SmithWatermanDialog::stripFormatSymbolsFromPattern(QString & pattern)
{
    const qint32 fastaSequenceNameStart = pattern.indexOf(QRegExp("\\s*>"));

    if(0 == fastaSequenceNameStart)
        pattern = pattern.split(QRegExp("\\s+"), QString::SkipEmptyParts).last();
    else if(-1 != fastaSequenceNameStart)
        return;
    else {
        pattern.replace(QRegExp("\\s+"), "");
        pattern.replace(QRegExp("\\d+"), "");
    }
}

void SmithWatermanDialog::clearAll()
{
    config.sqnc = QByteArray();
    config.ptrn = QByteArray();
    config.globalRegion = U2Region(0, 0);
    config.gapModel.scoreGapOpen = 0;
    config.gapModel.scoreGapExtd = 0;
    config.pSm = SMatrix();
    config.percentOfScore = 0;
    config.resultCallback = 0;
    config.resultFilter   = 0;
    config.resultListener = 0;
    config.strand = StrandOption_DirectOnly;
    config.complTT = 0;
    config.aminoTT = 0;

    realization = 0;
}

void SmithWatermanDialog::loadDialogConfig()
{
    const SmithWatermanSearchType searchType = dialogConfig->searchType;
    switch (searchType) {
    case (SmithWatermanSearchType_inSequence):
        radioSequence->setChecked(true);
        break;
    case (SmithWatermanSearchType_inTranslation):
        radioTranslation->setChecked(true);
        break;
    default:
        break;
    }

    const StrandOption strand = dialogConfig->strand;
    switch (strand) {
    case (StrandOption_DirectOnly):
        radioDirect->setChecked(true);
        break;
    case (StrandOption_ComplementOnly):
        radioComplement->setChecked(true);
        break;
    case (StrandOption_Both):
        radioBoth->setChecked(true);
        break;
    default:
        assert(0);
    }

    const QByteArray& prevPattern = dialogConfig->ptrn;
    if (!prevPattern.isEmpty())
        teditPattern->setText(prevPattern);

    const QString& prevAlgVersion = dialogConfig->algVersion;
    if (swTaskFactoryRegistry->hadRegistered(prevAlgVersion)) {
        int algIndex = comboRealization->findText(prevAlgVersion);
        assert(-1 != algIndex);
        comboRealization->setCurrentIndex(algIndex);
    }

    const QString& prevScoringMatrix = dialogConfig->scoringMatrix;
    if (!substMatrixRegistry->getMatrix(prevScoringMatrix).isEmpty()) {
        int mtxIndex = comboMatrix->findText(prevScoringMatrix);
         if( -1 != mtxIndex )
             comboMatrix->setCurrentIndex(mtxIndex);
    }

    const float scoreGapOpen = dialogConfig->gm.scoreGapOpen;
    spinGapOpen->setValue(scoreGapOpen);

    const float scoreGapExtd = dialogConfig->gm.scoreGapExtd;
    spinGapExtd->setValue(scoreGapExtd);

    const QString& prevResultFilter = dialogConfig->resultFilter;
    if (swResultFilterRegistry->isRegistered(prevResultFilter)) {
        int filterIndex = comboResultFilter->findText(prevResultFilter);
        assert(-1 != filterIndex);
        comboResultFilter->setCurrentIndex(filterIndex);
    }

    const float minScoreInPercent = dialogConfig->minScoreInPercent;
    spinScorePercent->setValue(minScoreInPercent);

    const QString resultViewName = QString(SmithWatermanSettings::getResultViewNames()[dialogConfig->resultView]);
    const qint32 resultNameIndex = resultViewVariants->findText(resultViewName);
    assert(-1 != resultNameIndex);
    resultViewVariants->setCurrentIndex(resultNameIndex);
    changeResultSavingWidgets(resultViewVariants->currentText());

    if(!dialogConfig->alignFilesNamesTmpl.isEmpty())
        mObjectNameTmpl->setText(dialogConfig->alignFilesNamesTmpl);

    if(!dialogConfig->refSubseqNamesTmpl.isEmpty())
        refSubseqNameTmpl->setText(dialogConfig->refSubseqNamesTmpl);

    if(!dialogConfig->ptrnSubseqNamesTmpl.isEmpty())
        patternSubseqNameTmpl->setText(dialogConfig->ptrnSubseqNamesTmpl);

    if(!dialogConfig->patternSequenceName.isEmpty())
        patternSequenceName->setText(dialogConfig->patternSequenceName);

    if(mObjectNameTmpl->text().isEmpty() && refSubseqNameTmpl->text().isEmpty() && patternSubseqNameTmpl->text().isEmpty() &&
        patternSequenceName->text().isEmpty())
        fillTemplateNamesFieldsByDefault();

    advOptions->setChecked(dialogConfig->enableAdvancedMASettings);
    addPatternContentQualifier->setChecked( dialogConfig->addPatternSubsequenceAsQualifier );

    return;
}

void SmithWatermanDialog::saveDialogConfig()
{
    dialogConfig->ptrn = teditPattern->toPlainText().toLatin1();

    dialogConfig->algVersion = comboRealization->currentText();
    dialogConfig->scoringMatrix = comboMatrix->currentText();

    dialogConfig->gm.scoreGapOpen = (float)spinGapOpen->value();
    dialogConfig->gm.scoreGapExtd = (float)spinGapExtd->value();

    dialogConfig->resultFilter = comboResultFilter->currentText();
    dialogConfig->minScoreInPercent = spinScorePercent->value();

    dialogConfig->searchType =  (radioSequence->isChecked()) ?
                                (SmithWatermanSearchType_inSequence):
                                (SmithWatermanSearchType_inTranslation);

    dialogConfig->strand =  (radioDirect->isChecked()) ?
                            (StrandOption_DirectOnly):
                            (dialogConfig->strand);
    dialogConfig->strand =  (radioComplement->isChecked()) ?
                            (StrandOption_ComplementOnly):
                            (dialogConfig->strand);
    dialogConfig->strand =  (radioBoth->isChecked()) ?
                            (StrandOption_Both):
                            (dialogConfig->strand);

    const qint32 checkResultView = config.getResultViewKeyForString(resultViewVariants->currentText());
    assert(STRING_HAS_NO_KEY_MESSAGE != checkResultView);
    dialogConfig->resultView = static_cast<SmithWatermanSettings::SWResultView>(checkResultView);

    const QString alignFilesNames = mObjectNameTmpl->text();
    if(!alignFilesNames.isEmpty())
        dialogConfig->alignFilesNamesTmpl = alignFilesNames;

    const QString refSubseqNames = refSubseqNameTmpl->text();
    if(!refSubseqNames.isEmpty())
        dialogConfig->refSubseqNamesTmpl = refSubseqNames;

    const QString ptrnSubseqNames = patternSubseqNameTmpl->text();
    if(!ptrnSubseqNames.isEmpty())
        dialogConfig->ptrnSubseqNamesTmpl = ptrnSubseqNames;

    const QString patternName = patternSequenceName->text();
    if(!patternName.isEmpty())
        dialogConfig->patternSequenceName = patternName;

    dialogConfig->enableAdvancedMASettings = advOptions->isChecked();

    if(sender() == bttnRun)
        dialogConfig->countOfLaunchesAlgorithm++;

    dialogConfig->addPatternSubsequenceAsQualifier = addPatternContentQualifier->isChecked( );

    return;
}

void SmithWatermanDialogController::run(QWidget* p, ADVSequenceObjectContext* ctx, SWDialogConfig* dialogConfig)
{
    QObjectScopedPointer<SmithWatermanDialog> smv = new SmithWatermanDialog(p, ctx, dialogConfig);
    smv->exec();
}

} // namespace
