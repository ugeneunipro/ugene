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

#include "DocumentFormatSelectorController.h"

#include <QMessageBox>
#include <QMouseEvent>
#include <QPushButton>

#include <U2Core/DocumentImport.h>
#include <U2Core/DocumentModel.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/TextUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>

namespace U2 {

LabelClickProvider::LabelClickProvider(QLabel *label, QRadioButton *rb)
    : QObject(label),
      label(label),
      rb(rb) {
}

bool LabelClickProvider::eventFilter(QObject *object, QEvent *event) {
    CHECK(NULL != label, false);
    CHECK(NULL != rb, false);
    CHECK(label == object, false);

    CHECK(QEvent::MouseButtonPress == event->type(), false);
    QMouseEvent *mouseEvent = dynamic_cast<QMouseEvent *>(event);
    CHECK(NULL != event, false);
    CHECK(Qt::LeftButton == mouseEvent->button(), false);

    rb->toggle();
    return false;
}

DocumentFormatSelectorController::DocumentFormatSelectorController(QList<FormatDetectionResult> &results, QWidget *p)
    : QDialog(p), formatDetectionResults(results) {
    setupUi(this);
    new HelpButton(this, buttonBox, "46499637");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    setObjectName("DocumentFormatSelectorDialog");
}

int DocumentFormatSelectorController::selectResult(const GUrl &url, QByteArray &rawData, QList<FormatDetectionResult> &results) {
    SAFE_POINT(!results.isEmpty(), "Results list is empty!", -1);
    if (results.size() == 1) {
        return 0;
    }

    QObjectScopedPointer<DocumentFormatSelectorController> d = new DocumentFormatSelectorController(results, QApplication::activeModalWidget());
    d->optionsBox->setTitle(tr("Options for %1").arg(url.fileName()));
    QByteArray safeData = rawData;
    if (TextUtils::contains(TextUtils::BINARY, safeData.constData(), safeData.size())) {
        TextUtils::replace(safeData.data(), safeData.length(), TextUtils::BINARY, '?');
    }
    d->previewEdit->setPlainText(safeData);

    QVBoxLayout *vbox = new QVBoxLayout();
    QList<DocumentFormatId> detectedIds;
    for (int i = 0; i < results.size(); i++) {
        const FormatDetectionResult &r = results[i];
        if (NULL != r.format) {
            detectedIds.append(r.format->getFormatId());
        }
        QString text;
        QString objName;
        if (r.format != NULL) {
            text = tr("<b>%1</b> format. Score: %2 <i>(%3)</i>").arg(r.format->getFormatName()).arg(r.score()).arg(score2Text(r.score()));
            objName = r.format->getFormatName();
        } else if (r.importer != NULL) {
            // #A6392E is the same color as Theme::errorColorLabelStr(). For some reason Qt's HTML parser cannot handle this value as rgb.
            text = tr("<b><font color=#A6392E>Import: </font>%1</b>. Score: %2 (<i>%3</i>)").arg(r.importer->getImporterName()).arg(r.score()).arg(score2Text(r.score()));
            objName = r.importer->getImporterName();
        } else {
            assert(0);
            continue;
        }
        QHBoxLayout *hbox = new QHBoxLayout();
        QRadioButton *rb = new QRadioButton();
        rb->setObjectName(objName);
        rb->setChecked(i == 0);

        QLabel *label = new QLabel(text);
        label->setObjectName(QString("label_%1").arg(i + 1));
        label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        label->setSizePolicy(QSizePolicy::Expanding, label->sizePolicy().verticalPolicy());
        label->installEventFilter(new LabelClickProvider(label, rb));

        QToolButton *moreButton = new QToolButton();
        moreButton->setText(tr("more.."));
        moreButton->setEnabled(!r.getFormatDescriptionText().isEmpty());
        d->moreButtons << moreButton;
        QObject::connect(moreButton, SIGNAL(clicked()), d.data(), SLOT(sl_moreFormatInfo()));

        hbox->addWidget(rb);
        hbox->addWidget(label);
        hbox->addStretch(2);
        hbox->addWidget(moreButton);
        vbox->addLayout(hbox);
        d->radioButtons << rb;
    }
    //additional option: user selecting format
    {
        QString text(tr("Choose format manually"));
        QHBoxLayout *hbox = new QHBoxLayout();
        QRadioButton *rb = new QRadioButton();
        rb->setObjectName("chooseFormatManuallyRadio");

        QLabel *label = new QLabel(text);
        label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        label->setSizePolicy(QSizePolicy::Expanding, label->sizePolicy().verticalPolicy());
        label->installEventFilter(new LabelClickProvider(label, rb));

        d->userSelectedFormat = new QComboBox();
        d->userSelectedFormat->setObjectName("userSelectedFormat");
        const DocumentFormatRegistry *formatRegistry = AppContext::getDocumentFormatRegistry();
        SAFE_POINT(formatRegistry != NULL, "FormatRegistry is NULL!", -1);
        DocumentFormatConstraints constraints;
        constraints.addFlagToExclude(DocumentFormatFlag_Hidden);
        QMap<DocumentFormatId, QString> formats;
        foreach (const DocumentFormatId &id, formatRegistry->selectFormats(constraints)) {
            if (!detectedIds.contains(id)) {
                const QString formatName = formatRegistry->getFormatById(id)->getFormatName();
                formats[id] = formatName;
            }
        }
        QStringList formatNamesSorted = formats.values();
        formatNamesSorted.sort(Qt::CaseInsensitive);
        foreach (const QString &name, formatNamesSorted) {
            d->userSelectedFormat->addItem(name, formats.key(name));
        }

        hbox->addWidget(rb);
        hbox->addWidget(label);
        hbox->addStretch(2);
        hbox->addWidget(d->userSelectedFormat);
        vbox->addLayout(hbox);
        d->radioButtons << rb;
    }
    vbox->addStretch();
    d->optionsBox->setLayout(vbox);

    const int rc = d->exec();
    CHECK(!d.isNull(), -1);

    if (rc == QDialog::Rejected) {
        return -1;
    }
    int idx = d->getSelectedFormatIdx();
    if (idx == results.size()) {
        FormatDetectionResult *r = new FormatDetectionResult();
        DocumentFormatId id = d->userSelectedFormat->itemData(d->userSelectedFormat->currentIndex()).toString();
        r->format = AppContext::getDocumentFormatRegistry()->getFormatById(id);
        results.insert(idx, *r);
    }
    return idx;
}

QString DocumentFormatSelectorController::score2Text(int score) {
    if (score <= FormatDetection_NotMatched) {
        return tr("Not matched");
    }
    if (score <= FormatDetection_VeryLowSimilarity) {
        return tr("Very low similarity");
    }
    if (score <= FormatDetection_LowSimilarity) {
        return tr("Low similarity");
    }
    if (score <= FormatDetection_AverageSimilarity) {
        return tr("Average similarity");
    }
    if (score <= FormatDetection_HighSimilarity) {
        return tr("High similarity");
    }
    if (score < FormatDetection_Matched) {
        return tr("Very high similarity");
    }
    return tr("Perfect match");
}

void DocumentFormatSelectorController::sl_moreFormatInfo() {
    QToolButton *tb = qobject_cast<QToolButton *>(sender());
    SAFE_POINT(tb != NULL, "Failed to derive selected format info!", );
    int idx = moreButtons.indexOf(tb);
    const FormatDetectionResult &dr = formatDetectionResults[idx];
    QMessageBox::information(this, tr("Format details for '%1' format").arg(dr.getFormatOrImporterName()), dr.getFormatDescriptionText());
}

int DocumentFormatSelectorController::getSelectedFormatIdx() const {
    int idx = 0;
    for (int i = 0; i < radioButtons.size(); i++) {
        if (radioButtons[i]->isChecked()) {
            idx = i;
            break;
        }
    }
    return idx;
}

}    // namespace U2
