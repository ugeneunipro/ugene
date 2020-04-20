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

#ifndef _U2_TRIMMOMATIC_STEP_H_
#define _U2_TRIMMOMATIC_STEP_H_

#include <QVariant>
#include <QWidget>

#include <U2Core/IdRegistry.h>

namespace U2 {
namespace LocalWorkflow {

class TrimmomaticStep;

class TrimmomaticStepSettingsWidget : public QWidget {
    Q_OBJECT
public:
    TrimmomaticStepSettingsWidget();

    virtual bool validate() const = 0;

    virtual QVariantMap getState() const = 0;
    virtual void setState(const QVariantMap &state) = 0;

signals:
    void si_valueChanged();
    void si_widgetIsAboutToBeDestroyed(const QVariantMap &state);
};

class TrimmomaticStep : public QObject {
    Q_OBJECT
public:
    TrimmomaticStep(const QString &id);
    ~TrimmomaticStep();

    const QString &getId() const;
    const QString &getVisualName() const;
    const QString &getName() const;
    const QString &getDescription() const;

    QString getCommand() const;
    void setCommand(const QString &command);

    bool validate() const;

    TrimmomaticStepSettingsWidget *getSettingsWidget() const;

private slots:
    void sl_widgetDestroyed();
    void sl_widgetIsAboutToBeDestroyed(const QVariantMap &state);

signals:
    void si_valueChanged();

protected:
    virtual TrimmomaticStepSettingsWidget *createWidget() const = 0;

    virtual QString serializeState(const QVariantMap &widgetState) const = 0;
    virtual QVariantMap parseState(const QString &command) const = 0;

    QString id;
    QString name;
    QString description;
    mutable TrimmomaticStepSettingsWidget *settingsWidget;
    QVariantMap widgetState;
};

class TrimmomaticStepFactory {
public:
    TrimmomaticStepFactory(const QString &id);
    virtual ~TrimmomaticStepFactory();

    const QString &getId() const;
    virtual TrimmomaticStep *createStep() const = 0;

private:
    const QString id;
};

class TrimmomaticStepsRegistry : public IdRegistry<TrimmomaticStepFactory> {
public:
    static TrimmomaticStepsRegistry *getInstance();
    static void releaseInstance();

private:
    static QScopedPointer<TrimmomaticStepsRegistry> instance;
};

}   // namespace LocalWorkflow
}   // namespace U2

#endif // _U2_TRIMMOMATIC_STEP_H_
