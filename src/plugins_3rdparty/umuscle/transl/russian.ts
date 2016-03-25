<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru_RU">
<context>
    <name>MuscleAlignmentDialog</name>
    <message>
        <location filename="../src/MuscleAlignDialog.ui" line="20"/>
        <source>Align with MUSCLE</source>
        <translation>Выравнивание с помощью MUSCLE</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialog.ui" line="36"/>
        <source>Input file</source>
        <translation>Входной файл</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialog.ui" line="46"/>
        <location filename="../src/MuscleAlignDialog.ui" line="63"/>
        <source>...</source>
        <translation>...</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialog.ui" line="53"/>
        <source>Output file</source>
        <translation>Выходной файл</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialog.ui" line="83"/>
        <source>Mode</source>
        <translation>Конфигурация</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialog.ui" line="95"/>
        <source>Mode details:</source>
        <translation>Описание:</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialog.ui" line="115"/>
        <source>Advanced options</source>
        <translation>Дополнительные настройки</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialog.ui" line="138"/>
        <source>Max iterations</source>
        <translation>Максимальное число итераций</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialog.ui" line="181"/>
        <source>Max time (minutes)</source>
        <translation>Максимальное время работы (мин.)</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialog.ui" line="225"/>
        <source>Translating alignment to amino allows to avoid errors of inserting gaps within codon boundaries.</source>
        <translation>Трансляция выравнивания в амино позволяет избежать ошибок вставки пробелов в границах кодонов.</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialog.ui" line="228"/>
        <source>Translate to amino when aligning</source>
        <translation>Транслировать в амины в процессе выравнивания</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialog.ui" line="240"/>
        <source>Translation table:</source>
        <translation>Таблица трансляций:</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialog.ui" line="261"/>
        <source>Region to align</source>
        <translation>Выравнивать</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialog.ui" line="351"/>
        <source>-</source>
        <translation>-</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialog.ui" line="269"/>
        <source>Whole alignment</source>
        <translation>Целые последовательности</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialog.ui" line="284"/>
        <source>Column range</source>
        <translation>Указанный регион столбцов</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialog.ui" line="123"/>
        <source>Do not re-arrange sequences (-stable)</source>
        <translation>Сохранять порядок последовательностей (ключ -stable)</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/MuscleWorker.cpp" line="59"/>
        <source>Region should be set as &apos;start..end&apos;, start should be less than end, e.g. &apos;1..100&apos;</source>
        <translation>Region should be set as &apos;start..end&apos;, start should be less than end, e.g. &apos;1..100&apos;</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::MusclePrompter</name>
    <message>
        <location filename="../src/MuscleWorker.cpp" line="125"/>
        <source> from %1</source>
        <translation> из %1</translation>
    </message>
    <message>
        <location filename="../src/MuscleWorker.cpp" line="133"/>
        <source>Aligns each MSA supplied &lt;u&gt;%1&lt;/u&gt; with MUSCLE using &quot;&lt;u&gt;%2&lt;/u&gt;&quot; mode.</source>
        <translation>Для каждого выравнивания&lt;u&gt;%1&lt;/u&gt;, запустить MUSCLE в  &lt;u&gt;&quot;%2&quot;&lt;/u&gt; и выдать результат на  выход.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::MuscleWorker</name>
    <message>
        <location filename="../src/MuscleWorker.cpp" line="63"/>
        <source>Input MSA</source>
        <translation>Входное выравнивание</translation>
    </message>
    <message>
        <location filename="../src/MuscleWorker.cpp" line="65"/>
        <source>Multiple sequence alignment</source>
        <translation>Множественное выравнивание</translation>
    </message>
    <message>
        <location filename="../src/MuscleWorker.cpp" line="74"/>
        <source>Mode</source>
        <translation>Конфигурация</translation>
    </message>
    <message>
        <location filename="../src/MuscleWorker.cpp" line="77"/>
        <source>Stable order</source>
        <translation>Сохранять порядок</translation>
    </message>
    <message>
        <location filename="../src/MuscleWorker.cpp" line="64"/>
        <source>Multiple sequence alignment to be processed.</source>
        <translation>Входные данные для выравнивания.</translation>
    </message>
    <message>
        <location filename="../src/MuscleWorker.cpp" line="65"/>
        <source>Result of alignment.</source>
        <translation>Результат выравнивания.</translation>
    </message>
    <message>
        <location filename="../src/MuscleWorker.cpp" line="81"/>
        <source>Max iterations</source>
        <translation>Максимальное число итераций</translation>
    </message>
    <message>
        <location filename="../src/MuscleWorker.cpp" line="82"/>
        <source>Maximum number of iterations.</source>
        <translation>Максимальное число итераций.</translation>
    </message>
    <message>
        <location filename="../src/MuscleWorker.cpp" line="176"/>
        <source>An empty MSA &apos;%1&apos; has been supplied to MUSCLE.</source>
        <translation>An empty MSA &apos;%1&apos; has been supplied to MUSCLE.</translation>
    </message>
    <message>
        <location filename="../src/MuscleWorker.cpp" line="83"/>
        <source>Region to align</source>
        <translation>Выравнивать</translation>
    </message>
    <message>
        <location filename="../src/MuscleWorker.cpp" line="84"/>
        <source>Whole alignment or column range e.g. &lt;b&gt;1..100&lt;/b&gt;.</source>
        <translation>Все выравнивание или регион, например &lt;b&gt;1..100&lt;/b&gt;.</translation>
    </message>
    <message>
        <location filename="../src/MuscleWorker.cpp" line="91"/>
        <source>Align with MUSCLE</source>
        <translation>Выравнивание с помощью MUSCLE</translation>
    </message>
    <message>
        <location filename="../src/MuscleWorker.cpp" line="92"/>
        <source>MUSCLE is public domain multiple alignment software for protein and nucleotide sequences.&lt;p&gt;&lt;dfn&gt;MUSCLE stands for MUltiple Sequence Comparison by Log-Expectation.&lt;/dfn&gt;&lt;/p&gt;</source>
        <translation>&lt;p&gt;&lt;dfn&gt;Пакет MUSCLE предназначен для выравнивания множественных протеиновых и нуклеотидных последовательностей.&lt;/dfn&gt;&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/MuscleWorker.cpp" line="200"/>
        <source>Region end position should be greater than start position</source>
        <translation>Конец региона должен быть больше чем его начало</translation>
    </message>
    <message>
        <location filename="../src/MuscleWorker.cpp" line="234"/>
        <source>Aligned %1 with MUSCLE</source>
        <translation>Выровнял %1 с MUSCLE</translation>
    </message>
    <message>
        <location filename="../src/MuscleWorker.cpp" line="75"/>
        <source>Selector of preset configurations, that give you the choice of optimizing accuracy, speed, or some compromise between the two. The default favors accuracy.</source>
        <translation>Выбор между режимами максимальной точности, скорости либо компромиса между ними. По умолчанию оптимизируется точность выравнивания.</translation>
    </message>
    <message>
        <location filename="../src/MuscleWorker.cpp" line="78"/>
        <source>Do not rearrange aligned sequences (-stable switch of MUSCLE). &lt;p&gt;Otherwise, MUSCLE re-arranges sequences so that similar sequences are adjacent in the output file. This makes the alignment easier to evaluate by eye. </source>
        <translation>Сохранять начальный порядок последовательностей в выравнивании. &lt;p&gt;Иначе, MUSCLE группирует похожие последовательности в выходном выравнивании, для удобства визуального сравнения.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::ProfileToProfileWorker</name>
    <message>
        <location filename="../src/ProfileToProfileWorker.cpp" line="115"/>
        <source>Aligned profile to profile with MUSCLE</source>
        <translation>Выравнивание профиля на профиль при помощи MUSCLE</translation>
    </message>
    <message>
        <location filename="../src/ProfileToProfileWorker.cpp" line="211"/>
        <source>Master profile</source>
        <translation>Основной профиль</translation>
    </message>
    <message>
        <location filename="../src/ProfileToProfileWorker.cpp" line="212"/>
        <source>The main alignment which will be aligned on.</source>
        <translation>Основное выравнивание на которое будет произведено выравнивание.</translation>
    </message>
    <message>
        <location filename="../src/ProfileToProfileWorker.cpp" line="214"/>
        <source>Second profile</source>
        <translation>Второй профиль</translation>
    </message>
    <message>
        <location filename="../src/ProfileToProfileWorker.cpp" line="215"/>
        <source>Alignment which will be aligned to the master alignment.</source>
        <translation>Выравнивание, которое будет выровнено на основной выравнивание.</translation>
    </message>
    <message>
        <location filename="../src/ProfileToProfileWorker.cpp" line="228"/>
        <source>Align Profile to Profile With MUSCLE</source>
        <translation>Выравнивание профиля на профиль при помощи MUSCLE</translation>
    </message>
    <message>
        <location filename="../src/ProfileToProfileWorker.cpp" line="229"/>
        <source>Aligns second profile to master profile with MUSCLE aligner.</source>
        <translation>Выравнивает второй профиль на основной профиль при помощи MUSCLE.</translation>
    </message>
</context>
<context>
    <name>U2::MuscleAdapter</name>
    <message>
        <location filename="../src/MuscleAdapter.cpp" line="79"/>
        <source>No sequences in input file</source>
        <translation>Выравнивание не содержит данных</translation>
    </message>
    <message>
        <location filename="../src/MuscleAdapter.cpp" line="51"/>
        <location filename="../src/MuscleAdapter.cpp" line="207"/>
        <location filename="../src/MuscleAdapter.cpp" line="298"/>
        <location filename="../src/MuscleAdapter.cpp" line="485"/>
        <source>Internal MUSCLE error: %1</source>
        <translation>Внутренняя ошибка MUSCLE: %1</translation>
    </message>
    <message>
        <location filename="../src/MuscleAdapter.cpp" line="57"/>
        <location filename="../src/MuscleAdapter.cpp" line="213"/>
        <location filename="../src/MuscleAdapter.cpp" line="304"/>
        <location filename="../src/MuscleAdapter.cpp" line="491"/>
        <source>Undefined internal MUSCLE error</source>
        <translation>Undefined internal MUSCLE error</translation>
    </message>
    <message>
        <location filename="../src/MuscleAdapter.cpp" line="108"/>
        <source>Alignment is empty</source>
        <translation>Выравнивание не содержит данных</translation>
    </message>
    <message>
        <location filename="../src/MuscleAdapter.cpp" line="291"/>
        <source>Invalid input alignment</source>
        <translation>Некорректное входное выравнивание</translation>
    </message>
    <message>
        <location filename="../src/MuscleAdapter.cpp" line="312"/>
        <location filename="../src/MuscleAdapter.cpp" line="499"/>
        <source>Incompatible alphabets</source>
        <translation>Несовместимые алфавиты</translation>
    </message>
    <message>
        <location filename="../src/MuscleAdapter.cpp" line="340"/>
        <source>Aligning profiles</source>
        <translation>Выравнивание</translation>
    </message>
    <message>
        <location filename="../src/MuscleAdapter.cpp" line="343"/>
        <source>Building output</source>
        <translation>Подготовка результата</translation>
    </message>
    <message>
        <location filename="../src/MuscleAdapter.cpp" line="524"/>
        <source>Aligning sequence %1 of %2</source>
        <translation>Выравнивается последовательность %1 из %2</translation>
    </message>
    <message>
        <location filename="../src/MuscleAdapter.cpp" line="557"/>
        <source>Merging results: %1 of %2</source>
        <translation>Объединяются результаты: %1 из %2</translation>
    </message>
    <message>
        <location filename="../src/MuscleAdapter.cpp" line="572"/>
        <source>Not enough memory to do this alignment. You can try the 64-bit version of UGENE. In this case, more available memory will be used for aligning.</source>
        <translation>Недостаточно памяти для выравнивания. Вы можете попробовать 64-битную версию UGENE.</translation>
    </message>
    <message>
        <location filename="../src/MuscleAdapter.cpp" line="573"/>
        <source>Not enough memory to do this alignment.</source>
        <translation>Недостаточно памяти для выравнивания.</translation>
    </message>
    <message>
        <location filename="../src/MuscleUtils.cpp" line="91"/>
        <source>Unsupported alphabet: %1</source>
        <translation>Неподходящий алфавит: %1 </translation>
    </message>
</context>
<context>
    <name>U2::MuscleAddSequencesToProfileTask</name>
    <message>
        <location filename="../src/MuscleTask.cpp" line="260"/>
        <source>MUSCLE align profiles &apos;%1&apos; vs &apos;%2&apos;</source>
        <translation>MUSCLE выравнивает &apos;%1&apos; к &apos;%2&apos;</translation>
    </message>
    <message>
        <location filename="../src/MuscleTask.cpp" line="262"/>
        <source>MUSCLE align &apos;%2&apos; by profile &apos;%1&apos;</source>
        <translation>MUSCLE добавляет &apos;%2&apos; к &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/MuscleTask.cpp" line="271"/>
        <source>A problem occurred during aligning profile to profile with MUSCLE. The original alignment is no more available.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/MuscleTask.cpp" line="304"/>
        <source>Sequences in file have different alphabets %1</source>
        <translation>Последовательности в файле имеют разные алфавиты: %1</translation>
    </message>
    <message>
        <location filename="../src/MuscleTask.cpp" line="325"/>
        <source>No sequences found in file %1</source>
        <translation>Файл не содержит последовательностей: %1</translation>
    </message>
    <message>
        <location filename="../src/MuscleTask.cpp" line="327"/>
        <source>No alignment found in file %1</source>
        <translation>Файл не содержит выравниваний: %1</translation>
    </message>
</context>
<context>
    <name>U2::MuscleAlignDialogController</name>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="110"/>
        <source>Error</source>
        <translation>Ошибка</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="110"/>
        <source>Illegal alignment region</source>
        <translation>Неправильный регион</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="54"/>
        <source>Align</source>
        <translation>Выровнять</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="55"/>
        <source>Cancel</source>
        <translation>Отменить</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="259"/>
        <source>MUSCLE default</source>
        <translation>По умолчанию</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="260"/>
        <source>&lt;p&gt;The default settings are designed to give the best accuracy</source>
        <translation>&lt;p&gt;Наилучшая точность выравнивания</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="261"/>
        <source>&lt;p&gt;&lt;b&gt;Command line:&lt;/b&gt; muscle &lt;no-parameters&gt;</source>
        <translation>&lt;p&gt;&lt;b&gt;Командная строка:&lt;/b&gt; muscle &lt;no-parameters&gt;</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="265"/>
        <source>Large alignment</source>
        <translation>Большие выравнивания</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="266"/>
        <source>&lt;p&gt;If you have a large number of sequences (a few thousand), or they are very long, then the default settings may be too slow for practical use. A good compromise between speed and accuracy is to run just the first two iterations of the algorithm</source>
        <translation>&lt;p&gt;При наличии тысяч последовательностей либо их большой длине, конфигурация по умолчанию может оказаться неприемлемо медленной. Хороший компромисс между скоростью и точностью обеспечивается при прогоне только первых 2-х итераций алгоритма</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="267"/>
        <source>&lt;p&gt;&lt;b&gt;Command line:&lt;/b&gt; muscle &lt;i&gt;-maxiters 2&lt;/i&gt;</source>
        <translation>&lt;p&gt;&lt;b&gt;Командная строка:&lt;/b&gt; muscle &lt;i&gt;-maxiters 2&lt;/i&gt;</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="271"/>
        <source>Refine only</source>
        <translation>Только улучшить</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="272"/>
        <source>&lt;p&gt;Improves existing alignment without complete realignment</source>
        <translation>&lt;p&gt;Улучшение существующего выравнивания</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="273"/>
        <source>&lt;p&gt;&lt;b&gt;Command line:&lt;/b&gt; muscle &lt;i&gt;-refine&lt;/i&gt;</source>
        <translation>&lt;p&gt;&lt;b&gt;Командная строка:&lt;/b&gt; muscle &lt;i&gt;-refine&lt;/i&gt;</translation>
    </message>
</context>
<context>
    <name>U2::MuscleAlignWithExtFileSpecifyDialogController</name>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="142"/>
        <source>Align</source>
        <translation>Выровнять</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="143"/>
        <source>Cancel</source>
        <translation>Отменить</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="169"/>
        <source>Open an alignment file</source>
        <translation>Открыть выравнивание</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="179"/>
        <source>Save an multiple alignment file</source>
        <translation>Сохранить множественное выравнивание</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="215"/>
        <source>Error</source>
        <translation>Ошибка</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="215"/>
        <source>Illegal alignment region</source>
        <translation>Неправильный регион</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="232"/>
        <location filename="../src/MuscleAlignDialogController.cpp" line="235"/>
        <source>Kalign with Align</source>
        <translation>Выравнивание с помощью Kalign</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="233"/>
        <source>Input file is not set!</source>
        <translation>Входной файл не указан!</translation>
    </message>
    <message>
        <location filename="../src/MuscleAlignDialogController.cpp" line="236"/>
        <source>Output file is not set!</source>
        <translation>Выходной файл не указан!</translation>
    </message>
</context>
<context>
    <name>U2::MuscleGObjectRunFromSchemaTask</name>
    <message>
        <location filename="../src/MuscleTask.cpp" line="597"/>
        <source>Workflow wrapper &apos;%1&apos;</source>
        <translation>Workflow wrapper &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/MuscleTask.cpp" line="610"/>
        <source>MUSCLE align &apos;%1&apos;</source>
        <translation>MUSCLE выравнивает &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/MuscleTask.cpp" line="613"/>
        <source>MUSCLE refine &apos;%1&apos;</source>
        <translation>MUSCLE улучшает &apos;%1&apos;</translation>
    </message>
</context>
<context>
    <name>U2::MuscleGObjectTask</name>
    <message>
        <location filename="../src/MuscleTask.cpp" line="357"/>
        <source>MUSCLE align &apos;%1&apos;</source>
        <translation>MUSCLE выравнивает &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/MuscleTask.cpp" line="360"/>
        <source>MUSCLE refine &apos;%1&apos;</source>
        <translation>MUSCLE улучшает &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/MuscleTask.cpp" line="363"/>
        <source>MUSCLE add to profile &apos;%1&apos;</source>
        <translation>MUSCLE добавляет в выравнивание &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/MuscleTask.cpp" line="366"/>
        <source>MUSCLE align profiles</source>
        <translation>MUSCLE выравнивает пару выравниваний</translation>
    </message>
    <message>
        <location filename="../src/MuscleTask.cpp" line="412"/>
        <source>MAlignment object has been changed</source>
        <translation>Объект множественного выравнивания был изменен</translation>
    </message>
</context>
<context>
    <name>U2::MuscleMSAEditorContext</name>
    <message>
        <location filename="../src/MusclePlugin.cpp" line="141"/>
        <source>Align with MUSCLE...</source>
        <translation>Выравнивание с помощью MUSCLE...</translation>
    </message>
    <message>
        <location filename="../src/MusclePlugin.cpp" line="150"/>
        <source>Align sequences to profile with MUSCLE...</source>
        <translation>Выровнять последовательности на профиль при помощи MUSCLE...</translation>
    </message>
    <message>
        <location filename="../src/MusclePlugin.cpp" line="159"/>
        <source>Align profile to profile with MUSCLE...</source>
        <translation>Выровнять профиль на профиль при помощи MUSCLE...</translation>
    </message>
    <message>
        <location filename="../src/MusclePlugin.cpp" line="238"/>
        <location filename="../src/MusclePlugin.cpp" line="241"/>
        <source>Select file with sequences</source>
        <translation>Выбор файла последовательностей</translation>
    </message>
    <message>
        <location filename="../src/MusclePlugin.cpp" line="266"/>
        <location filename="../src/MusclePlugin.cpp" line="270"/>
        <source>Select file with alignment</source>
        <translation>Выбор файла выравнивания</translation>
    </message>
</context>
<context>
    <name>U2::MuscleParallelTask</name>
    <message>
        <location filename="../src/MuscleParallel.cpp" line="52"/>
        <source>MuscleParallelTask</source>
        <translation>MuscleParallelTask</translation>
    </message>
    <message>
        <location filename="../src/MuscleParallel.cpp" line="62"/>
        <source>There is not enough memory to align these sequences with MUSCLE.</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::MusclePlugin</name>
    <message>
        <location filename="../src/MusclePlugin.cpp" line="60"/>
        <source>MUSCLE</source>
        <translation>MUSCLE</translation>
    </message>
    <message>
        <location filename="../src/MusclePlugin.cpp" line="61"/>
        <source>A port of MUSCLE package for multiple sequence alignment. Check http://www.drive5.com/muscle/ for the original version</source>
        <translation>Порт пакета MUSCLE для выравнивания множественных последовательностей. ￼Сайт оригинального пакета http://www.drive5.com/muscle/</translation>
    </message>
    <message>
        <location filename="../src/MusclePlugin.cpp" line="69"/>
        <source>Align with MUSCLE...</source>
        <translation>Выравнивание с помощью MUSCLE...</translation>
    </message>
</context>
<context>
    <name>U2::MusclePrepareTask</name>
    <message>
        <location filename="../src/MuscleParallel.cpp" line="112"/>
        <source>Preparing MUSCLE alignment...</source>
        <translation>Preparing MUSCLE alignment...</translation>
    </message>
    <message>
        <location filename="../src/MuscleParallel.cpp" line="120"/>
        <source>Internal parallel MUSCLE error: %1</source>
        <translation>Внутренняя ошибка: %1</translation>
    </message>
    <message>
        <location filename="../src/MuscleParallel.cpp" line="129"/>
        <source>MUSCLE prepared successfully</source>
        <translation>MUSCLE prepared successfully</translation>
    </message>
    <message>
        <location filename="../src/MuscleParallel.cpp" line="166"/>
        <source>No sequences in input file</source>
        <translation>Выравнивание не содержит данных</translation>
    </message>
    <message>
        <location filename="../src/MuscleParallel.cpp" line="195"/>
        <source>Alignment is empty</source>
        <translation>Выравнивание не содержит данных</translation>
    </message>
</context>
<context>
    <name>U2::MuscleTask</name>
    <message>
        <location filename="../src/MuscleTask.cpp" line="75"/>
        <source>MUSCLE alignment</source>
        <translation>Выравнивание с помощью MUSCLE</translation>
    </message>
    <message>
        <location filename="../src/MuscleTask.cpp" line="83"/>
        <source>MUSCLE alignment started</source>
        <translation>MUSCLE alignment started</translation>
    </message>
    <message>
        <location filename="../src/MuscleTask.cpp" line="104"/>
        <source>Incorrect region to align</source>
        <translation>Incorrect region to align</translation>
    </message>
    <message>
        <location filename="../src/MuscleTask.cpp" line="106"/>
        <source>Stopping MUSCLE task, because of error in MAlignment::mid function</source>
        <translation>Stopping MUSCLE task, because of error in MAlignment::mid function</translation>
    </message>
    <message>
        <location filename="../src/MuscleTask.cpp" line="125"/>
        <source>Performing MUSCLE alignment...</source>
        <translation>Performing MUSCLE alignment...</translation>
    </message>
    <message>
        <location filename="../src/MuscleTask.cpp" line="148"/>
        <source>MUSCLE alignment successfully finished</source>
        <translation>MUSCLE alignment successfully finished</translation>
    </message>
    <message>
        <location filename="../src/MuscleTask.cpp" line="201"/>
        <source>Unexpected number of rows in the result multiple alignment!</source>
        <translation>Unexpected number of rows in the result multiple alignment!</translation>
    </message>
</context>
<context>
    <name>U2::ProgressiveAlignTask</name>
    <message>
        <location filename="../src/MuscleParallel.cpp" line="307"/>
        <source>ProgressiveAlignTask</source>
        <translation>ProgressiveAlignTask</translation>
    </message>
    <message>
        <location filename="../src/MuscleParallel.cpp" line="329"/>
        <source>Internal parallel MUSCLE error: %1</source>
        <translation>Внутренняя ошибка: %1</translation>
    </message>
    <message>
        <location filename="../src/MuscleParallel.cpp" line="339"/>
        <source>alignment &quot;%1&quot; Parallel MUSCLE Iter 1 accomplished. Time elapsed %2 ms</source>
        <translation>alignment &quot;%1&quot; Parallel MUSCLE Iter 1 accomplished. Time elapsed %2 ms</translation>
    </message>
</context>
<context>
    <name>U2::ProgressiveAlignWorker</name>
    <message>
        <location filename="../src/MuscleParallel.cpp" line="390"/>
        <source>ProgressiveAlignWorker</source>
        <translation>ProgressiveAlignWorker</translation>
    </message>
    <message>
        <location filename="../src/MuscleParallel.cpp" line="404"/>
        <source>Internal parallel MUSCLE error: %1</source>
        <translation>Внутренняя ошибка: %1</translation>
    </message>
</context>
<context>
    <name>U2::RefineTask</name>
    <message>
        <location filename="../src/MuscleParallel.cpp" line="574"/>
        <source>RefineTask</source>
        <translation>RefineTask</translation>
    </message>
    <message>
        <location filename="../src/MuscleParallel.cpp" line="597"/>
        <source>Internal parallel MUSCLE error: %1</source>
        <translation>Внутренняя ошибка: %1</translation>
    </message>
    <message>
        <location filename="../src/MuscleParallel.cpp" line="604"/>
        <source>Can&apos;t allocate enough memory to perform aligning, try to use 64bit UGENE version</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::RefineTreeTask</name>
    <message>
        <location filename="../src/MuscleParallel.cpp" line="524"/>
        <source>RefineTreeTask</source>
        <translation>RefineTreeTask</translation>
    </message>
    <message>
        <location filename="../src/MuscleParallel.cpp" line="536"/>
        <source>Internal parallel MUSCLE error: %1</source>
        <translation>Внутренняя ошибка: %1</translation>
    </message>
</context>
<context>
    <name>U2::RefineWorker</name>
    <message>
        <location filename="../src/MuscleParallel.cpp" line="655"/>
        <source>Internal parallel MUSCLE error: %1</source>
        <translation>Внутренняя ошибка: %1</translation>
    </message>
    <message>
        <location filename="../src/MuscleParallel.cpp" line="661"/>
        <source>Can&apos;t allocate enough memory to perform aligning, try to use 64bit UGENE version</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
