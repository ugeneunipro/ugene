#include "ProxyStyle.h"

#include <QMenu>

namespace U2 {

ProxyStyle::ProxyStyle(QStyle *style)
    : QProxyStyle(style)
{

}

void ProxyStyle::polish(QWidget *widget) {
    QProxyStyle::polish(widget);

#if QT_VERSION_CHECK(5, 10, 0) < QT_VERSION
    // It's a workaround for https://bugreports.qt.io/browse/QTBUG-49435:
    // shortcuts are not shown in context menus.
    auto *menu = qobject_cast<QMenu *>(widget);
    if (nullptr != menu) {
        const auto actions = menu->actions();
        for (auto *action: actions) {
            action->setShortcutVisibleInContextMenu(true);
        }
    }
#endif
}

} // namespace U2
