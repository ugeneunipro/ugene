<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru_RU">
<context>
    <name>Primer3Dialog</name>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="36"/>
        <source>Main</source>
        <translation>Основные параметры</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="300"/>
        <source>Targets</source>
        <translation>Целевые регионы</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="775"/>
        <location filename="../src/Primer3Dialog.ui" line="814"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A list of product size ranges, for example:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot;font-weight:600;&quot;&gt; 150-250 100-300 301-400 &lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 first tries to pick primers in the first range. If that is not possible, it goes to the next range and tries again. It continues in this way until it has either picked all necessary primers or until there are no more ranges. For technical reasons this option makes much lighter computational demands than the Product Size option.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Список диапазонов размера продукта, например:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot;font-weight:600;&quot;&gt; 150-250 100-300 301-400 &lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="821"/>
        <location filename="../src/Primer3Dialog.ui" line="834"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This selection indicates what mispriming library (if any) Primer3 should use to screen for interspersed repeats or for other sequence to avoid as a location for primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Этот параметр указывает какую библиотеку будет использовать Primer3.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="785"/>
        <location filename="../src/Primer3Dialog.ui" line="851"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum number of primer pairs to return.&lt;/p&gt;&lt;p&gt;Primer pairs returned are sorted by their &amp;quot;quality&amp;quot;, in other words by the value of the objective function (where a lower number indicates a better primer pair).&lt;/p&gt;&lt;p&gt;Caution: setting this parameter to a large value will increase running time.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимальное число пар праймеров.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="867"/>
        <location filename="../src/Primer3Dialog.ui" line="883"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum stability for the last five 3&apos; bases of a left or right primer.&lt;/p&gt;&lt;p&gt;Bigger numbers mean more stable 3&apos; ends. The value is the maximum delta G (kcal/mol) for duplex disruption for the five 3&apos; bases as calculated using the Nearest-Neighbor parameter values specified by the option of &apos;Table of thermodynamic parameters&apos;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Макс. стабильности для последних пяти 3&apos; оснований левого или правого праймеров. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="798"/>
        <location filename="../src/Primer3Dialog.ui" line="918"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed weighted similarity with any sequence in Mispriming Library.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимально допустимое взвешенное сходство с любой последовательностью из библиотеки ошибочного праймирования.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="934"/>
        <location filename="../src/Primer3Dialog.ui" line="950"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed sum of similarities of a primer pair (one similarity for each primer) with any single sequence in Mispriming Library. &lt;/p&gt;&lt;p&gt;Library sequence weights are not used in computing the sum of similarities.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимально допустимая сумма сходств пар праймеров с любой последовательностью из библиотеки ошибочного праймирования.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="121"/>
        <source>Pick left primer</source>
        <translation>Выбрать левый праймер</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="175"/>
        <source>or use left primer below</source>
        <translation>или использовать этот левый праймер</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="999"/>
        <source>General Settings</source>
        <translation>Общие настройки</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1002"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section covers such settings as primer size, temperature, GC and other general settings.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;В этом разделе рассматриваются такие параметры, как размер праймера, температура, GC и другие общие настройки.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1057"/>
        <location filename="../src/Primer3Dialog.ui" line="1099"/>
        <location filename="../src/Primer3Dialog.ui" line="1236"/>
        <location filename="../src/Primer3Dialog.ui" line="1282"/>
        <location filename="../src/Primer3Dialog.ui" line="1302"/>
        <source>Min</source>
        <translation>Минимум</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1381"/>
        <location filename="../src/Primer3Dialog.ui" line="1461"/>
        <location filename="../src/Primer3Dialog.ui" line="1598"/>
        <location filename="../src/Primer3Dialog.ui" line="1726"/>
        <location filename="../src/Primer3Dialog.ui" line="1736"/>
        <source>Opt</source>
        <translation>Оптимум</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1145"/>
        <location filename="../src/Primer3Dialog.ui" line="1194"/>
        <location filename="../src/Primer3Dialog.ui" line="1262"/>
        <location filename="../src/Primer3Dialog.ui" line="1272"/>
        <location filename="../src/Primer3Dialog.ui" line="1660"/>
        <source>Max</source>
        <translation>Максимум</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1174"/>
        <source>Primer Tm</source>
        <translation>Т. плав. праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1259"/>
        <location filename="../src/Primer3Dialog.ui" line="1279"/>
        <location filename="../src/Primer3Dialog.ui" line="1362"/>
        <location filename="../src/Primer3Dialog.ui" line="1378"/>
        <location filename="../src/Primer3Dialog.ui" line="1410"/>
        <location filename="../src/Primer3Dialog.ui" line="1432"/>
        <location filename="../src/Primer3Dialog.ui" line="1448"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum lengths (in bases) of a primer oligo.&lt;/p&gt;&lt;p&gt;Primer3 will not pick primers shorter than Min or longer than Max, and with default arguments will attempt to pick primers close with size close to Opt.&lt;/p&gt;&lt;p&gt;Min cannot be smaller than 1. Max cannot be larger than 36. (This limit is governed by maximum oligo size for which melting-temperature calculations are valid.)&lt;/p&gt;&lt;p&gt;Min cannot be greater than Max.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальная, оптимальная и максимальная длины олигонуклеотида праймера.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1041"/>
        <location filename="../src/Primer3Dialog.ui" line="1070"/>
        <location filename="../src/Primer3Dialog.ui" line="1096"/>
        <location filename="../src/Primer3Dialog.ui" line="1158"/>
        <location filename="../src/Primer3Dialog.ui" line="1171"/>
        <location filename="../src/Primer3Dialog.ui" line="1595"/>
        <location filename="../src/Primer3Dialog.ui" line="1657"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum melting temperatures (Celsius) for a primer oligo.&lt;/p&gt;&lt;p&gt;Primer3 will not pick oligos with temperatures smaller than Min or larger than Max, and with default conditions will try to pick primers with melting temperatures close to Opt.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальная, оптимальная и максимальная температуры плавления олигонуклеотида праймера.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1319"/>
        <location filename="../src/Primer3Dialog.ui" line="1709"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum percentage of Gs and Cs in any primer or oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;
</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальное, оптимальное и максимальное процентное содержание нуклеотидов G и C в праймере или олигонуклеотиде.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;
</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1191"/>
        <location filename="../src/Primer3Dialog.ui" line="1233"/>
        <location filename="../src/Primer3Dialog.ui" line="1391"/>
        <location filename="../src/Primer3Dialog.ui" line="1582"/>
        <location filename="../src/Primer3Dialog.ui" line="1723"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum percentage of Gs and Cs in any primer or oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальное, оптимальное и максимальное процентное содержание Gs и Cs в праймере или олигонуклеотиде.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1142"/>
        <location filename="../src/Primer3Dialog.ui" line="1289"/>
        <location filename="../src/Primer3Dialog.ui" line="1299"/>
        <location filename="../src/Primer3Dialog.ui" line="1506"/>
        <location filename="../src/Primer3Dialog.ui" line="1538"/>
        <location filename="../src/Primer3Dialog.ui" line="1560"/>
        <location filename="../src/Primer3Dialog.ui" line="1733"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum, optimum, and maximum melting temperature of the amplicon.&lt;/p&gt;&lt;p&gt;Primer3 will not pick a product with melting temperature less than min or greater than max. If Opt is supplied and the Penalty Weights for Product Size are non-0 Primer3 will attempt to pick an amplicon with melting temperature close to Opt.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальная, оптимальная и максимальная температуры плавления ампликона.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1012"/>
        <location filename="../src/Primer3Dialog.ui" line="1207"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Maximum acceptable (unsigned) difference between the melting temperatures of the left and right primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимально допустимое различие между температурами плавления левого и правого праймеров.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1135"/>
        <source>Table of thermodynamic parameters</source>
        <translation>Таблица термодинамических параметров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4350"/>
        <source>Primers</source>
        <translation>Праймеры</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5006"/>
        <source>Primer Pairs</source>
        <translation>Пары праймеров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6363"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section allows to set output annotations&apos; settings.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;В этой секции можно задать настройки для аннотаций.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1674"/>
        <source>Breslauer et. al 1986</source>
        <translation>Breslauer et. al 1986</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1679"/>
        <location filename="../src/Primer3Dialog.ui" line="2609"/>
        <source>SantaLucia 1998</source>
        <translation>SantaLucia 1998</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="20"/>
        <source>Primer Designer</source>
        <translation>Дизайнер праймеров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="824"/>
        <source>Mispriming/Repeat library</source>
        <translation>Библиотека повторов/ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="788"/>
        <source>Number to return</source>
        <translation>Результатов не более</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2166"/>
        <source>Max template mispriming</source>
        <translation>Макс. шаблон ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2243"/>
        <source>Pair max template mispriming</source>
        <translation>Парный макс. шаблон ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="185"/>
        <source>or use oligo below</source>
        <translation>или использовать эту oligo</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="162"/>
        <source>Pick right primer</source>
        <translation>Выбрать правый праймер</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="192"/>
        <source>or use right primer below (5&apos; to 3&apos; on opposite strand)</source>
        <translation>или использовать этот правый праймер (5&apos; в 3&apos; в противоположном направлении)</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1451"/>
        <source>Primer size</source>
        <translation>Размер праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1292"/>
        <location filename="../src/Primer3Dialog.ui" line="5073"/>
        <source>Product Tm</source>
        <translation>Т. плав. продукта</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1323"/>
        <source>Primer GC%</source>
        <translation>GC состав %</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1015"/>
        <source>Max Tm difference</source>
        <translation>Макс. разница Т. плав</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2912"/>
        <source>Inside target penalty</source>
        <translation>Внутренний целевой штраф</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2467"/>
        <source>Outside target penalty</source>
        <translation>Внешний целевой штраф</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2554"/>
        <source>Max poly-X</source>
        <translation>Макс. poly-X</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2935"/>
        <source>CG clamp</source>
        <translation>Зажим CG</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2597"/>
        <location filename="../src/Primer3Dialog.ui" line="2808"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Specifies the salt correction formula for the melting temperature calculation.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Задает коррекционную формулу для расчета температуры плавления.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3159"/>
        <source>Show debugging info</source>
        <translation>Показывать отладочную информацию</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3352"/>
        <location filename="../src/Primer3Dialog.ui" line="3494"/>
        <location filename="../src/Primer3Dialog.ui" line="3559"/>
        <location filename="../src/Primer3Dialog.ui" line="3597"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer Size for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр размера праймера для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3388"/>
        <location filename="../src/Primer3Dialog.ui" line="3436"/>
        <location filename="../src/Primer3Dialog.ui" line="3484"/>
        <location filename="../src/Primer3Dialog.ui" line="3630"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer Tm for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Tm олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3420"/>
        <location filename="../src/Primer3Dialog.ui" line="3524"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer GC% for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;GC% олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3407"/>
        <location filename="../src/Primer3Dialog.ui" line="3581"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer GC% for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;GC% олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6120"/>
        <source>RT-PCR</source>
        <translation>ПЦР с обратной транскрипцией</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6132"/>
        <source>Design primers for RT-PCR analysis</source>
        <translation>Дизайн праймеров для анализа RT-PCR</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6152"/>
        <source>Exon annotation name:</source>
        <translation>Имя экзон аннотации:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6162"/>
        <source>exon</source>
        <translation>exon</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6187"/>
        <source>Minimum exon junction overlap size</source>
        <translation>Минимальный размер перекрытия соединения экзона</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6251"/>
        <source>Exon range:</source>
        <translation>Диапазон экзона:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6298"/>
        <source>Max number of pairs to query:</source>
        <translation>Максимальное число пар для запроса:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6025"/>
        <source>Min sequence quality:</source>
        <translation>Минимальное качество последовательности:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6048"/>
        <source>Min 3&apos; sequence quality:</source>
        <translation>Минимальное 3&apos;качество последовательности:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6071"/>
        <source>Sequence quality range min:</source>
        <translation>Минимум интервала качества последовательности:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6094"/>
        <source>Sequence quality range max:</source>
        <translation>Макс. интервала качества последовательности:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6411"/>
        <source>Help</source>
        <translation>Помощь</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6431"/>
        <source>Save settings</source>
        <translation>Сохранить настройки</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6438"/>
        <source>Load settings</source>
        <translation>Загрузить настройки</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6445"/>
        <source>Reset form</source>
        <translation>Сброс параметров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6452"/>
        <source>Pick primers</source>
        <translation>Подобрать</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1132"/>
        <location filename="../src/Primer3Dialog.ui" line="1670"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Specifies details of melting temperature calculation.&lt;/p&gt;&lt;p&gt;First method uses the table of thermodynamic parameters from the paper [Breslauer KJ, et. al (1986), Proc Natl Acad Sci 83:4746-50 http://dx.doi.org/10.1073/pnas.83.11.3746]&lt;/p&gt;&lt;p&gt;Second method (recommended) was suggested in the paper [SantaLucia JR (1998), Proc Natl Acad Sci 95:1460-65 http://dx.doi.org/10.1073/pnas.95.4.1460].&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Указывает детали расчета температуры плавления.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2811"/>
        <source>Salt correction formula</source>
        <translation>Коррекционная формула</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2600"/>
        <location filename="../src/Primer3Dialog.ui" line="2604"/>
        <source>Schildkraut and Lifson 1965</source>
        <translation>Schildkraut and Lifson 1965</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2614"/>
        <source>Owczarzy et. 2004</source>
        <translation>Owczarzy et. 2004</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2945"/>
        <source>Concentration of dNTPs</source>
        <translation>Концентрация dNTP</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2841"/>
        <location filename="../src/Primer3Dialog.ui" line="2958"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Maximum number of unknown bases (N) allowable in any primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимальное число неизвестных оснований (N), допустимых в праймере.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2551"/>
        <location filename="../src/Primer3Dialog.ui" line="2873"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowable length of a mononucleotide repeat, for example AAAAAA.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Макс. допустимая для мононуклеотидного повтора, например AAAAAA.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2672"/>
        <location filename="../src/Primer3Dialog.ui" line="2795"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This parameter is the index of the first base in the input sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Индекс первого основания во входной последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2932"/>
        <location filename="../src/Primer3Dialog.ui" line="2977"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Require the specified number of consecutive Gs and Cs at the 3&apos; end of both the left and right primer.&lt;/p&gt;&lt;p&gt;This parameter has no effect on the internal oligo if one is requested.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Зажим CG.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2366"/>
        <location filename="../src/Primer3Dialog.ui" line="3048"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The millimolar (mM) concentration of monovalent salt cations (usually KCl) in the PCR.&lt;/p&gt;&lt;p&gt;Primer3 uses this argument to calculate oligo and primer melting temperatures.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Концентрация одновалентных катионов.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2441"/>
        <location filename="../src/Primer3Dialog.ui" line="2628"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The millimolar concentration of divalent salt cations (usually MgCl^(2+)) in the PCR.&lt;/p&gt;&lt;p&gt;Primer3 converts concentration of divalent cations to concentration of monovalent cations using following formula:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;[Monovalent cations] = [Monovalent cations] + 120*(([divalent cations] - [dNTP])^0.5)&lt;/span&gt;&lt;/p&gt;&lt;p&gt;In addition, if the specified concentration of dNTPs is larger than the concentration of divalent cations then the effect of the divalent cations is not considered.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Концентрация двухвалентных катионов.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2653"/>
        <location filename="../src/Primer3Dialog.ui" line="2942"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The millimolar concentration of the sum of all deoxyribonucleotide triphosphates.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Концентрация dNTP.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2405"/>
        <location filename="../src/Primer3Dialog.ui" line="3064"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A value to use as nanomolar (nM) concentration of each annealing oligo over the course the PCR.&lt;/p&gt;&lt;p&gt;Primer3 uses this argument to esimate oligo melting temperatures.&lt;/p&gt;&lt;p&gt;The default (50nM) works well with the standard protocol used at the Whitehead/MIT Center for Genome Research --0.5 microliters of 20 micromolar concentration for each primer in a 20 microliter reaction with 10 nanograms template, 0.025 units/microliter Taq polymerase in 0.1 mM each dNTP, 1.5mM MgCl2, 50mM KCl, 10mM Tris-HCL (pH 9.3) using 35 cycles with an annealing temperature of 56 degrees Celsius.&lt;/p&gt;&lt;p&gt;The value of this parameter is less than the actual concentration of oligos in the initial reaction mix because  it is the concentration of annealing oligos, which in turn depends on the amount of template (including PCR product) in a given cycle. This concentration increases a great deal during a PCR; fortunately PCR seems quite robust for a variety of oligo melting temperatures.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Концентрация отжига олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3242"/>
        <source>Lowercase masking</source>
        <translation>Строчная маскировка</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3280"/>
        <source>Internal Oligo</source>
        <translation>Внутренний олигонуклеотид</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3283"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Parameters governing choice of internal oligos are analogous to the parameters governing choice of primer pairs.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Параметры, регулирующие выбор внутренних олигонуклеотидов аналогичны параметрам, регулирующих выбор пар праймеров.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3449"/>
        <location filename="../src/Primer3Dialog.ui" line="3504"/>
        <location filename="../src/Primer3Dialog.ui" line="3511"/>
        <location filename="../src/Primer3Dialog.ui" line="3610"/>
        <source>Min:</source>
        <translation>Мин:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3368"/>
        <location filename="../src/Primer3Dialog.ui" line="3456"/>
        <location filename="../src/Primer3Dialog.ui" line="3477"/>
        <location filename="../src/Primer3Dialog.ui" line="3675"/>
        <source>Opt:</source>
        <translation>Опт:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3375"/>
        <location filename="../src/Primer3Dialog.ui" line="3463"/>
        <location filename="../src/Primer3Dialog.ui" line="3470"/>
        <location filename="../src/Primer3Dialog.ui" line="3617"/>
        <source>Max:</source>
        <translation>Макс:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3965"/>
        <source>Max #Ns</source>
        <translation>Макс N</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4332"/>
        <source>Penalty Weights</source>
        <translation>Веса штрафов</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4504"/>
        <source>Tm</source>
        <translation>Tm</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4416"/>
        <source>Size  </source>
        <translation>Размер  </translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4362"/>
        <source>GC%</source>
        <translation>GC%</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4369"/>
        <location filename="../src/Primer3Dialog.ui" line="4406"/>
        <location filename="../src/Primer3Dialog.ui" line="4423"/>
        <location filename="../src/Primer3Dialog.ui" line="4557"/>
        <location filename="../src/Primer3Dialog.ui" line="5015"/>
        <location filename="../src/Primer3Dialog.ui" line="5120"/>
        <location filename="../src/Primer3Dialog.ui" line="5495"/>
        <location filename="../src/Primer3Dialog.ui" line="5502"/>
        <location filename="../src/Primer3Dialog.ui" line="5580"/>
        <location filename="../src/Primer3Dialog.ui" line="5663"/>
        <source>Lt:</source>
        <translation>Мч:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4392"/>
        <location filename="../src/Primer3Dialog.ui" line="4399"/>
        <location filename="../src/Primer3Dialog.ui" line="4478"/>
        <location filename="../src/Primer3Dialog.ui" line="4564"/>
        <location filename="../src/Primer3Dialog.ui" line="5096"/>
        <location filename="../src/Primer3Dialog.ui" line="5113"/>
        <location filename="../src/Primer3Dialog.ui" line="5557"/>
        <location filename="../src/Primer3Dialog.ui" line="5587"/>
        <location filename="../src/Primer3Dialog.ui" line="5646"/>
        <location filename="../src/Primer3Dialog.ui" line="5670"/>
        <source>Gt:</source>
        <translation>Бч:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4781"/>
        <source>#N&apos;s</source>
        <translation>Количество N</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4833"/>
        <location filename="../src/Primer3Dialog.ui" line="6009"/>
        <source>Sequence quality</source>
        <translation>Качество последовательности</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4862"/>
        <source>End sequence quality</source>
        <translation>Качество конца последовательности</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4891"/>
        <source>Position penalty</source>
        <translation>Позиция штрафа</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4920"/>
        <source>End stability</source>
        <translation>Стабильность конца</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5106"/>
        <source>Product size</source>
        <translation>Размер продукта</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6144"/>
        <source>mRNA sequence</source>
        <translation>Последовательность mRNA</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6199"/>
        <source>At 5&apos; side (bp):</source>
        <translation>Со стороны 5&apos; (нк):</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6229"/>
        <source>At 3&apos;side (bp)</source>
        <translation>Со стороны 3&apos; (нк)</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6286"/>
        <source>Primer product must span at least one intron on the corresponding genomic DNA</source>
        <translation>Продукт праймера должен охватывать по крайней мере один интрон на соответствующей геномной ДНК</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6000"/>
        <source>Sequence Quality</source>
        <translation>Качество последовательности</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="47"/>
        <source>Select the Task for primer selection</source>
        <translation>Выберите задачу подбора праймеров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="44"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This tag tells Primer3 what task to perform. Legal values are:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;generic&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Design a primer pair (the classic Primer3 task) if the &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_LEFT_PRIMER=1&lt;/span&gt;, and &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_RIGHT_PRIMER=1&lt;/span&gt;. In addition, pick an internal hybridization oligo if &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_INTERNAL_OLIGO=1&lt;/span&gt;.&lt;/p&gt;&lt;p&gt;NOTE: If &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_LEFT_PRIMER=1&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_RIGHT_PRIMER=0&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_INTERNAL_OLIGO=1&lt;/span&gt;, then behaves similarly to &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_TASK=pick_primer_list&lt;/span&gt;.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;check_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 only checks the primers provided in &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_PRIMER&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_INTERNAL_OLIGO&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_PRIMER_REVCOMP&lt;/span&gt;. It is the only task that does not require a sequence. However, if &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_TEMPLATE&lt;/span&gt; is provided, it is used.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_primer_list&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick all primers in &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_TEMPLATE&lt;/span&gt; (possibly limited by &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_INCLUDED_REGION&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_EXCLUDED_REGION&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_PRIMER_PAIR_OK_REGION_LIST&lt;/span&gt;, etc.). Returns the primers sorted by quality starting with the best primers. If &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_LEFT_PRIMER&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_RIGHT_PRIMER&lt;/span&gt; is selected Primer3 does not to pick primer pairs but generates independent lists of left primers, right primers, and, if requested, internal oligos.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick primers suited to sequence a region. &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_TARGET&lt;/span&gt; can be used to indicate several targets. The position of each primer is calculated for optimal sequencing results.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;pick_cloning_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick primers suited to clone a gene were the start nucleotide and the end nucleotide of the PCR fragment must be fixed, for example to clone an ORF. &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_INCLUDED_REGION&lt;/span&gt; must be used to indicate the first and the last nucleotide. Due to these limitations Primer3 can only vary the length of the primers. Set &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_ANYWAY=1&lt;/span&gt; to obtain primers even if they violate specific constraints.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_discriminative_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick primers suited to select primers which bind with their end at a specific position. This can be used to force the end of a primer to a polymorphic site, with the goal of discriminating between sequence variants. &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_TARGET&lt;/span&gt; must be used to provide a single target indicating the last nucleotide of the left (nucleotide before the first nucleotide of the target) and the right primer (nucleotide following the target). The primers border the &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_TARGET&lt;/span&gt;. Due to these limitations Primer3 can only vary the length of the primers. Set &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_ANYWAY=1&lt;/span&gt; to obtain primers even if they violate specific constraints.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;
   &lt;head/&gt;
   &lt;body&gt;
      &lt;p&gt;Этот параметр сообщает Primer3, какую задачу выполнять. Возможные значения:&lt;/p&gt;
      &lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;generic&lt;/span&gt;&lt;/p&gt;
      &lt;p&gt;Дизайн праймеров (классическая задача Primer3) если &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_LEFT_PRIMER=1&lt;/span&gt; и &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_RIGHT_PRIMER=1&lt;/span&gt;. Кроме того, выберите внутренний гибридизационный олиго, если &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_INTERNAL_OLIGO=1&lt;/span&gt;.&lt;/p&gt;
      &lt;p&gt;ЗАМЕЧАНИЕ: Если &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_LEFT_PRIMER=1&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_RIGHT_PRIMER=0&lt;/span&gt; и &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_INTERNAL_OLIGO=1&lt;/span&gt;, тогда результат такой же как и в &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_TASK=pick_primer_list&lt;/span&gt;.&lt;/p&gt;
      &lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;check_primers&lt;/span&gt;&lt;/p&gt;
      &lt;p&gt;Primer3 только проверяет праймеры, представленные в
&lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_PRIMER&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_INTERNAL_OLIGO&lt;/span&gt; и &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_PRIMER_REVCOMP&lt;/span&gt;. Это единственная задача, которая не требует последовательности. Тем не менее, если &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_TEMPLATE&lt;/span&gt; предоставлена, она будет использоваться.&lt;/p&gt;
      &lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_primer_list&lt;/span&gt;&lt;/p&gt;
      &lt;p&gt;Выбрать все праймеры в &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_TEMPLATE&lt;/span&gt; (возможно ограничение &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_INCLUDED_REGION&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_EXCLUDED_REGION&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_PRIMER_PAIR_OK_REGION_LIST&lt;/span&gt;, и т.д.). Возвращает праймеры, отсортированные по качеству, начиная с лучших праймеров. Если &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_LEFT_PRIMER&lt;/span&gt; и &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_RIGHT_PRIMER&lt;/span&gt; выбран, Primer3 не выбирает пары праймеров, а генерирует независимые списки левых праймеров, правых праймеров и, если требуется, внутренних олигонуклеотидов.&lt;/p&gt;
      &lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;&lt;/p&gt;
      &lt;p&gt;Выберите праймеры, подходящие для секвенирования области. &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_TARGET&lt;/span&gt; может использоваться для обозначения нескольких целей. Положение каждого праймера рассчитывается для оптимальных результатов секвенирования.&lt;/p&gt;
      &lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;pick_cloning_primers&lt;/span&gt;&lt;/p&gt;
      &lt;p&gt;Выберите праймеры, подходящие для клонирования гена, где начальный нуклеотид и конечный нуклеотид фрагмента ПЦР должны быть зафиксированы, например, для клонирования ORF. &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_INCLUDED_REGION&lt;/span&gt; должен использоваться для обозначения первого и последнего нуклеотидов. Из-за этих ограничений Primer3 может изменять только длину праймеров. Задайте &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_ANYWAY=1&lt;/span&gt; для получения праймеров, даже если они нарушают определенные ограничения.&lt;/p&gt;
      &lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_discriminative_primers&lt;/span&gt;&lt;/p&gt;
      &lt;p&gt;Подбор таких праймеров, которые связываются своим концом в определенном положении. Это может быть использовано для принудительного прикрепления конца праймера к полиморфному сайту с целью различения вариантов последовательности. &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_TARGET&lt;/span&gt; должен использоваться для получения единственной цели, указывающей последний нуклеотид слева (нуклеотид перед первым целевым нуклеотидом) и правый праймер (нуклеотид, следующий за целевым). Праймеры граничат с &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_TARGET&lt;/span&gt;. Из-за этих ограничений Primer3 может изменять только длину праймеров. Задайте &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_ANYWAY=1&lt;/span&gt; для получения праймеров, даже если они нарушают определенные ограничения.&lt;/p&gt;
   &lt;/body&gt;
&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="58"/>
        <source>generic</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="63"/>
        <source>pick_sequencing_primers</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="68"/>
        <source>pick_primer_list</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="73"/>
        <source>check_primers</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="78"/>
        <source>pick_cloning_primers</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="83"/>
        <source>pick_discriminative_primers</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="234"/>
        <location filename="../src/Primer3Dialog.ui" line="268"/>
        <source>5&apos; Overhang:</source>
        <translation>5&apos; перекрытие:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="231"/>
        <location filename="../src/Primer3Dialog.ui" line="241"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The provided sequence is added to the 5&apos; end of the left primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Предоставленная последовательность добавляется к 5&apos;-концу левого праймера.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="265"/>
        <location filename="../src/Primer3Dialog.ui" line="275"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The provided sequence is added to the 5&apos; end of the right primer. It is reverse complementary to the template sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Предоставленная последовательность добавляется к 5&apos;-концу правого праймера. Она обратно комплементарна последовательности шаблона.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="310"/>
        <source>Overlap Junction List</source>
        <translation>Список перекрывающихся соединений</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="320"/>
        <source>Excluded Regions</source>
        <translation>Исключаемые регионы</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="330"/>
        <source>Pair OK Region List</source>
        <translation>Список ОК парных регионов</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="340"/>
        <source>Included region</source>
        <translation>Включенные регионы</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="352"/>
        <source>Start Codon Position</source>
        <translation>Позиция стартового кодона</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="364"/>
        <source>Start Codon Sequence</source>
        <translation>Последовательность стартового кодона</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="374"/>
        <source>Force Left Primer Start</source>
        <translation>Принудительное начало левого праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="384"/>
        <source>Force Left Primer End</source>
        <translation>Принудительный конец левого праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="394"/>
        <source>Five Matches on Primer&apos;s 5&apos;</source>
        <translation>Пять совпадений на 5&apos; конце</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="404"/>
        <source>Five Matches on Internal Oligo&apos;s  5&apos;</source>
        <translation>Пять совпадений на 5&apos; конце внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="297"/>
        <location filename="../src/Primer3Dialog.ui" line="415"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If one or more Targets is specified then a legal primer pair must flank at least one of them. A Target might be a simple sequence repeat site (for example a CA repeat) or a single-base-pair polymorphism. The value should be a space-separated list of &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs where &lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt; is the index of the first base of a Target, and length is its &lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если указан один или несколько Целевых регионов, то пара праймеров должна обрамлять как минимум один из них. Целевой регион может быть простым сайтом повтора последовательности (например, повтором CA) или полиморфизмом с одной парой оснований. Значение должно представлять собой разделенный пробелами список &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;начало&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt;&lt;/p&gt;&lt;p&gt;пар, где &lt;span style=&quot; font-weight:700;&quot;&gt;начало&lt;/span&gt; является порядковым номером первого нуклеотида Целевого региона, а его длина - это &lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="307"/>
        <location filename="../src/Primer3Dialog.ui" line="422"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If this list is not empty, then the forward OR the reverse primer must overlap one of these junction points by at least &lt;span style=&quot; font-weight:700;&quot;&gt;3 Prime Junction Overlap&lt;/span&gt; nucleotides at the 3&apos; end and at least &lt;span style=&quot; font-weight:700;&quot;&gt;5 Prime Junction Overlap&lt;/span&gt; nucleotides at the 5&apos; end.&lt;/p&gt;&lt;p&gt;In more detail: The junction associated with a given position is the space immediately to the right of that position in the template sequence on the strand given as input.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если этот список не пуст, то прямой ИЛИ обратный праймер должен перекрывать одну из этих точек из Списка Перекрывающихся Соединений как минимум на &lt;span style=&quot; font-weight:700;&quot;&gt;Перекрытие Первичного Соединения 3&lt;/span&gt; на 3&apos;-конце и не менее &lt;span style=&quot; font-weight:700;&quot;&gt;Перекрытие Первичного Соединения 5&lt;/span&gt; на 5&apos;-конце.&lt;/p&gt;&lt;p&gt;Более подробно: соединение, связанное с данной позицией, представляет собой пространство непосредственно справа от этой позиции в шаблонной последовательности на цепи, заданной в качестве входных данных.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="337"/>
        <location filename="../src/Primer3Dialog.ui" line="495"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A sub-region of the given sequence in which to pick primers. For example, often the first dozen or so bases of a sequence are vector, and should be excluded from consideration. The value for this parameter has the form&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;where &lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt; is the index of the first base to consider, and &lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt; is the number of subsequent bases in the primer-picking region.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Подобласть данной последовательности, в которой нужно выбрать праймеры. Например, часто первая дюжина или около того символов последовательности является вектором и должна быть исключена из рассмотрения. Значение этого параметра имеет вид &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;начало&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt;&lt;/p&gt;&lt;p&gt;где &lt;span style=&quot; font-weight:700;&quot;&gt;начало&lt;/span&gt; это индекс первого символа для рассмотрения, а &lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt; - количество последующих символов в области выбора праймера.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="522"/>
        <source>Force Right Primer End</source>
        <translation>Принудительный конец правого праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="381"/>
        <location filename="../src/Primer3Dialog.ui" line="535"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Forces the 3&apos; end of the left primer to be at the indicated position. Primers are also picked if they violate certain constraints. The default value indicates that the end of the left primer can be anywhere.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Заставляет 3&apos;-конец левого праймера находиться в указанной позиции. Праймеры также выбираются, если они нарушают определенные ограничения. Значение по умолчанию указывает, что конец левого праймера может быть где угодно.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="371"/>
        <location filename="../src/Primer3Dialog.ui" line="570"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Forces the 5&apos; end of the left primer to be at the indicated position. Primers are also picked if they violate certain constraints. The default value indicates that the start of the left primer can be anywhere.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Заставляет 5&apos;-конец левого праймера находиться в указанной позиции. Праймеры также выбираются, если они нарушают определенные ограничения. Значение по умолчанию указывает, что конец левого праймера может быть где угодно.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="401"/>
        <location filename="../src/Primer3Dialog.ui" line="612"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Five Matches on Primer&apos;s 5&apos;&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Пять совпадений на 5&apos; конце&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="622"/>
        <source>Five Matches on Primer&apos;s 3&apos;</source>
        <translation>Пять совпадений на 3&apos; конце</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="636"/>
        <location filename="../src/Primer3Dialog.ui" line="643"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Five Matches on Primer&apos;s 3&apos;&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Пять совпадений на 3&apos; конце&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="646"/>
        <source>Five Matches on Internal Oligo&apos;s 3&apos;</source>
        <translation>Пять совпадений на 3&apos; конце внутренного олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="347"/>
        <location filename="../src/Primer3Dialog.ui" line="653"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This parameter should be considered EXPERIMENTAL at this point. Please check the output carefully; some erroneous inputs might cause an error in Primer3.&lt;br&gt;&lt;br&gt;
Index of the first base of a start codon. This parameter allows Primer3 to select primer pairs to create in-frame amplicons e.g. to create a template for a fusion protein. Primer3 will attempt to select an in-frame left primer, ideally starting at or to the left of the start codon, or to the right if necessary. Negative values of this parameter are legal if the actual start codon is to the left of available sequence. If this parameter is non-negative Primer3 signals an error if the codon at the position specified by this parameter is not an ATG. A value less than or equal to -10^6 indicates that Primer3 should ignore this parameter.&lt;br&gt;&lt;br&gt;
Primer3 selects the position of the right primer by scanning right from the left primer for a stop codon. Ideally the right primer will end at or after the stop codon.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;На данный момент этот параметр следует считать ЭКСПЕРИМЕНТАЛЬНЫМ. Пожалуйста, внимательно проверьте выходные данные; некоторые ошибочные входные данные могут вызвать ошибку в Primer3.&lt;br&gt;&lt;br&gt;
Индекс первого символа стартового кодона. Этот параметр позволяет Primer3 выбирать пары праймеров для создания внутрирамочных ампликонов, например, для создания шаблона для слитого белка. Primer3 попытается выбрать левый праймер в рамке, в идеале начиная с начального кодона или слева от него, или справа, если необходимо. Отрицательные значения этого параметра допустимы, если фактический стартовый кодон находится слева от доступной последовательности. Если этот параметр неотрицательный, Primer3 сигнализирует об ошибке, если кодон в позиции, указанной этим параметром, не является ATG. Значение, меньшее или равное -10^6, указывает на то, что Primer3 должен игнорировать этот параметр.&lt;br&gt;&lt;br&gt;
Primer3 выбирает положение правого праймера, сканируя справа от левого праймера стоп-кодон. В идеале правильный праймер должен заканчиваться на стоп-кодоне или после него.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="693"/>
        <location filename="../src/Primer3Dialog.ui" line="716"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Forces the 5&apos; end of the right primer to be at the indicated position. Primers are also picked if they violate certain constraints. The default value indicates that the start of the right primer can be anywhere.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Заставляет 5&apos;-конец правого праймера находиться в указанной позиции. Праймеры также выбираются, если они нарушают определенные ограничения. Значение по умолчанию указывает, что конец левого праймера может быть где угодно.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="719"/>
        <source>Force Right Primer Start</source>
        <translation>Принудительный конец правого праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="519"/>
        <location filename="../src/Primer3Dialog.ui" line="732"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Forces the 3&apos; end of the right primer to be at the indicated position. Primers are also picked if they violate certain constraints. The default value indicates that the end of the right primer can be anywhere.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Заставляет 3&apos;-конец правого праймера находиться в указанной позиции. Праймеры также выбираются, если они нарушают определенные ограничения. Значение по умолчанию указывает, что конец левого праймера может быть где угодно.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="359"/>
        <location filename="../src/Primer3Dialog.ui" line="748"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The sequence of the start codon, by default ATG. Some bacteria use different start codons, this tag allows to specify alternative start codons.

Any triplet can be provided as start codon.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Последовательность стартового кодона, по умолчанию ATG. Некоторые бактерии используют разные стартовые кодоны, этот тег позволяет указать альтернативные стартовые кодоны.

Любой триплет может быть использован в качестве стартового кодона.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="753"/>
        <source>ATG</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="778"/>
        <source>Product Size Ranges</source>
        <translation>Диапазоны размеров продукта</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="801"/>
        <source>Max Library Mispriming</source>
        <translation>Макс. несоответствие бибилотеке праймеров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="870"/>
        <source>Max 3&apos; Stability</source>
        <translation>Макс. стабильность на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="937"/>
        <source>Pair Max Library Mispriming</source>
        <translation>Макс. несоответствие библиотеке праймеров (пары)</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1798"/>
        <location filename="../src/Primer3Dialog.ui" line="1885"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This is the most stable monomer structure of internal oligo calculated by thermodynamic approach. The hairpin loops, bulge loops, internal loops, internal single mismatches, dangling ends, terminal mismatches have been considered. This parameter is calculated only if &lt;span style=&quot; font-weight:700;&quot;&gt;Use Thermodynamic Oligo Alignment&lt;/span&gt; is checked. The default value is 10 degrees lower than the default value of &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Tm Min&lt;/span&gt;. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Это наиболее стабильная мономерная структура внутреннего олигонуклеотида, рассчитанная термодинамическим подходом. Были рассмотрены петли-шпильки, петли-выпуклости, внутренние петли, внутренние одиночные несовпадения, свисающие концы, концевые несовпадения. Этот параметр рассчитывается только в том случае, если &lt;span style=&quot; font-weight:700;&quot;&gt;Использовать термодинамическое олигонуклеотидное выравнивание&lt;/span&gt; отмечено. Значение по умолчанию на 10 градусов ниже, чем значение по умолчанию &lt;span style=&quot; font-weight:700;&quot;&gt;Миннимамльная Т. плав. праймера&lt;/span&gt;. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1119"/>
        <location filename="../src/Primer3Dialog.ui" line="1309"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The annealing temperature (Celsius) used in the PCR reaction. Usually it is chosen up to 10°C below the melting temperature of the primers. If provided, Primer3 will calculate the fraction of primers bound at the provided annealing temperature for each oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Температура отжига (по Цельсию), используемая в реакции ПЦР. Обычно ее выбирают на 10°С ниже температуры плавления праймеров. Если задано, Primer3 вычислит долю праймеров, связанных при указанной температуре отжига для каждого олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1184"/>
        <source>Primer Bound</source>
        <translation>Связь праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1054"/>
        <location filename="../src/Primer3Dialog.ui" line="1181"/>
        <location filename="../src/Primer3Dialog.ui" line="1243"/>
        <location filename="../src/Primer3Dialog.ui" line="1269"/>
        <location filename="../src/Primer3Dialog.ui" line="1458"/>
        <location filename="../src/Primer3Dialog.ui" line="1468"/>
        <location filename="../src/Primer3Dialog.ui" line="1687"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum acceptable fraction of primer bound at &lt;span style=&quot; font-weight:700;&quot;&gt;Annealing Temperature&lt;/span&gt; for a primer oligo in percent.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальная, оптимальная и максимальная допустимая доля Связи праймера при &lt;span style=&quot; font-weight:700;&quot;&gt;Температуре отжига&lt;/span&gt; для олигонуклеотида в процентах.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1312"/>
        <source>Annealing Temperature</source>
        <translation>Температура отжига</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1757"/>
        <source>Use Thermodynamic Oligo Alignment</source>
        <translation>Использовать термодинамическое олигонуклеотидное выравнивание</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1769"/>
        <location filename="../src/Primer3Dialog.ui" line="1908"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Same as&lt;span style=&quot; font-weight:700;&quot;&gt; Max 3&apos; Self Complementarity&lt;/span&gt; but is based on thermodynamical approach - the stability of structure is analyzed.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;То же, что и &lt;span style=&quot; font-weight:700;&quot;&gt; Макс. самокомплементарность на 3&apos;&lt;/span&gt;, но основанная на термодинамическом подходе - анализируется стабильность структуры.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1811"/>
        <location filename="../src/Primer3Dialog.ui" line="2067"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Same as &lt;span style=&quot; font-weight:700;&quot;&gt;Max 3&apos; Pair Complementarity&lt;/span&gt; but for calculating the score (melting temperature of structure) thermodynamical approach is used.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;То же, что и &lt;span style=&quot; font-weight:700;&quot;&gt;Макс. Парная Комплементарность на 3&apos;&lt;/span&gt;, но для расчета значения (температуры плавления структуры) используется термодинамический подход&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1830"/>
        <source>Max 3&apos; Pair Complementarity</source>
        <translation>Макс. парная комплементарность на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1837"/>
        <location filename="../src/Primer3Dialog.ui" line="2077"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Describes the tendency of a primer to bind to itself (interfering with target sequence binding). It will score ANY binding occurring within the entire primer sequence.&lt;/p&gt;&lt;p&gt;It is the maximum allowable local alignment score when testing a single primer for (local) self-complementarity. Local self-complementarity is taken to predict the tendency of primers to anneal to each other without necessarily causing self-priming in the PCR. The scoring system gives 1.00 for complementary bases, -0.25 for a match of any base (or N) with an N, -1.00 for a mismatch, and -2.00 for a gap. Only single-base-pair gaps are allowed.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Описывает тенденцию праймера связываться с самим собой (препятствуя связыванию целевой последовательности). Будет оцениваться ЛЮБОЕ связывание, происходящее во всей последовательности праймера.
&lt;/p&gt;&lt;p&gt;Это максимально допустимая оценка локального выравнивания при тестировании одного праймера на (локальную) самокомплементарность. Локальная самокомплементарность используется для предсказания тенденции праймеров отжигаться друг с другом, не обязательно вызывая самозаполнение в ПЦР. Система подсчета очков дает 1,00 за комплементарные символы, -0,25 за совпадение любого символа (или N) с N, -1,00 за несоответствие и -2,00 за гэп. Разрешаются только одинарные пары гэпов.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1856"/>
        <source>TH: Max Self Complementarity</source>
        <translation>TH: Макс. самокомплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1853"/>
        <location filename="../src/Primer3Dialog.ui" line="1869"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The same as &lt;span style=&quot; font-weight:700;&quot;&gt;Max Self Complementarity&lt;/span&gt; but all calculations are based on thermodynamical approach.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;То же, что и &lt;span style=&quot; font-weight:700;&quot;&gt;Макс. самокомплементарность&lt;/span&gt; но все расчеты основаны на термодинамическом подходе.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1888"/>
        <source>TH: Max Primer Hairpin	</source>
        <translation>TH: Макс. шпилечная структура	</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1911"/>
        <source>TH: Max 3&apos; Self Complementarity</source>
        <translation>TH: Макс. самокомплементарность на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1934"/>
        <source>Max Pair Complementarity</source>
        <translation>Макс. парная комплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1954"/>
        <location filename="../src/Primer3Dialog.ui" line="1983"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Describes the tendency of the left primer to bind to the right primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Описывает тенденцию левого праймера связываться с правым праймером.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1986"/>
        <source>TH: Max Pair Complementarity</source>
        <translation>TH: Макс. парная комплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1931"/>
        <location filename="../src/Primer3Dialog.ui" line="2038"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Describes the tendency of the left primer to bind to the right primer. It is the maximum allowable local alignment score when testing for complementarity between left and right primers. It is similar to &lt;span style=&quot; font-weight:700;&quot;&gt;Max Self Complementarity&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Описывает тенденцию левого праймера связываться с правым праймером. Это максимально допустимая оценка локального выравнивания при тестировании на комплементарность между левым и правым праймерами. Это похоже на &lt;span style=&quot; font-weight:700;&quot;&gt;Максимальную самокомплементарность&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2087"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Tries to bind the 3&apos;-END to a identical primer and scores the best binding it can find. This is critical for primer quality because it allows primers use itself as a target and amplify a short piece (forming a primer-dimer). These primers are then unable to bind and amplify the target sequence.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;Max 3&apos; Self Complementarity&lt;/span&gt; is the maximum allowable 3&apos;-anchored global alignment score when testing a single primer for self-complementarity. The 3&apos;-anchored global alignment score is taken to predict the likelihood of PCR-priming primer-dimers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Пытается связать 3&apos;-КОНЕЦ с идентичным праймером и оценивает лучшее связывание, которое может найти. Это имеет решающее значение для качества праймеров, поскольку позволяет праймерам использовать себя в качестве цели и амплифицировать короткий фрагмент (образуя праймер-димер). Затем эти праймеры не могут связываться и амплифицировать целевую последовательность.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;Макс. самокомплементарность на 3&apos;&lt;/span&gt; это максимально допустимая оценка глобального выравнивания с 3&apos;-привязкой при тестировании одного праймера на самодополняемость. Оценка глобального выравнивания с 3&apos;-привязкой используется для прогнозирования вероятности ПЦР-праймирования димеров праймеров.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2070"/>
        <source>TH: Max 3&apos; Pair Complementarity</source>
        <translation>TH: Макс. парная комплементарность на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2080"/>
        <source>Max Self Complementarity</source>
        <translation>Макс. самокомплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2090"/>
        <source>Max 3&apos; Self Complementarity</source>
        <translation>Макс. самокомплементарность на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2110"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Use thermodynamic models to calculate the the propensity of oligos to anneal to undesired sites in the template sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Используйте термодинамические модели для расчета склонности олигонуклеотидов к отжигу с нежелательными сайтами в шаблонной  последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2113"/>
        <source>Use Thermodynamic Template Alignment	</source>
        <translation>Использовать термодинамическое шаблонное выранивание	</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2131"/>
        <source>TH: Max Template Mispriming</source>
        <translation>TH: Макс. шаблон ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2128"/>
        <location filename="../src/Primer3Dialog.ui" line="2147"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Similar to &lt;span style=&quot; font-weight:700;&quot;&gt;Max template mispriming&lt;/span&gt; but assesses alternative binding sites in the template using thermodynamic models. This parameter specifies the maximum allowed melting temperature of an oligo (primer) at an &amp;quot;ectopic&amp;quot; site within the template sequence; 47.0 would be a reasonable choice if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Tm Min&lt;/span&gt; is 57.0.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;То же, что и &lt;span style=&quot; font-weight:700;&quot;&gt;Максимальный шаблон ошибочного праймирования&lt;/span&gt;но оценивает альтернативные сайты связывания в матрице с использованием термодинамических моделей. Этот параметр определяет максимально допустимую температуру плавления олиго (праймера) при &amp;quot;смещенном&amp;quot; сайт в шаблонной последовательности; 47.0 будет разумным выбором, если &lt;span style=&quot; font-weight:700;&quot;&gt;Минимальная Т. плав. праймера&lt;/span&gt; равна 57.0.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2163"/>
        <location filename="../src/Primer3Dialog.ui" line="2179"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed similarity to ectopic sites in the template. A negative value means do not check.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимально допустимое сходство с смещенными сайтами в шаблоне. При отрицательном значении не проверяется.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2214"/>
        <source>TH: Pair Max Template Mispriming</source>
        <translation>TH: Парный макс. шаблон ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2211"/>
        <location filename="../src/Primer3Dialog.ui" line="2224"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed summed melting temperatures of both primers at ectopic sites within the template (with the two primers in an orientation that would allow PCR amplification.) The melting temperatures are calculated as for &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max Template Mispriming&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимально допустимая суммарная температура плавления обоих праймеров на смещенных участках шаблона (с двумя праймерами в ориентации, позволяющей ПЦР-амплификацию). Температуры плавления рассчитываются как для параметра &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Максимальный шаблон ошибочного праймирования&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2240"/>
        <location filename="../src/Primer3Dialog.ui" line="2256"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed summed similarity of both primers to ectopic sites in the template. A negative value means do not check.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимально допустимое суммарное сходство обоих праймеров со смещенными сайтами в шаблоне. При отрицательном значении не учитывается.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2305"/>
        <source>Advanced Settings</source>
        <translation>Расширенные настройки</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2308"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section covers more accurate settings, which are related to &lt;span style=&quot; font-weight:700;&quot;&gt;General Settings&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;В этой секции описаны более точные настройки, относящиеся к &lt;span style=&quot; font-weight:700;&quot;&gt;Общим настройкам&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2318"/>
        <location filename="../src/Primer3Dialog.ui" line="2561"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The 5&apos; end of the left OR the right primer must overlap one of the junctions in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt; by this amount. See details in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;5&apos;-конец левого ИЛИ правого праймера должен перекрывать одно из соединений в &lt;span style=&quot; font-weight:700;&quot;&gt;Списке Перекрывающихся Соединений&lt;/span&gt; на это значение. Детальное описание смотри в &lt;span style=&quot; font-weight:700;&quot;&gt;Списке Перекрывающихся Соединений&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2337"/>
        <source>Sequencing Interval</source>
        <translation>Интервал секвенирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2344"/>
        <location filename="../src/Primer3Dialog.ui" line="2571"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Defines the space from the calculated position of the 3&apos;end to both sides in which Primer3Plus picks the best primer. Value only used if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Task&lt;/span&gt; is &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Определяет расстояние от рассчитанного положения 3&apos;-конца до обеих сторон, где Primer3 выбирает лучший праймер. Значение используется, только если &lt;span style=&quot; font-weight:700;&quot;&gt;Задачей подбора праймера&lt;/span&gt; является &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2385"/>
        <source>Max GC in primer 3&apos; end</source>
        <translation>Максимальный GC на 3&apos; конце праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2408"/>
        <source>Annealing Oligo Concentration</source>
        <translation>Концентрация олигонуклеотидов при отжиге</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2444"/>
        <source>Concentration of Divalent Cations</source>
        <translation>Концентрация двухвалентных катионов</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2382"/>
        <location filename="../src/Primer3Dialog.ui" line="2474"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum number of Gs or Cs allowed in the last five 3&apos; bases of a left or right primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимальное количество G или C, допустимое в последних пяти символах на 3&apos; конце левого или правого праймера.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2490"/>
        <location filename="../src/Primer3Dialog.ui" line="2831"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Analogous to &lt;span style=&quot; font-weight:700;&quot;&gt;3&apos; End Distance Between Left Primers&lt;/span&gt;, but for right primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Аналогично &lt;span style=&quot; font-weight:700;&quot;&gt;Расстояние между 3&apos; концами левых праймеров&lt;/span&gt;, но для правых праймеров.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2506"/>
        <location filename="../src/Primer3Dialog.ui" line="2695"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Defines the space from the 3&apos;end of the primer to the point were the trace signals are readable. Value only used if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Task&lt;/span&gt; is &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Определяет расстояние между 3&apos; концом праймера до точки, где след сигнала читаемый. Значение используется только если &lt;span style=&quot; font-weight:700;&quot;&gt;Задачей Подбора Праймеров&lt;/span&gt; является &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2522"/>
        <location filename="../src/Primer3Dialog.ui" line="2779"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The 3&apos; end of the left OR the right primer must overlap one of the junctions in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt; by this amount. See details in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;3&apos;-конец левого ИЛИ правого праймера должен перекрывать одно из соединений в &lt;span style=&quot; font-weight:700;&quot;&gt;Списке Перекрывающихся Соединений&lt;/span&gt; на это значение. Детальное описание смотри в &lt;span style=&quot; font-weight:700;&quot;&gt;Списке Перекрывающихся Соединений&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2564"/>
        <source>5 Prime Junction Overlap</source>
        <translation>5&apos; перерытие основного соединения</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2574"/>
        <source>Sequencing Accuracy</source>
        <translation>Точность секвенирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2675"/>
        <source>First Base Index</source>
        <translation>Индекс первого символа</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2698"/>
        <source>Sequencing Lead</source>
        <translation>Ведущее значение секвенирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2705"/>
        <location filename="../src/Primer3Dialog.ui" line="2889"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;When returning multiple primer pairs, the minimum number of base pairs between the 3&apos; ends of any two left primers.&lt;/p&gt;&lt;p&gt;Primers with 3&apos; ends at positions e.g. 30 and 31 in the template sequence have a three-prime distance of 1.&lt;/p&gt;&lt;p&gt;In addition to positive values, the values -1 and 0 are acceptable and have special interpretations:&lt;/p&gt;&lt;p&gt;-1 indicates that a given left primer can appear in multiple primer pairs returned by Primer3. This is the default behavior.&lt;/p&gt;&lt;p&gt;0 indicates that a left primer is acceptable if it was not already used. In other words, two left primers are allowed to have the same 3&apos; position provided their 5&apos; positions differ.&lt;/p&gt;&lt;p&gt;For n &amp;gt; 0: A left primer is acceptable if:&lt;/p&gt;&lt;p&gt;NOT(3&apos; end of left primer closer than n to the 3&apos; end of a previously used left primer)&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если найдено несколько пар праймеров, то это значение определяет минимальное количество спаренных оснований между 3&apos;-концами любых двух левых праймеров.&lt;/p&gt;&lt;p&gt;Праймеры с 3&apos;-концами в позициях, скажем, 30 и 31 в шаблонной последовательности, имеют расстояние между данное значение равное 1.&lt;/p&gt;&lt;p&gt;В дополнение к положительным значениям значения -1 и 0 тоже допустимы и имеют специальную интерпретацию:&lt;/p&gt;&lt;p&gt;-1 указывает, что данный левый праймер может появляться в нескольких парах праймеров, найденных с помощью Primer3. Это поведение по умолчанию.&lt;/p&gt;&lt;p&gt;0 означает, что левый праймер подходит, если он еще не использовался. Другими словами, два левых праймера могут иметь одинаковые 3&apos;-положения при условии, что их 5&apos;-положения различаются.&lt;/p&gt;&lt;p&gt;Для n &amp;gt; 0: A левый праймер подходит, если:&lt;/p&gt;&lt;p&gt;НЕ(3&apos;-конец левого праймера ближе, чем n, к 3&apos;-концу ранее использовавшегося левого праймера)&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2724"/>
        <source>Sequencing Spacing</source>
        <translation>Пробелы секвенирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2334"/>
        <location filename="../src/Primer3Dialog.ui" line="2731"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Defines the space from the 3&apos;end of the primer to the 3&apos;end of the next primer on the reverse strand. Value only used if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Task&lt;/span&gt; is &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Определяет пространство от 3&apos;-конца праймера до 3&apos;-конца следующего праймера на обратной цепи. Значение используется только если &lt;span style=&quot; font-weight:700;&quot;&gt;Задачей Подбора Праймеров&lt;/span&gt; является &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2782"/>
        <source>3 Prime Junction Overlap</source>
        <translation>3&apos; перекрытие основного соединения</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2834"/>
        <source>3&apos; End Distance Between Right Primers	</source>
        <translation>Расстояние между 3&apos; концами правых праймеров	</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2844"/>
        <source>Max #N&apos;s accepted</source>
        <translation>Максимально допустимое #N</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2851"/>
        <location filename="../src/Primer3Dialog.ui" line="2909"/>
        <source>Non-default values are valid only for sequences with 0 or 1 target regions. If the primer is part of a pair that spans a target and overlaps the target, then multiply this value times the number of nucleotide positions by which the primer overlaps the (unique) target to get the &apos;position penalty&apos;. The effect of this parameter is to allow Primer3 to include overlap with the target as a term in the objective function.</source>
        <translation>Значения, отличные от значений по умолчанию, допустимы только для последовательностей с 0 или 1 целевым регионом. Если праймер является частью пары, которая охватывает цель и перекрывает её, то умножьте это значение на количество позиций нуклеотидов, на которые праймер перекрывает (уникальную) цель, чтобы получить &quot;штраф за позицию&quot;. Эффект этого параметра заключается в том, чтобы позволить Primer3 включать перекрытие с целью в качестве члена в целевой функции.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2892"/>
        <source>3&apos; End Distance Between Left Primers</source>
        <translation>Расстояние между 3&apos; концами левых праймеров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2902"/>
        <source>DMSO Concentration</source>
        <translation>Концентрация ДМСО</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2721"/>
        <location filename="../src/Primer3Dialog.ui" line="3032"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Defines the space from the 3&apos;end of the primer to the 3&apos;end of the next primer on the same strand. Value only used if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Task&lt;/span&gt; is &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Определяет расстояние от 3&apos;-конца праймера до 3&apos;-конца следующего праймера на той же цепи. Значение используется только если &lt;span style=&quot; font-weight:700;&quot;&gt;Задачей Подбора Праймеров&lt;/span&gt; является &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3051"/>
        <source>Concentration of Monovalent Cations</source>
        <translation>Концентрация одновалентных катионов</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3083"/>
        <source>Formamide Concentration</source>
        <translation>Концентрация формамида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3093"/>
        <source>DMSO Factor</source>
        <translation>Фактор ДМСО</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2899"/>
        <location filename="../src/Primer3Dialog.ui" line="3100"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The concentration of DMSO in percent. See PRIMER_DMSO_FACTOR for details of Tm correction.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Концентрация ДМСО в процентах. Смотри &quot;Фактор ДСМО&quot; для дополнительной информации о коррекции температуры.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3090"/>
        <location filename="../src/Primer3Dialog.ui" line="3113"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The melting temperature of primers can be approximately corrected for DMSO:&lt;/p&gt;&lt;p&gt;Tm = Tm (without DMSO) - &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Factor&lt;/span&gt; * &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Concentration&lt;/span&gt;&lt;/p&gt;&lt;p&gt;The &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Concentration&lt;/span&gt; concentration must be given in %.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Температуру плавления праймеров можно приблизительно скорректировать на ДМСО:&lt;/p&gt;&lt;p&gt;Tm = Tm (без ДСМО) - &lt;span style=&quot; font-weight:700;&quot;&gt;Фактор ДСМО&lt;/span&gt; * &lt;span style=&quot; font-weight:700;&quot;&gt;Концентрация ДСМО&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;Концентрация ДСМО&lt;/span&gt; представлена в %.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3080"/>
        <location filename="../src/Primer3Dialog.ui" line="3132"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The concentration of formamide in mol/l. The melting temperature of primers can be approximately corrected for formamide:&lt;/p&gt;&lt;p&gt;Tm = Tm (without formamide) +(0.453 * PRIMER_[LEFT/INTERNAL/RIGHT]_4_GC_PERCENT / 100 - 2.88) * &lt;span style=&quot; font-weight:700;&quot;&gt;Formamide Concentration&lt;/span&gt;&lt;/p&gt;&lt;p&gt;The &lt;span style=&quot; font-weight:700;&quot;&gt;Formamide Concentration&lt;/span&gt; correction was suggested by Blake and Delcourt (R D Blake and S G Delcourt. Nucleic Acids Research, 24, 11:2095–2103, 1996).&lt;/p&gt;&lt;p&gt;Convert % into mol/l:&lt;/p&gt;&lt;p&gt;[DMSO in mol/l] = [DMSO in % weight] * 10 / 45.04 g/mol&lt;/p&gt;&lt;p&gt;[DMSO in mol/l] = [DMSO in % volume] * 10 * 1.13 g/cm3 / 45.04 g/mol&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Концентрация формамида в моль/л. Температуру плавления праймеров можно приблизительно скорректировать на формамид:&lt;/p&gt;&lt;p&gt;Tm = Tm (без формамида) +(0.453 * [GC-состав в процентах для правого/внутреннего/левого праймера] / 100 - 2.88) * &lt;span style=&quot; font-weight:700;&quot;&gt;Концентрация формамида&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Значение &lt;span style=&quot; font-weight:700;&quot;&gt;Концентрация формамида&lt;/span&gt; было предложено Blake and Delcourt (R D Blake and S G Delcourt. Nucleic Acids Research, 24, 11:2095–2103, 1996).&lt;/p&gt;&lt;p&gt;Преобразовать % в моль/л:&lt;/p&gt;&lt;p&gt;[ДМСО в моль/л] = [массДМСО в %] * 10 / 45.04 г/моль&lt;/p&gt;&lt;p&gt;[ДМСО в моль/л] = [объем ДМСО в %] * 10 * 1.13 г/см3 / 45.04 г/моль&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3172"/>
        <source>Use formatted output</source>
        <translation>Использовать форматированный вывод</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3182"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If this flag is true, produce PRIMER_LEFT_EXPLAIN, PRIMER_RIGHT_EXPLAIN, PRIMER_INTERNAL_EXPLAIN and/or PRIMER_PAIR_EXPLAIN output tags as appropriate.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если этот флаг установлен, то выходные тэги PRIMER_LEFT_EXPLAIN, PRIMER_RIGHT_EXPLAIN, PRIMER_INTERNAL_EXPLAIN и/или PRIMER_PAIR_EXPLAIN будут сгенерированы по возможности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3185"/>
        <source>Print Statistics</source>
        <translation>Статистика паймеров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3195"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the associated value = 1, then Primer3 will print out the calculated secondary structures&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если отмечено, то Primer3 распечатает рассчитанные вторичные структуры.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3198"/>
        <source>Print secondary structures</source>
        <translation>Напечатать вторичные структуры</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3216"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If set to 1, treat ambiguity codes as if they were consensus codes when matching oligos to mispriming or mishyb libraries. For example, if this flag is set, then a C in an oligo will be scored as a perfect match to an S in a library sequence, as will a G in the oligo. More importantly, though, any base in an oligo will be scored as a perfect match to an N in the library. This is very bad if the library contains strings of Ns, as no oligo will be legal (and it will take a long time to find this out). So unless you know for sure that your library does not have runs of Ns (or Xs), then set this flag to 0.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если задано, то обрабатывайте неоднозначности кода так, как если бы они были согласованными кодами при сопоставлении олигонуклеотидов с библиотеками mispriming или mishyb. Например, если этот флаг установлен, то C в олигонуклеотиде будет считаться полным совпадением с S в библиотечной последовательности, как и G в олигонуклеотиде.Однако, что еще более важно, любое основание в олигонуклеотида будет оцениваться как идеальное совпадение с N в библиотеке. Это очень плохо, если в библиотеке есть строки из N, так как ни один олигонуклеотид не будет легальным (и выяснять это придется долго). Поэтому, если вы точно не знаете, что в вашей библиотеке нет серий N (или X), установите этот флаг на 0.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3219"/>
        <source>Treat ambiguity codes in libraries as consensus</source>
        <translation>Относить неоднозначности кода в библиотеке к консенсусу</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3229"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If true use primer provided in left, right, or internal primer even if it violates specific constraints.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если true, используйте праймер, указанный в левом, правом или внутреннем праймере, даже если он нарушает определенные ограничения.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3232"/>
        <source>Pick anyway</source>
        <translation>Выбрать в любом случае</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3239"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This option allows for intelligent design of primers in sequence in which masked regions (for example repeat-masked regions) are lower-cased. (New in v. 1.1.0, added by Maido Remm and Triinu Koressaar)&lt;/p&gt;&lt;p&gt;A value of 1 directs Primer3 to reject primers overlapping lowercase a base exactly at the 3&apos; end.&lt;/p&gt;&lt;p&gt;This property relies on the assumption that masked features (e.g. repeats) can partly overlap primer, but they cannot overlap the 3&apos;-end of the primer. In other words, lowercase bases at other positions in the primer are accepted, assuming that the masked features do not influence the primer performance if they do not overlap the 3&apos;-end of primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эта опция позволяет интеллектуально проектировать праймеры в последовательности, в которой маскированные области (например, повторяющиеся маскированные области) имеют нижний регистр.&lt;/p&gt;&lt;p&gt;Выставленное значение указывает Primer3 отклонять праймеры, перекрывающие основание в нижнем регистре точно на 3&apos;-конце.&lt;/p&gt;&lt;p&gt;Это свойство основано на предположении, что маскированные признаки (например, повторы) могут частично перекрывать праймер, но не могут перекрывать 3&apos;-конец праймера. Другими словами, символы нижнего регистра в других позициях в праймере допускаются, предполагая, что маскированные признаки не влияют на эффективность праймера, если они не перекрывают 3&apos;-конец праймера.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3249"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This parameter provides a quick-and-dirty way to get Primer3 to accept IUB / IUPAC codes for ambiguous bases (i.e. by changing all unrecognized bases to N). If you wish to include an ambiguous base in an oligo, you must set &lt;span style=&quot; font-weight:700;&quot;&gt;Max #N&apos;s accepted&lt;/span&gt; to a 1 (non-0) value.&lt;/p&gt;&lt;p&gt;Perhaps &apos;-&apos; and &apos;* &apos; should be squeezed out rather than changed to &apos;N&apos;, but currently they simply get converted to N&apos;s. The authors invite user comments.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Этот параметр обеспечивает быстрый и грязный способ заставить Primer3 принимать коды IUB / IUPAC для неоднозначных оснований. (т.е. заменив все нераспознанные базы на N). Если вы хотите включить неоднозначное основание в олигонуклеотид, вы должны выставить &lt;span style=&quot; font-weight:700;&quot;&gt;Максимально допустимое #N&lt;/span&gt; на не-нулевое значение.&lt;/p&gt;&lt;p&gt;Возможно, «-» и «*» следует убрать, а не заменить на «N», но в настоящее время они просто преобразуются в N.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3252"/>
        <source>Liberal base</source>
        <translation>Свободный символ</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3294"/>
        <source>Internal Oligo Excluded Region</source>
        <translation>Исключаемые регионы внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3291"/>
        <location filename="../src/Primer3Dialog.ui" line="3301"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Middle oligos may not overlap any region specified by this tag. The associated value must be a space-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs, where &lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt; is the index of the first base of an excluded region, and &lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt; is its length. Often one would make Target regions excluded regions for internal oligos.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Средние олигонуклеотиды не могут перекрывать какую-либо область, указанную этим тегом. Связанное значение должно быть списком разделенных пробелом пар&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;начало&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt;&lt;/p&gt;&lt;p&gt;, где &lt;span style=&quot; font-weight:700;&quot;&gt;начало&lt;/span&gt; индекс первого символа исключенной области, а &lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt; - это её длина. Достаточно частая задача - сделать &quot;Целевые рерионы&quot; исключенными регионами для внутренних олигонуклеотидов.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3311"/>
        <source>Internal Oligo Overlap Positions</source>
        <translation>Позиции перекрывания внутренних олигонуклеотидов</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3308"/>
        <location filename="../src/Primer3Dialog.ui" line="3318"/>
        <location filename="../src/Primer3Dialog.ui" line="4080"/>
        <location filename="../src/Primer3Dialog.ui" line="4154"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The 5&apos; end of the middle oligo / probe must overlap one of the junctions in &lt;span style=&quot; font-weight:700;&quot;&gt;Internal Oligo Overlap Positions&lt;/span&gt; by this amount. See details in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;5&apos;-конец среднего олигонуклеотида должен перекрывать одно из соединений в &lt;span style=&quot; font-weight:700;&quot;&gt;Позициях перекрывания внутреннего олигонуклеотида&lt;/span&gt; на это значение. Смотри детальное описание параметра &lt;span style=&quot; font-weight:700;&quot;&gt;Список перекрывающихся соединений&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3339"/>
        <source>Internal Oligo Bound</source>
        <translation>Связь внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3423"/>
        <source>Internal Oligo GC%</source>
        <translation>GC% внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3487"/>
        <source>Internal Oligo Tm</source>
        <translation>Т. плав. внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3497"/>
        <source>Internal Oligo Size</source>
        <translation>Размер внутренного олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3336"/>
        <location filename="../src/Primer3Dialog.ui" line="3537"/>
        <location filename="../src/Primer3Dialog.ui" line="3643"/>
        <location filename="../src/Primer3Dialog.ui" line="3659"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer Bound for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Параметр, эквивалентный Связи праймера для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3699"/>
        <location filename="../src/Primer3Dialog.ui" line="3814"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max Self Complementarity&lt;/span&gt; for the internal oligo&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Макс. самокомплементарность&lt;/span&gt; для внутреннего олигонуклеотида&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3721"/>
        <location filename="../src/Primer3Dialog.ui" line="3750"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Same as &lt;span style=&quot; font-weight:700;&quot;&gt;Internal Oligo Max 3&apos; End Self Complementarity&lt;/span&gt; but for calculating the score (melting temperature of structure) thermodynamical approach is used&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;То же что и  &lt;span style=&quot; font-weight:700;&quot;&gt;Макс. самокомплементарность на 3&apos; для внутреннего олигонуклеотида&lt;/span&gt; но с использованием термодинамического подхода при расчете&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3753"/>
        <source>TH: Internal Oligo Max 3&apos; End Self Complementarity	</source>
        <translation>TH: Макс. самокомплементарность на 3&apos; для внутреннего олигонуклеотида	</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3766"/>
        <location filename="../src/Primer3Dialog.ui" line="3804"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer &lt;span style=&quot; font-weight:700;&quot;&gt;Max Self Complementarity&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Макс. самокомплементарность&lt;/span&gt; для внутреннего олигонуклеотида&lt;/p&gt;&lt;/body&gt;&lt;/html</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3788"/>
        <location filename="../src/Primer3Dialog.ui" line="3869"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer &lt;span style=&quot; font-weight:700;&quot;&gt;Max 3&apos; Self Complementarity&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Макс. самокомплементарность на 3&apos;&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3807"/>
        <location filename="../src/Primer3Dialog.ui" line="5798"/>
        <source>Internal Oligo Self Complementarity</source>
        <translation>Самокомплементарность внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3817"/>
        <location filename="../src/Primer3Dialog.ui" line="5711"/>
        <source>TH: Internal Oligo Self Complementarity</source>
        <translation>TH: Макс. самокомплементарность внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3843"/>
        <location filename="../src/Primer3Dialog.ui" line="3859"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The most stable monomer structure of internal oligo calculated by thermodynamic approach. See &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max Primer Hairpin&lt;/span&gt;	 for details&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Наиболее стабильная мономерная структура внутреннего олигонуклеотида, рассчитанная термодинамическим подходом. Смотри &lt;span style=&quot; font-weight:700;&quot;&gt;TH: макс. шпилечаная структура&lt;/span&gt; для деталей&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3862"/>
        <location filename="../src/Primer3Dialog.ui" line="5769"/>
        <source>TH: Internal Oligo Hairpin</source>
        <translation>TH: Шпилечная структура внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3872"/>
        <source>Internal Oligo Max 3&apos; End Self Complementarity</source>
        <translation>Самокомплементарность внутреннего олигонуклеотида на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3893"/>
        <source>Internal Oligo Max Poly-X</source>
        <translation>Макс. poly-x внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3903"/>
        <source>Internal Oligo DNA Concentration</source>
        <translation>Концентрация ДНК внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3913"/>
        <source>Internal Oligo Formamide Concentration</source>
        <translation>Концентрация формамида внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3926"/>
        <location filename="../src/Primer3Dialog.ui" line="4125"/>
        <location filename="../src/Primer3Dialog.ui" line="4170"/>
        <location filename="../src/Primer3Dialog.ui" line="4190"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Similar to &lt;span style=&quot; font-weight:700;&quot;&gt;Max Library Mispriming&lt;/span&gt; except that this parameter applies to the similarity of candidate internal oligos to the library specified in &lt;span style=&quot; font-weight:700;&quot;&gt;Internal Oligo Mishyb Library&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;То же что и &lt;span style=&quot; font-weight:700;&quot;&gt;Максимальное несоответствие библиотеке праймеров&lt;/span&gt; за исключением того, что этот параметр применяется к сходству внутренних олигонуклеотидов-кандидатов с библиотекой, указанной в &lt;span style=&quot; font-weight:700;&quot;&gt;Библиотека Mishyb Внутреннего Олиго&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3936"/>
        <source>Internal Oligo Min Sequence Quality</source>
        <translation>Минимальное качество последовательности внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3949"/>
        <location filename="../src/Primer3Dialog.ui" line="3972"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Parameter for internal oligos analogous to &lt;span style=&quot; font-weight:700;&quot;&gt;Concentration of dNTPs&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Параметр для внутренних олигонуклеотидов, аналогичный &lt;span style=&quot; font-weight:700;&quot;&gt;Концентрации dNTPs&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/ HTML&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3975"/>
        <source>Internal Oligo [dNTP]</source>
        <translation>Внутренние олигонуклеотиды [dNTP]</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3900"/>
        <location filename="../src/Primer3Dialog.ui" line="3988"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Annealing Oligo Concentration&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Концентрация олигонуклеотидов при отжиге&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt; /html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3933"/>
        <location filename="../src/Primer3Dialog.ui" line="4010"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Min sequence quality&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Минимальное качество последовательности&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt; /html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4029"/>
        <location filename="../src/Primer3Dialog.ui" line="4090"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Concentration of Monovalent Cations&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Концентрация одновалентных катионов&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt; &lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4048"/>
        <source>Internal Oligo DMSO Concentration</source>
        <translation>Концнтрация ДМСО для внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4061"/>
        <location filename="../src/Primer3Dialog.ui" line="4200"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Concentration of Divalent Cations&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Концентрация двухвалентных катионов&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt; &lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4083"/>
        <source>Internal Oligo 5 Prime Junction Overlap</source>
        <translation>5&apos; перекрытие основного соединения для внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4093"/>
        <source>Internal Oligo Conc of monovalent cations</source>
        <translation>Концентрация одновалентных катионов внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3962"/>
        <location filename="../src/Primer3Dialog.ui" line="4106"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer &lt;span style=&quot; font-weight:700;&quot;&gt;Max #Ns&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр праймера &lt;span style=&quot; font-weight:700;&quot;&gt;Мксимально допустимое к #Ns&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt; &lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3910"/>
        <location filename="../src/Primer3Dialog.ui" line="4141"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Formamide Concentration&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Концентрация формамида&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/ HTML&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4173"/>
        <source>Internal Oligo Mishyb Library</source>
        <translation>Библиотека mishyb внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4183"/>
        <source>Internal Oligo 3 Prime Junction Overlap</source>
        <translation>3&apos; перекрытие основного соединения для внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4193"/>
        <source>Internal Oligo Max Library Mishyb	</source>
        <translation>Макс. библиотеки Mishyb внутреннего олигонуклеотида	</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4203"/>
        <source>Internal Oligo conc of divalent cations</source>
        <translation>Концентрация двухвалентных катионов внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4045"/>
        <location filename="../src/Primer3Dialog.ui" line="4210"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Concentration&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр праймера &lt;span style=&quot; font-weight:700;&quot;&gt;Концентрация ДМСО&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body &gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3890"/>
        <location filename="../src/Primer3Dialog.ui" line="4229"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer &lt;span style=&quot; font-weight:700;&quot;&gt;Max poly-X&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр праймера &lt;span style=&quot; font-weight:700;&quot;&gt;Макс. poly-X&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body &gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4245"/>
        <location filename="../src/Primer3Dialog.ui" line="4264"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Factor&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;фактор ДМСО&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/ HTML&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4267"/>
        <source>Internal Oligo DMSO Factor</source>
        <translation>Фактор ДМСО внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4180"/>
        <location filename="../src/Primer3Dialog.ui" line="4274"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The 3&apos; end of the middle oligo / probe must overlap one of the junctions in &lt;span style=&quot; font-weight:700;&quot;&gt;Internal Oligo Overlap Positions&lt;/span&gt; by this amount. See details in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;3&apos;-конец среднего олигонуклеотида должен перекрывать одно из соединений в &lt;span style=&quot; font-weight:700;&quot;&gt;Позициях перекрывания внутреннего олигонуклеотида&lt;/span&gt; на это значение. Смотри детальное описание параметра &lt;span style=&quot; font-weight:700;&quot;&gt;Список перекрывающихся соединений&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4290"/>
        <location filename="../src/Primer3Dialog.ui" line="4306"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Analogous to &lt;span style=&quot; font-weight:700;&quot;&gt;3&apos; End Distance Between Left Primers&lt;/span&gt;, but for internal primer / probe.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Аналогично &lt;span style=&quot; font-weight:700;&quot;&gt;Расстояние Между 3&apos; концами Левых Праймеров&lt;/span&gt;, но для внутреннего праймера.&lt;/p &gt;&lt;/тело&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4309"/>
        <source>Min Internal Oligo End Distance</source>
        <translation>Мин. расстояние между концами внутренних олигонуклеотидов</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4335"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section describes &amp;quot;penalty weights&amp;quot;, which allow the user to modify the criteria that Primer3 uses to select the &amp;quot;best&amp;quot; primers.&lt;/p&gt;&lt;p&gt;There are two classes of weights: for some parameters there is a &apos;Lt&apos; (less than) and a &apos;Gt&apos; (greater than) weight. These are the weights that Primer3 uses when the value is less or greater than (respectively) the specified optimum.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эта секция описывает &amp;quot;веса штрафов&amp;quot;, которые позволяют пользователю модифицировать критерии, которые Primer3 использует при выборе &amp;quot;лучших&amp;quot; праймеров.&lt;/p&gt;&lt;p&gt;Существует два класса параметров весов - &apos;Мч&apos; (менее чем) и &apos;Бч&apos; (более чем). Эти веса Primer3 использует в случаях, когда значение менее или более чем (соответственно) заданное оптимальное значение.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4359"/>
        <location filename="../src/Primer3Dialog.ui" line="4382"/>
        <location filename="../src/Primer3Dialog.ui" line="4413"/>
        <location filename="../src/Primer3Dialog.ui" line="4436"/>
        <location filename="../src/Primer3Dialog.ui" line="4452"/>
        <location filename="../src/Primer3Dialog.ui" line="4468"/>
        <location filename="../src/Primer3Dialog.ui" line="4491"/>
        <location filename="../src/Primer3Dialog.ui" line="4501"/>
        <location filename="../src/Primer3Dialog.ui" line="4517"/>
        <location filename="../src/Primer3Dialog.ui" line="4527"/>
        <location filename="../src/Primer3Dialog.ui" line="4537"/>
        <location filename="../src/Primer3Dialog.ui" line="4547"/>
        <location filename="../src/Primer3Dialog.ui" line="5677"/>
        <location filename="../src/Primer3Dialog.ui" line="5687"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for corressponding parameter of a primer less than (Lt) or greater than (Gt) the optimal value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для соответствующего параметра праймера меньше чем (Lt) или больше чем (Gt), что является оптимальным значением.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4550"/>
        <source>Bound</source>
        <translation>Связь</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4585"/>
        <source>TH: Self Complementarity</source>
        <translation>TH: Самокомплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4608"/>
        <source>TH: 3&apos; End Self Complementarity	</source>
        <translation>TH: Самокомплементарность на 3&apos;	</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4631"/>
        <source>TH: Hairpin</source>
        <translation>TH: Шпильки</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4657"/>
        <source>TH: Template Mispriming</source>
        <translation>TH: Шаблон ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4683"/>
        <source>Self Complementarity</source>
        <translation>Самокомплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4582"/>
        <location filename="../src/Primer3Dialog.ui" line="4592"/>
        <location filename="../src/Primer3Dialog.ui" line="4605"/>
        <location filename="../src/Primer3Dialog.ui" line="4615"/>
        <location filename="../src/Primer3Dialog.ui" line="4628"/>
        <location filename="../src/Primer3Dialog.ui" line="4638"/>
        <location filename="../src/Primer3Dialog.ui" line="4654"/>
        <location filename="../src/Primer3Dialog.ui" line="4667"/>
        <location filename="../src/Primer3Dialog.ui" line="4680"/>
        <location filename="../src/Primer3Dialog.ui" line="4696"/>
        <location filename="../src/Primer3Dialog.ui" line="4709"/>
        <location filename="../src/Primer3Dialog.ui" line="4725"/>
        <location filename="../src/Primer3Dialog.ui" line="4738"/>
        <location filename="../src/Primer3Dialog.ui" line="4754"/>
        <location filename="../src/Primer3Dialog.ui" line="4778"/>
        <location filename="../src/Primer3Dialog.ui" line="4794"/>
        <location filename="../src/Primer3Dialog.ui" line="4807"/>
        <location filename="../src/Primer3Dialog.ui" line="4817"/>
        <location filename="../src/Primer3Dialog.ui" line="4830"/>
        <location filename="../src/Primer3Dialog.ui" line="4846"/>
        <location filename="../src/Primer3Dialog.ui" line="4859"/>
        <location filename="../src/Primer3Dialog.ui" line="4875"/>
        <location filename="../src/Primer3Dialog.ui" line="4917"/>
        <location filename="../src/Primer3Dialog.ui" line="4933"/>
        <location filename="../src/Primer3Dialog.ui" line="4946"/>
        <location filename="../src/Primer3Dialog.ui" line="4962"/>
        <location filename="../src/Primer3Dialog.ui" line="5138"/>
        <location filename="../src/Primer3Dialog.ui" line="5154"/>
        <location filename="../src/Primer3Dialog.ui" line="5167"/>
        <location filename="../src/Primer3Dialog.ui" line="5183"/>
        <location filename="../src/Primer3Dialog.ui" line="5199"/>
        <location filename="../src/Primer3Dialog.ui" line="5218"/>
        <location filename="../src/Primer3Dialog.ui" line="5708"/>
        <location filename="../src/Primer3Dialog.ui" line="5724"/>
        <location filename="../src/Primer3Dialog.ui" line="5737"/>
        <location filename="../src/Primer3Dialog.ui" line="5753"/>
        <location filename="../src/Primer3Dialog.ui" line="5766"/>
        <location filename="../src/Primer3Dialog.ui" line="5782"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for a primer parameter different from predefined optimum value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для параметра праймера отличается от оптимального предопределенного значения.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4712"/>
        <source>3&apos; End Self Complementarity</source>
        <translation>Самокомплементарность на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4741"/>
        <source>Template Mispriming</source>
        <translation>Шаблон ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4810"/>
        <location filename="../src/Primer3Dialog.ui" line="5361"/>
        <source>Library Mispriming</source>
        <translation>Библиотека ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4888"/>
        <location filename="../src/Primer3Dialog.ui" line="4904"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Determines the overall weight of the position penalty in calculating the penalty for a primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Позиция штрафа.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4949"/>
        <source>Primer failure rate</source>
        <translation>Величина ошибки праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5028"/>
        <location filename="../src/Primer3Dialog.ui" line="5044"/>
        <location filename="../src/Primer3Dialog.ui" line="5060"/>
        <location filename="../src/Primer3Dialog.ui" line="5070"/>
        <location filename="../src/Primer3Dialog.ui" line="5086"/>
        <location filename="../src/Primer3Dialog.ui" line="5103"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for corressponding parameter of a primer pair less than (Lt) or greater than (Gt) the optimal value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для соответствующего параметра пары праймеров меньше чем (Lt) или больше чем (Gt), что является оптимальным значением.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5141"/>
        <source>TH: Any Complementarity</source>
        <translation>TH: Любая комплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5170"/>
        <source>TH: 3&apos; End Complementarity</source>
        <translation>TH: Комплементарность на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5202"/>
        <source>TH: Template Mispriming Weight</source>
        <translation>TH: Вес шаблона ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5234"/>
        <source>Any Complementarity</source>
        <translation>Любая комплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5231"/>
        <location filename="../src/Primer3Dialog.ui" line="5247"/>
        <location filename="../src/Primer3Dialog.ui" line="5260"/>
        <location filename="../src/Primer3Dialog.ui" line="5276"/>
        <location filename="../src/Primer3Dialog.ui" line="5289"/>
        <location filename="../src/Primer3Dialog.ui" line="5305"/>
        <location filename="../src/Primer3Dialog.ui" line="5329"/>
        <location filename="../src/Primer3Dialog.ui" line="5345"/>
        <location filename="../src/Primer3Dialog.ui" line="5358"/>
        <location filename="../src/Primer3Dialog.ui" line="5374"/>
        <location filename="../src/Primer3Dialog.ui" line="5387"/>
        <location filename="../src/Primer3Dialog.ui" line="5403"/>
        <location filename="../src/Primer3Dialog.ui" line="5416"/>
        <location filename="../src/Primer3Dialog.ui" line="5432"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for a primer pair parameter different from predefined optimum value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для параметра пары праймеров отличается от оптимального предопределенного значения.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5263"/>
        <source>3&apos; End Complementarity</source>
        <translation>Комплементарность на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5292"/>
        <source>Template Mispriming Weight</source>
        <translation>Вес шаблона ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5332"/>
        <source>Tm Difference</source>
        <translation>Разница температуры плавления</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5390"/>
        <source>Primer Penalty Weight</source>
        <translation>Вес штрафа праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5419"/>
        <source>Internal Oligo Penalty Weight	</source>
        <translation>Вес штрафа внутреннего олигонуклеотида	</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5476"/>
        <source>Internal Oligos</source>
        <translation>Внутренние олигонуклеотиды</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5488"/>
        <source>Internal oligo Size</source>
        <translation>Размер внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5485"/>
        <location filename="../src/Primer3Dialog.ui" line="5515"/>
        <location filename="../src/Primer3Dialog.ui" line="5531"/>
        <location filename="../src/Primer3Dialog.ui" line="5547"/>
        <location filename="../src/Primer3Dialog.ui" line="5570"/>
        <location filename="../src/Primer3Dialog.ui" line="5600"/>
        <location filename="../src/Primer3Dialog.ui" line="5610"/>
        <location filename="../src/Primer3Dialog.ui" line="5626"/>
        <location filename="../src/Primer3Dialog.ui" line="5636"/>
        <location filename="../src/Primer3Dialog.ui" line="5653"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for corressponding parameter of a internal oligo less than (Lt) or greater than (Gt) the optimal value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для соответствующего параметра внутреннего олигонуклеотида меньше чем (Lt) или больше чем (Gt), что является оптимальным значением.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2464"/>
        <location filename="../src/Primer3Dialog.ui" line="2766"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Non-default values are valid only for sequences with 0 or 1 target regions.&lt;/p&gt;&lt;p&gt;If the primer is part of a pair that spans a target and does not overlap the target, then multiply this value times the number of nucleotide positions from the 3&apos; end to the (unique) target to get the &apos;position penalty&apos;. The effect of this parameter is to allow Primer3 to include nearness to the target as a term in the objective function.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Значения, отличные от значений по умолчанию, допустимы только для последовательностей с целевым регионом 0 или 1.&lt;/p&gt;&lt;p&gt;Если праймер является частью пары, которая охватывает цель и не перекрывает мишень, то умножьте это значение на количество позиций нуклеотидов от 3&apos;-конца до (уникальной) мишени, чтобы получить «штраф за позицию». Эффект этого параметра заключается в том, чтобы позволить Primer3 включить ближайшие к цели в качестве члена целевой функции.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="54"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This tag tells Primer3 what task to perform. Legal values are:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;generic&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Design a primer pair (the classic Primer3 task) if the &lt;span style=&quot; font-weight:700;&quot;&gt;Pick left primer &lt;/span&gt;is checked and &lt;span style=&quot; font-weight:700;&quot;&gt;Pick right primer&lt;/span&gt; is checked. In addition, pick an internal hybridization oligo if &lt;span style=&quot; font-weight:700;&quot;&gt;Pick internal oligo&lt;/span&gt; is checked.&lt;/p&gt;&lt;p&gt;NOTE: If &lt;span style=&quot; font-weight:700;&quot;&gt;Pick left primer&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt; Pick right primer&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;Pick internal oligo &lt;/span&gt;are checked, then behaves similarly to &lt;span style=&quot; font-weight:700;&quot;&gt;Primer task &lt;/span&gt;is&lt;span style=&quot; font-weight:700;&quot;&gt; pick_primer_list&lt;/span&gt;.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;check_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 only checks the primers provided in &lt;span style=&quot; font-weight:700;&quot;&gt;Left primer&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;Internal oligo&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;Right primer&lt;/span&gt;. It is the only task that does not require a sequence.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_primer_list&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick all primers in sequence (possibly limited by &lt;span style=&quot; font-weight:700;&quot;&gt;Include region&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;Exclude region&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;Pair OK Region List&lt;/span&gt;, etc.). Returns the primers sorted by quality starting with the best primers. If &lt;span style=&quot; font-weight:700;&quot;&gt;Pick left primer&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;Pick right primer &lt;/span&gt;are selected, Primer3 does not to pick primer pairs but generates independent lists of left primers, right primers, and, if requested, internal oligos.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick primers suited to sequence a region. The position of each primer is calculated for optimal sequencing results.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;pick_cloning_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick primers suited to clone a gene were the start nucleotide and the end nucleotide of the PCR fragment must be fixed, for example to clone an ORF. &lt;span style=&quot; font-weight:700;&quot;&gt;Included region&lt;/span&gt; must be used to indicate the first and the last nucleotide. Due to these limitations Primer3 can only vary the length of the primers. Set &lt;span style=&quot; font-weight:700;&quot;&gt;Pick anyway&lt;/span&gt; to obtain primers even if they violate specific constraints.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_discriminative_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick primers suited to select primers which bind with their end at a specific position. This can be used to force the end of a primer to a polymorphic site, with the goal of discriminating between sequence variants. &lt;span style=&quot; font-weight:700;&quot;&gt;Targets&lt;/span&gt; must be used to provide a single target indicating the last nucleotide of the left (nucleotide before the first nucleotide of the target) and the right primer (nucleotide following the target). The primers border the &lt;span style=&quot; font-weight:700;&quot;&gt;Targets&lt;/span&gt;. Due to these limitations Primer3 can only vary the length of the primers. Set &lt;span style=&quot; font-weight:700;&quot;&gt;Pick anyway&lt;/span&gt; to obtain primers even if they violate specific constraints.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Этот параметр сообщает Primer3, какую задачу выполнять. Возможные значения:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;generic&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Дизайн праймеров (классическая задача Primer3) если &lt;span style=&quot; font-weight:700;&quot;&gt;Выбрать левый праймер &lt;/span&gt;отмечен и &lt;span style=&quot; font-weight:700;&quot;&gt;Выбрать правый праймер&lt;/span&gt; отмечен. Кроме того, выберите внутренний гибридизационный олиго, если &lt;span style=&quot; font-weight:700;&quot;&gt;Выбрать внутренний олигонуклеотид&lt;/span&gt; отмечен.&lt;/p&gt;&lt;p&gt;ЗАМЕЧАНИЕ: Если &lt;span style=&quot; font-weight:700;&quot;&gt;Выбрать левый праймер&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt; Выбрать правый праймер&lt;/span&gt; и &lt;span style=&quot; font-weight:700;&quot;&gt;Выбрать внутренний олигонуклеотид &lt;/span&gt;отмечены, тогда результат такой же как и для &lt;span style=&quot; font-weight:700;&quot;&gt;Задачи&lt;/span&gt; &lt;span style=&quot; font-weight:700;&quot;&gt; pick_primer_list&lt;/span&gt;.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;check_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 только проверяет праймеры, представленные в &lt;span style=&quot; font-weight:700;&quot;&gt;Левом праймере&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;Внутренном олигонуклеотиде&lt;/span&gt; и &lt;span style=&quot; font-weight:700;&quot;&gt;Правом праймере&lt;/span&gt;. Это единственная задача, которая не требует последовательности.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_primer_list&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Выбрать все праймеры в последовательности (возможно ограничение по праметрам &lt;span style=&quot; font-weight:700;&quot;&gt;Включенные регионы&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;Исключаемые регионы&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;Список ОК парных регионов&lt;/span&gt;, и т.д.). Возвращает праймеры, отсортированные по качеству, начиная с лучших праймеров. Если &lt;span style=&quot; font-weight:700;&quot;&gt;Выбрать левый праймер&lt;/span&gt; и &lt;span style=&quot; font-weight:700;&quot;&gt;Выбрать правый праймер &lt;/span&gt;выбраны, Primer3 не выбирает пары праймеров, а генерирует независимые списки левых праймеров, правых праймеров и, если требуется, внутренних олигонуклеотидов.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;ыберите праймеры, подходящие для секвенирования области. Положение каждого праймера рассчитывается для оптимальных результатов секвенирования.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;pick_cloning_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Выберите праймеры, подходящие для клонирования гена, где начальный нуклеотид и конечный нуклеотид фрагмента ПЦР должны быть зафиксированы, например, для клонирования ORF. Параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Включенные регионы&lt;/span&gt; должен использоваться для обозначения первого и последнего нуклеотидов. Из-за этих ограничений Primer3 может изменять только длину праймеров. Задайте &lt;span style=&quot; font-weight:700;&quot;&gt;Выбрать в любом случае&lt;/span&gt; для получения праймеров, даже если они нарушают определенные ограничения.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_discriminative_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Подбор таких праймеров, которые связываются своим концом в определенном положении. Это может быть использовано для принудительного прикрепления конца праймера к полиморфному сайту с целью различения вариантов последовательности. Параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Целевые регионы&lt;/span&gt; должен использоваться для получения единственной цели, указывающей последний нуклеотид слева (нуклеотид перед первым целевым нуклеотидом) и правый праймер (нуклеотид, следующий за целевым). Праймеры граничат с &lt;span style=&quot; font-weight:700;&quot;&gt;Целевыми регионами&lt;/span&gt;. Из-за этих ограничений Primer3 может изменять только длину праймеров. Задайте &lt;span style=&quot; font-weight:700;&quot;&gt;Выбрать в любом случае&lt;/span&gt; для получения праймеров, даже если они нарушают определенные ограничения.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="118"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the associated value is checked, then Primer3 will attempt to pick left primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если соответствующее значение отмечено флажком, Primer3 попытается выбрать левые праймеры.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="137"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the associated value is checked, then Primer3 will attempt to pick an internal oligo (hybridization probe to detect the PCR product).&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если соответствующее значение отмечено флажком, Primer3 попытается выбрать внутренний олигонуклеотид (зонд гибридизации для обнаружения продукта ПЦР).&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="140"/>
        <source>Pick internal oligo</source>
        <translation>Выберите внутренний олигонуклеотид</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="159"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the associated value is checked, then Primer3 will attempt to pick a right primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если выбрано соответствующее значение, Primer3 попытается выбрать правый праймер.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="202"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The sequence of a left primer to check and around which to design right primers and optional internal oligos.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Последовательность левого праймера, которую нужно проверить и вокруг которой нужно подобрать правые праймеры и, если задано, внутренние олигонуклеотиды.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="212"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The sequence of an internal oligo to check and around which to design left and right primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Последовательность внутреннего олигонуклеотида, который нужно проверить и вокруг которого подобрать левый и правый праймеры.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="222"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The sequence of a right primer to check and around which to design left primers and optional internal oligos.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Последовательность правого праймера, которую необходимо проверить и вокруг которой подобрать левые праймеры и необязательные внутренние олигонуклеотиды.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="317"/>
        <location filename="../src/Primer3Dialog.ui" line="449"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Primer oligos may not overlap any region specified in this tag. The associated value must be a space-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs where &lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt; is the index of the first base of the excluded region, and &lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt; is its length. This tag is useful for tasks such as excluding regions of low sequence quality or for excluding regions containing repetitive elements such as ALUs or LINEs.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Олигонуклеотиды праймеров не могут перекрывать какую-либо область, указанную в этом параметре. Значение должно быть списком разделенных пробелом пар&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;начало&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt;&lt;/p&gt;&lt;p&gt;где &lt;span style=&quot; font-weight:700;&quot;&gt;начало&lt;/span&gt; — это индекс первого символа исключаемой области, а &lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt; е длина. Этот тег полезен для таких задач, как исключение областей с низким качеством последовательности или для исключения областей, содержащих повторяющиеся элементы, такие как ALU или LINE.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="327"/>
        <location filename="../src/Primer3Dialog.ui" line="442"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This tag allows detailed specification of possible locations of left and right primers in primer pairs.&lt;/p&gt;&lt;p&gt;The associated value must be a semicolon-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;left_start&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;left_length&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;right_start&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;right_length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;quadruples. The left primer must be in the region specified by &lt;span style=&quot; font-weight:700;&quot;&gt;left_start&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;left_length&lt;/span&gt; and the right primer must be in the region specified by &lt;span style=&quot; font-weight:700;&quot;&gt;right_start&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;right_length&lt;/span&gt;. &lt;span style=&quot; font-weight:700;&quot;&gt;left_start&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;left_length&lt;/span&gt; specify the location of the left primer in terms of the index of the first base in the region and the length of the region. &lt;span style=&quot; font-weight:700;&quot;&gt;right_start&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;right_length&lt;/span&gt; specify the location of the right primer in analogous fashion. As seen in the example below, if no integers are specified for a region then the location of the corresponding primer is not constrained.&lt;/p&gt;&lt;p&gt;Example:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;Pair OK Region List&lt;/span&gt;=100,50,300,50 ; 900,60,, ; ,,930,100&lt;/p&gt;&lt;p&gt;Specifies that there are three choices:&lt;/p&gt;&lt;p&gt;Left primer in the 50 bp region starting at position 100 AND right primer in the 50 bp region starting at position 300&lt;/p&gt;&lt;p&gt;OR&lt;/p&gt;&lt;p&gt;Left primer in the 60 bp region starting at position 900 (and right primer anywhere)&lt;/p&gt;&lt;p&gt;OR&lt;/p&gt;&lt;p&gt;Right primer in the 100 bp region starting at position 930 (and left primer anywhere)&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Этот параметр позволяет подробно указать возможные положения левого и правого праймеров в парах праймеров.&lt;/p&gt;&lt;p&gt;Значение должно быть списком четверок, разделенных точкой с запятой: &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;левое_начало&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;левая_длина&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;правое_начало&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;правая_длина&lt;/span&gt;&lt;/p&gt;&lt;p&gt;. Левый праймер должен находиться в области, указанной в значении &lt;span style=&quot; font-weight:700;&quot;&gt;левое_начало&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;левая_длина&lt;/span&gt; и правый праймер должен быть в области, указанной в паре &lt;span style=&quot; font-weight:700;&quot;&gt;правое_начало&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;правая_длина&lt;/span&gt;. &lt;span style=&quot; font-weight:700;&quot;&gt;левое_начало&lt;/span&gt; и &lt;span style=&quot; font-weight:700;&quot;&gt;левая_длина&lt;/span&gt; определяют расположение левого праймера с точки зрения индекса первого символа в регионе и длины региона. &lt;span style=&quot; font-weight:700;&quot;&gt;правое_начало&lt;/span&gt; и &lt;span style=&quot; font-weight:700;&quot;&gt;правая_длина&lt;/span&gt; аналогичным образом указывают расположение правого праймера. Как видно из приведенного ниже примера, если для региона не указаны целые числа, то расположение соответствующего праймера не ограничено.&lt;/p&gt;&lt;p&gt;Пример:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;Список ОК парных регионов&lt;/span&gt;=100,50,300,50 ; 900,60,, ; ,,930,100&lt;/p&gt;&lt;p&gt;Указывает, что есть три варианта:&lt;/p&gt;&lt;p&gt;Левый праймер находится в области длиной 100 нуклеотидов начиная с 50-го нуклеотида, а правый праймер - в области в 50 нуклеотидов начиная с 300-го нуклеотида&lt;/p&gt;&lt;p&gt;ИЛИ&lt;/p&gt;&lt;p&gt;Левый праймер находится в области длиной 60 нуклеотидов начиная с 900-го нуклеотида (а правый праймер - где угодно)&lt;/p&gt;&lt;p&gt;ИЛИ&lt;/p&gt;&lt;p&gt;Правый праймер - в области в 100 нуклеотидов начиная с 930-го нуклеотида (а левый праймер - где угодно)&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="391"/>
        <location filename="../src/Primer3Dialog.ui" line="629"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Discards all primers which do not match this match sequence at the 5&apos; end. (New in v. 2.3.6, added by A. Untergasser.)&lt;br/&gt;&lt;br/&gt;The match sequence must be 5 nucleotides long and can contain the following letters:&lt;/p&gt;&lt;p&gt;N Any nucleotide&lt;br/&gt;A Adenine&lt;br/&gt;G Guanine&lt;br/&gt;C Cytosine&lt;br/&gt;T Thymine&lt;br/&gt;R Purine (A or G)&lt;br/&gt;Y Pyrimidine (C or T)&lt;br/&gt;W Weak (A or T)&lt;br/&gt;S Strong (G or C)&lt;br/&gt;M Amino (A or C)&lt;br/&gt;K Keto (G or T)&lt;br/&gt;B Not A (G or C or T)&lt;br/&gt;H Not G (A or C or T)&lt;br/&gt;D Not C (A or G or T)&lt;br/&gt;V Not T (A or G or C)&lt;/p&gt;&lt;p&gt;Any primer which will not match the entire match sequence at the 5&apos; end will be discarded and not evaluated. Setting strict requirements here will result in low quality primers due to the high numbers of primers discarded at this step.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Отбрасывает все праймеры, которые не соответствуют этой последовательности соответствия на 5&apos; конце. (Новый с v. 2.3.6, автор A. Untergasser.)&lt;br/&gt;&lt;br/&gt;Последовательность должна состоять из 5 нуклеотидов и может содержать следующие буквы:&lt;/p&gt;&lt;p&gt;N Любой нуклеотид&lt;br/&gt;A Аденин&lt;br/&gt;G Гуанин&lt;br/&gt;C Цитозин&lt;br/&gt;T Тимин&lt;br/&gt;R Пурин (A или G)&lt;br/&gt;Y Пиримидин (C или T)&lt;br/&gt;W Слабый (A или T)&lt;br/&gt;S Сильный (G или C)&lt;br/&gt;M Амино (A или C)&lt;br/&gt;K Кето (G или T)&lt;br/&gt;B не A (G или C или T)&lt;br/&gt;H не G (A или C или T)&lt;br/&gt;D не C (A или G или T)&lt;br/&gt;V не T (A или G или C)&lt;/p&gt;&lt;p&gt;Любой праймер, который не будет соответствовать всей последовательности соответствия на 5&apos; конце, будет отброшен и не будет оцениваться. Установление здесь строгих требований приведет к получению праймеров низкого качества из-за большого количества праймеров, отбрасываемых на этом этапе.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="619"/>
        <location filename="../src/Primer3Dialog.ui" line="709"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Discards all primers which do not match this match sequence at the 3&apos; end. Similar parameter to &lt;span style=&quot; font-weight:700;&quot;&gt;Five Matches on Primer&apos;s 5&apos;&lt;/span&gt;, but limits the 3&apos; end. (New in v. 2.3.6, added by A. Untergasser.)&lt;br/&gt;&lt;br/&gt;The match sequence must be 5 nucleotides long and can contain the following letters:&lt;/p&gt;&lt;p&gt;N Any nucleotide&lt;br/&gt;A Adenine&lt;br/&gt;G Guanine&lt;br/&gt;C Cytosine&lt;br/&gt;T Thymine&lt;br/&gt;R Purine (A or G)&lt;br/&gt;Y Pyrimidine (C or T)&lt;br/&gt;W Weak (A or T)&lt;br/&gt;S Strong (G or C)&lt;br/&gt;M Amino (A or C)&lt;br/&gt;K Keto (G or T)&lt;br/&gt;B Not A (G or C or T)&lt;br/&gt;H Not G (A or C or T)&lt;br/&gt;D Not C (A or G or T)&lt;br/&gt;V Not T (A or G or C)&lt;/p&gt;&lt;p&gt;Any primer which will not match the entire match sequence at the 3&apos; end will be discarded and not evaluated. Setting strict requirements here will result in low quality primers due to the high numbers of primers discarded at this step.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Отбрасывает все праймеры, которые не соответствуют этой последовательности соответствия на 3&apos; конце. Схоже с параметром &lt;span style=&quot; font-weight:700;&quot;&gt;Пять совпадений на 5&apos; конце&lt;/span&gt;, но для 3&apos; конца. (Новый с v. 2.3.6, автор A. Untergasser.)&lt;br/&gt;&lt;br/&gt;Последовательность должна состоять из 5 нуклеотидов и может содержать следующие буквы:&lt;/p&gt;&lt;p&gt;N Любой нуклеотид&lt;br/&gt;A Аденин&lt;br/&gt;G Гуанин&lt;br/&gt;C Цитозин&lt;br/&gt;T Тимин&lt;br/&gt;R Пурин (A или G)&lt;br/&gt;Y Пиримидин (C или T)&lt;br/&gt;W Слабый (A или T)&lt;br/&gt;S Сильный (G или C)&lt;br/&gt;M Амино (A или C)&lt;br/&gt;K Кето (G или T)&lt;br/&gt;B не A (G или C или T)&lt;br/&gt;H не G (A или C или T)&lt;br/&gt;D не C (A или G или T)&lt;br/&gt;V не T (A или G или C)&lt;/p&gt;&lt;p&gt;Любой праймер, который не будет соответствовать всей последовательности соответствия на 3&apos; конце, будет отброшен и не будет оцениваться. Установление здесь строгих требований приведет к получению праймеров низкого качества из-за большого количества праймеров, отбрасываемых на этом этапе.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1754"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Use thermodynamic models to calculate the propensity of oligos to form hairpins and dimers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Используйте термодинамические модели для расчета склонности олигонуклеотидов к образованию шпилек и димеров.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1827"/>
        <location filename="../src/Primer3Dialog.ui" line="1993"/>
        <location filename="../src/Primer3Dialog.ui" line="2022"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Tries to bind the 3&apos;-END of the left primer to the right primer and scores the best binding it can find. It is similar to &lt;span style=&quot; font-weight:700;&quot;&gt;Max 3&apos; Self Complementarity&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Пытается связать 3&apos;-КОНЕЦ левого праймера с правым праймером и оценивает лучшее связывание, которое может найти. То же самое, что и &lt;span style=&quot; font-weight:700;&quot;&gt;Макс. самокомплементарность на 3&apos;&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5613"/>
        <source>Internal oligo Tm</source>
        <translation>Т плав. внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5639"/>
        <source>Internal oligo GC%</source>
        <translation>GC% внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5656"/>
        <source>Internal oligo Bound</source>
        <translation>Связь внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5740"/>
        <source>TH: Internal Oligo 3&apos; End Complementarity</source>
        <translation>TH: Комплементарность на 3&apos; внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5795"/>
        <location filename="../src/Primer3Dialog.ui" line="5811"/>
        <location filename="../src/Primer3Dialog.ui" line="5824"/>
        <location filename="../src/Primer3Dialog.ui" line="5840"/>
        <location filename="../src/Primer3Dialog.ui" line="5864"/>
        <location filename="../src/Primer3Dialog.ui" line="5880"/>
        <location filename="../src/Primer3Dialog.ui" line="5893"/>
        <location filename="../src/Primer3Dialog.ui" line="5909"/>
        <location filename="../src/Primer3Dialog.ui" line="5922"/>
        <location filename="../src/Primer3Dialog.ui" line="5938"/>
        <location filename="../src/Primer3Dialog.ui" line="5951"/>
        <location filename="../src/Primer3Dialog.ui" line="5967"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for an internal oligo parameter different from predefined optimum value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для параметра внутреннего олигонуклеотида отличается от оптимального предопределенного значения.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5827"/>
        <source>Internal Oligo 3&apos; End Complementarity</source>
        <translation>Комплементарность на 3&apos; внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5867"/>
        <source>Internal Oligo #N&apos;s</source>
        <translation>#N внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5896"/>
        <source>Internal Oligo Library Mishybing</source>
        <translation>Библиотечный mishybing внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5925"/>
        <source>Internal Oligo Sequence Quality</source>
        <translation>Качество последовательности внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5954"/>
        <source>Internal Oligo Sequence End Quality</source>
        <translation>Качество конца последовательности внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6123"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;In this section user can specify if primer product must overlap exon-exon junction or span intron. This only applies when designing primers for a cDNA (mRNA) sequence with annotated exons.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;В этой секции пользователь может определить должен ли праймер иметь экзон-экзон перекрытие или протяжный интрон. Работает только в случае дизайна праймеров для cDNA (mRNA) последовательностей, аннотированных экзонами.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6129"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This feature allows to specify if primer product must span intron-exon boundaries.&lt;/p&gt;&lt;p&gt;Checking this option will result in ignoring &lt;span style=&quot; font-style:italic;&quot;&gt;Excluded&lt;/span&gt; and &lt;span style=&quot; font-style:italic;&quot;&gt;Target Regions&lt;/span&gt; from Main section.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эта опция позволяет указать если праймер должен охватывать интрон-экзон границы.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6159"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The name of the annotation which defines the exons in the mRNA sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Имя экзон аннотации.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6264"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the range is set, primer search will be restricted to selected exons. For example:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;1-5&lt;/span&gt;&lt;/p&gt;&lt;p&gt;If the range is larger than actual exon range or the starting exon number exceeds number of exons, error message is shown. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если диапазон установлен, поиск праймеров будет ограничен выбранными экзонами. Например:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;1-5&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Если диапазон больше фактического диапазона экзонов или начальный номер экзона превышает количество экзонов, отображается сообщение об ошибке. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6305"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This number of found primer pairs wil bel queried to check if they fullfill the requirements for RTPCR.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимальное число пар для запроса.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6003"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;In this section user can specify sequence quality of target sequence and related parameters.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;В данной секции пользователь может задать качество целевой последовательности и связанные параметры.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6016"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A list of space separated integers. There must be exactly one integer for each base in the Source Sequence if this argument is non-empty. High numbers indicate high confidence in the base call at that position and low numbers indicate low confidence in the base call at that position.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Список разделенных пробелами целых чисел.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6038"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum sequence quality allowed within a primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальное качество последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6061"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum sequence quality allowed within the 3&apos; pentamer of a primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальное 3&amp;apos;качество последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6084"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum legal sequence quality.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимум интервала качества последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6107"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum legal sequence quality.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Макс. интервала качества последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6360"/>
        <source>Result Settings</source>
        <translation>Параметры результатов</translation>
    </message>
</context>
<context>
    <name>Primer3TmCalculatorFactory</name>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorFactory.cpp" line="29"/>
        <source>Primer 3</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>Primer3TmCalculatorSettingsWidget</name>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="14"/>
        <source>Form</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="32"/>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="42"/>
        <source>Concentration of DNA strands</source>
        <translation>Концентрация цепей ДНК</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="35"/>
        <source>DNA Concentration</source>
        <translation>Концентрация ДНК</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="48"/>
        <source> nM</source>
        <translation> нМ</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="61"/>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="71"/>
        <source>Concentration of monovalent cations</source>
        <translation>Концентрация одновалентных катионов</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="64"/>
        <source>Monovalent Concentration</source>
        <translation>Одновалентные катионы</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="74"/>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="100"/>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="126"/>
        <source> mM</source>
        <translation> мМ</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="87"/>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="97"/>
        <source>Concentration of divalent cations</source>
        <translation>Концентрация двухвалентных катионов</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="90"/>
        <source>Divalent Concentration</source>
        <translation>Двухвалентные катионы</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="113"/>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="123"/>
        <source>Concentration of deoxynycleotide triphosphate</source>
        <translation>Концентрация дезоксинуклеотидтрифосфата</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="116"/>
        <source>DNTP Concentration</source>
        <translation>Концентрация ДНТФ</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="139"/>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="149"/>
        <source>Concentration of DMSO</source>
        <translation>Концентрация диметилсульфоксида</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="142"/>
        <source>DMSO Concentration</source>
        <translation>Концентрация ДМСО</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="152"/>
        <source> %</source>
        <translation> %</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="162"/>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="172"/>
        <source>Correction factor for DMSO</source>
        <translation>Фактор коррекции ДМСО</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="165"/>
        <source>DMSO Factor</source>
        <translation>Фактор ДМСО</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="188"/>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="198"/>
        <source>Concentration of formamide</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="191"/>
        <source>Formamide Concentration</source>
        <translation>Концентрация формамида</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="201"/>
        <source> mol/l</source>
        <translation> мол/л</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="211"/>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="221"/>
        <source>The maximum sequence length for using the nearest neighbor model. For sequences longer than this, uses the &quot;GC%&quot; formula</source>
        <translation>Максимальная длина последовательности, для расчета температуры плавления которой будет использоваться модель &quot;метода ближайшего соседа&quot;. Для последовательностей короче заданной будет использоваться формула, основанная на GC%</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="214"/>
        <source>NN Max Length</source>
        <translation>Максимальная длина МБС</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="224"/>
        <source> nt</source>
        <translation> нт</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="240"/>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="250"/>
        <source>Specifies the table of thermodynamic parameters and the method of melting temperature calculation</source>
        <translation>Задать таблицу термодинамических параметров и метод расчета температуры плавления</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="243"/>
        <source>Thermodynamic Table</source>
        <translation>Таблица терм. значений</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="260"/>
        <source>Breslauer</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="265"/>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="301"/>
        <source>SantaLucia</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="273"/>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="283"/>
        <source>Specifies salt correction formula for the melting temperature calculation</source>
        <translation>Задать формулу коррекции соли для расчета температуры плавления</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="276"/>
        <source>Salt Correction Formula</source>
        <translation>Коррекция соли</translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="296"/>
        <source>Schildkraut</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="306"/>
        <source>Owczarzy</source>
        <translation></translation>
    </message>
</context>
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/Primer3Query.cpp" line="168"/>
        <source>Primer</source>
        <translation>Праймер</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="169"/>
        <source>PCR primer design</source>
        <translation>Конструкция PCR праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="171"/>
        <source>Excluded regions</source>
        <translation>Исключенные регионы</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="172"/>
        <source>Targets</source>
        <translation>Целевые регионы</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="173"/>
        <source>Product size ranges</source>
        <translation>Диапазоны размера продукта</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="174"/>
        <source>Number to return</source>
        <translation>Результатов не более</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="176"/>
        <source>Max repeat mispriming</source>
        <translation>Максимальный повтор ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="177"/>
        <source>Max template mispriming</source>
        <translation>Максимальный шаблон ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="178"/>
        <source>Max 3&apos; stability</source>
        <translation>Макс. 3&apos; стабильность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="179"/>
        <source>Pair max repeat mispriming</source>
        <translation>Парный максимальный повтор ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="180"/>
        <source>Pair max template mispriming</source>
        <translation>Парный максимальный шаблон ошибочного праймирования</translation>
    </message>
</context>
<context>
    <name>U2::FindExonRegionsTask</name>
    <message>
        <location filename="../src/FindExonRegionsTask.cpp" line="93"/>
        <source>Failed to search for exon annotations. The sequence %1 doesn&apos;t have any related annotations.</source>
        <translation>Failed to search for exon annotations. The sequence %1 doesn&apos;t have any related annotations.</translation>
    </message>
</context>
<context>
    <name>U2::GTest</name>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="507"/>
        <source>Error in sequence quality data</source>
        <translation>Error in sequence quality data</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="511"/>
        <source>Sequence quality data missing</source>
        <translation>Sequence quality data missing</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="37"/>
        <location filename="../src/Primer3Tests.cpp" line="320"/>
        <source>QDomNode isn&apos;t element</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="84"/>
        <location filename="../src/Primer3Tests.cpp" line="90"/>
        <source>Illegal SEQUENCE_TARGET value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="104"/>
        <source>Illegal SEQUENCE_OVERLAP_JUNCTION_LIST value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="119"/>
        <source>Illegal SEQUENCE_INTERNAL_OVERLAP_JUNCTION_LIST value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="134"/>
        <source>Illegal SEQUENCE_EXCLUDED_REGION value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="160"/>
        <source>Illegal SEQUENCE_PRIMER_PAIR_OK_REGION_LIST value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="174"/>
        <source>Illegal SEQUENCE_INCLUDED_REGION value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="192"/>
        <location filename="../src/Primer3Tests.cpp" line="199"/>
        <source>Illegal SEQUENCE_INTERNAL_EXCLUDED_REGION value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="214"/>
        <source>Illegal PRIMER_PRODUCT_SIZE_RANGE value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="365"/>
        <source>Incorrect results num. Pairs: %1, left: %2, right: %3, inernal: %4</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="380"/>
        <location filename="../src/Primer3Tests.cpp" line="391"/>
        <location filename="../src/Primer3Tests.cpp" line="402"/>
        <source>Incorrect parameter: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="522"/>
        <source>No error, but expected: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="525"/>
        <source>An unexpected error. Expected: %1, but Actual: %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="534"/>
        <source>No warning, but expected: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="538"/>
        <source>An unexpected warning. Expected: %1, but Actual: %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="550"/>
        <source>PRIMER_PAIR_NUM_RETURNED is incorrect. Expected:%1, but Actual:%2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="566"/>
        <source>Incorrect single primers num. Expected:%1, but Actual:%2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="577"/>
        <source>Incorrect single primer type, num: %1. Expected:%2, but Actual:%3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="733"/>
        <source>PRIMER_PAIR%1_LIBRARY_MISPRIMING_NAME name is incorrect. Expected:%2, but Actual:%3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="737"/>
        <source>PRIMER_PAIR%1_COMPL_ANY_STUCT name is incorrect. Expected:%2, but Actual:%3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="741"/>
        <source>PRIMER_PAIR%1_COMPL_END_STUCT name is incorrect. Expected:%2, but Actual:%3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="753"/>
        <source>%1 is incorrect. Expected:%2,%3, but Actual:NULL</source>
        <translation>%1 is incorrect. Expected:%2,%3, but Actual:NULL</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="761"/>
        <source>%1 is incorrect. Expected:NULL, but Actual:%2,%3</source>
        <translation>%1 is incorrect. Expected:NULL, but Actual:%2,%3</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="770"/>
        <source>%1 is incorrect. Expected:%2,%3, but Actual:%4,%5</source>
        <translation>%1 is incorrect. Expected:%2,%3, but Actual:%4,%5</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="795"/>
        <source>%1_LIBRARY_MISPRIMING_NAME name is incorrect. Expected:%2, but Actual:%3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="799"/>
        <source>%1_SELF_ANY_STUCT name is incorrect. Expected:%2, but Actual:%3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="803"/>
        <source>%1_SELF_END_STUCT name is incorrect. Expected:%2, but Actual:%3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="817"/>
        <location filename="../src/Primer3Tests.cpp" line="825"/>
        <source>%1 is incorrect. Expected:%2, but Actual:%3</source>
        <translation>%1 is incorrect. Expected:%2, but Actual:%3</translation>
    </message>
</context>
<context>
    <name>U2::Primer3ADVContext</name>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="90"/>
        <source>Primer3...</source>
        <translation>Primer3: Подбор праймеров...</translation>
    </message>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="118"/>
        <source>Error</source>
        <translation>Ошибка</translation>
    </message>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="119"/>
        <source>Cannot create an annotation object. Please check settings</source>
        <translation>Невозможно создать аннотацию. Проверьте настройки</translation>
    </message>
</context>
<context>
    <name>U2::Primer3Dialog</name>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="115"/>
        <source>NONE</source>
        <translation>нет</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="116"/>
        <source>HUMAN</source>
        <translation>HUMAN</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="117"/>
        <source>RODENT_AND_SIMPLE</source>
        <translation>RODENT_AND_SIMPLE</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="118"/>
        <source>RODENT</source>
        <translation>RODENT</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="119"/>
        <source>DROSOPHILA</source>
        <translation>DROSOPHILA</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="524"/>
        <source>The &quot;Include region&quot; should be the only one</source>
        <translation>&quot;Включенный регион&quot; должен быть только один</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="702"/>
        <source>Sequence quality list length must be equal to the sequence length</source>
        <translation>Длина списка качества последовательности должна быть равна длине последовательности</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="720"/>
        <source>Task &quot;pick_discriminative_primers&quot; requires exactly one &quot;Targets&quot; region.</source>
        <translation>Опция &quot;pick_discriminative_primers&quot; Требует ровно один &quot;Целевой регион&quot;.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="730"/>
        <source>At least one primer on the &quot;Main&quot; settings page should be enabled.</source>
        <translation>Должен быть отмечен как минимум один из праймеров на странице &quot;Основные параметры&quot;.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="640"/>
        <source>Left primer</source>
        <translation>Левый праймер</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="631"/>
        <source>%1 sequence has incorrect alphabet, should be be simple DNA</source>
        <translation>%1 имеет некорректный алфавит, ожидается ДНК.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="646"/>
        <source>Left 5&apos; overhang</source>
        <translation>Левое 5&apos; перекрытие</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="660"/>
        <source>Internal oligo</source>
        <translation>Внутренний олигонуклеотид</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="672"/>
        <source>Right primer</source>
        <translation>Правый праймер</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="678"/>
        <source>Right 5&apos; overhang</source>
        <translation>Правое 5&apos; перекрытие</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="752"/>
        <source>Primer Size Ranges should have at least one range</source>
        <translation>Диапазон размеров продукта должен иметь как миниум один диапазон</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="764"/>
        <source>Sequence range region is too small for current product size ranges</source>
        <translation>Регион последовательности слишком мал для текущего диапазона размера продукта</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="781"/>
        <source>The priming sequence is too long, please, decrease the region</source>
        <translation>Последовательность слишком длинная, пожалуйста, уменьшите регион</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="791"/>
        <source>Incorrect sum &quot;Included Region Start + First Base Index&quot; - should be more or equal than 0</source>
        <translation>Некорректная сумма по параметрам &quot;Начало включенного региона + Индекс первого символа&quot; - значение должно быть больше либо равно 0</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="795"/>
        <source>The priming sequence is out of range.
Either make the priming region end &quot;%1&quot; less or equal than the sequence size &quot;%2&quot; plus the first base index value &quot;%3&quot;or mark the sequence as circular</source>
        <translation>Последовательность праймирования все заданного региона.
Либо сделайте конец праймируемого региона %1 меньше либо равным размеру последовательности %2 плюс индекс первого сиимвола %3, либо отметьте последовательность круговой</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="811"/>
        <source>%1 parameter(s) have an incorrect value(s), pay attention on red widgets. </source>
        <translation>%1 параметр(ов) имеют некорректное(ые) значение(я), обратите внимание на подсвеченние красным виджеты. </translation>
    </message>
    <message>
        <source>The following errors are possible:

</source>
        <translation type="vanished">Возможны следующие ошибки:

</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="816"/>
        <source>

Click OK to continue calculation, but the incorrect values will be ignored.</source>
        <translation>

Нажмите ОК чтбы продолжить расчет, некорректные значения будут проигнарированы.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="946"/>
        <source>Can&apos;t read to &quot;%1&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="966"/>
        <location filename="../src/Primer3Dialog.cpp" line="975"/>
        <location filename="../src/Primer3Dialog.cpp" line="984"/>
        <location filename="../src/Primer3Dialog.cpp" line="996"/>
        <location filename="../src/Primer3Dialog.cpp" line="1049"/>
        <location filename="../src/Primer3Dialog.cpp" line="1055"/>
        <location filename="../src/Primer3Dialog.cpp" line="1090"/>
        <source>Can&apos;t parse &quot;%1&quot; value: &quot;%2&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="985"/>
        <source>Incorrect value for &quot;%1&quot; value: &quot;%2&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="1068"/>
        <source>PRIMER_MISPRIMING_LIBRARY value should points to the file from the &quot;%1&quot; directory</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="1081"/>
        <source>PRIMER_INTERNAL_MISHYB_LIBRARY value should points to the file from the &quot;%1&quot; directory</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="1085"/>
        <source>PRIMER_MIN_THREE_PRIME_DISTANCE is unused in the UGENE GUI interface. We may either skip it or set PRIMER_MIN_LEFT_THREE_PRIME_DISTANCE and PRIMER_MIN_RIGHT_THREE_PRIME_DISTANCE to %1. Do you want to set?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="857"/>
        <source>Save primer settings</source>
        <translation>Сохранить настройки праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="813"/>
        <source>The following errors are possible:

</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="864"/>
        <source>Can&apos;t write to &quot;%1&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="938"/>
        <source>Text files</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="939"/>
        <source>Load settings</source>
        <translation>Загрузить настройки</translation>
    </message>
</context>
<context>
    <name>U2::Primer3Plugin</name>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="58"/>
        <source>Primer3</source>
        <translation>Primer3</translation>
    </message>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="58"/>
        <source>Integrated tool for PCR primers design.</source>
        <translation>Инструмент для дизайна праймеров.</translation>
    </message>
</context>
<context>
    <name>U2::Primer3SWTask</name>
    <message>
        <location filename="../src/Primer3Task.cpp" line="608"/>
        <source>Incorrect sum &quot;Included Region Start + First Base Index&quot; - should be more or equal than 0</source>
        <translation type="unfinished">Некорректная сумма по параметрам &quot;Начало включенного региона + Индекс первого символа&quot; - значение должно быть больше либо равно 0</translation>
    </message>
</context>
<context>
    <name>U2::Primer3Task</name>
    <message>
        <location filename="../src/Primer3Task.cpp" line="384"/>
        <source>Pick primers task</source>
        <translation>Выбор праймеров</translation>
    </message>
</context>
<context>
    <name>U2::Primer3ToAnnotationsTask</name>
    <message>
        <location filename="../src/Primer3Task.cpp" line="640"/>
        <source>Search primers to annotations</source>
        <translation>Search primers to annotations</translation>
    </message>
    <message>
        <location filename="../src/Primer3Task.cpp" line="673"/>
        <source>Failed to find any exon annotations associated with the sequence %1.Make sure the provided sequence is cDNA and has exonic structure annotated</source>
        <translation>Failed to find any exon annotations associated with the sequence %1.Make sure the provided sequence is cDNA and has exonic structure annotated</translation>
    </message>
    <message>
        <location filename="../src/Primer3Task.cpp" line="684"/>
        <source>The first exon from the selected range [%1,%2] is larger the number of exons (%3). Please set correct exon range.</source>
        <translation>Первый экзон из выбранного диапазона [%1,%2] больше общего количества экзонов(%3). Пожалуйста, задайте правильный диапазон экзонов.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Task.cpp" line="693"/>
        <source>The the selected exon range [%1,%2] is larger the number of exons (%3). Please set correct exon range.</source>
        <translation>Выбраный диапазон экзонов [%1,%2] больше общего количества экзонов(%3). Пожалуйста, задайте правильный диапазон экзонов.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Task.cpp" line="791"/>
        <source>Object with annotations was removed</source>
        <translation>Объект с аннотациями удален</translation>
    </message>
</context>
<context>
    <name>U2::QDPrimerActor</name>
    <message>
        <location filename="../src/Primer3Query.cpp" line="81"/>
        <source>%1 invalid input. Excluded regions.</source>
        <translation>%1 invalid input. Excluded regions.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="90"/>
        <source>%1 invalid input. Targets.</source>
        <translation>%1 invalid input. Targets.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="99"/>
        <source>%1 invalid input. Product size ranges.</source>
        <translation>%1 invalid input. Product size ranges.</translation>
    </message>
</context>
</TS>
