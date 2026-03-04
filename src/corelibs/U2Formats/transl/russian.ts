<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru">
<context>
    <name>EMBLGenbankAbstractDocument</name>
    <message>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="57"/>
        <source>The file contains features of another remote GenBank file. These features have been skipped.</source>
        <translation>Файл содержит аннотации другого GenBank файла. Эти аннотации проигнорированы.</translation>
    </message>
    <message>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="58"/>
        <source>The file contains joined annotations with regions, located on different strands. All such joined parts will be stored on the same strand.</source>
        <translation>Файл содержит аннотации присоединенные к регионам, расположенным на другой цепи. Все такие аннотации будут сохранены на одной цепи.</translation>
    </message>
    <message>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="59"/>
        <source>Location parsing error.</source>
        <translation>Ошибка разбора местоположения.</translation>
    </message>
    <message>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="60"/>
        <source>The number of valid sequence characters does not match with the declared size in the sequence header.</source>
        <translation>Число допустимых символов последовательности не совпадает с заявленным размером в заголовке последовательности.</translation>
    </message>
</context>
<context>
    <name>LocationParser</name>
    <message>
        <location filename="../src/GenbankLocationParser.cpp" line="625"/>
        <source>Ignoring remote entry</source>
        <translation>Игнорирование удаленной записи</translation>
    </message>
    <message>
        <location filename="../src/GenbankLocationParser.cpp" line="626"/>
        <source>Ignoring different strands in JOIN</source>
        <translation>Игнорирование разных цепей в операции JOIN</translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/BAMUtils.cpp" line="90"/>
        <source>Fail to open &quot;%1&quot; for reading</source>
        <translation>Невозможно открыть &quot;%1&quot; для чтения</translation>
    </message>
    <message>
        <location filename="../src/BAMUtils.cpp" line="94"/>
        <source>Fail to read the header from the file, probably, not an assembly format: &quot;%1&quot;</source>
        <translation>Не удалось считать заголовок из файла, вероятно, это не формат сборки: &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/BAMUtils.cpp" line="98"/>
        <source>Fail to write the header to the file: &quot;%1&quot;</source>
        <translation>Не удалось записать заголовок в файл: &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/BAMUtils.cpp" line="102"/>
        <source>Truncated file: &quot;%1&quot;</source>
        <translation>Обрезанный файл: &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/BAMUtils.cpp" line="707"/>
        <source>Can&apos;t open file with given url: %1.</source>
        <translation>Не удается открыть файл по указанному URL: %1.</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteBlobInputStream.cpp" line="96"/>
        <source>Can not read data. The database is closed or the data were changed.</source>
        <translation>Невозможно прочитать данные. База дынных закрыта или данные были изменены.</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteBlobOutputStream.cpp" line="49"/>
        <source>Can not write data. The database is closed or the data were changed.</source>
        <translation>Невозможно записать данные. База данных закрыта или данные были изменены.</translation>
    </message>
    <message>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="270"/>
        <location filename="../src/FastaFormat.cpp" line="305"/>
        <location filename="../src/FastqFormat.cpp" line="416"/>
        <source>Document sequences were merged</source>
        <translation>Последовательности были соединены</translation>
    </message>
    <message>
        <location filename="../src/PDWFormat.cpp" line="165"/>
        <location filename="../src/StockholmFormat.cpp" line="535"/>
        <source>The document is not created by UGENE</source>
        <translation>Документ создан не в UGENE</translation>
    </message>
</context>
<context>
    <name>U2::ABIFormat</name>
    <message>
        <location filename="../src/ABIFormat.cpp" line="48"/>
        <source>ABIF</source>
        <translation>ABIF</translation>
    </message>
    <message>
        <location filename="../src/ABIFormat.cpp" line="49"/>
        <source>A chromatogram file format</source>
        <translation>Формат типа хромотограмма</translation>
    </message>
    <message>
        <location filename="../src/ABIFormat.cpp" line="90"/>
        <source>Not a valid ABIF file: %1</source>
        <translation>Некорректный ABIF файл: %1</translation>
    </message>
    <message>
        <location filename="../src/ABIFormat.cpp" line="117"/>
        <source>Failed to load sequence from ABI file %1</source>
        <translation>Невозможно загрузить последоватлеьность из ABI файла %1</translation>
    </message>
    <message>
        <location filename="../src/ABIFormat.cpp" line="449"/>
        <source>Undefined sequence alphabet</source>
        <translation>Неизвестный алфавит</translation>
    </message>
</context>
<context>
    <name>U2::ACEFormat</name>
    <message>
        <location filename="../src/ace/AceFormat.cpp" line="52"/>
        <source>ACE</source>
        <translation>ACE</translation>
    </message>
    <message>
        <location filename="../src/ace/AceFormat.cpp" line="215"/>
        <source>Line is too long</source>
        <translation>Строка слишком длинная</translation>
    </message>
    <message>
        <location filename="../src/ace/AceFormat.cpp" line="214"/>
        <source>Unexpected end of file</source>
        <translation>Неожиданный конец файла</translation>
    </message>
    <message>
        <location filename="../src/ace/AceFormat.cpp" line="225"/>
        <location filename="../src/ace/AceFormat.cpp" line="266"/>
        <source>There is no AF note</source>
        <translation>Отсутствует</translation>
    </message>
    <message>
        <location filename="../src/ace/AceFormat.cpp" line="53"/>
        <source>ACE is a format used for storing information about genomic confgurations</source>
        <translation>ACE это формат используемый для хранения информации о геномных конфигурациях</translation>
    </message>
    <message>
        <location filename="../src/ace/AceFormat.cpp" line="231"/>
        <source>No consensus</source>
        <translation>Отсутствует консенсус</translation>
    </message>
    <message>
        <location filename="../src/ace/AceFormat.cpp" line="243"/>
        <source>BQ keyword hasn&apos;t been found</source>
        <translation>Не найдено ключевое слово BQ</translation>
    </message>
    <message>
        <location filename="../src/ace/AceFormat.cpp" line="246"/>
        <source>Bad consensus data</source>
        <translation>Плохие данные в консенсусе</translation>
    </message>
    <message>
        <location filename="../src/ace/AceFormat.cpp" line="273"/>
        <location filename="../src/ace/AceFormat.cpp" line="278"/>
        <source>Bad AF note</source>
        <translation>Плохое примечание AF</translation>
    </message>
    <message>
        <location filename="../src/ace/AceFormat.cpp" line="306"/>
        <source>There is no read note</source>
        <translation>Отсутствует</translation>
    </message>
    <message>
        <location filename="../src/ace/AceFormat.cpp" line="314"/>
        <source>No sequence</source>
        <translation>Нет последовательности</translation>
    </message>
    <message>
        <location filename="../src/ace/AceFormat.cpp" line="326"/>
        <source>QA keyword hasn&apos;t been found</source>
        <translation>Ключевое слово QA не было найдено</translation>
    </message>
    <message>
        <location filename="../src/ace/AceFormat.cpp" line="332"/>
        <location filename="../src/ace/AceFormat.cpp" line="335"/>
        <source>QA error no clear range</source>
        <translation>Ошибка QA нет четкого региона</translation>
    </message>
    <message>
        <location filename="../src/ace/AceFormat.cpp" line="339"/>
        <source>QA error bad range</source>
        <translation>Ошибка QA: плохой регион</translation>
    </message>
    <message>
        <location filename="../src/ace/AceFormat.cpp" line="344"/>
        <source>Bad sequence data</source>
        <translation>Некорректные данные в последовательности</translation>
    </message>
    <message>
        <location filename="../src/ace/AceFormat.cpp" line="406"/>
        <source>Alphabet unknown</source>
        <translation>Неизвестный алфавит</translation>
    </message>
    <message>
        <location filename="../src/ace/AceFormat.cpp" line="416"/>
        <source>Invalid source file</source>
        <translation>Входной файл невалиден</translation>
    </message>
    <message>
        <location filename="../src/ace/AceFormat.cpp" line="437"/>
        <source>File doesn&apos;t contain any msa objects</source>
        <translation>Файл не содержит msa объектов</translation>
    </message>
</context>
<context>
    <name>U2::ASNFormat</name>
    <message>
        <location filename="../src/ASNFormat.cpp" line="113"/>
        <source>MMDB</source>
        <translation>MMDB</translation>
    </message>
    <message>
        <location filename="../src/ASNFormat.h" line="148"/>
        <source>read error occurred</source>
        <translation>Ошибка чтения</translation>
    </message>
    <message>
        <location filename="../src/ASNFormat.h" line="160"/>
        <source>biostruct3d obj loading error: %1</source>
        <translation>Ошибка загрузки трехмерной структуры: %1</translation>
    </message>
    <message>
        <location filename="../src/ASNFormat.cpp" line="114"/>
        <source>ASN is a format used my the Molecular Modeling Database (MMDB)</source>
        <translation>ASN это формат используемый Molecular Modeling Database (MMDB)</translation>
    </message>
    <message>
        <location filename="../src/ASNFormat.cpp" line="210"/>
        <source>No &quot;name&quot; node found, possibly, the file is corrupted</source>
        <translation>Узел &quot;name&quot; не найден, вероятно, файл поврежден</translation>
    </message>
    <message>
        <location filename="../src/ASNFormat.cpp" line="261"/>
        <location filename="../src/ASNFormat.cpp" line="659"/>
        <source>Unknown error occurred</source>
        <translation>Неизвестная ошибка</translation>
    </message>
    <message>
        <location filename="../src/ASNFormat.cpp" line="645"/>
        <source>no root element</source>
        <translation>Отсутствует корневой элемент</translation>
    </message>
    <message>
        <location filename="../src/ASNFormat.cpp" line="651"/>
        <source>states stack is not empty</source>
        <translation>Стек состояний не пуст</translation>
    </message>
    <message>
        <location filename="../src/ASNFormat.cpp" line="674"/>
        <source>First line is too long</source>
        <translation>Первая строка слишком длинная</translation>
    </message>
    <message>
        <location filename="../src/ASNFormat.cpp" line="135"/>
        <source>Standard residue dictionary not found</source>
        <translation>Стандартный словарь остатков не найден</translation>
    </message>
</context>
<context>
    <name>U2::AbstractVariationFormat</name>
    <message>
        <location filename="../src/AbstractVariationFormat.cpp" line="61"/>
        <source>SNP formats are used to store single-nucleotide polymorphism data</source>
        <translation>SNP форматы используются для сохранения полиморфизма однонуклеотидных данных</translation>
    </message>
    <message>
        <location filename="../src/AbstractVariationFormat.cpp" line="118"/>
        <source>Line %1: There are too few columns in this line. The line was skipped.</source>
        <translation>Строка %1: Слишком мало столбцов в этой строке. Строка была проигнорирована.</translation>
    </message>
</context>
<context>
    <name>U2::AceImporter</name>
    <message>
        <location filename="../src/ace/AceImporter.cpp" line="149"/>
        <source>ACE file importer</source>
        <translation>Импорт ACE файла</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImporter.cpp" line="153"/>
        <source>ACE files importer is used to convert conventional ACE files into UGENE database format.Having ACE file converted into UGENE DB format you get an fast and efficient interface to your data with an option to change the content</source>
        <translation>Импорт ACE файлов используется для преобразования обычных ACE файлов в формат базы данных UGENE. Преобразовав ACE файл в формат UGENE DB вы получите быстрый и эффективный интерфейс к вашим данным с возможностью изменять содержимое файла</translation>
    </message>
</context>
<context>
    <name>U2::AceImporterTask</name>
    <message>
        <location filename="../src/ace/AceImporter.cpp" line="53"/>
        <source>ACE file import: %1</source>
        <translation>Импорт ACE файла: %1</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImporter.cpp" line="136"/>
        <source>Failed to get load task for : %1</source>
        <translation>Не удалось получить загрузку задачи для: %1</translation>
    </message>
</context>
<context>
    <name>U2::AprFormat</name>
    <message>
        <location filename="../src/apr/AprFormat.cpp" line="61"/>
        <source>Unexpected end of file</source>
        <translation>Неожиданный конец файла</translation>
    </message>
    <message>
        <location filename="../src/apr/AprFormat.cpp" line="71"/>
        <source>There is no sequences in alignment</source>
        <translation>Выравнивание не содержит последовательностей</translation>
    </message>
    <message>
        <location filename="../src/apr/AprFormat.cpp" line="98"/>
        <source>Attempt to find any number in the string failed</source>
        <translation>Попытка найти число в строке не удалась</translation>
    </message>
    <message>
        <location filename="../src/apr/AprFormat.cpp" line="150"/>
        <source>Vector NTI/AlignX</source>
        <translation>Vector NTI/AlignX</translation>
    </message>
    <message>
        <location filename="../src/apr/AprFormat.cpp" line="151"/>
        <source>Vector NTI/AlignX is a Vector NTI format for multiple alignment</source>
        <translation>Vector NTI/AlignX это Vector NTI формат для множественных выравниваний</translation>
    </message>
    <message>
        <location filename="../src/apr/AprFormat.cpp" line="172"/>
        <source>Open in read-only mode</source>
        <translation>Открыть только для чтения</translation>
    </message>
    <message>
        <location filename="../src/apr/AprFormat.cpp" line="182"/>
        <source>File doesn&apos;t contain any msa objects</source>
        <translation>Файл не содержит msa объектов</translation>
    </message>
    <message>
        <location filename="../src/apr/AprFormat.cpp" line="206"/>
        <source>Illegal header line</source>
        <translation>Неправильная строка заголовка</translation>
    </message>
    <message>
        <location filename="../src/apr/AprFormat.cpp" line="221"/>
        <source>Sequences not found</source>
        <translation>Последовательности не найдены</translation>
    </message>
    <message>
        <location filename="../src/apr/AprFormat.cpp" line="227"/>
        <source>Alphabet is unknown</source>
        <translation>Неизвестный алфавит</translation>
    </message>
</context>
<context>
    <name>U2::AprImporter</name>
    <message>
        <location filename="../src/apr/AprImporter.cpp" line="116"/>
        <source>Vector NTI/AlignX file importer</source>
        <translation>Инструмент для импорта Vector NTI/AlignX файлов</translation>
    </message>
    <message>
        <location filename="../src/apr/AprImporter.cpp" line="120"/>
        <source>Vector NTI/AlignX files importer is used to convert conventional APR files to a multiple sequence alignment formats</source>
        <translation>Инструмент для импорта Vector NTI/AlignX файлов используется для конвертации APR файлов в формат множественного выравнивания</translation>
    </message>
    <message>
        <location filename="../src/apr/AprImporter.cpp" line="135"/>
        <source>Convert to another format:</source>
        <translation>Конвертировать в другой формат:</translation>
    </message>
</context>
<context>
    <name>U2::AprImporterTask</name>
    <message>
        <location filename="../src/apr/AprImporter.cpp" line="54"/>
        <source>APR file import: %1</source>
        <translation>Импорт файла APR: %1</translation>
    </message>
</context>
<context>
    <name>U2::BAMUtils</name>
    <message>
        <location filename="../src/BAMUtils.cpp" line="170"/>
        <source>Wrong line in a SAM file: &quot;%1&quot;. Skipped</source>
        <translation>Неверная строка в SAM файле: &quot;%1&quot;. Пропущена</translation>
    </message>
    <message>
        <location filename="../src/BAMUtils.cpp" line="177"/>
        <source>Wrong left base position format: &quot;%1&quot;. Line has been skipped.</source>
        <translation>Неверный формат позиции левого символа: &quot;%1&quot;. Строка была пропущена.</translation>
    </message>
    <message>
        <location filename="../src/BAMUtils.cpp" line="240"/>
        <source>There is no header in the SAM file &quot;%1&quot;. The header information will be generated automatically.</source>
        <translation>Нет заголовка в SAM файле &quot;%1&quot;. Необходимая информация будет сгенерирована автоматически.</translation>
    </message>
    <message>
        <location filename="../src/BAMUtils.cpp" line="244"/>
        <source>No reference data in the file: %1</source>
        <translation>Файл не содержит референсных данныз: %1</translation>
    </message>
    <message>
        <location filename="../src/BAMUtils.cpp" line="378"/>
        <source>Sorting &quot;%1&quot; and saving the result to &quot;%2&quot;</source>
        <translation>Сортировка &quot;%1&quot; и сохранение результата в &quot;%2&quot;</translation>
    </message>
    <message>
        <location filename="../src/BAMUtils.cpp" line="382"/>
        <source>Cannot sort &quot;%1&quot;, abort</source>
        <translation>Невозможно провести сортировку &quot;%1&quot;, отмена</translation>
    </message>
    <message>
        <location filename="../src/BAMUtils.cpp" line="388"/>
        <source>Merging BAM files: &quot;%1&quot;. Resulting merged file is: &quot;%2&quot;</source>
        <translation>Слияние BAM файлов: &quot;%1&quot;. Соединенный файл: &quot;%2&quot;</translation>
    </message>
    <message>
        <location filename="../src/BAMUtils.cpp" line="401"/>
        <source>Failed to merge BAM files: %1 into %2</source>
        <translation>Не удалось объединить файлы BAM: %1 в %2</translation>
    </message>
    <message>
        <location filename="../src/BAMUtils.cpp" line="437"/>
        <source>Build index for bam file: &quot;%1&quot;</source>
        <translation>Построение индекса для bam файла: &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/BAMUtils.cpp" line="439"/>
        <source>Can&apos;t build the index: %1</source>
        <translation>Невозможно посмтроить индекс: %1</translation>
    </message>
</context>
<context>
    <name>U2::BedFormat</name>
    <message>
        <location filename="../src/BedFormat.cpp" line="100"/>
        <source>The BED (Browser Extensible Data) format was developed by UCSC for displaying transcript structures in the genome browser.</source>
        <translation>BED (Browser Extensible Data) формат был разработан UCSC для отображения транскриптных структур в геномном браузере.</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="136"/>
        <source>File &quot;%1&quot; contains too many annotation tables to be displayed. However, you can process these data using pipelines built with Workflow Designer.</source>
        <translation>Файл &quot;%1&quot; содержит слишком много таблиц аннотаций для отображения. Однако вы можете обработать эти данные, используя конвейеры, созданные с помощью Конструктора рабочего процесса (Workflow Designer).</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="864"/>
        <source>BED parsing error: incorrect format of the &apos;track&apos; header line!</source>
        <translation>Ошибка разбора формата BED: неверный формат строки заголовка &apos;track&apos;!</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="656"/>
        <source>BED parsing error: unexpected number of fields in the first annotations line!</source>
        <translation>Ошибка разбора формата BED: неожиданное количество полей в первой строке аннотаций!</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="99"/>
        <source>BED</source>
        <translation>BED</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="697"/>
        <source>The file does not contain valid annotations!</source>
        <translation>Файл не содержит корректных аннотаций!</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="933"/>
        <source>BED parsing error: incorrect number of fields at line %1!</source>
        <translation>Ошибка разбора формата BED: неверное количество полей в строке %1!</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="937"/>
        <source>BED parsing error: a field at line %1 is empty!</source>
        <translation>Ошибка разбора формата BED: поле в строке %1 пустое!</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="941"/>
        <source>BED parsing error: incorrect coordinates at line %1!</source>
        <translation>Ошибка разбора формата BED: неверные координаты в строке %1!</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="945"/>
        <source>BED parsing error: incorrect score value &apos;%1&apos; at line %2!</source>
        <translation>Ошибка разбора формата BED: неверное значение балла &apos;%1&apos; в строке %2!</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="952"/>
        <source>BED parsing error: incorrect strand value &apos;%1&apos; at line %2!</source>
        <translation>Ошибка разбора формата BED: неверное значение направления &apos;%1&apos; в строке %2!</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="959"/>
        <source>BED parsing error: incorrect thick coordinates at line %1!</source>
        <translation>Ошибка разбора формата BED: неверные координаты толщины в строке %1!</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="963"/>
        <source>BED parsing error: incorrect itemRgb value &apos;%1&apos; at line %2!</source>
        <translation>Ошибка разбора формата BED: неверное значение itemRgb &apos;%1&apos; в строке %2!</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="970"/>
        <source>BED parsing error: incorrect value of the block parameters at line %1!</source>
        <translation>Ошибка разбора формата BED: неверное значение параметров блока в строке %1!</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="403"/>
        <source>Starting BED saving: &apos;%1&apos;</source>
        <translation>Сохранение BED: &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="428"/>
        <source>Can not detect chromosome name. &apos;Chr&apos; name will be used.</source>
        <translation>Невозможно определить имя хромосомы. Будет использовано имя &apos;Chr&apos;.</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="444"/>
        <source>You are trying to save joined annotation to BED format! The joining will be lost</source>
        <translation>Вы пытаетесь сохранить связанные аннотации в BED формат! Соединения будут потеряны</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="507"/>
        <source>BED saving error: incorrect thick coordinates in the first annotation!</source>
        <translation>Ошибка сохранения BED: неверные координаты толщины в первой аннотации!</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="521"/>
        <source>BED saving error: incorrect block fields in the first annotation!</source>
        <translation>Ошибка сохранения BED: неверные поля блока в первой аннотации!</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="529"/>
        <source>BED saving: detected %1 fields per line for file &apos;%2&apos;</source>
        <translation>Сохранение в формате BED: обнаружено %1 полей в строке для файла &apos;%2&apos;</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="538"/>
        <source>BED saving error: an annotation is expected to have &apos;%1&apos; qualifier, but it is absent! Skipping the annotation.</source>
        <translation>Ошибка сохранения в формате BED: ожидалось наличие &apos;%1&apos; квалификатора для аннотации, но его нет! Аннотация будет пропущена.</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="592"/>
        <source>BED saving error: an annotation is expected to have the block qualifiers! Skipping the annotation.</source>
        <translation>Ошибка сохранения в формате BED: ожидалось, что аннотация будет иметь квалификаторы блока! Аннотация будет пропущена.</translation>
    </message>
    <message>
        <location filename="../src/BedFormat.cpp" line="614"/>
        <source>Finished BED saving: &apos;%1&apos;</source>
        <translation>Сохранение BED закончено: &apos;%1&apos;</translation>
    </message>
</context>
<context>
    <name>U2::BgzipTask</name>
    <message>
        <location filename="../src/tasks/BgzipTask.cpp" line="52"/>
        <source>Bgzip Compression task</source>
        <translation>Задача по сжатию с использованием Bgzip</translation>
    </message>
    <message>
        <location filename="../src/tasks/BgzipTask.cpp" line="58"/>
        <source>Start bgzip compression &apos;%1&apos;</source>
        <translation>Начало сжатия bgzip &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/tasks/BgzipTask.cpp" line="69"/>
        <source>Can not open input file &apos;%1&apos;</source>
        <translation>Не удается открыть входной файл &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/tasks/BgzipTask.cpp" line="78"/>
        <source>Can not open output file &apos;%2&apos;</source>
        <translation>Невозможно открыть выходной файл &apos;%2&apos;</translation>
    </message>
    <message>
        <location filename="../src/tasks/BgzipTask.cpp" line="93"/>
        <source>Error reading file</source>
        <translation>Ошибка чтения файла</translation>
    </message>
    <message>
        <location filename="../src/tasks/BgzipTask.cpp" line="98"/>
        <source>Error writing to file</source>
        <translation>Ошибка записи в файл</translation>
    </message>
    <message>
        <location filename="../src/tasks/BgzipTask.cpp" line="105"/>
        <source>Bgzip compression finished</source>
        <translation>Сжатие bgzip завершено</translation>
    </message>
    <message>
        <location filename="../src/tasks/BgzipTask.cpp" line="110"/>
        <source>Bgzip compression task was finished with an error: %1</source>
        <translation>Сжатие bgzip закончилось с ошибкой: %1</translation>
    </message>
    <message>
        <location filename="../src/tasks/BgzipTask.cpp" line="112"/>
        <source>Bgzip compression task was finished. A new bgzf file is: &lt;a href=&quot;%1&quot;&gt;%2&lt;/a&gt;</source>
        <translation>Сжатие bgzip завершено. Новый bgzf файл: &lt;a href=&quot;%1&quot;&gt;%2&lt;/a&gt;</translation>
    </message>
</context>
<context>
    <name>U2::CalculateSequencesNumberTask</name>
    <message>
        <location filename="../src/tasks/CalculateSequencesNumberTask.cpp" line="29"/>
        <source>Calculate sequences number</source>
        <translation>Рассчитать количество последовательностей</translation>
    </message>
</context>
<context>
    <name>U2::CloneAssemblyWithReferenceToDbiTask</name>
    <message>
        <location filename="../src/ace/CloneAssemblyWithReferenceToDbiTask.cpp" line="43"/>
        <source>Clone assembly object to the destination database</source>
        <translation>Клонирование объекта сборки в базу данных</translation>
    </message>
</context>
<context>
    <name>U2::ClustalWAlnFormat</name>
    <message>
        <location filename="../src/ClustalWAlnFormat.cpp" line="71"/>
        <source>Clustalw is a format for storing multiple sequence alignments</source>
        <translation>Clustalw это формат для сохранения множественных выравниваний</translation>
    </message>
    <message>
        <location filename="../src/ClustalWAlnFormat.cpp" line="99"/>
        <source>Illegal header line</source>
        <translation>Неправильная строка заголовка</translation>
    </message>
    <message>
        <location filename="../src/ClustalWAlnFormat.cpp" line="123"/>
        <source>Error parsing file</source>
        <translation>Ошибка разбора файла</translation>
    </message>
    <message>
        <location filename="../src/ClustalWAlnFormat.cpp" line="134"/>
        <source>Invalid alignment format</source>
        <translation>Неверный формат выравнивания</translation>
    </message>
    <message>
        <location filename="../src/ClustalWAlnFormat.cpp" line="172"/>
        <source>Incorrect number of sequences in block</source>
        <translation>Неверное количество последовательностей в блоке</translation>
    </message>
    <message>
        <location filename="../src/ClustalWAlnFormat.cpp" line="178"/>
        <source>Sequence names are not matched: &apos;%1&apos; vs &apos;%2&apos;, row index: %3</source>
        <translation>Имена последовательностей не совпадают:&apos;%1&apos; и &apos;%2&apos;, индекс: %3</translation>
    </message>
    <message>
        <location filename="../src/ClustalWAlnFormat.cpp" line="308"/>
        <source>No data to write</source>
        <translation>Нет данных для записи</translation>
    </message>
    <message>
        <location filename="../src/ClustalWAlnFormat.cpp" line="308"/>
        <source>Too many objects: %1</source>
        <translation>Слишком много объектов: %1</translation>
    </message>
    <message>
        <location filename="../src/ClustalWAlnFormat.cpp" line="311"/>
        <source>Not a multiple alignment object</source>
        <translation>Объект не является выравниванием</translation>
    </message>
    <message>
        <location filename="../src/ClustalWAlnFormat.cpp" line="201"/>
        <source>Alphabet is unknown</source>
        <translation>Неизвестный алфавит</translation>
    </message>
    <message>
        <location filename="../src/ClustalWAlnFormat.cpp" line="70"/>
        <source>CLUSTALW</source>
        <translation>CLUSTALW</translation>
    </message>
</context>
<context>
    <name>U2::ConvertAceToSqliteTask</name>
    <message>
        <location filename="../src/ace/ConvertAceToSqliteTask.cpp" line="47"/>
        <source>Convert ACE to UGENE database (%1)</source>
        <translation>Преобразование ACE в UGENE database формат(%1)</translation>
    </message>
    <message>
        <location filename="../src/ace/ConvertAceToSqliteTask.cpp" line="58"/>
        <source>Converting assembly from %1 to %2 started</source>
        <translation>Конвертация сборки из %1 в %2 начата</translation>
    </message>
    <message>
        <location filename="../src/ace/ConvertAceToSqliteTask.cpp" line="70"/>
        <source>Can&apos;t open file &apos;%1&apos;</source>
        <translation>Невозможно открыть файл &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/ace/ConvertAceToSqliteTask.cpp" line="88"/>
        <source>Importing</source>
        <translation>Импорт</translation>
    </message>
    <message>
        <location filename="../src/ace/ConvertAceToSqliteTask.cpp" line="142"/>
        <source>There is no assemblies in input file</source>
        <translation>В файле нет сборок</translation>
    </message>
    <message>
        <location filename="../src/ace/ConvertAceToSqliteTask.cpp" line="156"/>
        <location filename="../src/ace/ConvertAceToSqliteTask.cpp" line="199"/>
        <source>Invalid source file</source>
        <translation>Invalid source file</translation>
    </message>
    <message>
        <location filename="../src/ace/ConvertAceToSqliteTask.cpp" line="220"/>
        <source>Packing reads for assembly &apos;%1&apos; (%2 of %3)</source>
        <translation>Упаковка ридов для сборки &apos;%1&apos; (%2 от %3)</translation>
    </message>
    <message>
        <location filename="../src/ace/ConvertAceToSqliteTask.cpp" line="275"/>
        <source>Warning: incorrect maxProw == %1, probably packing was not done! Attribute was not set</source>
        <translation>Warning: incorrect maxProw == %1, probably packing was not done! Attribute was not set</translation>
    </message>
</context>
<context>
    <name>U2::ConvertAssemblyToSamTask</name>
    <message>
        <location filename="../src/tasks/ConvertAssemblyToSamTask.cpp" line="89"/>
        <source>Given file is not valid UGENE database file</source>
        <translation>Данный файл не является корректным файлом формата базы данных UGENE</translation>
    </message>
</context>
<context>
    <name>U2::ConvertSnpeffVariationsToAnnotationsTask</name>
    <message>
        <location filename="../src/tasks/ConvertSnpeffVariationsToAnnotationsTask.cpp" line="50"/>
        <source>Convert SnpEff variations to annotations task</source>
        <translation>Convert SnpEff variations to annotations task</translation>
    </message>
</context>
<context>
    <name>U2::DNAQualityIOUtils</name>
    <message>
        <location filename="../src/DNAQualityIOUtils.cpp" line="61"/>
        <source>No IO adapter found for URL: %1</source>
        <translation>No IO adapter found for URL: %1</translation>
    </message>
</context>
<context>
    <name>U2::Database</name>
    <message>
        <location filename="../src/Database.cpp" line="46"/>
        <source>Not a valid S3-database file: %1</source>
        <translation>Not a valid S3-database file: %1</translation>
    </message>
    <message>
        <location filename="../src/Database.cpp" line="56"/>
        <source>File already exists: %1</source>
        <translation>Файл уже существует: %1</translation>
    </message>
</context>
<context>
    <name>U2::DefaultConvertFileTask</name>
    <message>
        <location filename="../src/tasks/ConvertFileTask.cpp" line="125"/>
        <source>The formats are not compatible: %1 and %2</source>
        <translation>Форматы не совместимы: %1 и %2</translation>
    </message>
</context>
<context>
    <name>U2::DifferentialFormat</name>
    <message>
        <location filename="../src/DifferentialFormat.cpp" line="44"/>
        <source>Differential</source>
        <translation>Дифференциальный</translation>
    </message>
    <message>
        <location filename="../src/DifferentialFormat.cpp" line="46"/>
        <source>Differential format is a text-based format for representing Cuffdiff differential output files: expression, splicing, promoters and cds.</source>
        <translation>Дифференциальный формат это текстовый формат для представления выходных файлов Cuffdiff.</translation>
    </message>
    <message>
        <location filename="../src/DifferentialFormat.cpp" line="103"/>
        <source>Can not parse locus string: %1</source>
        <translation>Can not parse locus string: %1</translation>
    </message>
    <message>
        <location filename="../src/DifferentialFormat.cpp" line="200"/>
        <source>Annotation has not regions</source>
        <translation>У аннотации нет регионов</translation>
    </message>
    <message>
        <location filename="../src/DifferentialFormat.cpp" line="204"/>
        <source>Annotation has more than one region</source>
        <translation>У аннотации более одного региона</translation>
    </message>
    <message>
        <location filename="../src/DifferentialFormat.cpp" line="224"/>
        <source>Required value is missed: %1</source>
        <translation>Не задан необходимый параметр: %1</translation>
    </message>
    <message>
        <location filename="../src/DifferentialFormat.cpp" line="238"/>
        <source>Annotation object not found</source>
        <translation>Аннотация не найдена</translation>
    </message>
    <message>
        <location filename="../src/DifferentialFormat.cpp" line="261"/>
        <source>Required column is missed: %1</source>
        <translation>Пропущен необходимый столбец: %1</translation>
    </message>
</context>
<context>
    <name>U2::Document</name>
    <message>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="264"/>
        <location filename="../src/FastaFormat.cpp" line="277"/>
        <location filename="../src/FastqFormat.cpp" line="402"/>
        <location filename="../src/PDWFormat.cpp" line="152"/>
        <source>Document is empty.</source>
        <translation>Документ не содержит данных.</translation>
    </message>
</context>
<context>
    <name>U2::DocumentFormatUtils</name>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="156"/>
        <source>First line is not an ace header</source>
        <translation>Первая строка не является заголовком ACE</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="173"/>
        <source>There are not enough assemblies</source>
        <translation>Недостаточно сборок</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="193"/>
        <source>Expected %1 reads, but only %2 AF tags found</source>
        <translation>Ожидается %1 прочтений, но только %2 тэгов AF было найдено</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="232"/>
        <location filename="../src/ace/AceImportUtils.cpp" line="482"/>
        <source>Unexpected end of file</source>
        <translation>Неожиданный конец файла</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="236"/>
        <source>Line is too long</source>
        <translation>Строка слишком длинная</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="245"/>
        <source>No contig count tag in the header line</source>
        <translation>Отсутствует тег числа контигов в строке заголовка</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="257"/>
        <source>Not enough parameters in current line</source>
        <translation>Недостаточно параметров в текущей строке</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="268"/>
        <source>Parameter is not a digit</source>
        <translation>Параметр не является цифрой</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="276"/>
        <source>There is no note about reads count</source>
        <translation>Отсуствтует информация о числе считываний</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="291"/>
        <source>No consensus</source>
        <translation>Отсутствует консенсус</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="301"/>
        <source>BQ keyword hasn&apos;t been found</source>
        <translation>Не найдено ключевое слово BQ</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="304"/>
        <source>Unexpected symbols in consensus data</source>
        <translation>Неоижданные символы в консенсусе</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="315"/>
        <source>Can&apos;t find a sequence name in current line</source>
        <translation>Невозможно найти имя последовательности в текущей строке</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="325"/>
        <source>An empty sequence name</source>
        <translation>Не указано имя последовательности</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="395"/>
        <source>Not all reads were found</source>
        <translation>Не все риды были найдены</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="404"/>
        <location filename="../src/ace/AceImportUtils.cpp" line="408"/>
        <location filename="../src/ace/AceImportUtils.cpp" line="440"/>
        <location filename="../src/ace/AceImportUtils.cpp" line="452"/>
        <source>Bad AF note</source>
        <translation>Плохое примечание AF</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="478"/>
        <source>There is no read note</source>
        <translation>Отсутствует</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="491"/>
        <source>Invalid RD part</source>
        <translation>Неверная часть RD</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="503"/>
        <source>QA keyword hasn&apos;t been found</source>
        <translation>Ключевое слово QA не было найдено</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="512"/>
        <source>QA error bad range</source>
        <translation>Ошибка QA: плохой регион</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="515"/>
        <source>Unexpected symbols in sequence data</source>
        <translation>Неоижданные символы в последовательности</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="495"/>
        <source>A name is not match with AF names</source>
        <translation>Имя не совпадает с именами AF</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="521"/>
        <source>Can&apos;t find clear range start in current line</source>
        <translation>Невозможно найти начало диапазона в текущей строке</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="522"/>
        <source>Clear range start is invalid</source>
        <translation>Неверное начало диапазона</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="529"/>
        <source>Can&apos;t find clear range end in current line</source>
        <translation>Невозможно найти конец диапазона в текущей строке</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="530"/>
        <source>Clear range end is invalid</source>
        <translation>Неверный конец диапазона</translation>
    </message>
    <message>
        <location filename="../src/ace/AceImportUtils.cpp" line="554"/>
        <source>There is no next element</source>
        <translation>Отсутствует следующий элемент</translation>
    </message>
    <message>
        <location filename="../src/tasks/ConvertFileTask.cpp" line="56"/>
        <source>Conversion file from %1 to %2</source>
        <translation>Преобразование файла %1 в %2</translation>
    </message>
    <message>
        <location filename="../src/tasks/MergeBamTask.cpp" line="45"/>
        <source>Merge BAM files with SAMTools merge</source>
        <translation>Слияние BAM файлов с SAMTools</translation>
    </message>
</context>
<context>
    <name>U2::EMBLGenbankAbstractDocument</name>
    <message>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="574"/>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="380"/>
        <source>Annotation name is empty</source>
        <translation>Не указано имя аннотации</translation>
    </message>
    <message>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="585"/>
        <source>Error parsing location</source>
        <translation>Не указан регион аннотации</translation>
    </message>
    <message>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="681"/>
        <source>Error parsing sequence: unexpected empty line</source>
        <translation>Ошибка чтения последовательности: пустая строка</translation>
    </message>
    <message>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="613"/>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="423"/>
        <source>Unexpected line format</source>
        <translation>Слишком длинная строка или неожиданный конец файла</translation>
    </message>
    <message>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="126"/>
        <source>File &quot;%1&quot; contains too many sequences to be displayed. However, you can process these data using instruments from the menu &lt;i&gt;Tools -&gt; NGS data analysis&lt;/i&gt; or pipelines built with Workflow Designer.</source>
        <translation>File &quot;%1&quot; contains too many sequences to be displayed. However, you can process these data using instruments from the menu &lt;i&gt;Tools -&gt; NGS data analysis&lt;/i&gt; or pipelines built with Workflow Designer.</translation>
    </message>
    <message>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="143"/>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="313"/>
        <source>Reading entry header</source>
        <translation>Чтение заголовка</translation>
    </message>
    <message>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="221"/>
        <source>Merge error: found annotations without sequence</source>
        <translation>Ошибка слияния: обнаружена таблица аннотаций без соотв последовательности</translation>
    </message>
    <message>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="469"/>
        <source>The file contains an incorrect data that describes a qualifier value. </source>
        <translation>Файл содержит некорректные данные, которые описываются value. </translation>
    </message>
    <message>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="470"/>
        <source>The value cannot contain a single quote character. The qualifier is &apos;%1&apos;</source>
        <translation>Значение не может содержать одиночные кавычки. Квалификатор &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="671"/>
        <source>Reading sequence %1</source>
        <translation>Чтение последовательности: %1</translation>
    </message>
    <message>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="696"/>
        <source>Sequence is truncated</source>
        <translation>Последовательность повреждена</translation>
    </message>
    <message>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="702"/>
        <source>Reading annotations %1</source>
        <translation>Чтение аннотаций: %1</translation>
    </message>
    <message>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="712"/>
        <source>Invalid format of feature table</source>
        <translation>Таблица аннотаций повреждена</translation>
    </message>
    <message>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="809"/>
        <source>Line is too long.</source>
        <translation>Слишком длинная строка.</translation>
    </message>
    <message>
        <location filename="../src/EMBLGenbankAbstractDocument.cpp" line="811"/>
        <source>IO error.</source>
        <translation>Ошибка чтения.</translation>
    </message>
    <message>
        <location filename="../src/EMBLPlainTextFormat.cpp" line="228"/>
        <location filename="../src/GenbankPlainTextFormat.cpp" line="265"/>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="229"/>
        <source>Record is truncated.</source>
        <translation>Данные повреждены.</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="387"/>
        <source>Annotation start position is empty</source>
        <translation>Не указано начало аннотации</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="392"/>
        <source>Annotation end position is empty</source>
        <translation>Не указан конец аннотации</translation>
    </message>
</context>
<context>
    <name>U2::EMBLPlainTextFormat</name>
    <message>
        <location filename="../src/EMBLPlainTextFormat.cpp" line="43"/>
        <source>EMBL</source>
        <translation>EMBL</translation>
    </message>
    <message>
        <location filename="../src/EMBLPlainTextFormat.cpp" line="47"/>
        <source>EMBL Flat File Format is a rich format for storing sequences and associated annotations</source>
        <translation>EMBL Flat File Format это формат для хранения последовательностей и их аннотаций</translation>
    </message>
    <message>
        <location filename="../src/EMBLPlainTextFormat.cpp" line="84"/>
        <source>ID is not the first line</source>
        <translation>Строка идентификатора должна идти первой</translation>
    </message>
    <message>
        <location filename="../src/EMBLPlainTextFormat.cpp" line="91"/>
        <source>Error parsing ID line</source>
        <translation>Неверный заголовок</translation>
    </message>
</context>
<context>
    <name>U2::ExportAlignmentTask</name>
    <message>
        <location filename="../src/tasks/ExportTasks.cpp" line="48"/>
        <source>Export alignment to %1</source>
        <translation>Экспортировать выравнивание в %1</translation>
    </message>
    <message>
        <location filename="../src/tasks/ExportTasks.cpp" line="52"/>
        <source>Nothing to export: multiple alignment is empty</source>
        <translation>Нечего экспортировать: выравнивание пустое</translation>
    </message>
</context>
<context>
    <name>U2::ExportDNAChromatogramTask</name>
    <message>
        <location filename="../src/tasks/ExportTasks.cpp" line="200"/>
        <source>Export chromatogram to SCF</source>
        <translation>Экспортировать хроматограмму а SCF</translation>
    </message>
</context>
<context>
    <name>U2::ExportMSA2MSATask</name>
    <message>
        <location filename="../src/tasks/ExportTasks.cpp" line="133"/>
        <source>Export alignment as alignment to %1</source>
        <translation>Экспортировать выравнивание в выравнивание в %1</translation>
    </message>
    <message>
        <location filename="../src/tasks/ExportTasks.cpp" line="139"/>
        <source>Nothing to export: multiple alignment is empty</source>
        <translation>Нечего экспортировать: выравнивание пустое</translation>
    </message>
</context>
<context>
    <name>U2::ExportMSA2SequencesTask</name>
    <message>
        <location filename="../src/tasks/ExportTasks.cpp" line="83"/>
        <source>Export alignment as sequence to %1</source>
        <translation>Экспортировать выравнивание в последовательность в %1</translation>
    </message>
</context>
<context>
    <name>U2::FastaFormat</name>
    <message>
        <location filename="../src/FastaFormat.cpp" line="53"/>
        <source>FASTA format is a text-based format for representing either nucleotide sequences or peptide sequences, in which base pairs or amino acids are represented using single-letter codes. The format also allows for sequence names and comments to precede the sequences.</source>
        <translation>Формат FASTA является текстовым форматом для представления нуклеотидных или пептидных последовательностей, в котором пары оснований или аминокислоты представлены с использованием одно-буквенных кодов. Формат также позволяет задавать имена и комментарии к последовательностям.</translation>
    </message>
    <message>
        <location filename="../src/FastaFormat.cpp" line="145"/>
        <source>First line is not a FASTA header</source>
        <translation>Неправильный заголовок FASTA</translation>
    </message>
    <message>
        <location filename="../src/FastaFormat.cpp" line="241"/>
        <source>File &quot;%1&quot; contains too many sequences to be displayed. However, you can process these data using instruments from the menu &lt;i&gt;Tools -&gt; NGS data analysis&lt;/i&gt; or pipelines built with Workflow Designer.</source>
        <translation>File &quot;%1&quot; contains too many sequences to be displayed. However, you can process these data using instruments from the menu &lt;i&gt;Tools -&gt; NGS data analysis&lt;/i&gt; or pipelines built with Workflow Designer.</translation>
    </message>
    <message>
        <location filename="../src/FastaFormat.cpp" line="272"/>
        <source>The file format is invalid.</source>
        <translation>The file format is invalid.</translation>
    </message>
    <message>
        <location filename="../src/FastaFormat.cpp" line="280"/>
        <source>FASTA header is found in the file, but no sequence data are provided.</source>
        <translation>FASTA заголовок найден файле, но последовательность отсутствует.</translation>
    </message>
    <message>
        <location filename="../src/FastaFormat.cpp" line="283"/>
        <source>Loaded sequences: %1. 
</source>
        <translation>Loaded sequences: %1. 
</translation>
    </message>
    <message>
        <location filename="../src/FastaFormat.cpp" line="284"/>
        <source>Skipped sequences: %1. 
</source>
        <translation>Skipped sequences: %1. 
</translation>
    </message>
    <message>
        <location filename="../src/FastaFormat.cpp" line="285"/>
        <source>The following sequences are empty: 
%1</source>
        <translation>The following sequences are empty: 
%1</translation>
    </message>
    <message>
        <location filename="../src/FastaFormat.cpp" line="300"/>
        <source>Too many sequences (%1) to add to annotation table</source>
        <translation>Слишком много последовательностей (%1) для добавления в таблицу аннотаций</translation>
    </message>
    <message>
        <location filename="../src/FastaFormat.cpp" line="472"/>
        <source>Unreferenced sequence in the beginning of patterns: %1</source>
        <translation>Неиспользуемая последовательность в начале образцов: %1</translation>
    </message>
    <message>
        <location filename="../src/FastaFormat.cpp" line="50"/>
        <source>FASTA</source>
        <translation>FASTA</translation>
    </message>
</context>
<context>
    <name>U2::FastqFormat</name>
    <message>
        <location filename="../src/FastqFormat.cpp" line="57"/>
        <source>FASTQ</source>
        <translation>FASTQ</translation>
    </message>
    <message>
        <location filename="../src/FastqFormat.cpp" line="58"/>
        <source>FASTQ format is a text-based format for storing both a biological sequence (usually nucleotide sequence) and its corresponding quality scores.         Both the sequence letter and quality score are encoded with a single ASCII character for brevity.         It was originally developed at the Wellcome Trust Sanger Institute to bundle a FASTA sequence and its quality data,         but has recently become the de facto standard for storing the output of high throughput sequencing instruments.</source>
        <translation>Формат FASTQ является текстовым форматом для хранения биологических последовательностей (обычно нуклеотидных) и соответствующих им показателей качества. Последовательность и показатель качества кодируются при помощи одного символа ASCII для краткости. Изначально он был разработан в Wellcome Trust Sanger Institute для связи последовательности в формате FASTA и их данных качества, но в последнее время стал стандартом  для хранения выходных данных инструментов секвенирования.</translation>
    </message>
    <message>
        <location filename="../src/FastqFormat.cpp" line="149"/>
        <location filename="../src/FastqFormat.cpp" line="156"/>
        <source>Error while trying to find sequence name start</source>
        <translation>Не удалось найти начало имени последовательности</translation>
    </message>
    <message>
        <location filename="../src/FastqFormat.cpp" line="176"/>
        <location filename="../src/FastqFormat.cpp" line="208"/>
        <source>Error while reading sequence</source>
        <translation>Ошибка чтения последовательности</translation>
    </message>
    <message>
        <location filename="../src/FastqFormat.cpp" line="340"/>
        <source>Sequence name differs from quality scores name: %1 and %2</source>
        <translation>Sequence name differs from quality scores name: %1 and %2</translation>
    </message>
    <message>
        <location filename="../src/FastqFormat.cpp" line="359"/>
        <source>Bad quality scores: inconsistent size.</source>
        <translation>Bad quality scores: inconsistent size.</translation>
    </message>
    <message>
        <location filename="../src/FastqFormat.cpp" line="374"/>
        <source>File &quot;%1&quot; contains too many sequences to be displayed. However, you can process these data using instruments from the menu &lt;i&gt;Tools -&gt; NGS data analysis&lt;/i&gt; or pipelines built with Workflow Designer.</source>
        <translation>File &quot;%1&quot; contains too many sequences to be displayed. However, you can process these data using instruments from the menu &lt;i&gt;Tools -&gt; NGS data analysis&lt;/i&gt; or pipelines built with Workflow Designer.</translation>
    </message>
    <message>
        <location filename="../src/FastqFormat.cpp" line="569"/>
        <source>Not a valid FASTQ file, sequence name differs from quality scores name</source>
        <translation>Некорректный FASTQ файл, имя последовательности отличается от имени показателей качества</translation>
    </message>
    <message>
        <location filename="../src/FastqFormat.cpp" line="577"/>
        <source>Not a valid FASTQ file. Bad quality scores: inconsistent size.</source>
        <translation>Некорректный FASTQ файл. Плохие показатели качества: несовместимый размер.</translation>
    </message>
</context>
<context>
    <name>U2::FpkmTrackingFormat</name>
    <message>
        <location filename="../src/FpkmTrackingFormat.cpp" line="85"/>
        <source>FPKM Tracking Format</source>
        <translation>FPKM Tracking Format</translation>
    </message>
    <message>
        <location filename="../src/FpkmTrackingFormat.cpp" line="86"/>
        <source>The FPKM (fragments per kilobase of exon model per million mapped fragments) Tracking Format is a native Cufflinks format to output estimated expression values.</source>
        <translation>FPKM (fragments per kilobase of exon model per million mapped fragments) Tracking Format это внутренний Cufflinks формат для выходных значений оценки выражений.</translation>
    </message>
    <message>
        <location filename="../src/FpkmTrackingFormat.cpp" line="150"/>
        <source>FPKM Tracking Format parsing error: incorrect number of fields at line %1!</source>
        <translation>FPKM Tracking Format parsing error: incorrect number of fields at line %1!</translation>
    </message>
    <message>
        <location filename="../src/FpkmTrackingFormat.cpp" line="155"/>
        <source>FPKM Tracking Format parsing error: a field at line %1 is empty!</source>
        <translation>FPKM Tracking Format parsing error: a field at line %1 is empty!</translation>
    </message>
    <message>
        <location filename="../src/FpkmTrackingFormat.cpp" line="160"/>
        <source>FPKM Tracking Format parsing error: incorrect coordinates at line %1!</source>
        <translation>FPKM Tracking Format parsing error: incorrect coordinates at line %1!</translation>
    </message>
    <message>
        <location filename="../src/FpkmTrackingFormat.cpp" line="172"/>
        <source>FPKM Tracking Format parsing error: different sequence names were detected in an input file. Sequence name &apos;%1&apos; is used.</source>
        <translation>FPKM Tracking Format parsing error: different sequence names were detected in an input file. Sequence name &apos;%1&apos; is used.</translation>
    </message>
    <message>
        <location filename="../src/FpkmTrackingFormat.cpp" line="188"/>
        <source>FPKM Tracking Format parsing error: tracking ID value is empty at line %1!</source>
        <translation>FPKM Tracking Format parsing error: tracking ID value is empty at line %1!</translation>
    </message>
    <message>
        <location filename="../src/FpkmTrackingFormat.cpp" line="212"/>
        <source>FPKM Tracking Format parsing error: incorrect length value at line %1!</source>
        <translation>FPKM Tracking Format parsing error: incorrect length value at line %1!</translation>
    </message>
    <message>
        <location filename="../src/FpkmTrackingFormat.cpp" line="218"/>
        <source>FPKM Tracking Format parsing error: incorrect coverage value at line %1!</source>
        <translation>FPKM Tracking Format parsing error: incorrect coverage value at line %1!</translation>
    </message>
    <message>
        <location filename="../src/FpkmTrackingFormat.cpp" line="495"/>
        <source>Skipped qualifier &apos;%1&apos; while saving a FPKM header.</source>
        <translation>Skipped qualifier &apos;%1&apos; while saving a FPKM header.</translation>
    </message>
    <message>
        <location filename="../src/FpkmTrackingFormat.cpp" line="561"/>
        <source>FPKM Tracking Format saving error: tracking ID shouldn&apos;t be empty!</source>
        <translation>FPKM Tracking Format saving error: tracking ID shouldn&apos;t be empty!</translation>
    </message>
    <message>
        <location filename="../src/FpkmTrackingFormat.cpp" line="585"/>
        <source>FPKM Tracking Format saving error: failed to parse locus qualifier &apos;%1&apos;, writing it to the output file anyway!</source>
        <translation>FPKM Tracking Format saving error: failed to parse locus qualifier &apos;%1&apos;, writing it to the output file anyway!</translation>
    </message>
    <message>
        <location filename="../src/FpkmTrackingFormat.cpp" line="593"/>
        <source>FPKM Tracking Format saving error: an annotation region (%1, %2) differs from the information stored in the &apos;locus&apos; qualifier (%3, %4). Writing the &apos;locus&apos; qualifier to output!</source>
        <translation>FPKM Tracking Format saving error: an annotation region (%1, %2) differs from the information stored in the &apos;locus&apos; qualifier (%3, %4). Writing the &apos;locus&apos; qualifier to output!</translation>
    </message>
    <message>
        <location filename="../src/FpkmTrackingFormat.cpp" line="626"/>
        <source>FPKM Tracking Format saving error: one or more errors occurred while saving a file, see TRACE log for details!</source>
        <translation>FPKM Tracking Format saving error: one or more errors occurred while saving a file, see TRACE log for details!</translation>
    </message>
</context>
<context>
    <name>U2::GFFFormat</name>
    <message>
        <location filename="../src/GFFFormat.cpp" line="55"/>
        <source>GFF</source>
        <translation>GFF</translation>
    </message>
    <message>
        <location filename="../src/GFFFormat.cpp" line="56"/>
        <source>GFF is a format used for storing features and annotations</source>
        <translation>GFF это формат используемый для хранения аннотаций</translation>
    </message>
    <message>
        <location filename="../src/GFFFormat.cpp" line="100"/>
        <source>Parsing error: invalid header</source>
        <translation>Parsing error: invalid header</translation>
    </message>
    <message>
        <location filename="../src/GFFFormat.cpp" line="104"/>
        <source>Parsing error: file does not contain version header</source>
        <translation>Parsing error: file does not contain version header</translation>
    </message>
    <message>
        <location filename="../src/GFFFormat.cpp" line="108"/>
        <source>Parsing error: format version is not an integer</source>
        <translation>Parsing error: format version is not an integer</translation>
    </message>
    <message>
        <location filename="../src/GFFFormat.cpp" line="112"/>
        <source>Parsing error: GFF version %1 is not supported</source>
        <translation>Parsing error: GFF version %1 is not supported</translation>
    </message>
    <message>
        <location filename="../src/GFFFormat.cpp" line="226"/>
        <source>File &quot;%1&quot; contains too many sequences to be displayed. However, you can process these data using instruments from the menu &lt;i&gt;Tools -&gt; NGS data analysis&lt;/i&gt; or pipelines built with Workflow Designer.</source>
        <translation>File &quot;%1&quot; contains too many sequences to be displayed. However, you can process these data using instruments from the menu &lt;i&gt;Tools -&gt; NGS data analysis&lt;/i&gt; or pipelines built with Workflow Designer.</translation>
    </message>
    <message>
        <location filename="../src/GFFFormat.cpp" line="282"/>
        <source>Parsing error: file contains empty line %1, line skipped</source>
        <translation>Parsing error: file contains empty line %1, line skipped</translation>
    </message>
    <message>
        <location filename="../src/GFFFormat.cpp" line="322"/>
        <source>Parsing error: sequence in FASTA sequence has whitespaces at line %1</source>
        <translation>Parsing error: sequence in FASTA sequence has whitespaces at line %1</translation>
    </message>
    <message>
        <location filename="../src/GFFFormat.cpp" line="329"/>
        <source>Parsing error: too few fields at line %1</source>
        <translation>Parsing error: too few fields at line %1</translation>
    </message>
    <message>
        <location filename="../src/GFFFormat.cpp" line="335"/>
        <source>Parsing error: start position at line %1 is not integer</source>
        <translation>Parsing error: start position at line %1 is not integer</translation>
    </message>
    <message>
        <location filename="../src/GFFFormat.cpp" line="341"/>
        <source>Parsing error: end position at line %1 is not integer</source>
        <translation>Parsing error: end position at line %1 is not integer</translation>
    </message>
    <message>
        <location filename="../src/GFFFormat.cpp" line="346"/>
        <source>Parsing error: incorrect annotation region at line %1</source>
        <translation>Parsing error: incorrect annotation region at line %1</translation>
    </message>
    <message>
        <location filename="../src/GFFFormat.cpp" line="397"/>
        <source>Parsing error: incorrect attributes field %1 at line %2</source>
        <translation>Parsing error: incorrect attributes field %1 at line %2</translation>
    </message>
    <message>
        <location filename="../src/GFFFormat.cpp" line="436"/>
        <source>Parsing error: incorrect score parameter at line %1. Score can be a float number or &apos;.&apos; symbol</source>
        <translation>Parsing error: incorrect score parameter at line %1. Score can be a float number or &apos;.&apos; symbol</translation>
    </message>
    <message>
        <location filename="../src/GFFFormat.cpp" line="446"/>
        <source>Parsing error: incorrect frame parameter at line %1. Frame can be a number between 0-2 or &apos;.&apos; symbol</source>
        <translation>Parsing error: incorrect frame parameter at line %1. Frame can be a number between 0-2 or &apos;.&apos; symbol</translation>
    </message>
    <message>
        <location filename="../src/GFFFormat.cpp" line="456"/>
        <source>Parsing error: incorrect strand patameter at line %1. Strand can be &apos;+&apos;,&apos;-&apos; or &apos;.&apos;</source>
        <translation>Parsing error: incorrect strand patameter at line %1. Strand can be &apos;+&apos;,&apos;-&apos; or &apos;.&apos;</translation>
    </message>
    <message>
        <location filename="../src/GFFFormat.cpp" line="514"/>
        <source>One or more sequences in this file don&apos;t have names. Their names are generated automatically.</source>
        <translation>One or more sequences in this file don&apos;t have names. Their names are generated automatically.</translation>
    </message>
    <message>
        <location filename="../src/GFFFormat.cpp" line="634"/>
        <source>Can not detect chromosome name. &apos;Chr&apos; name will be used.</source>
        <translation>Can not detect chromosome name. &apos;Chr&apos; name will be used.</translation>
    </message>
    <message>
        <location filename="../src/GFFFormat.cpp" line="387"/>
        <source>Wrong location for joined annotation at line %1. Line was skipped.</source>
        <translation>Wrong location for joined annotation at line %1. Line was skipped.</translation>
    </message>
</context>
<context>
    <name>U2::GTFFormat</name>
    <message>
        <location filename="../src/GTFFormat.cpp" line="99"/>
        <source>GTF</source>
        <translation>GTF</translation>
    </message>
    <message>
        <location filename="../src/GTFFormat.cpp" line="100"/>
        <source>The Gene transfer format (GTF) is a file format used to hold information about gene structure.</source>
        <translation>The Gene transfer format (GTF) это формат используемый для хранения информации о структуре гена.</translation>
    </message>
    <message>
        <location filename="../src/GTFFormat.cpp" line="154"/>
        <source>GTF parsing error: incorrect number of fields at line %1!</source>
        <translation>GTF parsing error: incorrect number of fields at line %1!</translation>
    </message>
    <message>
        <location filename="../src/GTFFormat.cpp" line="159"/>
        <source>GTF parsing error: a field at line %1 is empty!</source>
        <translation>GTF parsing error: a field at line %1 is empty!</translation>
    </message>
    <message>
        <location filename="../src/GTFFormat.cpp" line="164"/>
        <source>GTF parsing error: incorrect coordinates at line %1!</source>
        <translation>GTF parsing error: incorrect coordinates at line %1!</translation>
    </message>
    <message>
        <location filename="../src/GTFFormat.cpp" line="188"/>
        <source>GTF parsing error: incorrect score value &quot;%1&quot; at line %2!</source>
        <translation>GTF parsing error: incorrect score value &quot;%1&quot; at line %2!</translation>
    </message>
    <message>
        <location filename="../src/GTFFormat.cpp" line="197"/>
        <source>GTF parsing error: incorrect frame value &quot;%1&quot; at line %2!</source>
        <translation>GTF parsing error: incorrect frame value &quot;%1&quot; at line %2!</translation>
    </message>
    <message>
        <location filename="../src/GTFFormat.cpp" line="218"/>
        <source>GTF parsing error: invalid attributes format at line %1!</source>
        <translation>GTF parsing error: invalid attributes format at line %1!</translation>
    </message>
    <message>
        <location filename="../src/GTFFormat.cpp" line="226"/>
        <source>GTF parsing error: incorrect strand value &quot;%1&quot; at line %2!</source>
        <translation>GTF parsing error: incorrect strand value &quot;%1&quot; at line %2!</translation>
    </message>
    <message>
        <location filename="../src/GTFFormat.cpp" line="273"/>
        <source>File &quot;%1&quot; contains too many annotation tables to be displayed. However, you can process these data using pipelines built with Workflow Designer.</source>
        <translation>File &quot;%1&quot; contains too many annotation tables to be displayed. However, you can process these data using pipelines built with Workflow Designer.</translation>
    </message>
    <message>
        <location filename="../src/GTFFormat.cpp" line="541"/>
        <source>Can not detect chromosome name. &apos;Chr&apos; name will be used.</source>
        <translation>Не удается определить название хромосомы. Будет использовано название &apos;Chr&apos;.</translation>
    </message>
</context>
<context>
    <name>U2::Genbank::LocationParser</name>
    <message>
        <location filename="../src/GenbankLocationParser.cpp" line="353"/>
        <location filename="../src/GenbankLocationParser.cpp" line="422"/>
        <source>&apos;a single base from a range&apos; in combination with &apos;sequence span&apos; is not supported</source>
        <translation>&apos;a single base from a range&apos; in combination with &apos;sequence span&apos; is not supported</translation>
    </message>
    <message>
        <location filename="../src/GenbankLocationParser.cpp" line="356"/>
        <source>Ignoring &apos;&lt;&apos; at start position</source>
        <translation>Ignoring &apos;&lt;&apos; at start position</translation>
    </message>
    <message>
        <location filename="../src/GenbankLocationParser.cpp" line="425"/>
        <source>Ignoring &apos;&gt;&apos; at end position</source>
        <translation>Ignoring &apos;&gt;&apos; at end position</translation>
    </message>
    <message>
        <location filename="../src/GenbankLocationParser.cpp" line="488"/>
        <source>Wrong token after JOIN %1</source>
        <translation>Wrong token after JOIN %1</translation>
    </message>
    <message>
        <location filename="../src/GenbankLocationParser.cpp" line="493"/>
        <source>Wrong token after JOIN  - order %1</source>
        <translation>Wrong token after JOIN  - order %1</translation>
    </message>
    <message>
        <location filename="../src/GenbankLocationParser.cpp" line="502"/>
        <source>Can&apos;t parse location on JOIN</source>
        <translation>Can&apos;t parse location on JOIN</translation>
    </message>
    <message>
        <location filename="../src/GenbankLocationParser.cpp" line="508"/>
        <location filename="../src/GenbankLocationParser.cpp" line="534"/>
        <location filename="../src/GenbankLocationParser.cpp" line="555"/>
        <location filename="../src/GenbankLocationParser.cpp" line="601"/>
        <source>Must be RIGHT_PARENTHESIS instead of %1</source>
        <translation>Must be RIGHT_PARENTHESIS instead of %1</translation>
    </message>
    <message>
        <location filename="../src/GenbankLocationParser.cpp" line="514"/>
        <source>Wrong token after ORDER %1</source>
        <translation>Wrong token after ORDER %1</translation>
    </message>
    <message>
        <location filename="../src/GenbankLocationParser.cpp" line="519"/>
        <source>Wrong token after ORDER - join %1</source>
        <translation>Wrong token after ORDER - join %1</translation>
    </message>
    <message>
        <location filename="../src/GenbankLocationParser.cpp" line="528"/>
        <source>Can&apos;t parse location on ORDER</source>
        <translation>Can&apos;t parse location on ORDER</translation>
    </message>
    <message>
        <location filename="../src/GenbankLocationParser.cpp" line="540"/>
        <source>Wrong token after BOND %1</source>
        <translation>Wrong token after BOND %1</translation>
    </message>
    <message>
        <location filename="../src/GenbankLocationParser.cpp" line="549"/>
        <source>Can&apos;t parse location on BONDs</source>
        <translation>Can&apos;t parse location on BONDs</translation>
    </message>
    <message>
        <location filename="../src/GenbankLocationParser.cpp" line="577"/>
        <source>Must be LEFT_PARENTHESIS instead of %1</source>
        <translation>Must be LEFT_PARENTHESIS instead of %1</translation>
    </message>
    <message>
        <location filename="../src/GenbankLocationParser.cpp" line="594"/>
        <source>Can&apos;t parse location on COMPLEMENT</source>
        <translation>Can&apos;t parse location on COMPLEMENT</translation>
    </message>
</context>
<context>
    <name>U2::GenbankPlainTextFormat</name>
    <message>
        <location filename="../src/GenbankPlainTextFormat.cpp" line="402"/>
        <location filename="../src/GenbankPlainTextFormat.cpp" line="647"/>
        <location filename="../src/GenbankPlainTextFormat.cpp" line="652"/>
        <location filename="../src/GenbankPlainTextFormat.cpp" line="658"/>
        <location filename="../src/GenbankPlainTextFormat.cpp" line="800"/>
        <location filename="../src/GenbankPlainTextFormat.cpp" line="828"/>
        <location filename="../src/GenbankPlainTextFormat.cpp" line="835"/>
        <location filename="../src/GenbankPlainTextFormat.cpp" line="842"/>
        <location filename="../src/GenbankPlainTextFormat.cpp" line="852"/>
        <source>Error writing document</source>
        <translation>Ошибка записи</translation>
    </message>
    <message>
        <location filename="../src/GenbankPlainTextFormat.cpp" line="46"/>
        <source>GenBank</source>
        <translation>GenBank</translation>
    </message>
    <message>
        <location filename="../src/GenbankPlainTextFormat.cpp" line="48"/>
        <source>GenBank Flat File Format is a rich format for storing sequences and associated annotations</source>
        <translation>GenBank Flat File Format это формат для хранения последовательностей и их аннотаций</translation>
    </message>
    <message>
        <location filename="../src/GenbankPlainTextFormat.cpp" line="102"/>
        <source>LOCUS is not the first line</source>
        <translation>Строка локуса должна идти первой</translation>
    </message>
    <message>
        <location filename="../src/GenbankPlainTextFormat.cpp" line="122"/>
        <source>Error parsing LOCUS line</source>
        <translation>Ошибка чтения локуса</translation>
    </message>
    <message>
        <location filename="../src/GenbankPlainTextFormat.cpp" line="187"/>
        <source>incomplete SOURCE record</source>
        <translation>Данные повреждены: запись SOURCE</translation>
    </message>
    <message>
        <location filename="../src/GenbankPlainTextFormat.cpp" line="812"/>
        <source>Invalid annotation table!</source>
        <translation>Invalid annotation table!</translation>
    </message>
</context>
<context>
    <name>U2::GzipDecompressTask</name>
    <message>
        <location filename="../src/tasks/BgzipTask.cpp" line="128"/>
        <source>Decompression task</source>
        <translation>Задача разархивации</translation>
    </message>
    <message>
        <location filename="../src/tasks/BgzipTask.cpp" line="132"/>
        <source>&apos;%1&apos; is not zipped file</source>
        <translation>&apos;%1&apos; не является zip файлом</translation>
    </message>
    <message>
        <location filename="../src/tasks/BgzipTask.cpp" line="137"/>
        <source>Start decompression &apos;%1&apos;</source>
        <translation>Начало разархивации &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/tasks/BgzipTask.cpp" line="162"/>
        <source>Can not open output file &apos;%1&apos;</source>
        <translation>Невозможно открыть выходной файл &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/tasks/BgzipTask.cpp" line="168"/>
        <source>Can not open input file &apos;%1&apos;</source>
        <translation>Can not open input file &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/tasks/BgzipTask.cpp" line="183"/>
        <source>Error reading file</source>
        <translation>Ошибка чтения файла</translation>
    </message>
    <message>
        <location filename="../src/tasks/BgzipTask.cpp" line="189"/>
        <source>Error writing to file</source>
        <translation>Ошибка записи в файл</translation>
    </message>
    <message>
        <location filename="../src/tasks/BgzipTask.cpp" line="194"/>
        <source>Decompression finished</source>
        <translation>Разархивация завершена</translation>
    </message>
    <message>
        <location filename="../src/tasks/BgzipTask.cpp" line="199"/>
        <source>Decompression task was finished with an error: %1</source>
        <translation>Задача разархивации завершилась с ошибкой : %1</translation>
    </message>
    <message>
        <location filename="../src/tasks/BgzipTask.cpp" line="201"/>
        <source>Decompression task was finished. A new decompressed file is: &lt;a href=&quot;%1&quot;&gt;%1&lt;/a&gt;</source>
        <translation>Задача разархивации завершена. Новый файл: &lt;a href=&quot;%1&quot;&gt;%1&lt;/a&gt;</translation>
    </message>
</context>
<context>
    <name>U2::InfoPartParser</name>
    <message>
        <location filename="../src/util/SnpeffInfoParser.cpp" line="111"/>
        <source>Too few values in the entry: &apos;%1&apos;. Expected at least %2 values.</source>
        <translation>Too few values in the entry: &apos;%1&apos;. Expected at least %2 values.</translation>
    </message>
    <message>
        <location filename="../src/util/SnpeffInfoParser.cpp" line="131"/>
        <source>Too many values in the entry &apos;%1&apos;, extra entries are ignored</source>
        <translation>Too many values in the entry &apos;%1&apos;, extra entries are ignored</translation>
    </message>
</context>
<context>
    <name>U2::KrakenResultsPlainTextFormat</name>
    <message>
        <location filename="../src/KrakenResultsPlainTextFormat.cpp" line="57"/>
        <source>Kraken results</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/KrakenResultsPlainTextFormat.cpp" line="58"/>
        <source>Kraken classification results text format</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/KrakenResultsPlainTextFormat.cpp" line="116"/>
        <source>Error on line %1, line contains paired results opposite of previous lines in file.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/KrakenResultsPlainTextFormat.cpp" line="120"/>
        <source>Error on line %1, line contains single results opposite of previous lines in file.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/KrakenResultsPlainTextFormat.cpp" line="126"/>
        <source>Error on line %1, 1st word should be &quot;C&quot; or &quot;U&quot;.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/KrakenResultsPlainTextFormat.cpp" line="133"/>
        <source>Error on line %1, 3rd word should be number.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/KrakenResultsPlainTextFormat.cpp" line="135"/>
        <source>Error on line %1, 3rd word should be number greater or equal zero.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/KrakenResultsPlainTextFormat.cpp" line="142"/>
        <source>Error on line %1, 4th word should be number.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/KrakenResultsPlainTextFormat.cpp" line="144"/>
        <source>Error on line %1, 4th word should be number greater than zero.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/KrakenResultsPlainTextFormat.cpp" line="151"/>
        <source>Error on line %1, 4th word not match format &quot;&lt;number&gt;|&lt;number&gt;&quot;.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/KrakenResultsPlainTextFormat.cpp" line="154"/>
        <source>Error on line %1, 4th word, first element of the pair should be a number.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/KrakenResultsPlainTextFormat.cpp" line="156"/>
        <source>Error on line %1, 4th, first element of the pair should be number greater than zero.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/KrakenResultsPlainTextFormat.cpp" line="160"/>
        <source>Error on line %1, 4th word, second element of the pair should be a number.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/KrakenResultsPlainTextFormat.cpp" line="162"/>
        <source>Error on line %1, 4th, second element of the pair should be number greater than zero.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/KrakenResultsPlainTextFormat.cpp" line="182"/>
        <source>Error on line %1, %2th word not match format &quot;&lt;string_or_number&gt;:&lt;number&gt;&quot;.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/KrakenResultsPlainTextFormat.cpp" line="185"/>
        <source>Error on line %1, %2th word second element of the pair is not a number.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/KrakenResultsPlainTextFormat.cpp" line="186"/>
        <source>Error on line %1, %2th word second element of the pair should be number greater than zero.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/KrakenResultsPlainTextFormat.cpp" line="202"/>
        <source>Summary fragment length %1 should be less or equal sequence length %2.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/KrakenResultsPlainTextFormat.cpp" line="221"/>
        <location filename="../src/KrakenResultsPlainTextFormat.cpp" line="225"/>
        <source>Sequence %1 skipped, because no classified result found</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/KrakenResultsPlainTextFormat.cpp" line="239"/>
        <source>Two or more classification results with same sequence name &quot;%1&quot; were found.They will be merged in one annotation table</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::LoadConvertAndSaveSnpeffVariationsToAnnotationsTask</name>
    <message>
        <location filename="../src/tasks/ConvertSnpeffVariationsToAnnotationsTask.cpp" line="118"/>
        <source>Load file and convert SnpEff variations to annotations task</source>
        <translation>Load file and convert SnpEff variations to annotations task</translation>
    </message>
    <message>
        <location filename="../src/tasks/ConvertSnpeffVariationsToAnnotationsTask.cpp" line="157"/>
        <source>&apos;%1&apos; load failed, the result document is NULL</source>
        <translation>&apos;%1&apos; load failed, the result document is NULL</translation>
    </message>
    <message>
        <location filename="../src/tasks/ConvertSnpeffVariationsToAnnotationsTask.cpp" line="161"/>
        <source>File &apos;%1&apos; doesn&apos;t contain variation tracks</source>
        <translation>File &apos;%1&apos; doesn&apos;t contain variation tracks</translation>
    </message>
</context>
<context>
    <name>U2::MSFFormat</name>
    <message>
        <location filename="../src/MSFFormat.cpp" line="57"/>
        <source>MSF</source>
        <translation>MSF</translation>
    </message>
    <message>
        <location filename="../src/MSFFormat.cpp" line="59"/>
        <source>MSF format is used to store multiple aligned sequences. Files include the sequence name and the sequence itself, which is usually aligned with other sequences in the file.</source>
        <translation>MSF формат используется для множественных выравниваний. Файлы включают имя последовательности и последовательность, которая выровнена с другими последовательностями в файле.</translation>
    </message>
    <message>
        <location filename="../src/MSFFormat.cpp" line="154"/>
        <source>File check sum is incorrect: expected value: %1, current value %2</source>
        <translation>Контрольная сумма некорректна: ожидается значение %1, текущее значение %2</translation>
    </message>
    <message>
        <location filename="../src/MSFFormat.cpp" line="177"/>
        <source>MSF: too many rows in the block, line: %1</source>
        <translation>MSF: too many rows in the block, line: %1</translation>
    </message>
    <message>
        <location filename="../src/MSFFormat.cpp" line="180"/>
        <source>MSF: can&apos;t find name and value separator spacing, line: %1</source>
        <translation>MSF: can&apos;t find name and value separator spacing, line: %1</translation>
    </message>
    <message>
        <location filename="../src/MSFFormat.cpp" line="184"/>
        <source>MSF: row names do not match: %1 vs %2, line: %3</source>
        <translation>MSF: row names do not match: %1 vs %2, line: %3</translation>
    </message>
    <message>
        <location filename="../src/MSFFormat.cpp" line="201"/>
        <source>Unexpected check sum in the row number %1, name: %2; expected value: %3, current value %4</source>
        <translation>Некорректная контрольная сумма в ряду %1, имя: %2; ожидается значение: %3, текущее значение %4</translation>
    </message>
    <message>
        <location filename="../src/MSFFormat.cpp" line="208"/>
        <source>Alphabet unknown</source>
        <translation>Неизвестный алфавит</translation>
    </message>
</context>
<context>
    <name>U2::MegaFormat</name>
    <message>
        <location filename="../src/MegaFormat.cpp" line="54"/>
        <source>Mega</source>
        <translation>Mega</translation>
    </message>
    <message>
        <location filename="../src/MegaFormat.cpp" line="55"/>
        <source>Mega is a file format of native MEGA program</source>
        <translation>Mega это формат файла программы MEGA</translation>
    </message>
    <message>
        <location filename="../src/MegaFormat.cpp" line="316"/>
        <source>Found sequences of different sizes</source>
        <translation>Обнаружены последовательности разной длины</translation>
    </message>
    <message>
        <location filename="../src/MegaFormat.cpp" line="153"/>
        <source>Bad name of sequence</source>
        <translation>Неверное имя последовательности</translation>
    </message>
    <message>
        <location filename="../src/MegaFormat.cpp" line="173"/>
        <source>Unexpected # in comments</source>
        <translation>Неожиданный символ &quot;#&quot; в комментариях</translation>
    </message>
    <message>
        <location filename="../src/MegaFormat.cpp" line="182"/>
        <source>A comment has not end</source>
        <translation>У комментария отсутствует окончание</translation>
    </message>
    <message>
        <location filename="../src/MegaFormat.cpp" line="206"/>
        <source>Unexpected symbol between comments</source>
        <translation>Неоижданные символы между комментариями</translation>
    </message>
    <message>
        <location filename="../src/MegaFormat.cpp" line="264"/>
        <source>Incorrect format</source>
        <translation>Неверный формат</translation>
    </message>
    <message>
        <location filename="../src/MegaFormat.cpp" line="277"/>
        <source>Identical symbol at the first sequence</source>
        <translation>Идентичный символ в первой последовательности</translation>
    </message>
    <message>
        <location filename="../src/MegaFormat.cpp" line="298"/>
        <source>Incorrect order of sequences&apos; names</source>
        <translation>Неправильный порядок имён последовательностей</translation>
    </message>
    <message>
        <location filename="../src/MegaFormat.cpp" line="304"/>
        <source>Incorrect sequence</source>
        <translation>Incorrect sequence</translation>
    </message>
    <message>
        <location filename="../src/MegaFormat.cpp" line="324"/>
        <source>Alphabet is unknown</source>
        <translation>Неизвестный алфавит</translation>
    </message>
    <message>
        <location filename="../src/MegaFormat.cpp" line="395"/>
        <location filename="../src/MegaFormat.cpp" line="402"/>
        <source>No header</source>
        <translation>Отсутствует заголовок</translation>
    </message>
    <message>
        <location filename="../src/MegaFormat.cpp" line="396"/>
        <source>No # before header</source>
        <translation>Отсутствует символ &quot;#&quot; перед заголовком</translation>
    </message>
    <message>
        <location filename="../src/MegaFormat.cpp" line="403"/>
        <source>Not MEGA-header</source>
        <translation>Не является заголовком MEGA</translation>
    </message>
    <message>
        <location filename="../src/MegaFormat.cpp" line="426"/>
        <location filename="../src/MegaFormat.cpp" line="435"/>
        <location filename="../src/MegaFormat.cpp" line="460"/>
        <source>No data in file</source>
        <translation>Отсутствуют данные в файле</translation>
    </message>
    <message>
        <location filename="../src/MegaFormat.cpp" line="441"/>
        <location filename="../src/MegaFormat.cpp" line="446"/>
        <source>Incorrect title</source>
        <translation>Неправильный заголовок</translation>
    </message>
    <message>
        <location filename="../src/MegaFormat.cpp" line="492"/>
        <source>Sequence has empty part</source>
        <translation>Часть последовательности пустая</translation>
    </message>
    <message>
        <location filename="../src/MegaFormat.cpp" line="512"/>
        <source>Bad symbols in a sequence</source>
        <translation>Некорректные символы в последовательности</translation>
    </message>
</context>
<context>
    <name>U2::NEXUSFormat</name>
    <message>
        <location filename="../src/NEXUSFormat.cpp" line="51"/>
        <source>NEXUS</source>
        <translation>NEXUS</translation>
    </message>
    <message>
        <location filename="../src/NEXUSFormat.cpp" line="52"/>
        <source>Nexus is a multiple alignment and phylogenetic trees file format</source>
        <translation>Nexus формат файла для множественных выравниваний и филогенетических деревьев</translation>
    </message>
    <message>
        <location filename="../src/NEXUSFormat.cpp" line="724"/>
        <source>#NEXUS header missing</source>
        <translation>Отсутствует заголовок #NEXUS</translation>
    </message>
</context>
<context>
    <name>U2::NewickFormat</name>
    <message>
        <location filename="../src/NewickFormat.cpp" line="38"/>
        <source>Newick Standard</source>
        <translation>Стандарт Newick</translation>
    </message>
    <message>
        <location filename="../src/NewickFormat.cpp" line="39"/>
        <source>Newick is a simple format used to write out trees in a text file</source>
        <translation>Newick iэто формат используемый для записи деревьев в текстовый файл</translation>
    </message>
</context>
<context>
    <name>U2::PDBFormat</name>
    <message>
        <location filename="../src/PDBFormat.cpp" line="214"/>
        <source>Line is too long</source>
        <translation>Слишком длинная строка</translation>
    </message>
    <message>
        <location filename="../src/PDBFormat.cpp" line="57"/>
        <source>The Protein Data Bank (PDB) format provides a standard representation for macromolecular structure data derived from X-ray diffraction and NMR studies.</source>
        <translation>The Protein Data Bank (PDB) формат обеспечивает стандартное представление для данных высокомолекулярных структур, полученных из рентгеновской дифракции и исследований ЯМР.</translation>
    </message>
    <message>
        <location filename="../src/PDBFormat.cpp" line="194"/>
        <source>Inconsistent atom indexes in pdb file: %1.</source>
        <translation>Несогласованные индексы атомов в файле PDB: %1.</translation>
    </message>
    <message>
        <location filename="../src/PDBFormat.cpp" line="276"/>
        <source>Some mandatory records are absent</source>
        <translation>Некоторые обязательные записи отсутствуют</translation>
    </message>
    <message>
        <location filename="../src/PDBFormat.cpp" line="417"/>
        <source>PDB warning: unknown residue name: %1</source>
        <translation>PDB предупреждение: неизвестное имя остатка: %1</translation>
    </message>
    <message>
        <location filename="../src/PDBFormat.cpp" line="504"/>
        <source>Invalid secondary structure record</source>
        <translation>Неверная запись вторичной структуры</translation>
    </message>
    <message>
        <location filename="../src/PDBFormat.cpp" line="538"/>
        <source>Invalid SEQRES: less then 24 characters</source>
        <translation>Неверный SEQRES: меньше чем 24 символа</translation>
    </message>
    <message>
        <location filename="../src/PDBFormat.cpp" line="56"/>
        <source>PDB</source>
        <translation>PDB</translation>
    </message>
</context>
<context>
    <name>U2::PDWFormat</name>
    <message>
        <location filename="../src/PDWFormat.cpp" line="56"/>
        <source>pDRAW</source>
        <translation>pDRAW</translation>
    </message>
    <message>
        <location filename="../src/PDWFormat.cpp" line="57"/>
        <source>pDRAW is a sequence file format used by pDRAW software</source>
        <translation>pDRAW это формат файла для хранения последовательности используемый инструментом pDRAW</translation>
    </message>
    <message>
        <location filename="../src/PDWFormat.cpp" line="96"/>
        <location filename="../src/PDWFormat.cpp" line="186"/>
        <source>Line is too long</source>
        <translation>Слишком длинная строка</translation>
    </message>
</context>
<context>
    <name>U2::PairedFastqComparator</name>
    <message>
        <location filename="../src/util/PairedFastqComparator.cpp" line="71"/>
        <source>Too much reads without a pair (&gt;%1). Check the input data are set correctly.</source>
        <translation>Too much reads without a pair (&gt;%1). Check the input data are set correctly.</translation>
    </message>
</context>
<context>
    <name>U2::PhylipFormat</name>
    <message>
        <location filename="../src/PhylipFormat.cpp" line="78"/>
        <source>Wrong row count. Header: %1, actual: %2</source>
        <translation>Wrong row count. Header: %1, actual: %2</translation>
    </message>
    <message>
        <location filename="../src/PhylipFormat.cpp" line="80"/>
        <source>Wrong column count. Header: %1, actual: %2</source>
        <translation>Wrong column count. Header: %1, actual: %2</translation>
    </message>
    <message>
        <location filename="../src/PhylipFormat.cpp" line="85"/>
        <source>PHYLIP multiple alignment format for phylogenetic applications.</source>
        <translation>PHYLIP формат филогенетических деревьев.</translation>
    </message>
    <message>
        <location filename="../src/PhylipFormat.cpp" line="90"/>
        <source>Incorrect number of objects in document: %1</source>
        <translation>Incorrect number of objects in document: %1</translation>
    </message>
    <message>
        <location filename="../src/PhylipFormat.cpp" line="109"/>
        <source>Alphabet is unknown</source>
        <translation>Неизвестный алфавит</translation>
    </message>
    <message>
        <location filename="../src/PhylipFormat.cpp" line="144"/>
        <location filename="../src/PhylipFormat.cpp" line="257"/>
        <source>Failed to find MSA object to store</source>
        <translation>Failed to find MSA object to store</translation>
    </message>
    <message>
        <location filename="../src/PhylipFormat.cpp" line="221"/>
        <location filename="../src/PhylipFormat.cpp" line="350"/>
        <source>Failed to parse header line</source>
        <translation>Failed to parse header line</translation>
    </message>
    <message>
        <location filename="../src/PhylipFormat.cpp" line="224"/>
        <source>Unexpected end of data in Phylip file</source>
        <translation>Unexpected end of data in Phylip file</translation>
    </message>
    <message>
        <location filename="../src/PhylipFormat.cpp" line="230"/>
        <location filename="../src/PhylipFormat.cpp" line="356"/>
        <source>Line with a name is too short %1</source>
        <translation>Line with a name is too short %1</translation>
    </message>
    <message>
        <location filename="../src/PhylipFormat.cpp" line="236"/>
        <source>Unexpected end of file</source>
        <translation>Неожиданный конец файла</translation>
    </message>
</context>
<context>
    <name>U2::PhylipInterleavedFormat</name>
    <message>
        <location filename="../src/PhylipFormat.cpp" line="252"/>
        <source>PHYLIP Interleaved</source>
        <translation>PHYLIP Interleaved</translation>
    </message>
</context>
<context>
    <name>U2::PhylipSequentialFormat</name>
    <message>
        <location filename="../src/PhylipFormat.cpp" line="139"/>
        <source>PHYLIP Sequential</source>
        <translation>PHYLIP Sequential</translation>
    </message>
</context>
<context>
    <name>U2::PlainTextFormat</name>
    <message>
        <location filename="../src/PlainTextFormat.cpp" line="37"/>
        <source>Plain text</source>
        <translation>Текст</translation>
    </message>
    <message>
        <location filename="../src/PlainTextFormat.cpp" line="39"/>
        <source>A simple plain text file.</source>
        <translation>Текстовый файл.</translation>
    </message>
</context>
<context>
    <name>U2::RawDNASequenceFormat</name>
    <message>
        <location filename="../src/RawDNASequenceFormat.cpp" line="43"/>
        <source>Raw sequence</source>
        <translation>Произвольная последовательность</translation>
    </message>
    <message>
        <location filename="../src/RawDNASequenceFormat.cpp" line="45"/>
        <source>Raw sequence file - a whole content of the file is treated either as a single/multiple nucleotide or peptide sequence(s). UGENE will remove all non-alphabetic chars from the result sequence. By default the characters in the file are considered a single sequence.</source>
        <translation>Сырой файл последовательности - все содержимое файла рассматривается как один нуклеотид или пептидная последовательность. UGENE удалит все символы не принадлежащие алфавиту из результирующей последовательности.</translation>
    </message>
    <message>
        <location filename="../src/RawDNASequenceFormat.cpp" line="111"/>
        <source>Sequence is empty</source>
        <translation>Последовательность пуста</translation>
    </message>
</context>
<context>
    <name>U2::SAMFormat</name>
    <message>
        <location filename="../src/SAMFormat.cpp" line="82"/>
        <source>Field &quot;%1&quot; not matched pattern &quot;%2&quot;, expected pattern &quot;%3&quot;</source>
        <translation>Field &quot;%1&quot; not matched pattern &quot;%2&quot;, expected pattern &quot;%3&quot;</translation>
    </message>
    <message>
        <location filename="../src/SAMFormat.cpp" line="91"/>
        <source>SAM</source>
        <translation>SAM</translation>
    </message>
    <message>
        <location filename="../src/SAMFormat.cpp" line="92"/>
        <source>The Sequence Alignment/Map (SAM) format is a generic alignment format forstoring read alignments against reference sequence</source>
        <translation>The Sequence Alignment/Map (SAM) это общий формат выравниваний для хранения выравниваний с референсной последовательностью</translation>
    </message>
</context>
<context>
    <name>U2::SCFFormat</name>
    <message>
        <location filename="../src/SCFFormat.cpp" line="48"/>
        <source>SCF</source>
        <translation>SCF</translation>
    </message>
    <message>
        <location filename="../src/SCFFormat.cpp" line="49"/>
        <source>It is Standard Chromatogram Format</source>
        <translation>Это стандартный формат хроматограмм</translation>
    </message>
    <message>
        <location filename="../src/SCFFormat.cpp" line="67"/>
        <source>Failed to parse SCF file: %1</source>
        <translation>Failed to parse SCF file: %1</translation>
    </message>
    <message>
        <location filename="../src/SCFFormat.cpp" line="1235"/>
        <source>Failed to load sequence from SCF file %1</source>
        <translation>Невозможно загрузить последовательность из SCF файла %1</translation>
    </message>
</context>
<context>
    <name>U2::SnpeffInfoParser</name>
    <message>
        <location filename="../src/util/SnpeffInfoParser.cpp" line="47"/>
        <source>Can&apos;t parse the next INFO part: &apos;%1&apos;</source>
        <translation>Can&apos;t parse the next INFO part: &apos;%1&apos;</translation>
    </message>
</context>
<context>
    <name>U2::StockholmFormat</name>
    <message>
        <location filename="../src/StockholmFormat.cpp" line="358"/>
        <source>Invalid file: empty sequence name</source>
        <translation>Не указано имя последовательности</translation>
    </message>
    <message>
        <location filename="../src/StockholmFormat.cpp" line="359"/>
        <source>Invalid file: duplicate sequence names in one block: %1</source>
        <translation>одинаковые имена последовательностей в блоке: %1</translation>
    </message>
    <message>
        <location filename="../src/StockholmFormat.cpp" line="363"/>
        <source>Invalid file: sequence names are not equal in blocks</source>
        <translation>Непарное имя последовательности в блоке</translation>
    </message>
    <message>
        <location filename="../src/StockholmFormat.cpp" line="370"/>
        <source>Invalid file: sequences in block are not of equal size</source>
        <translation>Блок содержит последовательности разной длины</translation>
    </message>
    <message>
        <location filename="../src/StockholmFormat.cpp" line="523"/>
        <source>Stockholm</source>
        <translation>Stockholm</translation>
    </message>
    <message>
        <location filename="../src/StockholmFormat.cpp" line="524"/>
        <source>A multiple sequence alignments file format</source>
        <translation>Формат файла для множественных выравниваний</translation>
    </message>
    <message>
        <location filename="../src/StockholmFormat.cpp" line="392"/>
        <source>invalid file: empty sequence alignment</source>
        <translation>Выравнивание не содержит последовательностей</translation>
    </message>
    <message>
        <location filename="../src/StockholmFormat.cpp" line="395"/>
        <source>invalid file: unknown alphabet</source>
        <translation>Не удалось установить алфавит</translation>
    </message>
</context>
<context>
    <name>U2::StreamSequenceReader</name>
    <message>
        <location filename="../src/StreamSequenceReader.cpp" line="92"/>
        <source>File %1 unsupported format.</source>
        <translation>File %1 unsupported format.</translation>
    </message>
    <message>
        <location filename="../src/StreamSequenceReader.cpp" line="110"/>
        <source>Unsupported file format or short reads list is empty</source>
        <translation>Unsupported file format or short reads list is empty</translation>
    </message>
</context>
<context>
    <name>U2::SwissProtPlainTextFormat</name>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="63"/>
        <source>Swiss-Prot</source>
        <translation>Swiss-Prot</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="64"/>
        <source>SwissProt is a format of the UniProtKB/Swiss-prot database used for storing annotated protein sequence</source>
        <translation>SwissProt это формат базы данных UniProtKB/Swiss-prot используемый для хранения аннотированных белковых последовательностей</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="105"/>
        <source>ID is not the first line</source>
        <translation>Строка идентификатора должна идти первой</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="112"/>
        <source>Error parsing ID line</source>
        <translation>Неверный заголовок</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="122"/>
        <source>Error parsing ID line. Not found sequence length</source>
        <translation>Ошибка распознавания ID строки. Не найдена длина последовательности</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="239"/>
        <source>Reading sequence %1</source>
        <translation>Чтение последовательности: %1</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="258"/>
        <source>Error parsing sequence: unexpected empty line</source>
        <translation>Ошибка чтения последовательности: пустая строка</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="282"/>
        <source>Error reading sequence: memory allocation failed</source>
        <translation>Ошибка чтения последовательности: не удалось выделить память</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="297"/>
        <source>Sequence is truncated</source>
        <translation>Последовательность повреждена</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="304"/>
        <source>Reading annotations %1</source>
        <translation>Чтение аннотаций: %1</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="311"/>
        <source>Invalid format of feature table</source>
        <translation>Таблица аннотаций повреждена</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="343"/>
        <source>The DT string doesn&apos;t contain date.</source>
        <translation>The DT string doesn&apos;t contain date.</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="347"/>
        <source>The format of the date is unexpected.</source>
        <translation>The format of the date is unexpected.</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="351"/>
        <source>Day is incorrect.</source>
        <translation>Day is incorrect.</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="354"/>
        <source>Mounth is incorrect.</source>
        <translation>Mounth is incorrect.</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="357"/>
        <source>Year is incorrect.</source>
        <translation>Year is incorrect.</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="398"/>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="485"/>
        <source>The annotation start position is unexpected.</source>
        <translation>The annotation start position is unexpected.</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="401"/>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="490"/>
        <source>The annotation end position is unexpected.</source>
        <translation>The annotation end position is unexpected.</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="473"/>
        <source>Unexpected annotation strings.</source>
        <translation>Unexpected annotation strings.</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="480"/>
        <source>The annotation name is empty.</source>
        <translation>The annotation name is empty.</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="503"/>
        <source>Annotation qualifier is corrupted</source>
        <translation>Annotation qualifier is corrupted</translation>
    </message>
    <message>
        <location filename="../src/SwissProtPlainTextFormat.cpp" line="515"/>
        <source>Unexpected qulifiers values.</source>
        <translation>Unexpected qulifiers values.</translation>
    </message>
</context>
<context>
    <name>U2::TabulatedFormatReader</name>
    <message>
        <location filename="../src/util/TabulatedFormatReader.cpp" line="35"/>
        <source>IO adapter is not opened</source>
        <translation>IO adapter is not opened</translation>
    </message>
</context>
<context>
    <name>U2::TextDocumentFormat</name>
    <message>
        <location filename="../src/TextDocumentFormat.cpp" line="116"/>
        <source>The document format does not support streaming reading mode: %1</source>
        <translation>Формат документа не позволяет чтение в потоковом режиме: %1</translation>
    </message>
    <message>
        <location filename="../src/TextDocumentFormat.cpp" line="127"/>
        <source>The document format does not support writing of documents: %1</source>
        <translation>Формат документа не поддерживает запись документов: %1</translation>
    </message>
    <message>
        <location filename="../src/TextDocumentFormat.cpp" line="136"/>
        <source>The document format does not support writing of documents in streaming mode: %1</source>
        <translation>Формат документа не позволяет запись в потоковом режиме: %1</translation>
    </message>
</context>
<context>
    <name>U2::U2DbiL10n</name>
    <message>
        <location filename="../src/sqlite_dbi/assembly/MultiTableAssemblyAdapter.cpp" line="125"/>
        <source>Failed to detect assembly storage format: %1</source>
        <translation>Не удалось определить формат хранения сборки: %1</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/assembly/MultiTableAssemblyAdapter.cpp" line="149"/>
        <location filename="../src/sqlite_dbi/assembly/MultiTableAssemblyAdapter.cpp" line="153"/>
        <location filename="../src/sqlite_dbi/assembly/MultiTableAssemblyAdapter.cpp" line="158"/>
        <source>Failed to parse packed row range info %1</source>
        <translation>Failed to parse packed row range info %1</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteAssemblyDbi.cpp" line="80"/>
        <source>There is no assembly object with the specified id.</source>
        <translation>Нет объекта сборки с указанным идентификатором.</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteAssemblyDbi.cpp" line="93"/>
        <source>Unsupported reads storage type: %1</source>
        <translation>Unsupported reads storage type: %1</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteAssemblyDbi.cpp" line="359"/>
        <source>Packing method is not supported: %1</source>
        <translation>Packing method is not supported: %1</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteAssemblyDbi.cpp" line="443"/>
        <source>Packed data are empty!</source>
        <translation>Packed data are empty!</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteAssemblyDbi.cpp" line="450"/>
        <source>Packing method prefix is not supported: %1</source>
        <translation>Префикс метода упаковки не поддерживается: %1</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteAssemblyDbi.cpp" line="458"/>
        <source>Data are corrupted, no name end marker found: %1</source>
        <translation>Данные повреждены, маркер окончания имени не найден: %1</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteAssemblyDbi.cpp" line="467"/>
        <source>Data are corrupted, no sequence end marker found: %1</source>
        <translation>Данные повреждены, маркер конца последовательности не найден: %1</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteAssemblyDbi.cpp" line="476"/>
        <source>Data are corrupted, no CIGAR end marker found: %1</source>
        <translation>Данные повреждены, маркер конца CIGAR не найден: %1</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteAssemblyDbi.cpp" line="495"/>
        <source>Data are corrupted, no rnext end marker found: %1</source>
        <translation>Данные повреждены, маркер rnext не найден: %1</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteAssemblyDbi.cpp" line="510"/>
        <source>Can not convert pnext to a number: %1</source>
        <translation>Невозможно конвертировать pnext в число: %1</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteDbi.cpp" line="227"/>
        <source>Error checking SQLite database: %1!</source>
        <translation>Ошибка в базе данных SQLite: %1!</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteDbi.cpp" line="240"/>
        <source>Error creating table: %1, error: %2</source>
        <translation>Error creating table: %1, error: %2</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteDbi.cpp" line="278"/>
        <source>Not a %1 SQLite database: %2</source>
        <translation>Not a %1 SQLite database: %2</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteDbi.cpp" line="343"/>
        <source>Database is already opened!</source>
        <translation>База данных уже открыта!</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteDbi.cpp" line="347"/>
        <source>Illegal database state: %1</source>
        <translation>Недопустимое состояние базы данных: %1</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteDbi.cpp" line="353"/>
        <source>URL is not specified</source>
        <translation>Не задан путь</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteDbi.cpp" line="284"/>
        <source>The file was created with a newer version of UGENE. Current version: %1, minimum version required by database: %2. File: %3.</source>
        <translation>Файл был создан с более новой версией UGENE. Текущая версия: %1, минимальная версия, требуемая базой данных: %2. Файл: %3.</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteDbi.cpp" line="367"/>
        <source>Error opening SQLite database: &apos;%1&apos;. Error: %2</source>
        <translation>Ошибка при открытии базы данных SQLite: &apos;%1&apos;. Ошибка: %2</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteDbi.cpp" line="421"/>
        <source>Database is already closed!</source>
        <translation>База данных уже закрыта!</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteDbi.cpp" line="425"/>
        <source>Illegal database state %1!</source>
        <translation>Illegal database state %1!</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteDbi.cpp" line="431"/>
        <source>Can&apos;t synchronize database state</source>
        <translation>Не удается синхронизировать состояние базы данных</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteDbi.cpp" line="450"/>
        <source>Failed to close database: %1, err: %2</source>
        <translation>Failed to close database: %1, err: %2</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteFeatureDbi.cpp" line="146"/>
        <source>Annotation table object not found.</source>
        <translation>Annotation table object not found.</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteModDbi.cpp" line="147"/>
        <source>An object single modification step not found!</source>
        <translation>An object single modification step not found!</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteMsaDbi.cpp" line="391"/>
        <source>Msa object not found</source>
        <translation>Объект MSA не найден</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteMsaDbi.cpp" line="526"/>
        <location filename="../src/sqlite_dbi/SQLiteMsaDbi.cpp" line="541"/>
        <location filename="../src/sqlite_dbi/SQLiteMsaDbi.cpp" line="710"/>
        <source>Msa object not found!</source>
        <translation>Msa object not found!</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteMsaDbi.cpp" line="616"/>
        <location filename="../src/sqlite_dbi/SQLiteMsaDbi.cpp" line="764"/>
        <location filename="../src/sqlite_dbi/SQLiteMsaDbi.cpp" line="801"/>
        <source>Msa row not found!</source>
        <translation>Msa row not found!</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteMsaDbi.cpp" line="1251"/>
        <location filename="../src/sqlite_dbi/SQLiteMsaDbi.cpp" line="1261"/>
        <source>An error occurred during updating an msa length</source>
        <translation>Произошла ошибка при обновлении длины msa</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteObjectDbi.cpp" line="279"/>
        <source>Not an object! Id: %1, type: %2</source>
        <translation>Not an object! Id: %1, type: %2</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteObjectDbi.cpp" line="303"/>
        <source>Unknown object type! Id: %1, type: %2</source>
        <translation>Неизвестный тип объекта! Идентификатор: %1, тип: %2</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteObjectDbi.cpp" line="617"/>
        <source>Can&apos;t undo an operation for the object!</source>
        <translation>Can&apos;t undo an operation for the object!</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteObjectDbi.cpp" line="700"/>
        <source>Can&apos;t redo an operation for the object!</source>
        <translation>Can&apos;t redo an operation for the object!</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteObjectDbi.cpp" line="870"/>
        <source>Object not found!</source>
        <translation>Object not found!</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteObjectDbi.cpp" line="932"/>
        <location filename="../src/sqlite_dbi/SQLiteObjectDbi.cpp" line="945"/>
        <source>Object not found.</source>
        <translation>Object not found.</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteObjectDbi.cpp" line="982"/>
        <source>Folder not found: %1</source>
        <translation>Folder not found: %1</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteSequenceDbi.cpp" line="74"/>
        <source>Sequence object not found.</source>
        <translation>Sequence object not found.</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteVariantDbi.cpp" line="103"/>
        <location filename="../src/sqlite_dbi/SQLiteVariantDbi.cpp" line="136"/>
        <source>Sequence name is not set!</source>
        <translation>Sequence name is not set!</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteUdrDbi.cpp" line="56"/>
        <location filename="../src/sqlite_dbi/SQLiteUdrDbi.cpp" line="67"/>
        <source>An error occurred during updating UDR</source>
        <translation>An error occurred during updating UDR</translation>
    </message>
    <message>
        <location filename="../src/sqlite_dbi/SQLiteAttributeDbi.cpp" line="237"/>
        <source>Unsupported attribute type: %1</source>
        <translation>Неподдерживаемый тип атрибута: %1</translation>
    </message>
</context>
<context>
    <name>U2::VectorNtiSequenceFormat</name>
    <message>
        <location filename="../src/VectorNtiSequenceFormat.cpp" line="44"/>
        <source>Vector NTI sequence</source>
        <translation>Последовательность Vector NTI</translation>
    </message>
    <message>
        <location filename="../src/VectorNtiSequenceFormat.cpp" line="45"/>
        <source>Vector NTI sequence format is a rich format based on NCBI GenBank format for storing sequences and associated annotations</source>
        <translation>Формат Vector NTI это формат основанный на формате NCBI GenBank для хранения последовательностей и аннотаций</translation>
    </message>
    <message>
        <location filename="../src/VectorNtiSequenceFormat.cpp" line="296"/>
        <location filename="../src/VectorNtiSequenceFormat.cpp" line="310"/>
        <location filename="../src/VectorNtiSequenceFormat.cpp" line="314"/>
        <location filename="../src/VectorNtiSequenceFormat.cpp" line="319"/>
        <location filename="../src/VectorNtiSequenceFormat.cpp" line="325"/>
        <source>Error writing document</source>
        <translation>Ошибка записи</translation>
    </message>
</context>
</TS>
