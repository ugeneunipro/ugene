#pragma once

#include <QProxyStyle>

namespace U2 {

class ProxyStyle : public QProxyStyle {
public:
    ProxyStyle(QStyle* style = nullptr);

    // Initializes the appearance of the given widget
    void polish(QWidget* widget) override;
};

}  // namespace U2
