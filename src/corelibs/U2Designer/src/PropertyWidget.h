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

#ifndef _U2_PROPERTYWIDGET_H_
#define _U2_PROPERTYWIDGET_H_

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLayout>
#include <QSpinBox>
#include <QToolButton>
#include <QVariant>
#include <QWidget>

#include <U2Core/Formatters.h>

#include <U2Designer/URLLineEdit.h>

#include <U2Lang/ConfigurationEditor.h>

class QStandardItem;
class QStandardItemModel;

namespace U2 {

typedef QPair<QString, QVariant> ComboItem;

/************************************************************************/
/* BaseDefaultPropertyWidget */
/************************************************************************/

static const int NO_LIMIT = -1;

class BaseDefaultPropertyWidget : public PropertyWidget {
    Q_OBJECT
public:
    BaseDefaultPropertyWidget(QWidget* parent);
    virtual QVariant value();
    virtual void setValue(const QVariant& value);
    virtual void setRequired();

protected:
    void configureLineEdit(const int maxLength);

    QLineEdit* lineEdit;

private slots:
    void sl_valueChanged(const QString& value);
};

/************************************************************************/
/* DefaultPropertyWidget */
/************************************************************************/

class DefaultPropertyWidget : public BaseDefaultPropertyWidget {
    Q_OBJECT
public:
    DefaultPropertyWidget(int maxLength = NO_LIMIT, QWidget* parent = nullptr);
};

/************************************************************************/
/* IgnoreUpDownPropertyWidget */
/************************************************************************/

class IgnoreUpDownPropertyWidget : public BaseDefaultPropertyWidget {
    Q_OBJECT
public:
    IgnoreUpDownPropertyWidget(int maxLength = NO_LIMIT, QWidget* parent = nullptr);
};

/************************************************************************/
/* LineEditSkipUpDown */
/************************************************************************/

class LineEditIgnoreUpDown : public QLineEdit {
public:
    LineEditIgnoreUpDown(QWidget* parent = nullptr);

private:
    void keyPressEvent(QKeyEvent* e) override;
};

/************************************************************************/
/* SpinBoxWidget */
/************************************************************************/
class SpinBoxWidget : public PropertyWidget {
    Q_OBJECT
public:
    SpinBoxWidget(const QVariantMap& spinProperties, QWidget* parent = nullptr);
    virtual QVariant value();
    virtual void setValue(const QVariant& value);
    virtual void processDelegateTags();

    /** Override */
    bool setProperty(const char* name, const QVariant& value);

signals:
    void valueChanged(int value);

private:
    QSpinBox* spinBox;

private slots:
    void sl_valueChanged(int value);
};

/************************************************************************/
/* DoubleSpinBoxWidget */
/************************************************************************/
class DoubleSpinBoxWidget : public PropertyWidget {
    Q_OBJECT
public:
    DoubleSpinBoxWidget(const QVariantMap& spinProperties, QWidget* parent = nullptr);
    virtual QVariant value();
    virtual void setValue(const QVariant& value);

private:
    QDoubleSpinBox* spinBox;

private slots:
    void sl_valueChanged(double value);
};

/** Base class for different kinds of ComboBox widgets that adds support of features like value formatting, sorting, etc.. */
class ComboBoxWidgetBase : public PropertyWidget {
public:
    ComboBoxWidgetBase(QWidget* parent = nullptr,
                       const QSharedPointer<StringFormatter>& formatter = nullptr,
                       bool isSorted = false);

    /** Returns formatted value for the item with the given name. */
    QString getFormattedItemText(const QString& itemKey) const;

protected:
    /** Sorts in-place combo box items by name (item.first) in case-insensitive mode. */
    static void sortComboItemsByName(QList<ComboItem>& itemList);

    QSharedPointer<StringFormatter> formatter;

    /** Makes combo-box list sorted. The sorting is case insensitive. */
    bool isSorted = false;
};

/************************************************************************/
/* ComboBoxWidget */
/************************************************************************/
class ComboBoxWidget : public ComboBoxWidgetBase {
    Q_OBJECT
public:
    ComboBoxWidget(const QList<ComboItem>& items,
                   QWidget* parent = nullptr,
                   const QSharedPointer<StringFormatter>& formatter = nullptr,
                   bool isSorted = false);

    QVariant value() override;
    void setValue(const QVariant& value) override;

    static ComboBoxWidget* createBooleanWidget(QWidget* parent = nullptr);

signals:
    void valueChanged(const QString& value);

private:
    QComboBox* comboBox;

private slots:
    void sl_valueChanged(int index);
};

/************************************************************************/
/* ComboBoxEditableWidget */
/************************************************************************/
class ComboBoxEditableWidget : public PropertyWidget {
    Q_OBJECT
public:
    ComboBoxEditableWidget(const QVariantMap& items, QWidget* parent = nullptr);
    virtual QVariant value();
    virtual void setValue(const QVariant& value);

signals:
    void valueChanged(const QString& value);

protected slots:
    virtual void sl_edit(const QString& val);

protected:
    QComboBox* comboBox;
    int customIdx;

private slots:
    void sl_valueChanged(int index);
};

/************************************************************************/
/* ComboBoxWithUrlWidget */
/************************************************************************/
class ComboBoxWithUrlWidget : public PropertyWidget {
    Q_OBJECT
public:
    ComboBoxWithUrlWidget(const QVariantMap& items, bool isPath = false, QWidget* parent = nullptr);
    virtual QVariant value();
    virtual void setValue(const QVariant& value);

signals:
    void valueChanged(const QString& value);

protected slots:
    virtual void sl_browse();

protected:
    QComboBox* comboBox;
    int customIdx;
    bool isPath;

private slots:
    void sl_valueChanged(int index);
};

/************************************************************************/
/* ComboBoxWithChecksWidget */
/************************************************************************/
class U2DESIGNER_EXPORT ComboBoxWithChecksWidget : public ComboBoxWidgetBase {
    Q_OBJECT
public:
    ComboBoxWithChecksWidget(const QVariantMap& items,
                             QWidget* parent = nullptr,
                             const QSharedPointer<StringFormatter>& formatter = nullptr,
                             bool isSorted = false);

    QVariant value() override;

    void setValue(const QVariant& value) override;

signals:
    void valueChanged(const QString& value);

protected:
    QComboBox* comboBox;
    QStandardItemModel* cm;
    QVariantMap items;

protected slots:
    virtual void sl_valueChanged(int index);
    virtual void sl_itemChanged(QStandardItem* item);

private:
    void initModelView();

    /** Returns formatted version of the current value. */
    QString getFormattedValue();
};

/************************************************************************/
/* URLWidget */
/************************************************************************/
class RunFileSystem;

class U2DESIGNER_EXPORT URLWidget : public PropertyWidget {
    Q_OBJECT
public:
    URLWidget(const QString& type, bool multi, bool isPath, bool saveFile, DelegateTags* tags, QWidget* parent = nullptr);

    // PropertyWidget
    virtual QVariant value();
    virtual void setValue(const QVariant& value);
    virtual void setRequired();
    virtual void activate();

signals:
    void finished();

private slots:
    void sl_browse();
    void sl_finished();

protected:
    virtual QString finalyze(const QString& url);

private:
    RunFileSystem* getRFS();

private:
    URLLineEdit* urlLine;
    QToolButton* browseButton;
    QToolButton* addButton;
    QString initialValue;

private slots:
    void sl_textChanged(const QString& text);
};

/************************************************************************/
/* NoFileURLWidget */
/************************************************************************/
class NoFileURLWidget : public URLWidget {
public:
    NoFileURLWidget(const QString& type, bool multi, bool isPath, bool saveFile, DelegateTags* tags, QWidget* parent = nullptr);

    static QString finalyze(const QString& url, DelegateTags* tags);

protected:
    virtual QString finalyze(const QString& url);
};

}  // namespace U2

#endif  // _U2_PROPERTYWIDGET_H_
