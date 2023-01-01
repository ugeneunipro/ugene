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

#ifndef _U2_SPLASH_SCREEN_CONTROLLER_
#define _U2_SPLASH_SCREEN_CONTROLLER_

#include <QDialog>

namespace U2 {

class SplashScreen : public QDialog {
    Q_OBJECT
public:
    explicit SplashScreen(QWidget* parent = nullptr);

public slots:
    void sl_close();

protected:
    bool eventFilter(QObject* obj, QEvent* ev) override;
};

////////////////////////////////////////////////////
/// SplashScreenWidget
///
class SplashScreenWidget : public QWidget {
    Q_OBJECT
public:
    SplashScreenWidget();

    void updateActiveTaskName();
    void getDots();

protected:
    void timerEvent(QTimerEvent* e) override;
    void paintEvent(QPaintEvent* e) override;

private:
    void drawInfo();

    int dots_number;
    int dots_timer_id;

    /** Formatted version as shown on the splash screen. */
    QString version;

    QString activeTaskName;
    QImage image1;
    QImage image2;
};

}  // namespace U2

#endif
