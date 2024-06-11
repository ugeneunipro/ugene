#pragma once

#include <QProxyStyle>

namespace U2 {

class ProxyStyle : public QProxyStyle {
public:
    ProxyStyle(QStyle* style = nullptr);

    void polish(QWidget* widget) override;
};

}  // namespace U2
