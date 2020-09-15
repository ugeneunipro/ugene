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

#include "ColorSchemaDialogController.h"

#include <QColorDialog>
#include <QMessageBox>
#include <QPainter>

#include <U2Algorithm/ColorSchemeUtils.h>

#include <U2Core/AppContext.h>
#include <U2Core/FileAndDirectoryUtils.h>
#include <U2Core/GUrlUtils.h>
#include <U2Core/L10n.h>
#include <U2Core/QObjectScopedPointer.h>
#include <U2Core/Theme.h>
#include <U2Core/U2OpStatusUtils.h>
#include <U2Core/U2SafePoints.h>

#include <U2Gui/HelpButton.h>
#include <U2Gui/U2FileDialog.h>

namespace U2 {

ColorSchemaDialogController::ColorSchemaDialogController(QMap<char, QColor> &colors)
    : QDialog(),
      alphabetColorsView(NULL),
      newColors(colors),
      storedColors(colors) {
}

int ColorSchemaDialogController::adjustAlphabetColors() {
    setupUi(this);
    new HelpButton(this, buttonBox, "49447346");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    alphabetColorsView = new QPixmap(alphabetColorsFrame->size());
    connect(clearButton, SIGNAL(clicked()), SLOT(sl_onClear()));
    connect(restoreButton, SIGNAL(clicked()), SLOT(sl_onRestore()));

    update();

    return exec();
}

ColorSchemaDialogController::~ColorSchemaDialogController() {
    delete alphabetColorsView;
}

void ColorSchemaDialogController::paintEvent(QPaintEvent *) {
    QPainter dialogPainter(this);
    const int columns = 6;

    const int rect_width = static_cast<double>(alphabetColorsFrame->size().width()) / columns;
    if (rect_width == 0) {
        return;
    }

    const int rows = (newColors.size() / columns) + ((newColors.size() % columns) ? 1 : 0);
    const int rect_height = static_cast<double>(alphabetColorsFrame->size().height()) / rows;
    if (rect_height == 0) {
        return;
    }
    int rect_height_rest = alphabetColorsFrame->size().height() % rows;

    delete alphabetColorsView;
    alphabetColorsView = new QPixmap(alphabetColorsFrame->size());

    QPainter painter(alphabetColorsView);
    QFont font;
    font.setFamily("Verdana");
    font.setPointSize(qMin(rect_width, rect_height) / 2);
    painter.setFont(font);

    QMapIterator<char, QColor> it(newColors);

    int hLineY = 0;
    for (int i = 0; i < rows; ++i) {
        int rh = rect_height;
        int rect_width_rest = alphabetColorsFrame->size().width() % columns;
        if (rect_height_rest > 0) {
            rh++;
            rect_height_rest--;
        }
        painter.drawLine(0, hLineY, alphabetColorsView->size().width(), hLineY);
        int vLineX = 0;
        for (int j = 0; j < columns; ++j) {
            if (!it.hasNext()) {
                break;
            }

            it.next();
            int rw = rect_width;
            if (rect_width_rest > 0) {
                rw++;
                rect_width_rest--;
            }
            QRect nextRect(vLineX, hLineY + 1, rw, rh - 1);
            painter.fillRect(nextRect, it.value());
            painter.drawText(nextRect, Qt::AlignCenter, QString(it.key()));
            painter.drawLine(vLineX, hLineY, vLineX, hLineY + rh);
            painter.drawLine(vLineX + rw, hLineY, vLineX + rw, hLineY + rh);
            vLineX += rw;
            charsPlacement[it.key()] = nextRect;
        }
        hLineY += rh;
        if (!it.hasNext()) {
            painter.fillRect(vLineX + 1, hLineY - rh + 1, alphabetColorsView->size().width() - vLineX - 1, rh - 1, dialogPainter.background());
            break;
        }
    }

    painter.drawLine(2, alphabetColorsView->size().height() - 2, alphabetColorsView->size().width() - 2, alphabetColorsView->size().height() - 2);
    painter.drawLine(alphabetColorsView->size().width() - 2, 2, alphabetColorsView->size().width() - 2, alphabetColorsView->size().height() - 2);

    painter.drawLine(alphabetColorsView->size().width() - 2, alphabetColorsView->size().height() - 2, 2, alphabetColorsView->size().height() - 2);
    painter.drawLine(alphabetColorsView->size().width() - 2, alphabetColorsView->size().height() - 2, alphabetColorsView->size().width() - 2, 2);

    dialogPainter.drawPixmap(alphabetColorsFrame->geometry().x(), alphabetColorsFrame->geometry().y(), *alphabetColorsView);
}

void ColorSchemaDialogController::sl_onClear() {
    storedColors = newColors;

    QMapIterator<char, QColor> it(newColors);
    while (it.hasNext()) {
        it.next();
        newColors[it.key()] = QColor(Qt::white);
    }

    update();
}

void ColorSchemaDialogController::sl_onRestore() {
    newColors = storedColors;
    update();
}

void ColorSchemaDialogController::mouseReleaseEvent(QMouseEvent *event) {
    QMapIterator<char, QRect> it(charsPlacement);

    while (it.hasNext()) {
        it.next();
        if (it.value().contains(event->pos().x() - alphabetColorsFrame->geometry().x(), event->pos().y() - alphabetColorsFrame->geometry().y())) {
            QObjectScopedPointer<QColorDialog> d = new QColorDialog(this);
#ifdef Q_OS_MAC
            // A workaround because of UGENE-2263
            // Another way should be found.
            // I suppose, that it is bug in the Qt libraries (Qt-4.8.5 for mac)
            d->setOption(QColorDialog::DontUseNativeDialog);
#endif
            const int res = d->exec();
            CHECK(!d.isNull(), );

            if (res == QDialog::Accepted) {
                newColors[it.key()] = d->selectedColor();
            }
            break;
        }
    }

    update();
}

/*Create MSA scheme dialog*/

CreateColorSchemaDialog::CreateColorSchemaDialog(ColorSchemeData *_newSchema, QStringList _usedNames)
    : usedNames(_usedNames), newSchema(_newSchema) {
    setupUi(this);
    new HelpButton(this, buttonBox, "49447346");
    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Create"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    alphabetComboBox->insertItem(0, QString(tr("Amino acid")), DNAAlphabet_AMINO);
    alphabetComboBox->insertItem(1, QString(tr("Nucleotide")), DNAAlphabet_NUCL);

    connect(alphabetComboBox, SIGNAL(currentIndexChanged(int)), SLOT(sl_alphabetChanged(int)));
    extendedModeBox->setVisible(false);
    validLabel->setStyleSheet("color: " + Theme::errorColorLabelStr() + "; font: bold;");
    validLabel->setVisible(false);
    adjustSize();

    connect(schemeName, SIGNAL(textEdited(const QString &)), SLOT(sl_schemaNameEdited(const QString &)));

    buttonBox->button(QDialogButtonBox::Ok)->setText(tr("OK"));
    buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    QPushButton *createButton = buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *cancelButton = buttonBox->button(QDialogButtonBox::Cancel);

    connect(createButton, SIGNAL(clicked()), this, SLOT(sl_createSchema()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(sl_cancel()));

    QSet<QString> excluded;
    foreach (const QString &usedName, usedNames) {
        excluded.insert(usedName);
    }
    schemeName->setText(GUrlUtils::rollFileName("Custom color scheme", excluded));
}

bool CreateColorSchemaDialog::isNameExist(const QString &text) {
    foreach (const QString &usedName, usedNames) {
        if (usedName == text) {
            return true;
        }
    }
    return false;
}

bool CreateColorSchemaDialog::isSchemaNameValid(const QString &text, QString &description) {
    if (text.isEmpty()) {
        description = tr("Name of scheme is empty.");
        return false;
    }
    int spaces = 0;
    for (int i = 0; i < text.length(); i++) {
        if (text[i] == ' ') {
            spaces++;
        }
    }
    if (spaces == text.length()) {
        description = tr("Name can't contain only spaces.");
        return false;
    }
    for (int i = 0; i < text.length(); ++i) {
        if (!text[i].isDigit() && !text[i].isLetter() && text[i] != QChar('_') && !text[i].isSpace()) {
            description = tr("Name has to consist of letters, digits, spaces") + "<br>" + tr("or underscore symbols only.");
            return false;
        }
    }
    if (isNameExist(text)) {
        description = tr("Color scheme with the same name already exists.");
        return false;
    }

    return true;
}

void CreateColorSchemaDialog::sl_schemaNameEdited(const QString &text) {
    QString description;
    const bool isNameValid = isSchemaNameValid(text, description);
    validLabel->setVisible(!isNameValid);
    adjustSize();
    if (isNameValid) {
        validLabel->clear();
    } else {
        validLabel->setText("Warning: " + description);
    }
}

void CreateColorSchemaDialog::sl_alphabetChanged(int index) {
    if (0 > index || index >= alphabetComboBox->count()) {
        return;
    }

    if (DNAAlphabet_AMINO == static_cast<DNAAlphabetType>(
                                 alphabetComboBox->itemData(index).toInt())) {
        extendedModeBox->setVisible(false);
        extendedModeBox->setChecked(false);
    } else {
        extendedModeBox->setVisible(true);
    }
    adjustSize();
}

int CreateColorSchemaDialog::createNewScheme() {
    return exec();
}

void CreateColorSchemaDialog::sl_createSchema() {
    QString description;
    if (!isSchemaNameValid(schemeName->text(), description)) {
        return;
    }

    int index = alphabetComboBox->currentIndex();
    if (index < 0 || index >= alphabetComboBox->count()) {
        return;
    }

    DNAAlphabetType type = DNAAlphabet_AMINO;
    bool defaultAlpType = true;

    if (static_cast<DNAAlphabetType>(alphabetComboBox->itemData(index).toInt()) == DNAAlphabet_NUCL) {
        type = DNAAlphabet_NUCL;
        if (extendedModeBox->isChecked()) {
            defaultAlpType = false;
        }
    }

    QMap<char, QColor> alpColors = ColorSchemeUtils::getDefaultSchemaColors(type, defaultAlpType);

    QObjectScopedPointer<ColorSchemaDialogController> controller = new ColorSchemaDialogController(alpColors);
    const int r = controller->adjustAlphabetColors();
    CHECK(!controller.isNull(), );

    if (r == QDialog::Rejected) {
        return;
    }

    newSchema->name = schemeName->text();
    newSchema->type = type;
    newSchema->defaultAlpType = defaultAlpType;

    QMapIterator<char, QColor> it(alpColors);
    while (it.hasNext()) {
        it.next();
        newSchema->alpColors[it.key()] = it.value();
    }
    accept();
}

void CreateColorSchemaDialog::sl_cancel() {
    reject();
}

ColorSchemaSettingsPageWidget::ColorSchemaSettingsPageWidget(ColorSchemaSettingsPageController *) {
    setupUi(this);

    connect(colorsDirButton, SIGNAL(clicked()), SLOT(sl_onColorsDirButton()));
    connect(changeSchemaButton, SIGNAL(clicked()), SLOT(sl_onChangeColorSchema()));
    connect(addSchemaButton, SIGNAL(clicked()), SLOT(sl_onAddColorSchema()));
    connect(deleteSchemaButton, SIGNAL(clicked()), SLOT(sl_onDeleteColorSchema()));
    connect(colorSchemas, SIGNAL(currentRowChanged(int)), SLOT(sl_schemaChanged(int)));

    sl_schemaChanged(colorSchemas->currentRow());
}

void ColorSchemaSettingsPageWidget::setState(AppSettingsGUIPageState *s) {
    ColorSchemaSettingsPageState *state = qobject_cast<ColorSchemaSettingsPageState *>(s);
    colorsDirEdit->setText(state->colorsDir);
    customSchemas = state->customSchemas;
    removedCustomSchemas = state->removedCustomSchemas;
    colorSchemas->clear();

    foreach (const ColorSchemeData &customSchema, customSchemas) {
        colorSchemas->addItem(new QListWidgetItem(customSchema.name, colorSchemas));
    }
    update();
}

AppSettingsGUIPageState *ColorSchemaSettingsPageWidget::getState(QString &err) const {
    QString colorsDir = colorsDirEdit->text();
    U2OpStatusImpl os;
    GUrlUtils::prepareDirLocation(colorsDir, os);
    if (os.hasError()) {
        err = os.getError();
        return nullptr;
    }
    ColorSchemaSettingsPageState *state = new ColorSchemaSettingsPageState();
    state->colorsDir = colorsDir;
    state->customSchemas = customSchemas;
    state->removedCustomSchemas = removedCustomSchemas;
    return state;
}

void ColorSchemaSettingsPageWidget::sl_schemaChanged(int index) {
    if (index < 0 || index >= colorSchemas->count()) {
        changeSchemaButton->setDisabled(true);
        deleteSchemaButton->setDisabled(true);
    } else {
        changeSchemaButton->setEnabled(true);
        deleteSchemaButton->setEnabled(true);
    }
}

void ColorSchemaSettingsPageWidget::sl_onColorsDirButton() {
    QString path = colorsDirEdit->text();
    QString dir = U2FileDialog::getExistingDirectory(this, tr("Choose Folder"), path, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (dir.isEmpty()) {
        return;
    }
    if (!FileAndDirectoryUtils::isDirectoryWritable(dir)) {
        QMessageBox::warning(this, L10N::warningTitle(), tr("You don't have permissions to write in selected folder."));
        return;
    }
    colorsDirEdit->setText(dir);
    ColorSchemeUtils::setColorsDir(dir);
    customSchemas.clear();
    colorSchemas->clear();
    customSchemas = ColorSchemeUtils::getSchemas();
    foreach (ColorSchemeData schema, customSchemas) {
        colorSchemas->addItem(new QListWidgetItem(schema.name, colorSchemas));
    }
}

void ColorSchemaSettingsPageWidget::sl_onAddColorSchema() {
    QStringList usedNames;
    foreach (const ColorSchemeData &customScheme, customSchemas) {
        usedNames << customScheme.name;
    }
    ColorSchemeData schema;

    QObjectScopedPointer<CreateColorSchemaDialog> d = new CreateColorSchemaDialog(&schema, usedNames);
    const int r = d->createNewScheme();
    CHECK(!d.isNull(), );

    if (r == QDialog::Rejected) {
        return;
    }

    customSchemas.append(schema);
    colorSchemas->addItem(new QListWidgetItem(schema.name, colorSchemas));
}

void ColorSchemaSettingsPageWidget::sl_onChangeColorSchema() {
    QMap<char, QColor> alpColors;

    QListWidgetItem *item = colorSchemas->currentItem();
    if (item == NULL) {
        return;
    }

    QString schemaName = item->text();
    for (int i = 0; i < customSchemas.size(); ++i) {
        ColorSchemeData &customSchema = customSchemas[i];
        if (customSchema.name == schemaName) {
            alpColors = customSchema.alpColors;
            QObjectScopedPointer<ColorSchemaDialogController> controller = new ColorSchemaDialogController(alpColors);
            const int r = controller->adjustAlphabetColors();
            CHECK(!controller.isNull(), );

            if (r == QDialog::Rejected) {
                return;
            }

            QMapIterator<char, QColor> it(alpColors);
            while (it.hasNext()) {
                it.next();
                customSchema.alpColors[it.key()] = it.value();
            }
            break;
        }
    }
}

void ColorSchemaSettingsPageWidget::sl_onDeleteColorSchema() {
    QListWidgetItem *item = colorSchemas->currentItem();
    SAFE_POINT(item != NULL, "current item for deletion is NULL", );

    QString schemaName = item->text();
    for (int i = 0; i < customSchemas.size(); ++i) {
        ColorSchemeData &customSchema = customSchemas[i];
        if (customSchema.name == schemaName) {
            removedCustomSchemas.append(customSchemas[i]);
            customSchemas.removeAt(i);
            colorSchemas->removeItemWidget(item);
            delete item;
            return;
        }
    }
    FAIL("something wrong causes color scheme deletion, this code must be unreacheble", );
}

}    // namespace U2
