#pragma once

#include <QWidget>

#include <U2Core/Task.h>

#include <U2Gui/ObjectViewModel.h>

namespace biopipe {

class AssemblyWidget : public QWidget {
public:

    AssemblyWidget(QWidget* parent, const QString& assemblyPath, const QString& referencePath);
    AssemblyWidget(const AssemblyWidget& copy) = delete;
    AssemblyWidget(AssemblyWidget&& move) = delete;
    AssemblyWidget& operator=(const AssemblyWidget& copy) = delete;
    AssemblyWidget& operator=(AssemblyWidget&& move) = delete;

    ~AssemblyWidget();

protected:
    void showEvent(QShowEvent* event) override;

private:
    static void init();

    static void free();

    void createWidget(QWidget* parent, const QString& assemblyPath, const QString& referencePath);

    static void loopUntilTaskIsFinished(U2::Task* task);

    static void setDataSearchPaths();

    static void setSearchPaths();

    static void initOptionsPanels();

    static QString findKey(const QStringList& envList, const QString& key);

#ifdef Q_OS_DARWIN
    static void AssemblyWidget::fixMacFonts();
#endif

    static QStringList getArguments(int argc, char** argv);

    U2::GObjectViewWindow* assemblyWidget = nullptr;

};

}  // namespace biopipe
