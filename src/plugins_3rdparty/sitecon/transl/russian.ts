<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru_RU">
<context>
    <name>SiteconBuildDialog</name>
    <message>
        <location filename="../src/SiteconBuildDialog.ui" line="22"/>
        <source>Input alignment (nucleic)</source>
        <translation>Входное выравнивание</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialog.ui" line="32"/>
        <location filename="../src/SiteconBuildDialog.ui" line="49"/>
        <source>...</source>
        <translation>...</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialog.ui" line="39"/>
        <source>Output model</source>
        <translation>Результирующая модель</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialog.ui" line="58"/>
        <source>Options</source>
        <translation>Настройки</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialog.ui" line="64"/>
        <location filename="../src/SiteconBuildDialog.ui" line="95"/>
        <source>Window is used to pick out the most important alignment region and is located at the center of the alignment.
           Must be: windows size is not greater than TFBS alignment length,
           recommended: windows size is not greater than 50 bp.</source>
        <translation>Размер окна должен быть не короче консенсуса для данного сайта но не более длины выравнивания.</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialog.ui" line="69"/>
        <source>Window size</source>
        <translation>Размер окна</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialog.ui" line="103"/>
        <source>bp</source>
        <translation>нукл</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialog.ui" line="122"/>
        <source>Calibration random seed</source>
        <translation>Случайная затравка</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialog.ui" line="173"/>
        <source>Calibration sequence length</source>
        <translation>Длина калибровочной последовательности</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialog.ui" line="206"/>
        <source>       100 Kb</source>
        <translation>       100 Кб</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialog.ui" line="211"/>
        <source>       500 Kb</source>
        <translation>       500 Кб</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialog.ui" line="216"/>
        <source>         1 Mb</source>
        <translation>         1 Мб</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialog.ui" line="221"/>
        <source>         5 Mb</source>
        <translation>         5 Мб</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialog.ui" line="232"/>
        <source>Weight algorithm</source>
        <translation>Алгоритм весов</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialog.ui" line="290"/>
        <source>Default status</source>
        <translation>Статус по умолчанию</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialog.ui" line="259"/>
        <source>       None</source>
        <translation>Без весов</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialog.ui" line="264"/>
        <source>  Algorithm N2</source>
        <translation>Алгоритм №2</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialog.ui" line="170"/>
        <location filename="../src/SiteconBuildDialog.ui" line="193"/>
        <source>Length of random synthetic sequences used to calibrate the profile. Should not be less than window size.</source>
        <translation>Длина случайных синтетических последовательностей для калибрации профайла. Не должна быть меньше размера окна.</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialog.ui" line="119"/>
        <location filename="../src/SiteconBuildDialog.ui" line="148"/>
        <source>The random seed, where &lt;n&gt; is a positive integer. You can use this option to generate reproducible results for different runs on the same data.</source>
        <translation>Случайная затравка. Использование этого параметра полезно для воспроизведения результата калибрации.</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialog.ui" line="14"/>
        <source>SITECON Build</source>
        <translation>Построение модели SITECON</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialog.ui" line="229"/>
        <location filename="../src/SiteconBuildDialog.ui" line="252"/>
        <source>Optional feature, in most cases applying no weight will fit. In some cases choosing algorithm 2 will increase the recognition quality.</source>
        <translation>В большинстве случаев алгоритм без весов вполне адекватен. Использование других алгоритмов может улучшить качество распознавания в отдельных случаях.</translation>
    </message>
</context>
<context>
    <name>SiteconSearchDialog</name>
    <message>
        <location filename="../src/SiteconSearchDialog.ui" line="14"/>
        <source>SITECON Search</source>
        <translation>Поиск сайтов связывания ТФ с помощью SITECON</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialog.ui" line="22"/>
        <source>File with model</source>
        <translation>Модель</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialog.ui" line="36"/>
        <source>...</source>
        <translation>...</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialog.ui" line="46"/>
        <source>Threshold</source>
        <translation>Минимальная ошибка</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialog.ui" line="190"/>
        <source>Strands</source>
        <translation>Направление</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialog.ui" line="196"/>
        <source>Both strands</source>
        <translation>Обе</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialog.ui" line="215"/>
        <source>Search in direct strand only</source>
        <translation>Искать ССТФ только в прямой последовательности</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialog.ui" line="218"/>
        <source>Direct strand</source>
        <translation>Прямая</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialog.ui" line="240"/>
        <source>Search in complement strand</source>
        <translation>Искать ССТФ только в комплeментарной последовательности</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialog.ui" line="243"/>
        <source>Complement strand</source>
        <translation>Комплементарная</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialog.ui" line="106"/>
        <source>Range</source>
        <translation>Регион</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialog.ui" line="111"/>
        <source>Strand</source>
        <translation>Стренд</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialog.ui" line="116"/>
        <source>PSUM</source>
        <translation>Оценка</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialog.ui" line="121"/>
        <source>First type error</source>
        <translation>Ошибка 1-го рода</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialog.ui" line="126"/>
        <source>Second type error</source>
        <translation>Ошибка 2-го рода</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialog.ui" line="70"/>
        <source>Clear results</source>
        <translation>Сбросить результаты</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialog.ui" line="77"/>
        <source>Save as annotations</source>
        <translation>Сохранить результаты</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialog.ui" line="136"/>
        <source>Ready</source>
        <translation>Готово</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialog.ui" line="43"/>
        <location filename="../src/SiteconSearchDialog.ui" line="59"/>
        <source>Recognition quality threshold, should be less than 100%. Choosing too low threshold will lead to recognition of too many TFBS recognised with too low trustworthiness. Choosing too high threshold may result in no TFBS recognised.</source>
        <translation>Нижний предел качества распознавания, величина более 60% но меньше 100%. Слишком низкий предел приведёт к нахождению большого числа ложных сигналов, чрезмерно высокий отбросит качественные результаты.</translation>
    </message>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="165"/>
        <source>Recognition quality percentage threshold. If you need to switch off this filter choose &lt;b&gt;the lowest&lt;/b&gt; value&lt;/i&gt;&lt;/p&gt;.</source>
        <translation>Порог качества. Для того, чтобы отключить этот фильтр укажите наименьшее значение.</translation>
    </message>
</context>
<context>
    <name>U2::GTest_CompareSiteconModels</name>
    <message>
        <location filename="../src/SiteconAlgorithmTests.cpp" line="588"/>
        <source>Models not equal</source>
        <translation>Модели не равны</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::SiteconBuildPrompter</name>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="119"/>
        <source>For each MSA from &lt;u&gt;%1&lt;/u&gt;, build SITECON model.</source>
        <translation>Для каждого выравнивания из &lt;u&gt;%1&lt;/u&gt;, построить модель SITECON.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::SiteconBuildWorker</name>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="83"/>
        <source>Build SITECON model</source>
        <translation>Построение модели SITECON</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="106"/>
        <source>None</source>
        <translation>None</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="107"/>
        <source>Algorithm2</source>
        <translation>Algorithm2</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="137"/>
        <source>Random seed can not be less zero</source>
        <translation>Затравка не может быть меньше нуля</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="141"/>
        <source>Calibration length can not be less zero</source>
        <translation>Калибровочная длина не может быть меньше нуля</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="146"/>
        <source>Window size can not be less zero</source>
        <translation>Window size can not be less zero</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="177"/>
        <source>Built SITECON model from: %1</source>
        <translation>Построен профайл SITECON из %1</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="59"/>
        <source>Input alignment</source>
        <translation>Входное выравнивание</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="65"/>
        <source>Sitecon model</source>
        <translation>Профайл SITECON</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="60"/>
        <source>Origin</source>
        <translation>Происхождение</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="72"/>
        <source>Window size, bp</source>
        <translation>Размер окна</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="72"/>
        <source>Window size.</source>
        <translation>Размер окна.</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="73"/>
        <source>Calibration length</source>
        <translation>Калибровочная длина</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="73"/>
        <source>Calibration length.</source>
        <translation>Калибровочная длина.</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="74"/>
        <source>Random seed</source>
        <translation>Затравка</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="74"/>
        <source>Random seed.</source>
        <translation>Затравка.</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="75"/>
        <source>Weight algorithm</source>
        <translation>Алгоритм весов</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="59"/>
        <source>Input multiple sequence alignment for building statistical model.</source>
        <translation>Входное множественное выравнивание для построения статистической модели данного ССТФ.</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="60"/>
        <source>Location of input alignment, used as optional hint for model description.</source>
        <translation>Адрес множественного выравнивания, используется для описания выходного профайла SITECON.</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="65"/>
        <source>Produced statistical model of specified TFBS data.</source>
        <translation>Произведённый профайл SITECON.</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="75"/>
        <source>Weight algorithm.</source>
        <translation>Алгоритм весов.</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildWorker.cpp" line="84"/>
        <source>Builds statistical profile for SITECON. The SITECON is a program for probabilistic recognition of transcription factor binding sites.</source>
        <translation>Строит статистический профайл для поиска ССТФ при помощи пакета SITECON.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::SiteconReadPrompter</name>
    <message>
        <location filename="../src/SiteconIOWorkers.cpp" line="189"/>
        <source>Read model(s) from &lt;u&gt;%1&lt;/u&gt;.</source>
        <translation>Загрузить профайлы SITECON из &lt;u&gt;%1&lt;/u&gt;.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::SiteconReader</name>
    <message>
        <location filename="../src/SiteconIOWorkers.cpp" line="231"/>
        <source>Loaded SITECON model from %1</source>
        <translation>Загружен профайл SITECON из %1</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::SiteconSearchPrompter</name>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="150"/>
        <source>unset</source>
        <translation>не указан</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="151"/>
        <source>For each sequence from &lt;u&gt;%1&lt;/u&gt;,</source>
        <translation>Для каждой последовательности из &lt;u&gt;%1&lt;/u&gt;,</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="165"/>
        <source>%1 search transcription factor binding sites (TFBS) %2.&lt;br&gt;Recognize sites with &lt;u&gt;similarity %3%&lt;/u&gt;, process &lt;u&gt;%4&lt;/u&gt;.&lt;br&gt;Output the list of found regions annotated as &lt;u&gt;%5&lt;/u&gt;.</source>
        <translation>%1 искать сайты связывания ТФ %2.&lt;br&gt;Распознавать сайты с достоверностью &lt;u&gt;не менее %3%&lt;/u&gt;, рассматривать &lt;u&gt;%4 стренды&lt;/u&gt;.&lt;br&gt;Выдать список найденных регионов аннотированных как &lt;u&gt;%5&lt;/u&gt;.</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="152"/>
        <source>with all profiles provided by &lt;u&gt;%1&lt;/u&gt;,</source>
        <translation>со всеми профайлами из &lt;u&gt;%1&lt;/u&gt;,</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::SiteconSearchWorker</name>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="159"/>
        <source>both strands</source>
        <translation>все</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="160"/>
        <source>direct strand</source>
        <translation>только прямые</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="161"/>
        <source>complement strand</source>
        <translation>только комплиментарные</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="65"/>
        <source>Sitecon Model</source>
        <translation>Профайл SITECON</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="66"/>
        <source>Sequence</source>
        <translation>Входная последовательность</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="67"/>
        <source>SITECON annotations</source>
        <translation>Аннотированные ССТФ</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="81"/>
        <source>Result annotation</source>
        <translation>Аннотации</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="87"/>
        <source>Alternative setting for filtering results, minimal value of Error type I.&lt;br&gt;Note that all thresholds (by score, by err1 and by err2) are applied when filtering results.&lt;p&gt;&lt;i&gt;If you need to switch off this filter choose &lt;b&gt;&quot;0&quot;&lt;/b&gt; value&lt;/i&gt;&lt;/p&gt;.</source>
        <translation>Фильтрация результатов по значению ошибки I рода. &lt;br&gt;Учтите что применяются все фильтры (по оценке и ошибкам 1, 2 рода).</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="91"/>
        <source>Alternative setting for filtering results, max value of Error type II.&lt;br&gt;Note that all thresholds (by score, by err1 and by err2) are applied when filtering results.&lt;p&gt;&lt;i&gt;If you need to switch off this filter choose &lt;b&gt;&quot;1&quot;&lt;/b&gt; value&lt;/i&gt;&lt;/p&gt;.</source>
        <translation>Фильтрация результатов по значению ошибки II рода. &lt;br&gt;Учтите что применяются все фильтры (по оценке и ошибкам 1, 2 рода).</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="101"/>
        <source>Search for TFBS with SITECON</source>
        <translation>Поиск сайтов связывания ТФ с помощью SITECON</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="204"/>
        <source>Min score can not be less 60% or more 100%</source>
        <translation>Минимальная оценка не может быть меньше 60 % и не больше 100%</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="207"/>
        <source>Min Err1 can not be less 0 or more 1</source>
        <translation>Минимальная ошибка первого рода не может быть меньше 0 или больше 1</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="210"/>
        <source>Max Err2 can not be less 0 or more 1</source>
        <translation>Максимальная ошибка второго рода не может быть меньше 0 или больше 1</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="213"/>
        <source>Search in strand can only be 0(both) or 1(direct) or -1(complement)</source>
        <translation>Направления могут быть следующими: 0 - обе цепи, 1- прямая, -1-комплементарная</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="253"/>
        <source>Find TFBS in %1</source>
        <translation>Поиск ССТФ в %1</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="257"/>
        <source>Bad sequence supplied to SiteconSearch: %1</source>
        <translation>Неправильная последовательность для SITECON поиска : %1</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="281"/>
        <source>Found %1 TFBS</source>
        <translation>Найдено %1 возможных сайтов связывания ТФ</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="65"/>
        <source>Profile data to search with.</source>
        <translation>Профайл SITECON характеризующий искомые сайты.</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="66"/>
        <source>Input nucleotide sequence to search in.</source>
        <translation>Входная нуклеотидная последовательность для поиска.</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="68"/>
        <source>Annotations marking found TFBS sites.</source>
        <translation>Аннотации маркирующие найденные регионы.</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="82"/>
        <source>Annotation name for marking found regions.</source>
        <translation>Имя аннотации для разметки найденных регионов.</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="102"/>
        <source>Searches each input sequence for transcription factor binding sites significantly similar to specified SITECON profiles. In case several profiles were supplied, searches with all profiles one by one and outputs merged set of annotations for each sequence.</source>
        <translation>Поиск сайтов связывания транскрипционных факторов (ССТФ). В каждой нуклеотидной последовательности, поданной на вход задачи, ищутся значимые совпадения с указанными профайлами SITECON. Найденные регионы выдаются в виде набора аннотаций. Профайлов может быть несколько, поиск отработает для каждого профайла и выдаст общий набор аннотаций для каждой последовательности.&lt;p&gt; Протеиновые последовательности на входе допустимы но игнорируются.</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="83"/>
        <source>Min score</source>
        <translation>Мин оценка</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="84"/>
        <source>Recognition quality percentage threshold.&lt;p&gt;&lt;i&gt;If you need to switch off this filter choose &lt;b&gt;the lowest&lt;/b&gt; value&lt;/i&gt;&lt;/p&gt;.</source>
        <translation>Порог качества. Для того, чтобы отключить этот фильтр укажите наименьшее значение.</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="86"/>
        <source>Min Err1</source>
        <translation>Мин ошибка 1 рода</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchWorker.cpp" line="90"/>
        <source>Max Err2</source>
        <translation>Макс ошибка 2 рода</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::SiteconWritePrompter</name>
    <message>
        <location filename="../src/SiteconIOWorkers.cpp" line="198"/>
        <source>Save the profile(s) from &lt;u&gt;%1&lt;/u&gt; to %2.</source>
        <translation>Сохранить все профайлы из &lt;u&gt;%1&lt;/u&gt; в %2.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::SiteconWriter</name>
    <message>
        <location filename="../src/SiteconIOWorkers.cpp" line="256"/>
        <source>Unspecified URL for writing Sitecon</source>
        <translation>Не укзазан адрес для записи модели SITECON</translation>
    </message>
    <message>
        <location filename="../src/SiteconIOWorkers.cpp" line="272"/>
        <source>Writing SITECON model to %1</source>
        <translation>Запись профайла SITECON в %1</translation>
    </message>
</context>
<context>
    <name>U2::QDSiteconActor</name>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="65"/>
        <source>%1</source>
        <translation>%1</translation>
    </message>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="69"/>
        <source>with profile provided by %1 %2&lt;/a&gt;</source>
        <translation>с профайлом %1 %2&lt;/a&gt;</translation>
    </message>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="72"/>
        <source>with all %1 %2 profiles&lt;/a&gt;</source>
        <translation>со всеми %1 %2 профайлами&lt;/a&gt;</translation>
    </message>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="75"/>
        <source>similarity %1%</source>
        <translation>сходство %1%</translation>
    </message>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="80"/>
        <source>both strands</source>
        <translation>оба стренда</translation>
    </message>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="81"/>
        <source>direct strand</source>
        <translation>прямой стренд</translation>
    </message>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="82"/>
        <source>complement strand</source>
        <translation>комплементарный стренд</translation>
    </message>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="107"/>
        <source>%1: incorrect sitecon model url(s)</source>
        <translation>%1: incorrect sitecon model url(s)</translation>
    </message>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="111"/>
        <source>%1: min score can not be less 60% or more 100%</source>
        <translation>%1: min score can not be less 60% or more 100%</translation>
    </message>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="115"/>
        <source>%1: min Err1 can not be less 0 or more 1</source>
        <translation>%1: min Err1 can not be less 0 or more 1</translation>
    </message>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="119"/>
        <source>%1: max Err2 can not be less 0 or more 1</source>
        <translation>%1: max Err2 can not be less 0 or more 1</translation>
    </message>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="159"/>
        <source>Sitecon</source>
        <translation>SITECON</translation>
    </message>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="160"/>
        <source>Searches for transcription factor binding sites significantly similar to specified SITECON profiles. In case several profiles were supplied, searches with all profiles one by one and outputs merged set of annotations.</source>
        <translation>Поиск сайтов связывания транскрипционных факторов (ССТФ). В каждой нуклеотидной последовательности, поданной на вход задачи, ищутся значимые совпадения с указанными профайлами SITECON. Найденные регионы выдаются в виде набора аннотаций. Профайлов может быть несколько, поиск отработает для каждого профайла и выдаст общий набор аннотаций для каждой последовательности.&lt;p&gt; Протеиновые последовательности на входе допустимы но игнорируются.</translation>
    </message>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="85"/>
        <source>Searches transcription factor binding sites (TFBS) %1.&lt;br&gt;Recognize sites with %2, process %3.</source>
        <translation>Ищет сайты связывания транскрипционных факторов (ТФ) %1. &lt;br&gt;Распознаёт сайты со сходством %2, рассматривает %3.</translation>
    </message>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="164"/>
        <source>Min score</source>
        <translation>Мин оценка</translation>
    </message>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="166"/>
        <source>Min Err1</source>
        <translation>Мин ошибка 1 рода</translation>
    </message>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="167"/>
        <source>Alternative setting for filtering results, minimal value of Error type I.&lt;br&gt;Note that all thresholds (by score, by err1 and by err2) are applied when filtering results.</source>
        <translation>Фильтрация результатов по значению ошибки I рода. &lt;br&gt;Учтите что применяются все фильтры (по оценке и ошибкам 1, 2 рода).</translation>
    </message>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="169"/>
        <source>Max Err2</source>
        <translation>Макс ошибка 2 рода</translation>
    </message>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="170"/>
        <source>Alternative setting for filtering results, max value of Error type II.&lt;br&gt;Note that all thresholds (by score, by err1 and by err2) are applied when filtering results.</source>
        <translation>Фильтрация результатов по значению ошибки II рода. &lt;br&gt;Учтите что применяются все фильтры (по оценке и ошибкам 1, 2 рода).</translation>
    </message>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="172"/>
        <source>Model</source>
        <translation>Модель</translation>
    </message>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="172"/>
        <source>Profile data to search with.</source>
        <translation>Профайл SITECON характеризующий искомые сайты.</translation>
    </message>
</context>
<context>
    <name>U2::QDSiteconTask</name>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="204"/>
        <source>Sitecon Query</source>
        <translation>SITECON запрос</translation>
    </message>
</context>
<context>
    <name>U2::SiteconADVContext</name>
    <message>
        <location filename="../src/SiteconPlugin.cpp" line="118"/>
        <source>Find TFBS with SITECON...</source>
        <translation>Поиск сайтов связывания ТФ с помощью SITECON...</translation>
    </message>
</context>
<context>
    <name>U2::SiteconBuildDialogController</name>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="59"/>
        <location filename="../src/SiteconBuildDialogController.cpp" line="148"/>
        <source>Build</source>
        <translation>Построить</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="60"/>
        <location filename="../src/SiteconBuildDialogController.cpp" line="129"/>
        <source>Cancel</source>
        <translation>Отменить</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="102"/>
        <source>Illegal alignment file</source>
        <translation>Укажите входной файл выравнивания</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="107"/>
        <source>Illegal SITECON model file</source>
        <translation>Укажите файл результирующей модели</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="113"/>
        <source>Error</source>
        <translation>Ошибка</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="125"/>
        <source>Starting calibration process</source>
        <translation>Начинается построение</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="128"/>
        <source>Hide</source>
        <translation>Скрыть</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="142"/>
        <source>Build finished with error: %1</source>
        <translation>Построение завершено с ошибкой: %1</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="144"/>
        <source>Build canceled</source>
        <translation>Построение отменено</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="146"/>
        <source>Build finished successfully</source>
        <translation>Построение успешно завершено</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="149"/>
        <source>Close</source>
        <translation>Закрыть</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="155"/>
        <source>Running... State :%1 Progress: %2</source>
        <translation>Прогресс %2%: %1</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="168"/>
        <source>Sitecon models</source>
        <translation>Профайлы SITECON</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="75"/>
        <source>Select file with alignment</source>
        <translation>Выбор выравнивания</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="165"/>
        <source>Select file to save model to...</source>
        <translation>Выбор файла для сохранения профайла...</translation>
    </message>
</context>
<context>
    <name>U2::SiteconBuildTask</name>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="185"/>
        <source>Build SITECON model</source>
        <translation>Построить SITECON</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="195"/>
        <source>Alignment contains gaps</source>
        <translation>Выравнивание содержит пробелы</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="199"/>
        <source>Alignment is empty</source>
        <translation>Выравнивание не содержит данных</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="203"/>
        <source>Alignment must have at least 2 sequences</source>
        <translation>Выравнивание должно содержать минимум 2 последовательности</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="207"/>
        <source>Alignment is not nucleic</source>
        <translation>Поддерживаются только нуклеотидные выравнивания</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="211"/>
        <source>Window size is greater than alignment length</source>
        <translation>Размер окна больше длины последовательности</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="225"/>
        <source>Calculating average and dispersion matrixes</source>
        <translation>Расчёт матрицы средних и дисперсии</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="230"/>
        <source>Calculating weights</source>
        <translation>Расчёт весов</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="236"/>
        <source>Calibrating first type error</source>
        <translation>Расчёт ошибки ошибки 1-го рода</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="242"/>
        <source>Calibrating second type error</source>
        <translation>Расчёт ошибки ошибки 2-го рода</translation>
    </message>
</context>
<context>
    <name>U2::SiteconBuildToFileTask</name>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="250"/>
        <source>Build SITECON model to file</source>
        <translation>Построить SITECON</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="261"/>
        <source>Unknown alignment format</source>
        <translation>Неверный формат файла</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="269"/>
        <source>Loading alignment</source>
        <translation>Загрузка выравнивания</translation>
    </message>
    <message>
        <location filename="../src/SiteconBuildDialogController.cpp" line="288"/>
        <source>No alignment found</source>
        <translation>Выравнивание не найдено</translation>
    </message>
</context>
<context>
    <name>U2::SiteconIO</name>
    <message>
        <location filename="../src/SiteconIO.cpp" line="107"/>
        <source>Not a sitecon model</source>
        <translation>Not a sitecon model</translation>
    </message>
    <message>
        <location filename="../src/SiteconIO.cpp" line="151"/>
        <source>Error parsing settings, line %1</source>
        <translation>Error parsing settings, line %1</translation>
    </message>
    <message>
        <location filename="../src/SiteconIO.cpp" line="162"/>
        <source>Error parsing window size: %1</source>
        <translation>Error parsing window size: %1</translation>
    </message>
    <message>
        <location filename="../src/SiteconIO.cpp" line="175"/>
        <source>Error parsing calibration len: %1</source>
        <translation>Error parsing calibration len: %1</translation>
    </message>
    <message>
        <location filename="../src/SiteconIO.cpp" line="181"/>
        <source>Error parsing RSEED: %1</source>
        <translation>Error parsing RSEED: %1</translation>
    </message>
    <message>
        <location filename="../src/SiteconIO.cpp" line="187"/>
        <source>Error parsing number of sequence in original alignment: %1</source>
        <translation>Error parsing number of sequence in original alignment: %1</translation>
    </message>
    <message>
        <location filename="../src/SiteconIO.cpp" line="193"/>
        <source>Error parsing in alignment %1</source>
        <translation>Error parsing in alignment %1</translation>
    </message>
    <message>
        <location filename="../src/SiteconIO.cpp" line="262"/>
        <source>Error parsing %1 in line %2</source>
        <translation>Error parsing %1 in line %2</translation>
    </message>
    <message>
        <location filename="../src/SiteconIO.cpp" line="280"/>
        <source>Error parsing in line %1</source>
        <translation>Error parsing in line %1</translation>
    </message>
    <message>
        <location filename="../src/SiteconIO.cpp" line="290"/>
        <location filename="../src/SiteconIO.cpp" line="295"/>
        <source>Error parsing %1</source>
        <translation>Error parsing %1</translation>
    </message>
    <message>
        <location filename="../src/SiteconIO.cpp" line="299"/>
        <source>Illegal error %1</source>
        <translation>Illegal error %1</translation>
    </message>
    <message>
        <location filename="../src/SiteconIO.cpp" line="309"/>
        <source>Error parsing file in line %1</source>
        <translation>Error parsing file in line %1</translation>
    </message>
    <message>
        <location filename="../src/SiteconIO.cpp" line="318"/>
        <source>Number of &apos;average&apos; and  &apos;sdev&apos; properties not matches</source>
        <translation>Number of &apos;average&apos; and  &apos;sdev&apos; properties not matches</translation>
    </message>
    <message>
        <location filename="../src/SiteconIO.cpp" line="323"/>
        <source>Number of &apos;average&apos; and &apos;weight&apos; properties not matches</source>
        <translation>Number of &apos;average&apos; and &apos;weight&apos; properties not matches</translation>
    </message>
    <message>
        <location filename="../src/SiteconIO.cpp" line="328"/>
        <source>Property in file %1 is not the same as built-in: %2</source>
        <translation>Property in file %1 is not the same as built-in: %2</translation>
    </message>
    <message>
        <location filename="../src/SiteconIO.cpp" line="356"/>
        <source>Error info in file is not complete</source>
        <translation>Error info in file is not complete</translation>
    </message>
    <message>
        <location filename="../src/SiteconIO.cpp" line="363"/>
        <source>Model verification error</source>
        <translation>Model verification error</translation>
    </message>
    <message>
        <location filename="../src/SiteconIO.cpp" line="201"/>
        <source>Illegal weight algorithm: %1</source>
        <translation>Illegal weight algorithm: %1</translation>
    </message>
    <message>
        <location filename="../src/SiteconIO.cpp" line="213"/>
        <source>Model size not matched: %1, expected: %2</source>
        <translation>Model size not matched: %1, expected: %2</translation>
    </message>
    <message>
        <location filename="../src/SiteconIO.cpp" line="236"/>
        <source>Duplicate property: %1</source>
        <translation>Duplicate property: %1</translation>
    </message>
    <message>
        <location filename="../src/SiteconIO.cpp" line="246"/>
        <source>Property not recognized: %1</source>
        <translation>Property not recognized: %1</translation>
    </message>
    <message>
        <location filename="../src/SiteconIOWorkers.cpp" line="63"/>
        <location filename="../src/SiteconIOWorkers.cpp" line="139"/>
        <location filename="../src/SiteconIOWorkers.cpp" line="148"/>
        <source>Sitecon model</source>
        <translation>Профайл SITECON</translation>
    </message>
    <message>
        <location filename="../src/SiteconIOWorkers.cpp" line="71"/>
        <source>SITECON</source>
        <translation>SITECON</translation>
    </message>
    <message>
        <location filename="../src/SiteconIO.cpp" line="50"/>
        <source>Sitecon models</source>
        <translation>Профайлы SITECON</translation>
    </message>
    <message>
        <location filename="../src/SiteconIOWorkers.cpp" line="139"/>
        <source>Input Sitecon model</source>
        <translation>Входящий профайл SITECON</translation>
    </message>
    <message>
        <location filename="../src/SiteconIOWorkers.cpp" line="141"/>
        <source>Saves all input SITECON profiles to specified location.</source>
        <translation>Сохраняет все входящие профайлы SITECON в указанный файл.</translation>
    </message>
    <message>
        <location filename="../src/SiteconIOWorkers.cpp" line="141"/>
        <source>Write SITECON Model</source>
        <translation>Сохранение модели SITECON</translation>
    </message>
    <message>
        <location filename="../src/SiteconIOWorkers.cpp" line="148"/>
        <source>Loaded SITECON profile data.</source>
        <translation>Загруженный профайл SITECON.</translation>
    </message>
    <message>
        <location filename="../src/SiteconIOWorkers.cpp" line="154"/>
        <source>Reads SITECON profiles from file(s). The files can be local or Internet URLs.</source>
        <translation>Считывает профайл(ы) SITECON из указанных файлов. Файлы могут быть локальными либо адресами Интернет.</translation>
    </message>
    <message>
        <location filename="../src/SiteconIOWorkers.cpp" line="154"/>
        <source>Read SITECON Model</source>
        <translation>Загрузка модели SITECON</translation>
    </message>
</context>
<context>
    <name>U2::SiteconPlugin</name>
    <message>
        <location filename="../src/SiteconPlugin.cpp" line="60"/>
        <source>SITECON</source>
        <translation>SITECON</translation>
    </message>
    <message>
        <location filename="../src/SiteconPlugin.cpp" line="60"/>
        <source>SITECON - is a program package for revealing and analysis of conservative conformational and physicochemical properties in transcription factor binding sites sets.</source>
        <translation>Адаптированная версия программы SITECON: поиск сайтов связывания транскрипционных факторов. Включает около сотни откалиброванных моделей ССТФ. 
Домашняя страница проекта: http://wwwmgs.bionet.nsc.ru/cgi-bin/mgs/sitecon/sitecon.pl?stage=0.</translation>
    </message>
    <message>
        <location filename="../src/SiteconPlugin.cpp" line="66"/>
        <source>Build SITECON model...</source>
        <translation>Построение модели SITECON...</translation>
    </message>
</context>
<context>
    <name>U2::SiteconReadMultiTask</name>
    <message>
        <location filename="../src/SiteconQuery.cpp" line="231"/>
        <source>Load sitecon models task</source>
        <translation>Load sitecon models task</translation>
    </message>
</context>
<context>
    <name>U2::SiteconReadTask</name>
    <message>
        <location filename="../src/SiteconIO.h" line="52"/>
        <source>Read SITECON Model</source>
        <translation>Загрузка модели SITECON</translation>
    </message>
</context>
<context>
    <name>U2::SiteconSearchDialogController</name>
    <message>
        <location filename="../src/SiteconSearchDialogController.cpp" line="90"/>
        <source>Search</source>
        <translation>Поиск</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialogController.cpp" line="91"/>
        <location filename="../src/SiteconSearchDialogController.cpp" line="153"/>
        <source>Cancel</source>
        <translation>Отмена</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialogController.cpp" line="153"/>
        <source>Close</source>
        <translation>Закрыть</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialogController.cpp" line="165"/>
        <source>Progress: %1% </source>
        <translation>Прогресс  %1%</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialogController.cpp" line="167"/>
        <source>%1 results found</source>
        <translation>найдено %1</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialogController.cpp" line="186"/>
        <source>Select file with SITECON model</source>
        <translation>Выбор модели SITECON</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialogController.cpp" line="195"/>
        <location filename="../src/SiteconSearchDialogController.cpp" line="300"/>
        <location filename="../src/SiteconSearchDialogController.cpp" line="310"/>
        <location filename="../src/SiteconSearchDialogController.cpp" line="322"/>
        <source>Error</source>
        <translation>Ошибка</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialogController.cpp" line="231"/>
        <source>%1%,  first type error %2,  second type error %3</source>
        <translation>%1% err1=%2	err2=%3</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialogController.cpp" line="300"/>
        <source>No model selected</source>
        <translation>Укажите файл модели</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialogController.cpp" line="310"/>
        <source>Range is too small</source>
        <translation>Слишком маленький регион для поиска</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialogController.cpp" line="322"/>
        <source>Error parsing minimum score</source>
        <translation>Неразборчивая строка минимальной ошибки</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialogController.cpp" line="386"/>
        <source>Complement strand</source>
        <translation>Только комплeментарные</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchDialogController.cpp" line="386"/>
        <source>Direct strand</source>
        <translation>Только прямые</translation>
    </message>
</context>
<context>
    <name>U2::SiteconSearchTask</name>
    <message>
        <location filename="../src/SiteconSearchTask.cpp" line="28"/>
        <source>SITECON search</source>
        <translation>Поиск сайтов связывания ТФ с помощью SITECON</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchTask.cpp" line="45"/>
        <source>SITECON search parallel subtask</source>
        <translation>Параллельный поиск ССТФ</translation>
    </message>
    <message>
        <location filename="../src/SiteconSearchTask.cpp" line="65"/>
        <source>Internal error: invalid PSUM: %1</source>
        <translation>Внутренняя ошибка, неверная сумма: %1</translation>
    </message>
</context>
<context>
    <name>U2::SiteconWriteTask</name>
    <message>
        <location filename="../src/SiteconIO.h" line="65"/>
        <source>Save SITECON model</source>
        <translation>Сохранение модели SITECON</translation>
    </message>
</context>
</TS>
