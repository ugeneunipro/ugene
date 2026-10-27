// Standalone runner for the embedded PCR regression suite and reproducible benchmarks.
#include <QCoreApplication>
#include <QDir>
#include <QElapsedTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTemporaryDir>
#include <QTextStream>

#include <algorithm>
#include <random>

#ifdef Q_OS_UNIX
#    include <sys/resource.h>
#endif

#include <U2Core/DNASequenceUtils.h>
#include <U2Core/DNATranslation.h>

#include "InSilicoPcrTask.h"
#include "InSilicoPcrTests.h"
#include "src/corelibs/U2Core/src/datatype/DNAAlphabetRegistryImpl.h"
#include "src/corelibs/U2Private/src/AppContextImpl.h"
#include "src/corelibs/U2Private/src/SettingsImpl.h"
#include <U2Algorithm/RoughTmCalculatorCmdFactory.h>

using namespace U2;

int main(int argc, char** argv) {
    QCoreApplication app(argc, argv);
    QTemporaryDir config;
    qputenv("UGENE_SYSTEM_INI", (config.path() + "/settings.ini").toUtf8());
    auto context = AppContextImpl::getApplicationContext();
    QDir::addSearchPath("data", PCR_TEST_DATA_DIR);
    context->setSettings(new SettingsImpl(QSettings::SystemScope));
    auto translations = new DNATranslationRegistry;
    context->setDNATranslationRegistry(translations);
    context->setDNAAlphabetRegistry(new DNAAlphabetRegistryImpl(translations));
    QTextStream output(stdout);
    const QStringList args = app.arguments();
    if (args.size() > 2 && args[1] == "--benchmark") {
        QString name = args[2];
        InSilicoPcrTaskSettings settings;
        settings.forwardPrimer = "ACGTCAGTACGATCGTACGA";
        settings.reversePrimer = "TGCACTGACTGACGTAGCTA";
        settings.perfectMatch = 15;
        settings.maxProductSize = 80;
        settings.temperatureCalculator = RoughTmCalculatorCmdFactory().createCalculator({});
        const QByteArray product = settings.forwardPrimer + QByteArray(40, 'C') + DNASequenceUtils::reverseComplement(settings.reversePrimer);
        if (name == "ordinary") {
            std::mt19937 random(1969);
            settings.sequence.resize(1000000);
            for (auto& base : settings.sequence) {
                base = "ACGT"[random() % 4];
            }
            settings.sequence.replace(500000, product.size(), product);
            settings.forwardMismatches = settings.reverseMismatches = 1;
        } else if (name == "rejected") {
            for (int i = 0; i < 10000; ++i) {
                settings.sequence += "ACGTCAGTACGATCGTACGTCCCCC";
            }
            settings.sequence += product;
            settings.forwardMismatches = settings.reverseMismatches = 1;
        } else if (name == "sparse" || name == "dense") {
            int count = name == "sparse" ? 10000 : 49;
            for (int i = 0; i < count; ++i) {
                settings.sequence += product + QByteArray(30, 'C');
            }
            if (name == "dense") {
                settings.maxProductSize = settings.sequence.size();
            }
        } else {
            output << "Unknown benchmark\n";
            return 2;
        }
        QList<double> searchTimes, pairingTimes, totalTimes;
        int products = 0;
        // One warm-up and seven measured iterations in the same process.
        for (int iteration = 0; iteration < 8; ++iteration) {
            QElapsedTimer timer;
            timer.start();
            InSilicoPcrTask task(new InSilicoPcrTaskSettings(settings));
            task.prepare();
            for (auto subtask : task.getSubtasks()) {
                subtask->run();
            }
            double searchMs = timer.nsecsElapsed() / 1e6;
            task.run();
            double totalMs = timer.nsecsElapsed() / 1e6;
            products = task.getResults().size();
            if (iteration > 0) {
                searchTimes.append(searchMs);
                pairingTimes.append(totalMs - searchMs);
                totalTimes.append(totalMs);
            }
        }
        auto median = [](QList<double> values) {
            std::sort(values.begin(), values.end());
            return values[values.size() / 2];
        };
        QJsonObject result{{"case", name}, {"sequence_bp", settings.sequence.size()}, {"products", products},
                           {"search_ms", median(searchTimes)}, {"pairing_ms", median(pairingTimes)}, {"total_ms", median(totalTimes)}};
#ifdef Q_OS_UNIX
        rusage usage{};
        getrusage(RUSAGE_SELF, &usage);
#    ifdef Q_OS_MACOS
        result.insert("peak_rss_bytes", qint64(usage.ru_maxrss));
#    else
        result.insert("peak_rss_bytes", qint64(usage.ru_maxrss) * 1024);
#    endif
#endif
        output << QJsonDocument(result).toJson(QJsonDocument::Compact) << Qt::endl;
        return 0;
    }
    QStringList scenarios = {"mismatch_limit", "many_sites", "orientations", "circular", "ambiguous", "overhangs", "exhaustive", "duplicates_and_temperature", "cancellation"};
    if (args.size() > 1) {
        scenarios = {args[1]};
    }
    int failures = 0;
    for (const auto& scenario : scenarios) {
        QString error = runInSilicoPcrTests(scenario);
        output << scenario << ": " << (error.isEmpty() ? "PASS" : "FAIL: " + error) << Qt::endl;
        failures += !error.isEmpty();
    }
    return failures ? 1 : 0;
}
