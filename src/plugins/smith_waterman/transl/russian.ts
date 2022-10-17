<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru_RU">
<context>
    <name>PairwiseAlignmentSmithWatermanOptionsPanelMainWidget</name>
    <message>
        <location filename="../src/PairwiseAlignmentSmithWatermanOptionsPanelMainWidget.ui" line="14"/>
        <source>Form</source>
        <translation>Форма</translation>
    </message>
    <message>
        <location filename="../src/PairwiseAlignmentSmithWatermanOptionsPanelMainWidget.ui" line="29"/>
        <source>Algorithm version:</source>
        <translation>Версия алгоритма:</translation>
    </message>
    <message>
        <location filename="../src/PairwiseAlignmentSmithWatermanOptionsPanelMainWidget.ui" line="39"/>
        <source>Scoring matrix:</source>
        <translation>Матрица:</translation>
    </message>
    <message>
        <location filename="../src/PairwiseAlignmentSmithWatermanOptionsPanelMainWidget.ui" line="62"/>
        <source>Gap penalty</source>
        <translation>Штраф за пробел</translation>
    </message>
    <message>
        <location filename="../src/PairwiseAlignmentSmithWatermanOptionsPanelMainWidget.ui" line="77"/>
        <source>Open:</source>
        <translation>Открыть:</translation>
    </message>
    <message>
        <location filename="../src/PairwiseAlignmentSmithWatermanOptionsPanelMainWidget.ui" line="84"/>
        <source>Extension:</source>
        <translation>Расширение:</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/SmithWatermanAlgorithm.cpp" line="155"/>
        <source>Smith-Waterman algorithm trying to allocate more memory than it was limited (%1 Mb). Calculation stopped.</source>
        <translation>Smith-Waterman algorithm trying to allocate more memory than it was limited (%1 Mb). Calculation stopped.</translation>
    </message>
</context>
<context>
    <name>U2::AlignmentAlgorithmsRegistry</name>
    <message>
        <location filename="../src/SWAlgorithmPlugin.cpp" line="140"/>
        <source>Smith-Waterman</source>
        <translation>Поиск Смита-Ватермана</translation>
    </message>
</context>
<context>
    <name>U2::GTest_SmithWatermnan</name>
    <message>
        <location filename="../src/SmithWatermanTests.cpp" line="216"/>
        <source>**</source>
        <translation>**</translation>
    </message>
    <message>
        <location filename="../src/SmithWatermanTests.cpp" line="219"/>
        <source>,</source>
        <translation>,</translation>
    </message>
    <message>
        <location filename="../src/SmithWatermanTests.cpp" line="226"/>
        <source>..</source>
        <translation>..</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::SWPrompter</name>
    <message>
        <location filename="../src/SWWorker.cpp" line="267"/>
        <source>unset</source>
        <translation>не указан</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="296"/>
        <source>translated</source>
        <translation>транслированной</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="306"/>
        <source>Searches regions in each sequence from &lt;u&gt;%1&lt;/u&gt; similar to all pattern(s) taken from &lt;u&gt;%2&lt;/u&gt;. &lt;br/&gt;Percent similarity between a sequence and a pattern is %3. &lt;br/&gt;Seaches in %4 of a %5 sequence. &lt;br/&gt;Outputs the regions found annotated as %6.</source>
        <translation>В каждой последовательности, полученной от &lt;u&gt;%1&lt;/u&gt; ищет регионы, похожие на паттерн(ы) &lt;u&gt;%2&lt;/u&gt;. &lt;br/&gt;Совпадение между регионом последовательности и паттерном должно быть как минимум &lt;u&gt;%3%&lt;/u&gt;. Поиск ведется в &lt;u&gt;%4&lt;/u&gt; %5последовательности.&lt;br/&gt;Возвращает найденные регионы как &lt;u&gt;%6&lt;/u&gt; аннотации.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::SWWorker</name>
    <message>
        <location filename="../src/SWWorker.cpp" line="127"/>
        <source>Annotate as</source>
        <translation>Аннотации</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="135"/>
        <source>Algorithm</source>
        <translation>Алгоритм</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="102"/>
        <source>Input Data</source>
        <translation>Входная последовательность</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="106"/>
        <source>Pattern Annotations</source>
        <translation>Найденные аннотации</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="107"/>
        <source>The regions found.</source>
        <translation>Аннотации, найденные для последовательности.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="128"/>
        <source>Name of the result annotations.</source>
        <translation>Имя аннотаций для найденных регионов.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="131"/>
        <source>Min Score</source>
        <translation>Минимальный процент совпадения</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="132"/>
        <source>Minimal percent similarity between a sequence and a pattern.</source>
        <translation>Минимальный процент совпадения между регионом последовательности и паттерном.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="136"/>
        <source>Version of the Smith-Waterman algorithm. You can use the optimized versions of the algorithm (SSE) if your hardware supports these capabilities.</source>
        <translation>Версия реализации алгоритма Смита-Ватермана. Если имеется соответствующее аппаратное обеспечения для ускорения вычислений могут быть использованы оптимизированные версии алгоритма (SSE).</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="141"/>
        <source>Search in Translation</source>
        <translation>Искать в трансляции</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="142"/>
        <source>Translates a supplied nucleotide sequence to protein and searches in the translated sequence.</source>
        <translation>Указывает транслировать входную нуклеотидную последовательность в протеиновую и искать указанный паттерн в полученной протеиновой последовательности.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="146"/>
        <source>Substitution Matrix</source>
        <translation>Матрица</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="147"/>
        <source>Describes the rate at which one character in a sequence changes to other character states over time.</source>
        <translation>Матрица весов для алгоритма.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="151"/>
        <source>Filter Results</source>
        <translation>Фильтрация результатов</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="152"/>
        <source>Specifies either to filter the intersected results or to return all the results.</source>
        <translation>Указывает, требуется ли отфильтровывать пересекающиеся результаты.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="156"/>
        <source>Gap Open Score</source>
        <translation>Вес открытия пробела</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="157"/>
        <source>Penalty for opening a gap.</source>
        <translation>Штраф, накладываемый за открытие пробела.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="160"/>
        <source>Gap Extension Score</source>
        <translation>Вес продления пробела</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="161"/>
        <source>Penalty for extending a gap.</source>
        <translation>Штраф, накладываемый за продление пробела.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="164"/>
        <source>Use Pattern Names</source>
        <translation>Использовать имя паттерна</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="165"/>
        <source>Use a pattern name as an annotation name.</source>
        <translation>Использовать имя паттерна в качестве имени аннотации.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="168"/>
        <source>Qualifier name for pattern name</source>
        <translation>Имя квалификатора для имени паттерна</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="169"/>
        <source>Name of qualifier in result annotations which is containing a pattern name.</source>
        <translation>Имя квалификатора в аннотации, которая содержит имя паттерна.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="186"/>
        <source>Smith-Waterman Search</source>
        <translation>Поиск подстроки (алгоритм Смита-Ватермана)</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="187"/>
        <source>Searches regions in a sequence similar to a pattern sequence. Outputs a set of annotations. &lt;p&gt;Under the hood is the well-known Smith-Waterman algorithm for performing local sequence alignment.</source>
        <translation>Поиск подпоследовательностей, похожих на указанный паттерн, в каждой входной последовательности (нуклеотидной или протеиновой), с учётом выбранной матрицы весов. &lt;p&gt;Используется реализация широко известного алгоритма Смита-Ватермана для локального выравнивания пар последовательностей.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="220"/>
        <source>Auto</source>
        <translation>Авто</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="282"/>
        <source>both strands</source>
        <translation>в обеих цепях</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="285"/>
        <source>direct strand</source>
        <translation>прямой цепи</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="288"/>
        <source>complementary strand</source>
        <translation>комплементарной цепи</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="387"/>
        <source>Null sequence supplied to Smith-Waterman: %1</source>
        <translation>Null sequence supplied to Smith-Waterman: %1</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="483"/>
        <source>Incorrect value: pattern alphabet doesn&apos;t match sequence alphabet </source>
        <translation>Incorrect value: pattern alphabet doesn&apos;t match sequence alphabet </translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="484"/>
        <source>Pattern symbols not matching to alphabet</source>
        <translation>Pattern symbols not matching to alphabet</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="424"/>
        <location filename="../src/SWWorker.cpp" line="425"/>
        <source>Incorrect value: score value must lay between 0 and 100</source>
        <translation>Incorrect value: score value must lay between 0 and 100</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="435"/>
        <source>Incorrect value:  filter name incorrect, default value used</source>
        <translation>Incorrect value:  filter name incorrect, default value used</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="442"/>
        <source>Incorrect value: result name is empty, default value used</source>
        <translation>Incorrect value: result name is empty, default value used</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="456"/>
        <source>Could not find complement translation for %1, searching only direct strand</source>
        <translation>Could not find complement translation for %1, searching only direct strand</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="396"/>
        <location filename="../src/SWWorker.cpp" line="397"/>
        <source>Invalid value: weight matrix with given name not exists</source>
        <translation>Invalid value: weight matrix with given name not exists</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="98"/>
        <source>Pattern Data</source>
        <translation>Паттерн(ы)</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="99"/>
        <source>An input pattern sequence to search for.</source>
        <translation>Входной паттерн для поиска.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="103"/>
        <source>An input reference sequence to search in.</source>
        <translation>Входная последовательность для поиска в ней.</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="359"/>
        <source>Null pattern supplied to Smith-Waterman: %1</source>
        <translation>Null pattern supplied to Smith-Waterman: %1</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="407"/>
        <source>Can&apos;t find weight matrix name: &apos;%1&apos;!</source>
        <translation>Can&apos;t find weight matrix name: &apos;%1&apos;!</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="407"/>
        <source>&lt;empty&gt;</source>
        <translation>&lt;empty&gt;</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="414"/>
        <source>Wrong matrix selected. Alphabets do not match</source>
        <translation>Wrong matrix selected. Alphabets do not match</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="472"/>
        <source>SmithWaterman algorithm not found: %1</source>
        <translation>SmithWaterman algorithm not found: %1</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="477"/>
        <location filename="../src/SWWorker.cpp" line="478"/>
        <source>Incorrect value: search pattern, pattern is empty</source>
        <translation>Incorrect value: search pattern, pattern is empty</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="500"/>
        <source>smith_waterman_task</source>
        <translation>Smith waterman task</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="507"/>
        <source>Smith waterman subtasks</source>
        <translation>Smith waterman subtasks</translation>
    </message>
    <message>
        <location filename="../src/SWWorker.cpp" line="550"/>
        <source>Found %1 matches of pattern &apos;%2&apos;</source>
        <translation>Found %1 matches of pattern &apos;%2&apos;</translation>
    </message>
</context>
<context>
    <name>U2::PairwiseAlignmentSmithWatermanTask</name>
    <message>
        <location filename="../src/SWAlgorithmTask.cpp" line="435"/>
        <source>Substitution matrix is empty</source>
        <translation>Substitution matrix is empty</translation>
    </message>
    <message>
        <location filename="../src/SWAlgorithmTask.cpp" line="598"/>
        <source>Needed amount of memory for this task is %1 MB, but it limited to %2 MB.</source>
        <translation>Needed amount of memory for this task is %1 MB, but it limited to %2 MB.</translation>
    </message>
    <message>
        <location filename="../src/SWAlgorithmTask.cpp" line="601"/>
        <source>Smith Waterman2 SequenceWalker</source>
        <translation>Smith Waterman2 SequenceWalker</translation>
    </message>
    <message>
        <location filename="../src/SWAlgorithmTask.cpp" line="638"/>
        <source>%1 results found</source>
        <translation>%1 results found</translation>
    </message>
</context>
<context>
    <name>U2::QDSWActor</name>
    <message>
        <location filename="../src/SWQuery.cpp" line="92"/>
        <source>matches with &lt;u&gt;at least %1 score&lt;/u&gt;</source>
        <translation>с оценкой не менее %1</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="92"/>
        <source>exact matches</source>
        <translation>точным совпадением</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="97"/>
        <source>both strands</source>
        <translation>все</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="100"/>
        <source>direct strand</source>
        <translation>только прямые</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="103"/>
        <source>complement strand</source>
        <translation>только комплементарные</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="109"/>
        <source>Finds pattern &lt;u&gt;%1&lt;/u&gt;.&lt;br&gt;Looks for &lt;u&gt;%2&lt;/u&gt; in &lt;u&gt;%3&lt;/u&gt;.</source>
        <translation>Искать паттерн &lt;u&gt;%1&lt;/u&gt;.&lt;br&gt;Искать совпадения &lt;u&gt;%2&lt;/u&gt; в &lt;u&gt;%3&lt;/u&gt;.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="137"/>
        <source>%1: percent of score out of bounds.</source>
        <translation>%1: percent of score out of bounds.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="147"/>
        <source>%1: incorrect result filter.</source>
        <translation>%1: incorrect result filter.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="152"/>
        <source>%1: pattern is empty.</source>
        <translation>%1: pattern is empty.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="158"/>
        <source>%1: can not find %2.</source>
        <translation>%1: can not find %2.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="201"/>
        <source>SSearch</source>
        <translation>SSearch</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="213"/>
        <source>smith_waterman_task</source>
        <translation>Поиск Смита-Ватермана</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="243"/>
        <source>Smith-Waterman</source>
        <translation>Поиск Смита-Ватермана</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="244"/>
        <source>Finds regions of similarity to the specified pattern in each input sequence (nucleotide or protein one). &lt;p&gt;Under the hood is the well-known Smith-Waterman algorithm for performing local sequence alignment.</source>
        <translation> Поиск подпоследовательностей, похожих на указанный паттерн, в каждой входной последовательности (нуклеотидной или протеиновой), с учётом выбранной матрицы весов. Используется реализация широко известного алгоритма Смита-Ватермана для локального выравнивания пар последовательностей.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="253"/>
        <source>Pattern</source>
        <translation>Паттерн</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="253"/>
        <source>A subsequence pattern to look for.</source>
        <translation>Паттерн для поиска интересующих подпоследовательностей.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="254"/>
        <source>Min score</source>
        <translation>Мин оценка</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="254"/>
        <source>The search stringency.</source>
        <translation>Строгость поиска.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="255"/>
        <source>Algorithm</source>
        <translation>Алгоритм</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="255"/>
        <source>Algorithm version.</source>
        <translation>Выбор версии алгоритма.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="256"/>
        <source>Search in translation</source>
        <translation>Искать в трансляции</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="256"/>
        <source>Translate a supplied nucleotide sequence to protein then search in the translated sequence.</source>
        <translation>Транслировать входную нуклеотидную последовательность в протеиновую, искать указанный паттерн в последней.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="257"/>
        <source>Scoring matrix</source>
        <translation>Матрица</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="257"/>
        <source>The scoring matrix.</source>
        <translation> Матрица весов для алгоритма.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="258"/>
        <source>Filter results</source>
        <translation>Фильтр результатов</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="258"/>
        <source>Result filtering strategy.</source>
        <translation> Дополнительная фильтрация результатов.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="259"/>
        <source>Gap open score</source>
        <translation>Вес открытия пробела</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="259"/>
        <source>Gap open score.</source>
        <translation>Вес открытия пробела.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="260"/>
        <source>Gap ext score</source>
        <translation>Вес продления пробела</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="260"/>
        <source>Gap extension score.</source>
        <translation>Вес продления пробела.</translation>
    </message>
    <message>
        <location filename="../src/SWQuery.cpp" line="297"/>
        <source>Auto</source>
        <translation>Авто</translation>
    </message>
</context>
<context>
    <name>U2::SWAlgorithmADVContext</name>
    <message>
        <location filename="../src/SWAlgorithmPlugin.cpp" line="116"/>
        <source>Find pattern [Smith-Waterman]...</source>
        <translation>Поиск подстроки (алгоритм Смита-Ватермана)...</translation>
    </message>
</context>
<context>
    <name>U2::SWAlgorithmPlugin</name>
    <message>
        <location filename="../src/SWAlgorithmPlugin.cpp" line="60"/>
        <source>Optimized Smith-Waterman </source>
        <translation>Оптимизированный алгроитм Смита-Ватермана</translation>
    </message>
    <message>
        <location filename="../src/SWAlgorithmPlugin.cpp" line="60"/>
        <source>Various implementations of Smith-Waterman algorithm</source>
        <translation>Различные реализации алгоритма Смита-Ватермана</translation>
    </message>
</context>
<context>
    <name>U2::SWAlgorithmTask</name>
    <message>
        <location filename="../src/SWAlgorithmTask.cpp" line="162"/>
        <source>Needed amount of memory for this task is %1 MB, but it limited to %2 MB.</source>
        <translation>Needed amount of memory for this task is %1 MB, but it limited to %2 MB.</translation>
    </message>
    <message>
        <location filename="../src/SWAlgorithmTask.cpp" line="165"/>
        <source>Smith Waterman2 SequenceWalker</source>
        <translation>Smith Waterman2 SequenceWalker</translation>
    </message>
    <message>
        <location filename="../src/SWAlgorithmTask.cpp" line="303"/>
        <source>%1 results found</source>
        <translation>%1 results found</translation>
    </message>
</context>
</TS>
