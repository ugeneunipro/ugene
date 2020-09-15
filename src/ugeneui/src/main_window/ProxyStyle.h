#ifndef PROXYSTYLE_H
#define PROXYSTYLE_H

#include <QProxyStyle>

namespace U2 {

class ProxyStyle : public QProxyStyle {
public:
    ProxyStyle(QStyle *style = nullptr);

    void polish(QWidget *widget) override;
};

#endif // PROXYSTYLE_H

} // namespace U2
