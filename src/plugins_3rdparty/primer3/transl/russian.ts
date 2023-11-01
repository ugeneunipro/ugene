<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru_RU">
<context>
    <name>Primer3Dialog</name>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="42"/>
        <source>Main</source>
        <translation>Основные параметры</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="388"/>
        <source>Targets</source>
        <translation>Целевые регионы</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="863"/>
        <location filename="../src/Primer3Dialog.ui" line="902"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A list of product size ranges, for example:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot;font-weight:600;&quot;&gt; 150-250 100-300 301-400 &lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 first tries to pick primers in the first range. If that is not possible, it goes to the next range and tries again. It continues in this way until it has either picked all necessary primers or until there are no more ranges. For technical reasons this option makes much lighter computational demands than the Product Size option.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Список диапазонов размера продукта, например:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot;font-weight:600;&quot;&gt; 150-250 100-300 301-400 &lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="909"/>
        <location filename="../src/Primer3Dialog.ui" line="922"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This selection indicates what mispriming library (if any) Primer3 should use to screen for interspersed repeats or for other sequence to avoid as a location for primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Этот параметр указывает какую библиотеку будет использовать Primer3.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="873"/>
        <location filename="../src/Primer3Dialog.ui" line="939"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum number of primer pairs to return.&lt;/p&gt;&lt;p&gt;Primer pairs returned are sorted by their &amp;quot;quality&amp;quot;, in other words by the value of the objective function (where a lower number indicates a better primer pair).&lt;/p&gt;&lt;p&gt;Caution: setting this parameter to a large value will increase running time.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимальное число пар праймеров.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="955"/>
        <location filename="../src/Primer3Dialog.ui" line="971"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum stability for the last five 3&apos; bases of a left or right primer.&lt;/p&gt;&lt;p&gt;Bigger numbers mean more stable 3&apos; ends. The value is the maximum delta G (kcal/mol) for duplex disruption for the five 3&apos; bases as calculated using the Nearest-Neighbor parameter values specified by the option of &apos;Table of thermodynamic parameters&apos;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Макс. стабильности для последних пяти 3&apos; оснований левого или правого праймеров. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="886"/>
        <location filename="../src/Primer3Dialog.ui" line="1006"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed weighted similarity with any sequence in Mispriming Library.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимально допустимое взвешенное сходство с любой последовательностью из библиотеки ошибочного праймирования.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1022"/>
        <location filename="../src/Primer3Dialog.ui" line="1038"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed sum of similarities of a primer pair (one similarity for each primer) with any single sequence in Mispriming Library. &lt;/p&gt;&lt;p&gt;Library sequence weights are not used in computing the sum of similarities.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимально допустимая сумма сходств пар праймеров с любой последовательностью из библиотеки ошибочного праймирования.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="209"/>
        <source>Pick left primer</source>
        <translation>Выбрать левый праймер</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="263"/>
        <source>or use left primer below</source>
        <translation>или использовать этот левый праймер</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1087"/>
        <source>General Settings</source>
        <translation>Общие настройки</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1090"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section covers such settings as primer size, temperature, GC and other general settings.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;В этом разделе рассматриваются такие параметры, как размер праймера, температура, GC и другие общие настройки.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1145"/>
        <location filename="../src/Primer3Dialog.ui" line="1187"/>
        <location filename="../src/Primer3Dialog.ui" line="1327"/>
        <location filename="../src/Primer3Dialog.ui" line="1373"/>
        <location filename="../src/Primer3Dialog.ui" line="1393"/>
        <source>Min</source>
        <translation>Минимум</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1472"/>
        <location filename="../src/Primer3Dialog.ui" line="1552"/>
        <location filename="../src/Primer3Dialog.ui" line="1689"/>
        <location filename="../src/Primer3Dialog.ui" line="1817"/>
        <location filename="../src/Primer3Dialog.ui" line="1827"/>
        <source>Opt</source>
        <translation>Оптимум</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1236"/>
        <location filename="../src/Primer3Dialog.ui" line="1285"/>
        <location filename="../src/Primer3Dialog.ui" line="1353"/>
        <location filename="../src/Primer3Dialog.ui" line="1363"/>
        <location filename="../src/Primer3Dialog.ui" line="1751"/>
        <source>Max</source>
        <translation>Максимум</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1265"/>
        <source>Primer Tm</source>
        <translation>Т. плав. праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1350"/>
        <location filename="../src/Primer3Dialog.ui" line="1370"/>
        <location filename="../src/Primer3Dialog.ui" line="1453"/>
        <location filename="../src/Primer3Dialog.ui" line="1469"/>
        <location filename="../src/Primer3Dialog.ui" line="1501"/>
        <location filename="../src/Primer3Dialog.ui" line="1523"/>
        <location filename="../src/Primer3Dialog.ui" line="1539"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum lengths (in bases) of a primer oligo.&lt;/p&gt;&lt;p&gt;Primer3 will not pick primers shorter than Min or longer than Max, and with default arguments will attempt to pick primers close with size close to Opt.&lt;/p&gt;&lt;p&gt;Min cannot be smaller than 1. Max cannot be larger than 36. (This limit is governed by maximum oligo size for which melting-temperature calculations are valid.)&lt;/p&gt;&lt;p&gt;Min cannot be greater than Max.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальная, оптимальная и максимальная длины олигонуклеотида праймера.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1129"/>
        <location filename="../src/Primer3Dialog.ui" line="1158"/>
        <location filename="../src/Primer3Dialog.ui" line="1184"/>
        <location filename="../src/Primer3Dialog.ui" line="1249"/>
        <location filename="../src/Primer3Dialog.ui" line="1262"/>
        <location filename="../src/Primer3Dialog.ui" line="1686"/>
        <location filename="../src/Primer3Dialog.ui" line="1748"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum melting temperatures (Celsius) for a primer oligo.&lt;/p&gt;&lt;p&gt;Primer3 will not pick oligos with temperatures smaller than Min or larger than Max, and with default conditions will try to pick primers with melting temperatures close to Opt.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальная, оптимальная и максимальная температуры плавления олигонуклеотида праймера.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1410"/>
        <location filename="../src/Primer3Dialog.ui" line="1800"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum percentage of Gs and Cs in any primer or oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;
</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальное, оптимальное и максимальное процентное содержание нуклеотидов G и C в праймере или олигонуклеотиде.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;
</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1282"/>
        <location filename="../src/Primer3Dialog.ui" line="1324"/>
        <location filename="../src/Primer3Dialog.ui" line="1482"/>
        <location filename="../src/Primer3Dialog.ui" line="1673"/>
        <location filename="../src/Primer3Dialog.ui" line="1814"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum percentage of Gs and Cs in any primer or oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальное, оптимальное и максимальное процентное содержание Gs и Cs в праймере или олигонуклеотиде.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1233"/>
        <location filename="../src/Primer3Dialog.ui" line="1380"/>
        <location filename="../src/Primer3Dialog.ui" line="1390"/>
        <location filename="../src/Primer3Dialog.ui" line="1597"/>
        <location filename="../src/Primer3Dialog.ui" line="1629"/>
        <location filename="../src/Primer3Dialog.ui" line="1651"/>
        <location filename="../src/Primer3Dialog.ui" line="1824"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum, optimum, and maximum melting temperature of the amplicon.&lt;/p&gt;&lt;p&gt;Primer3 will not pick a product with melting temperature less than min or greater than max. If Opt is supplied and the Penalty Weights for Product Size are non-0 Primer3 will attempt to pick an amplicon with melting temperature close to Opt.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальная, оптимальная и максимальная температуры плавления ампликона.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1100"/>
        <location filename="../src/Primer3Dialog.ui" line="1298"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Maximum acceptable (unsigned) difference between the melting temperatures of the left and right primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимально допустимое различие между температурами плавления левого и правого праймеров.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1226"/>
        <source>Table of thermodynamic parameters</source>
        <translation>Таблица термодинамических параметров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4468"/>
        <source>Primers</source>
        <translation>Праймеры</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5124"/>
        <source>Primer Pairs</source>
        <translation>Пары праймеров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6544"/>
        <source>Max base pairs in dimer</source>
        <translation>Максимальное количество пар нуклеотидов в димере</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6646"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section allows to set output annotations&apos; settings.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;В этой секции можно задать настройки для аннотаций.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1765"/>
        <source>Breslauer et. al 1986</source>
        <translation>Breslauer et. al 1986</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1770"/>
        <location filename="../src/Primer3Dialog.ui" line="2712"/>
        <source>SantaLucia 1998</source>
        <translation>SantaLucia 1998</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="26"/>
        <source>Primer Designer</source>
        <translation>Дизайнер праймеров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="912"/>
        <source>Mispriming/Repeat library</source>
        <translation>Библиотека повторов/ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="876"/>
        <source>Number to return</source>
        <translation>Результатов не более</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2257"/>
        <source>Max template mispriming</source>
        <translation>Макс. шаблон ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2334"/>
        <source>Pair max template mispriming</source>
        <translation>Парный макс. шаблон ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="273"/>
        <source>or use oligo below</source>
        <translation>или использовать эту oligo</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="250"/>
        <source>Pick right primer</source>
        <translation>Выбрать правый праймер</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="280"/>
        <source>or use right primer below (5&apos; to 3&apos; on opposite strand)</source>
        <translation>или использовать этот правый праймер (5&apos; в 3&apos; в противоположном направлении)</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1542"/>
        <source>Primer size</source>
        <translation>Размер праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1383"/>
        <location filename="../src/Primer3Dialog.ui" line="5191"/>
        <source>Product Tm</source>
        <translation>Т. плав. продукта</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1414"/>
        <source>Primer GC%</source>
        <translation>GC состав %</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1103"/>
        <source>Max Tm difference</source>
        <translation>Макс. разница Т. плав</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3024"/>
        <source>Inside target penalty</source>
        <translation>Внутренний целевой штраф</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2564"/>
        <source>Outside target penalty</source>
        <translation>Внешний целевой штраф</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2654"/>
        <source>Max poly-X</source>
        <translation>Макс. poly-X</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3047"/>
        <source>CG clamp</source>
        <translation>Зажим CG</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2700"/>
        <location filename="../src/Primer3Dialog.ui" line="2920"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Specifies the salt correction formula for the melting temperature calculation.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Задает коррекционную формулу для расчета температуры плавления.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3274"/>
        <source>Show debugging info</source>
        <translation>Показывать отладочную информацию</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3470"/>
        <location filename="../src/Primer3Dialog.ui" line="3612"/>
        <location filename="../src/Primer3Dialog.ui" line="3677"/>
        <location filename="../src/Primer3Dialog.ui" line="3715"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer Size for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр размера праймера для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3506"/>
        <location filename="../src/Primer3Dialog.ui" line="3554"/>
        <location filename="../src/Primer3Dialog.ui" line="3602"/>
        <location filename="../src/Primer3Dialog.ui" line="3748"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer Tm for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Tm олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3538"/>
        <location filename="../src/Primer3Dialog.ui" line="3642"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer GC% for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;GC% олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3525"/>
        <location filename="../src/Primer3Dialog.ui" line="3699"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer GC% for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;GC% олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6238"/>
        <source>RT-PCR</source>
        <translation>ПЦР с обратной транскрипцией</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6250"/>
        <source>Design primers for RT-PCR analysis</source>
        <translation>Дизайн праймеров для анализа RT-PCR</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6270"/>
        <source>Exon annotation name:</source>
        <translation>Имя экзон аннотации:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6280"/>
        <source>exon</source>
        <translation>exon</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6305"/>
        <source>Minimum exon junction overlap size</source>
        <translation>Минимальный размер перекрытия соединения экзона</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6369"/>
        <source>Exon range:</source>
        <translation>Диапазон экзона:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6416"/>
        <source>Max number of pairs to query:</source>
        <translation>Максимальное число пар для запроса:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6143"/>
        <source>Min sequence quality:</source>
        <translation>Минимальное качество последовательности:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="50"/>
        <location filename="../src/Primer3Dialog.ui" line="72"/>
        <source>&lt;html&gt;
   &lt;head/&gt;
   &lt;body&gt;
      &lt;p&gt;Specifies the Primer3 parameters set. Following presets are available:&lt;/p&gt;
      &lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;Default&lt;/span&gt;&lt;/p&gt;
      &lt;p&gt;Primer3 default parameters.&lt;/p&gt;
      &lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;Recombinase Polymerase Amplification&lt;/span&gt;&lt;/p&gt;
      &lt;p&gt;Preset with weaker temperature bounds, longer primer and product lengths. Also, this parameter enables &quot;Check complementary&quot; by default (see the &quot;Posterior Actions&quot; tab).&lt;/p&gt;
   &lt;/body&gt;
&lt;/html&gt;</source>
        <translation>&lt;html&gt;
   &lt;head/&gt;
   &lt;body&gt;
      &lt;p&gt;Задайте набор параметров для Primer3. Доступны следующие пресеты:&lt;/p&gt;
      &lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;По-умолчанию&lt;/span&gt;&lt;/p&gt;
      &lt;p&gt;Набор параметров Primer3 по-умолчанию.&lt;/p&gt;
      &lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;Рекомбиназная полимеразная амплификация&lt;/span&gt;&lt;/p&gt;
      &lt;p&gt;Пресет с ослабленными температурными границами, увеличенными длинами праймеров и продукта. Так же, этот параметр по-умолчанию активирует &quot;Проверить комплементарность&quot; (см. вкладку &quot;Действия после расчета&quot;).&lt;/p&gt;
   &lt;/body&gt;
&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6166"/>
        <source>Min 3&apos; sequence quality:</source>
        <translation>Минимальное 3&apos;качество последовательности:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6189"/>
        <source>Sequence quality range min:</source>
        <translation>Минимум интервала качества последовательности:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6212"/>
        <source>Sequence quality range max:</source>
        <translation>Макс. интервала качества последовательности:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6481"/>
        <source>Posterior Actions</source>
        <translation>Действия после расчета</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6484"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;In this section user can define actions for primers when Primer3 calculation is already finished - for example, check if result primer pairs self- and hetero-dimers are good enough.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;В этой секции пользователь может задать действия с праймерами, которые будут выполнены после завершения расчета Primer3 - например, проверка того, что гомо- и гетеро-димеры получившихся праймерова достаточно хороши.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6490"/>
        <source>Check self- and hetero-dimers of primer pairs and filter pairs with inappropriate parameters</source>
        <translation>ПРоверить гомо- и гетеро-димеры пар праймеров и отфильровать пары с неподходящими параметрами</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6493"/>
        <source>Check complementary</source>
        <translation>Проверить комплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6577"/>
        <location filename="../src/Primer3Dialog.ui" line="6600"/>
        <source>Maximum persantage of G/C base pairs</source>
        <translation>Максимальный процент пар оснований G/C</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6603"/>
        <source> bp</source>
        <translation> пн</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6590"/>
        <source>Max dimer GC-content</source>
        <translation>Максимальный GC-состав димера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6580"/>
        <source> %</source>
        <translation> %</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6510"/>
        <location filename="../src/Primer3Dialog.ui" line="6532"/>
        <location filename="../src/Primer3Dialog.ui" line="6567"/>
        <source>Save primer report in CSV file format</source>
        <translation>Сохранить отчет о найденных праймерах в файле формата CSV</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6570"/>
        <source>Generate CSV report</source>
        <translation>Сгенерировать отчет в формате CSV</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6535"/>
        <source>...</source>
        <translation>...</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6694"/>
        <source>Help</source>
        <translation>Помощь</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6714"/>
        <source>Close</source>
        <translation>Закрыть</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6721"/>
        <source>Save settings</source>
        <translation>Сохранить настройки</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6728"/>
        <source>Load settings</source>
        <translation>Загрузить настройки</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6735"/>
        <source>Reset form</source>
        <translation>Сброс параметров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6742"/>
        <source>Pick primers</source>
        <translation>Подобрать</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1223"/>
        <location filename="../src/Primer3Dialog.ui" line="1761"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Specifies details of melting temperature calculation.&lt;/p&gt;&lt;p&gt;First method uses the table of thermodynamic parameters from the paper [Breslauer KJ, et. al (1986), Proc Natl Acad Sci 83:4746-50 http://dx.doi.org/10.1073/pnas.83.11.3746]&lt;/p&gt;&lt;p&gt;Second method (recommended) was suggested in the paper [SantaLucia JR (1998), Proc Natl Acad Sci 95:1460-65 http://dx.doi.org/10.1073/pnas.95.4.1460].&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Указывает детали расчета температуры плавления.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2923"/>
        <source>Salt correction formula</source>
        <translation>Коррекционная формула</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2703"/>
        <location filename="../src/Primer3Dialog.ui" line="2707"/>
        <source>Schildkraut and Lifson 1965</source>
        <translation>Schildkraut and Lifson 1965</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2717"/>
        <source>Owczarzy et. 2004</source>
        <translation>Owczarzy et. 2004</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3057"/>
        <source>Concentration of dNTPs</source>
        <translation>Концентрация dNTP</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2953"/>
        <location filename="../src/Primer3Dialog.ui" line="3070"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Maximum number of unknown bases (N) allowable in any primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимальное число неизвестных оснований (N), допустимых в праймере.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2651"/>
        <location filename="../src/Primer3Dialog.ui" line="2985"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowable length of a mononucleotide repeat, for example AAAAAA.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Макс. допустимая для мононуклеотидного повтора, например AAAAAA.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2775"/>
        <location filename="../src/Primer3Dialog.ui" line="2907"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This parameter is the index of the first base in the input sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Индекс первого основания во входной последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3044"/>
        <location filename="../src/Primer3Dialog.ui" line="3089"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Require the specified number of consecutive Gs and Cs at the 3&apos; end of both the left and right primer.&lt;/p&gt;&lt;p&gt;This parameter has no effect on the internal oligo if one is requested.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Требуется указанное количество идущих подряд символов G и C на 3&apos;-конце как левого, так и правого праймера. .&lt;/p&gt;&lt;p&gt;Этот параметр не влияет на внутренний олигонуклеотид, если он запрошен.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2463"/>
        <location filename="../src/Primer3Dialog.ui" line="3163"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The millimolar (mM) concentration of monovalent salt cations (usually KCl) in the PCR.&lt;/p&gt;&lt;p&gt;Primer3 uses this argument to calculate oligo and primer melting temperatures.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Миллимолярная (мМ) концентрация катионов одновалентных солей (обычно KCl) в ПЦР.&lt;/p&gt;&lt;p&gt;Primer3 использует этот аргумент для расчета температур плавления олигонуклеотидов и праймеров..&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2538"/>
        <location filename="../src/Primer3Dialog.ui" line="2731"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The millimolar concentration of divalent salt cations (usually MgCl^(2+)) in the PCR.&lt;/p&gt;&lt;p&gt;Primer3 converts concentration of divalent cations to concentration of monovalent cations using following formula:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;[Monovalent cations] = [Monovalent cations] + 120*(([divalent cations] - [dNTP])^0.5)&lt;/span&gt;&lt;/p&gt;&lt;p&gt;In addition, if the specified concentration of dNTPs is larger than the concentration of divalent cations then the effect of the divalent cations is not considered.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Миллимолярная концентрация катионов двухвалентных солей (обычно MgCl^(2+)) в ПЦР.&lt;/p&gt;&lt;p&gt;Primer3 преобразует концентрацию двухвалентных катионов в концентрацию одновалентных катионов, используя следующую формулу:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;[Одновалентные катионы] = [Одновалентные катионы] + 120*(([двухвалентные катионы] - [dNTP])^0.5)&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Кроме того, если указанная концентрация dNTP превышает концентрацию двухвалентных катионов, то влияние двухвалентных катионов не учитывается.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2756"/>
        <location filename="../src/Primer3Dialog.ui" line="3054"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The millimolar concentration of the sum of all deoxyribonucleotide triphosphates.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Миллимолярная концентрация суммы всех дезоксирибонуклеотидтрифосфатов.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2502"/>
        <location filename="../src/Primer3Dialog.ui" line="3179"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A value to use as nanomolar (nM) concentration of each annealing oligo over the course the PCR.&lt;/p&gt;&lt;p&gt;Primer3 uses this argument to esimate oligo melting temperatures.&lt;/p&gt;&lt;p&gt;The default (50nM) works well with the standard protocol used at the Whitehead/MIT Center for Genome Research --0.5 microliters of 20 micromolar concentration for each primer in a 20 microliter reaction with 10 nanograms template, 0.025 units/microliter Taq polymerase in 0.1 mM each dNTP, 1.5mM MgCl2, 50mM KCl, 10mM Tris-HCL (pH 9.3) using 35 cycles with an annealing temperature of 56 degrees Celsius.&lt;/p&gt;&lt;p&gt;The value of this parameter is less than the actual concentration of oligos in the initial reaction mix because  it is the concentration of annealing oligos, which in turn depends on the amount of template (including PCR product) in a given cycle. This concentration increases a great deal during a PCR; fortunately PCR seems quite robust for a variety of oligo melting temperatures.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Значение, используемое в качестве наномолярной (нМ) концентрации каждого отжигаемого олигонуклеотида в ходе ПЦР.&lt;/p&gt;&lt;p&gt;Primer3 использует этот аргумент для оценки температуры плавления олигонуклеотидов.&lt;/p&gt;&lt;p&gt;Значение по умолчанию (50 нМ) хорошо работает со стандартным протоколом, используемым в Центре исследований генома Уайтхеда/MIT. --0,5 микролитра 20 микромолярной концентрации для каждого праймера в реакции 20 микролитров с матрицей 10 нанограмм, 0,025 единиц/микролитр Taq-полимеразы в 0,1 мМ каждого dNTP, 1,5 мМ MgCl2, 50 мМ KCl, 10 мМ Трис-HCL (pH 9,3) с использованием 35 циклов с температурой отжига 56 градусов Цельсия.&lt;/p&gt;&lt;p&gt;Значение этого параметра меньше фактической концентрации олигонуклеотидов в исходной реакционной смеси, поскольку это концентрация олигонуклеотидов отжига, которая, в свою очередь, зависит от количества матрицы (включая продукт ПЦР) в данном цикле. Эта концентрация значительно увеличивается во время ПЦР; к счастью, ПЦР кажется достаточно надежным для различных температур плавления олигонуклеотидов.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3360"/>
        <source>Lowercase masking</source>
        <translation>Строчная маскировка</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3398"/>
        <source>Internal Oligo</source>
        <translation>Внутренний олигонуклеотид</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3401"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Parameters governing choice of internal oligos are analogous to the parameters governing choice of primer pairs.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Параметры, регулирующие выбор внутренних олигонуклеотидов аналогичны параметрам, регулирующих выбор пар праймеров.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3567"/>
        <location filename="../src/Primer3Dialog.ui" line="3622"/>
        <location filename="../src/Primer3Dialog.ui" line="3629"/>
        <location filename="../src/Primer3Dialog.ui" line="3728"/>
        <source>Min:</source>
        <translation>Мин:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3486"/>
        <location filename="../src/Primer3Dialog.ui" line="3574"/>
        <location filename="../src/Primer3Dialog.ui" line="3595"/>
        <location filename="../src/Primer3Dialog.ui" line="3793"/>
        <source>Opt:</source>
        <translation>Опт:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3493"/>
        <location filename="../src/Primer3Dialog.ui" line="3581"/>
        <location filename="../src/Primer3Dialog.ui" line="3588"/>
        <location filename="../src/Primer3Dialog.ui" line="3735"/>
        <source>Max:</source>
        <translation>Макс:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4083"/>
        <source>Max #Ns</source>
        <translation>Макс N</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4450"/>
        <source>Penalty Weights</source>
        <translation>Веса штрафов</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4622"/>
        <source>Tm</source>
        <translation>Tm</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4534"/>
        <source>Size  </source>
        <translation>Размер  </translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4480"/>
        <source>GC%</source>
        <translation>GC%</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4487"/>
        <location filename="../src/Primer3Dialog.ui" line="4524"/>
        <location filename="../src/Primer3Dialog.ui" line="4541"/>
        <location filename="../src/Primer3Dialog.ui" line="4675"/>
        <location filename="../src/Primer3Dialog.ui" line="5133"/>
        <location filename="../src/Primer3Dialog.ui" line="5238"/>
        <location filename="../src/Primer3Dialog.ui" line="5613"/>
        <location filename="../src/Primer3Dialog.ui" line="5620"/>
        <location filename="../src/Primer3Dialog.ui" line="5698"/>
        <location filename="../src/Primer3Dialog.ui" line="5781"/>
        <source>Lt:</source>
        <translation>Мч:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4510"/>
        <location filename="../src/Primer3Dialog.ui" line="4517"/>
        <location filename="../src/Primer3Dialog.ui" line="4596"/>
        <location filename="../src/Primer3Dialog.ui" line="4682"/>
        <location filename="../src/Primer3Dialog.ui" line="5214"/>
        <location filename="../src/Primer3Dialog.ui" line="5231"/>
        <location filename="../src/Primer3Dialog.ui" line="5675"/>
        <location filename="../src/Primer3Dialog.ui" line="5705"/>
        <location filename="../src/Primer3Dialog.ui" line="5764"/>
        <location filename="../src/Primer3Dialog.ui" line="5788"/>
        <source>Gt:</source>
        <translation>Бч:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4899"/>
        <source>#N&apos;s</source>
        <translation>Количество N</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4951"/>
        <location filename="../src/Primer3Dialog.ui" line="6127"/>
        <source>Sequence quality</source>
        <translation>Качество последовательности</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4980"/>
        <source>End sequence quality</source>
        <translation>Качество конца последовательности</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5009"/>
        <source>Position penalty</source>
        <translation>Позиция штрафа</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5038"/>
        <source>End stability</source>
        <translation>Стабильность конца</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5224"/>
        <source>Product size</source>
        <translation>Размер продукта</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6262"/>
        <source>mRNA sequence</source>
        <translation>Последовательность mRNA</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6317"/>
        <source>At 5&apos; side (bp):</source>
        <translation>Со стороны 5&apos; (нк):</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6347"/>
        <source>At 3&apos;side (bp)</source>
        <translation>Со стороны 3&apos; (нк)</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6404"/>
        <source>Primer product must span at least one intron on the corresponding genomic DNA</source>
        <translation>Продукт праймера должен охватывать по крайней мере один интрон на соответствующей геномной ДНК</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6118"/>
        <source>Sequence Quality</source>
        <translation>Качество последовательности</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="135"/>
        <source>Select the Task for primer selection</source>
        <translation>Выберите задачу подбора праймеров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="132"/>
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
        <location filename="../src/Primer3Dialog.ui" line="146"/>
        <source>generic</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="151"/>
        <source>pick_sequencing_primers</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="156"/>
        <source>pick_primer_list</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="161"/>
        <source>check_primers</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="166"/>
        <source>pick_cloning_primers</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="171"/>
        <source>pick_discriminative_primers</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="322"/>
        <location filename="../src/Primer3Dialog.ui" line="356"/>
        <source>5&apos; Overhang:</source>
        <translation>5&apos; перекрытие:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="319"/>
        <location filename="../src/Primer3Dialog.ui" line="329"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The provided sequence is added to the 5&apos; end of the left primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Предоставленная последовательность добавляется к 5&apos;-концу левого праймера.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="353"/>
        <location filename="../src/Primer3Dialog.ui" line="363"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The provided sequence is added to the 5&apos; end of the right primer. It is reverse complementary to the template sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Предоставленная последовательность добавляется к 5&apos;-концу правого праймера. Она обратно комплементарна последовательности шаблона.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="398"/>
        <source>Overlap Junction List</source>
        <translation>Список перекрывающихся соединений</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="408"/>
        <source>Excluded Regions</source>
        <translation>Исключаемые регионы</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="418"/>
        <source>Pair OK Region List</source>
        <translation>Список ОК парных регионов</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="428"/>
        <source>Included region</source>
        <translation>Включенные регионы</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="440"/>
        <source>Start Codon Position</source>
        <translation>Позиция стартового кодона</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="452"/>
        <source>Start Codon Sequence</source>
        <translation>Последовательность стартового кодона</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="462"/>
        <source>Force Left Primer Start</source>
        <translation>Принудительное начало левого праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="472"/>
        <source>Force Left Primer End</source>
        <translation>Принудительный конец левого праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="482"/>
        <source>Five Matches on Primer&apos;s 5&apos;</source>
        <translation>Пять совпадений на 5&apos; конце</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="492"/>
        <source>Five Matches on Internal Oligo&apos;s  5&apos;</source>
        <translation>Пять совпадений на 5&apos; конце внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="385"/>
        <location filename="../src/Primer3Dialog.ui" line="503"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If one or more Targets is specified then a legal primer pair must flank at least one of them. A Target might be a simple sequence repeat site (for example a CA repeat) or a single-base-pair polymorphism. The value should be a space-separated list of &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs where &lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt; is the index of the first base of a Target, and length is its &lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если указан один или несколько Целевых регионов, то пара праймеров должна обрамлять как минимум один из них. Целевой регион может быть простым сайтом повтора последовательности (например, повтором CA) или полиморфизмом с одной парой оснований. Значение должно представлять собой разделенный пробелами список &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;начало&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt;&lt;/p&gt;&lt;p&gt;пар, где &lt;span style=&quot; font-weight:700;&quot;&gt;начало&lt;/span&gt; является порядковым номером первого нуклеотида Целевого региона, а его длина - это &lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="395"/>
        <location filename="../src/Primer3Dialog.ui" line="510"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If this list is not empty, then the forward OR the reverse primer must overlap one of these junction points by at least &lt;span style=&quot; font-weight:700;&quot;&gt;3 Prime Junction Overlap&lt;/span&gt; nucleotides at the 3&apos; end and at least &lt;span style=&quot; font-weight:700;&quot;&gt;5 Prime Junction Overlap&lt;/span&gt; nucleotides at the 5&apos; end.&lt;/p&gt;&lt;p&gt;In more detail: The junction associated with a given position is the space immediately to the right of that position in the template sequence on the strand given as input.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если этот список не пуст, то прямой ИЛИ обратный праймер должен перекрывать одну из этих точек из Списка Перекрывающихся Соединений как минимум на &lt;span style=&quot; font-weight:700;&quot;&gt;Перекрытие Первичного Соединения 3&lt;/span&gt; на 3&apos;-конце и не менее &lt;span style=&quot; font-weight:700;&quot;&gt;Перекрытие Первичного Соединения 5&lt;/span&gt; на 5&apos;-конце.&lt;/p&gt;&lt;p&gt;Более подробно: соединение, связанное с данной позицией, представляет собой пространство непосредственно справа от этой позиции в шаблонной последовательности на цепи, заданной в качестве входных данных.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="425"/>
        <location filename="../src/Primer3Dialog.ui" line="583"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A sub-region of the given sequence in which to pick primers. For example, often the first dozen or so bases of a sequence are vector, and should be excluded from consideration. The value for this parameter has the form&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;where &lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt; is the index of the first base to consider, and &lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt; is the number of subsequent bases in the primer-picking region.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Подобласть данной последовательности, в которой нужно выбрать праймеры. Например, часто первая дюжина или около того символов последовательности является вектором и должна быть исключена из рассмотрения. Значение этого параметра имеет вид &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;начало&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt;&lt;/p&gt;&lt;p&gt;где &lt;span style=&quot; font-weight:700;&quot;&gt;начало&lt;/span&gt; это индекс первого символа для рассмотрения, а &lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt; - количество последующих символов в области выбора праймера.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="610"/>
        <source>Force Right Primer End</source>
        <translation>Принудительный конец правого праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="469"/>
        <location filename="../src/Primer3Dialog.ui" line="623"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Forces the 3&apos; end of the left primer to be at the indicated position. Primers are also picked if they violate certain constraints. The default value indicates that the end of the left primer can be anywhere.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Заставляет 3&apos;-конец левого праймера находиться в указанной позиции. Праймеры также выбираются, если они нарушают определенные ограничения. Значение по умолчанию указывает, что конец левого праймера может быть где угодно.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="459"/>
        <location filename="../src/Primer3Dialog.ui" line="658"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Forces the 5&apos; end of the left primer to be at the indicated position. Primers are also picked if they violate certain constraints. The default value indicates that the start of the left primer can be anywhere.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Заставляет 5&apos;-конец левого праймера находиться в указанной позиции. Праймеры также выбираются, если они нарушают определенные ограничения. Значение по умолчанию указывает, что конец левого праймера может быть где угодно.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="489"/>
        <location filename="../src/Primer3Dialog.ui" line="700"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Five Matches on Primer&apos;s 5&apos;&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Пять совпадений на 5&apos; конце&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="710"/>
        <source>Five Matches on Primer&apos;s 3&apos;</source>
        <translation>Пять совпадений на 3&apos; конце</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="724"/>
        <location filename="../src/Primer3Dialog.ui" line="731"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Five Matches on Primer&apos;s 3&apos;&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Пять совпадений на 3&apos; конце&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="734"/>
        <source>Five Matches on Internal Oligo&apos;s 3&apos;</source>
        <translation>Пять совпадений на 3&apos; конце внутренного олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="435"/>
        <location filename="../src/Primer3Dialog.ui" line="741"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This parameter should be considered EXPERIMENTAL at this point. Please check the output carefully; some erroneous inputs might cause an error in Primer3.&lt;br&gt;&lt;br&gt;
Index of the first base of a start codon. This parameter allows Primer3 to select primer pairs to create in-frame amplicons e.g. to create a template for a fusion protein. Primer3 will attempt to select an in-frame left primer, ideally starting at or to the left of the start codon, or to the right if necessary. Negative values of this parameter are legal if the actual start codon is to the left of available sequence. If this parameter is non-negative Primer3 signals an error if the codon at the position specified by this parameter is not an ATG. A value less than or equal to -10^6 indicates that Primer3 should ignore this parameter.&lt;br&gt;&lt;br&gt;
Primer3 selects the position of the right primer by scanning right from the left primer for a stop codon. Ideally the right primer will end at or after the stop codon.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;На данный момент этот параметр следует считать ЭКСПЕРИМЕНТАЛЬНЫМ. Пожалуйста, внимательно проверьте выходные данные; некоторые ошибочные входные данные могут вызвать ошибку в Primer3.&lt;br&gt;&lt;br&gt;
Индекс первого символа стартового кодона. Этот параметр позволяет Primer3 выбирать пары праймеров для создания внутрирамочных ампликонов, например, для создания шаблона для слитого белка. Primer3 попытается выбрать левый праймер в рамке, в идеале начиная с начального кодона или слева от него, или справа, если необходимо. Отрицательные значения этого параметра допустимы, если фактический стартовый кодон находится слева от доступной последовательности. Если этот параметр неотрицательный, Primer3 сигнализирует об ошибке, если кодон в позиции, указанной этим параметром, не является ATG. Значение, меньшее или равное -10^6, указывает на то, что Primer3 должен игнорировать этот параметр.&lt;br&gt;&lt;br&gt;
Primer3 выбирает положение правого праймера, сканируя справа от левого праймера стоп-кодон. В идеале правильный праймер должен заканчиваться на стоп-кодоне или после него.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="781"/>
        <location filename="../src/Primer3Dialog.ui" line="804"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Forces the 5&apos; end of the right primer to be at the indicated position. Primers are also picked if they violate certain constraints. The default value indicates that the start of the right primer can be anywhere.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Заставляет 5&apos;-конец правого праймера находиться в указанной позиции. Праймеры также выбираются, если они нарушают определенные ограничения. Значение по умолчанию указывает, что конец левого праймера может быть где угодно.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="807"/>
        <source>Force Right Primer Start</source>
        <translation>Принудительный конец правого праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="607"/>
        <location filename="../src/Primer3Dialog.ui" line="820"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Forces the 3&apos; end of the right primer to be at the indicated position. Primers are also picked if they violate certain constraints. The default value indicates that the end of the right primer can be anywhere.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Заставляет 3&apos;-конец правого праймера находиться в указанной позиции. Праймеры также выбираются, если они нарушают определенные ограничения. Значение по умолчанию указывает, что конец левого праймера может быть где угодно.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="447"/>
        <location filename="../src/Primer3Dialog.ui" line="836"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The sequence of the start codon, by default ATG. Some bacteria use different start codons, this tag allows to specify alternative start codons.

Any triplet can be provided as start codon.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Последовательность стартового кодона, по умолчанию ATG. Некоторые бактерии используют разные стартовые кодоны, этот тег позволяет указать альтернативные стартовые кодоны.

Любой триплет может быть использован в качестве стартового кодона.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="841"/>
        <source>ATG</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="866"/>
        <source>Product Size Ranges</source>
        <translation>Диапазоны размеров продукта</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="889"/>
        <source>Max Library Mispriming</source>
        <translation>Макс. несоответствие бибилотеке праймеров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="958"/>
        <source>Max 3&apos; Stability</source>
        <translation>Макс. стабильность на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1025"/>
        <source>Pair Max Library Mispriming</source>
        <translation>Макс. несоответствие библиотеке праймеров (пары)</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1889"/>
        <location filename="../src/Primer3Dialog.ui" line="1976"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This is the most stable monomer structure of internal oligo calculated by thermodynamic approach. The hairpin loops, bulge loops, internal loops, internal single mismatches, dangling ends, terminal mismatches have been considered. This parameter is calculated only if &lt;span style=&quot; font-weight:700;&quot;&gt;Use Thermodynamic Oligo Alignment&lt;/span&gt; is checked. The default value is 10 degrees lower than the default value of &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Tm Min&lt;/span&gt;. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Это наиболее стабильная мономерная структура внутреннего олигонуклеотида, рассчитанная термодинамическим подходом. Были рассмотрены петли-шпильки, петли-выпуклости, внутренние петли, внутренние одиночные несовпадения, свисающие концы, концевые несовпадения. Этот параметр рассчитывается только в том случае, если &lt;span style=&quot; font-weight:700;&quot;&gt;Использовать термодинамическое олигонуклеотидное выравнивание&lt;/span&gt; отмечено. Значение по умолчанию на 10 градусов ниже, чем значение по умолчанию &lt;span style=&quot; font-weight:700;&quot;&gt;Миннимамльная Т. плав. праймера&lt;/span&gt;. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1207"/>
        <location filename="../src/Primer3Dialog.ui" line="1400"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The annealing temperature (Celsius) used in the PCR reaction. Usually it is chosen up to 10°C below the melting temperature of the primers. If provided, Primer3 will calculate the fraction of primers bound at the provided annealing temperature for each oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Температура отжига (по Цельсию), используемая в реакции ПЦР. Обычно ее выбирают на 10°С ниже температуры плавления праймеров. Если задано, Primer3 вычислит долю праймеров, связанных при указанной температуре отжига для каждого олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1275"/>
        <source>Primer Bound</source>
        <translation>Связь праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1142"/>
        <location filename="../src/Primer3Dialog.ui" line="1272"/>
        <location filename="../src/Primer3Dialog.ui" line="1334"/>
        <location filename="../src/Primer3Dialog.ui" line="1360"/>
        <location filename="../src/Primer3Dialog.ui" line="1549"/>
        <location filename="../src/Primer3Dialog.ui" line="1559"/>
        <location filename="../src/Primer3Dialog.ui" line="1778"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum acceptable fraction of primer bound at &lt;span style=&quot; font-weight:700;&quot;&gt;Annealing Temperature&lt;/span&gt; for a primer oligo in percent.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальная, оптимальная и максимальная допустимая доля Связи праймера при &lt;span style=&quot; font-weight:700;&quot;&gt;Температуре отжига&lt;/span&gt; для олигонуклеотида в процентах.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1403"/>
        <source>Annealing Temperature</source>
        <translation>Температура отжига</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1848"/>
        <source>Use Thermodynamic Oligo Alignment</source>
        <translation>Использовать термодинамическое олигонуклеотидное выравнивание</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1860"/>
        <location filename="../src/Primer3Dialog.ui" line="1999"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Same as&lt;span style=&quot; font-weight:700;&quot;&gt; Max 3&apos; Self Complementarity&lt;/span&gt; but is based on thermodynamical approach - the stability of structure is analyzed.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;То же, что и &lt;span style=&quot; font-weight:700;&quot;&gt; Макс. самокомплементарность на 3&apos;&lt;/span&gt;, но основанная на термодинамическом подходе - анализируется стабильность структуры.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1902"/>
        <location filename="../src/Primer3Dialog.ui" line="2158"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Same as &lt;span style=&quot; font-weight:700;&quot;&gt;Max 3&apos; Pair Complementarity&lt;/span&gt; but for calculating the score (melting temperature of structure) thermodynamical approach is used.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;То же, что и &lt;span style=&quot; font-weight:700;&quot;&gt;Макс. Парная Комплементарность на 3&apos;&lt;/span&gt;, но для расчета значения (температуры плавления структуры) используется термодинамический подход&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1921"/>
        <source>Max 3&apos; Pair Complementarity</source>
        <translation>Макс. парная комплементарность на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1928"/>
        <location filename="../src/Primer3Dialog.ui" line="2168"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Describes the tendency of a primer to bind to itself (interfering with target sequence binding). It will score ANY binding occurring within the entire primer sequence.&lt;/p&gt;&lt;p&gt;It is the maximum allowable local alignment score when testing a single primer for (local) self-complementarity. Local self-complementarity is taken to predict the tendency of primers to anneal to each other without necessarily causing self-priming in the PCR. The scoring system gives 1.00 for complementary bases, -0.25 for a match of any base (or N) with an N, -1.00 for a mismatch, and -2.00 for a gap. Only single-base-pair gaps are allowed.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Описывает тенденцию праймера связываться с самим собой (препятствуя связыванию целевой последовательности). Будет оцениваться ЛЮБОЕ связывание, происходящее во всей последовательности праймера.
&lt;/p&gt;&lt;p&gt;Это максимально допустимая оценка локального выравнивания при тестировании одного праймера на (локальную) самокомплементарность. Локальная самокомплементарность используется для предсказания тенденции праймеров отжигаться друг с другом, не обязательно вызывая самозаполнение в ПЦР. Система подсчета очков дает 1,00 за комплементарные символы, -0,25 за совпадение любого символа (или N) с N, -1,00 за несоответствие и -2,00 за гэп. Разрешаются только одинарные пары гэпов.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1947"/>
        <source>TH: Max Self Complementarity</source>
        <translation>TH: Макс. самокомплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1944"/>
        <location filename="../src/Primer3Dialog.ui" line="1960"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The same as &lt;span style=&quot; font-weight:700;&quot;&gt;Max Self Complementarity&lt;/span&gt; but all calculations are based on thermodynamical approach.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;То же, что и &lt;span style=&quot; font-weight:700;&quot;&gt;Макс. самокомплементарность&lt;/span&gt; но все расчеты основаны на термодинамическом подходе.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1979"/>
        <source>TH: Max Primer Hairpin	</source>
        <translation>TH: Макс. шпилечная структура	</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2002"/>
        <source>TH: Max 3&apos; Self Complementarity</source>
        <translation>TH: Макс. самокомплементарность на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2025"/>
        <source>Max Pair Complementarity</source>
        <translation>Макс. парная комплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2045"/>
        <location filename="../src/Primer3Dialog.ui" line="2074"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Describes the tendency of the left primer to bind to the right primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Описывает тенденцию левого праймера связываться с правым праймером.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2077"/>
        <source>TH: Max Pair Complementarity</source>
        <translation>TH: Макс. парная комплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2022"/>
        <location filename="../src/Primer3Dialog.ui" line="2129"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Describes the tendency of the left primer to bind to the right primer. It is the maximum allowable local alignment score when testing for complementarity between left and right primers. It is similar to &lt;span style=&quot; font-weight:700;&quot;&gt;Max Self Complementarity&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Описывает тенденцию левого праймера связываться с правым праймером. Это максимально допустимая оценка локального выравнивания при тестировании на комплементарность между левым и правым праймерами. Это похоже на &lt;span style=&quot; font-weight:700;&quot;&gt;Максимальную самокомплементарность&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2178"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Tries to bind the 3&apos;-END to a identical primer and scores the best binding it can find. This is critical for primer quality because it allows primers use itself as a target and amplify a short piece (forming a primer-dimer). These primers are then unable to bind and amplify the target sequence.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;Max 3&apos; Self Complementarity&lt;/span&gt; is the maximum allowable 3&apos;-anchored global alignment score when testing a single primer for self-complementarity. The 3&apos;-anchored global alignment score is taken to predict the likelihood of PCR-priming primer-dimers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Пытается связать 3&apos;-КОНЕЦ с идентичным праймером и оценивает лучшее связывание, которое может найти. Это имеет решающее значение для качества праймеров, поскольку позволяет праймерам использовать себя в качестве цели и амплифицировать короткий фрагмент (образуя праймер-димер). Затем эти праймеры не могут связываться и амплифицировать целевую последовательность.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;Макс. самокомплементарность на 3&apos;&lt;/span&gt; это максимально допустимая оценка глобального выравнивания с 3&apos;-привязкой при тестировании одного праймера на самодополняемость. Оценка глобального выравнивания с 3&apos;-привязкой используется для прогнозирования вероятности ПЦР-праймирования димеров праймеров.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2161"/>
        <source>TH: Max 3&apos; Pair Complementarity</source>
        <translation>TH: Макс. парная комплементарность на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2171"/>
        <source>Max Self Complementarity</source>
        <translation>Макс. самокомплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2181"/>
        <source>Max 3&apos; Self Complementarity</source>
        <translation>Макс. самокомплементарность на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2201"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Use thermodynamic models to calculate the the propensity of oligos to anneal to undesired sites in the template sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Используйте термодинамические модели для расчета склонности олигонуклеотидов к отжигу с нежелательными сайтами в шаблонной  последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2204"/>
        <source>Use Thermodynamic Template Alignment	</source>
        <translation>Использовать термодинамическое шаблонное выранивание	</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2222"/>
        <source>TH: Max Template Mispriming</source>
        <translation>TH: Макс. шаблон ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2219"/>
        <location filename="../src/Primer3Dialog.ui" line="2238"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Similar to &lt;span style=&quot; font-weight:700;&quot;&gt;Max template mispriming&lt;/span&gt; but assesses alternative binding sites in the template using thermodynamic models. This parameter specifies the maximum allowed melting temperature of an oligo (primer) at an &amp;quot;ectopic&amp;quot; site within the template sequence; 47.0 would be a reasonable choice if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Tm Min&lt;/span&gt; is 57.0.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;То же, что и &lt;span style=&quot; font-weight:700;&quot;&gt;Максимальный шаблон ошибочного праймирования&lt;/span&gt;но оценивает альтернативные сайты связывания в матрице с использованием термодинамических моделей. Этот параметр определяет максимально допустимую температуру плавления олиго (праймера) при &amp;quot;смещенном&amp;quot; сайт в шаблонной последовательности; 47.0 будет разумным выбором, если &lt;span style=&quot; font-weight:700;&quot;&gt;Минимальная Т. плав. праймера&lt;/span&gt; равна 57.0.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2254"/>
        <location filename="../src/Primer3Dialog.ui" line="2270"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed similarity to ectopic sites in the template. A negative value means do not check.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимально допустимое сходство с смещенными сайтами в шаблоне. При отрицательном значении не проверяется.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2305"/>
        <source>TH: Pair Max Template Mispriming</source>
        <translation>TH: Парный макс. шаблон ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2302"/>
        <location filename="../src/Primer3Dialog.ui" line="2315"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed summed melting temperatures of both primers at ectopic sites within the template (with the two primers in an orientation that would allow PCR amplification.) The melting temperatures are calculated as for &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max Template Mispriming&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимально допустимая суммарная температура плавления обоих праймеров на смещенных участках шаблона (с двумя праймерами в ориентации, позволяющей ПЦР-амплификацию). Температуры плавления рассчитываются как для параметра &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Максимальный шаблон ошибочного праймирования&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2331"/>
        <location filename="../src/Primer3Dialog.ui" line="2347"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed summed similarity of both primers to ectopic sites in the template. A negative value means do not check.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимально допустимое суммарное сходство обоих праймеров со смещенными сайтами в шаблоне. При отрицательном значении не учитывается.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2396"/>
        <source>Advanced Settings</source>
        <translation>Расширенные настройки</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2399"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section covers more accurate settings, which are related to &lt;span style=&quot; font-weight:700;&quot;&gt;General Settings&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;В этой секции описаны более точные настройки, относящиеся к &lt;span style=&quot; font-weight:700;&quot;&gt;Общим настройкам&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2409"/>
        <location filename="../src/Primer3Dialog.ui" line="2661"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The 5&apos; end of the left OR the right primer must overlap one of the junctions in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt; by this amount. See details in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;5&apos;-конец левого ИЛИ правого праймера должен перекрывать одно из соединений в &lt;span style=&quot; font-weight:700;&quot;&gt;Списке Перекрывающихся Соединений&lt;/span&gt; на это значение. Детальное описание смотри в &lt;span style=&quot; font-weight:700;&quot;&gt;Списке Перекрывающихся Соединений&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2431"/>
        <source>Sequencing Interval</source>
        <translation>Интервал секвенирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2441"/>
        <location filename="../src/Primer3Dialog.ui" line="2674"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Defines the space from the calculated position of the 3&apos;end to both sides in which Primer3Plus picks the best primer. Value only used if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Task&lt;/span&gt; is &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Определяет расстояние от рассчитанного положения 3&apos;-конца до обеих сторон, где Primer3 выбирает лучший праймер. Значение используется, только если &lt;span style=&quot; font-weight:700;&quot;&gt;Задачей подбора праймера&lt;/span&gt; является &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2482"/>
        <source>Max GC in primer 3&apos; end</source>
        <translation>Максимальный GC на 3&apos; конце праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2505"/>
        <source>Annealing Oligo Concentration</source>
        <translation>Концентрация олигонуклеотидов при отжиге</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2541"/>
        <source>Concentration of Divalent Cations</source>
        <translation>Концентрация двухвалентных катионов</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2479"/>
        <location filename="../src/Primer3Dialog.ui" line="2571"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum number of Gs or Cs allowed in the last five 3&apos; bases of a left or right primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимальное количество G или C, допустимое в последних пяти символах на 3&apos; конце левого или правого праймера.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2587"/>
        <location filename="../src/Primer3Dialog.ui" line="2943"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Analogous to &lt;span style=&quot; font-weight:700;&quot;&gt;3&apos; End Distance Between Left Primers&lt;/span&gt;, but for right primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Аналогично &lt;span style=&quot; font-weight:700;&quot;&gt;Расстояние между 3&apos; концами левых праймеров&lt;/span&gt;, но для правых праймеров.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2606"/>
        <location filename="../src/Primer3Dialog.ui" line="2801"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Defines the space from the 3&apos;end of the primer to the point were the trace signals are readable. Value only used if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Task&lt;/span&gt; is &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Определяет расстояние между 3&apos; концом праймера до точки, где след сигнала читаемый. Значение используется только если &lt;span style=&quot; font-weight:700;&quot;&gt;Задачей Подбора Праймеров&lt;/span&gt; является &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2622"/>
        <location filename="../src/Primer3Dialog.ui" line="2891"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The 3&apos; end of the left OR the right primer must overlap one of the junctions in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt; by this amount. See details in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;3&apos;-конец левого ИЛИ правого праймера должен перекрывать одно из соединений в &lt;span style=&quot; font-weight:700;&quot;&gt;Списке Перекрывающихся Соединений&lt;/span&gt; на это значение. Детальное описание смотри в &lt;span style=&quot; font-weight:700;&quot;&gt;Списке Перекрывающихся Соединений&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2664"/>
        <source>5 Prime Junction Overlap</source>
        <translation>5&apos; перерытие основного соединения</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2677"/>
        <source>Sequencing Accuracy</source>
        <translation>Точность секвенирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2778"/>
        <source>First Base Index</source>
        <translation>Индекс первого символа</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2804"/>
        <source>Sequencing Lead</source>
        <translation>Ведущее значение секвенирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2811"/>
        <location filename="../src/Primer3Dialog.ui" line="3001"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;When returning multiple primer pairs, the minimum number of base pairs between the 3&apos; ends of any two left primers.&lt;/p&gt;&lt;p&gt;Primers with 3&apos; ends at positions e.g. 30 and 31 in the template sequence have a three-prime distance of 1.&lt;/p&gt;&lt;p&gt;In addition to positive values, the values -1 and 0 are acceptable and have special interpretations:&lt;/p&gt;&lt;p&gt;-1 indicates that a given left primer can appear in multiple primer pairs returned by Primer3. This is the default behavior.&lt;/p&gt;&lt;p&gt;0 indicates that a left primer is acceptable if it was not already used. In other words, two left primers are allowed to have the same 3&apos; position provided their 5&apos; positions differ.&lt;/p&gt;&lt;p&gt;For n &amp;gt; 0: A left primer is acceptable if:&lt;/p&gt;&lt;p&gt;NOT(3&apos; end of left primer closer than n to the 3&apos; end of a previously used left primer)&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если найдено несколько пар праймеров, то это значение определяет минимальное количество спаренных оснований между 3&apos;-концами любых двух левых праймеров.&lt;/p&gt;&lt;p&gt;Праймеры с 3&apos;-концами в позициях, скажем, 30 и 31 в шаблонной последовательности, имеют расстояние между данное значение равное 1.&lt;/p&gt;&lt;p&gt;В дополнение к положительным значениям значения -1 и 0 тоже допустимы и имеют специальную интерпретацию:&lt;/p&gt;&lt;p&gt;-1 указывает, что данный левый праймер может появляться в нескольких парах праймеров, найденных с помощью Primer3. Это поведение по умолчанию.&lt;/p&gt;&lt;p&gt;0 означает, что левый праймер подходит, если он еще не использовался. Другими словами, два левых праймера могут иметь одинаковые 3&apos;-положения при условии, что их 5&apos;-положения различаются.&lt;/p&gt;&lt;p&gt;Для n &amp;gt; 0: A левый праймер подходит, если:&lt;/p&gt;&lt;p&gt;НЕ(3&apos;-конец левого праймера ближе, чем n, к 3&apos;-концу ранее использовавшегося левого праймера)&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2833"/>
        <source>Sequencing Spacing</source>
        <translation>Пробелы секвенирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2428"/>
        <location filename="../src/Primer3Dialog.ui" line="2843"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Defines the space from the 3&apos;end of the primer to the 3&apos;end of the next primer on the reverse strand. Value only used if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Task&lt;/span&gt; is &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Определяет пространство от 3&apos;-конца праймера до 3&apos;-конца следующего праймера на обратной цепи. Значение используется только если &lt;span style=&quot; font-weight:700;&quot;&gt;Задачей Подбора Праймеров&lt;/span&gt; является &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2894"/>
        <source>3 Prime Junction Overlap</source>
        <translation>3&apos; перекрытие основного соединения</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2946"/>
        <source>3&apos; End Distance Between Right Primers	</source>
        <translation>Расстояние между 3&apos; концами правых праймеров	</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2956"/>
        <source>Max #N&apos;s accepted</source>
        <translation>Максимально допустимое #N</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2963"/>
        <location filename="../src/Primer3Dialog.ui" line="3021"/>
        <source>Non-default values are valid only for sequences with 0 or 1 target regions. If the primer is part of a pair that spans a target and overlaps the target, then multiply this value times the number of nucleotide positions by which the primer overlaps the (unique) target to get the &apos;position penalty&apos;. The effect of this parameter is to allow Primer3 to include overlap with the target as a term in the objective function.</source>
        <translation>Значения, отличные от значений по умолчанию, допустимы только для последовательностей с 0 или 1 целевым регионом. Если праймер является частью пары, которая охватывает цель и перекрывает её, то умножьте это значение на количество позиций нуклеотидов, на которые праймер перекрывает (уникальную) цель, чтобы получить &quot;штраф за позицию&quot;. Эффект этого параметра заключается в том, чтобы позволить Primer3 включать перекрытие с целью в качестве члена в целевой функции.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3004"/>
        <source>3&apos; End Distance Between Left Primers</source>
        <translation>Расстояние между 3&apos; концами левых праймеров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3014"/>
        <source>DMSO Concentration</source>
        <translation>Концентрация ДМСО</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2830"/>
        <location filename="../src/Primer3Dialog.ui" line="3147"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Defines the space from the 3&apos;end of the primer to the 3&apos;end of the next primer on the same strand. Value only used if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Task&lt;/span&gt; is &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Определяет расстояние от 3&apos;-конца праймера до 3&apos;-конца следующего праймера на той же цепи. Значение используется только если &lt;span style=&quot; font-weight:700;&quot;&gt;Задачей Подбора Праймеров&lt;/span&gt; является &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3166"/>
        <source>Concentration of Monovalent Cations</source>
        <translation>Концентрация одновалентных катионов</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3198"/>
        <source>Formamide Concentration</source>
        <translation>Концентрация формамида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3208"/>
        <source>DMSO Factor</source>
        <translation>Фактор ДМСО</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3011"/>
        <location filename="../src/Primer3Dialog.ui" line="3215"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The concentration of DMSO in percent. See PRIMER_DMSO_FACTOR for details of Tm correction.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Концентрация ДМСО в процентах. Смотри &quot;Фактор ДСМО&quot; для дополнительной информации о коррекции температуры.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3205"/>
        <location filename="../src/Primer3Dialog.ui" line="3228"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The melting temperature of primers can be approximately corrected for DMSO:&lt;/p&gt;&lt;p&gt;Tm = Tm (without DMSO) - &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Factor&lt;/span&gt; * &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Concentration&lt;/span&gt;&lt;/p&gt;&lt;p&gt;The &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Concentration&lt;/span&gt; concentration must be given in %.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Температуру плавления праймеров можно приблизительно скорректировать на ДМСО:&lt;/p&gt;&lt;p&gt;Tm = Tm (без ДСМО) - &lt;span style=&quot; font-weight:700;&quot;&gt;Фактор ДСМО&lt;/span&gt; * &lt;span style=&quot; font-weight:700;&quot;&gt;Концентрация ДСМО&lt;/span&gt;&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;Концентрация ДСМО&lt;/span&gt; представлена в %.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3195"/>
        <location filename="../src/Primer3Dialog.ui" line="3247"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The concentration of formamide in mol/l. The melting temperature of primers can be approximately corrected for formamide:&lt;/p&gt;&lt;p&gt;Tm = Tm (without formamide) +(0.453 * PRIMER_[LEFT/INTERNAL/RIGHT]_4_GC_PERCENT / 100 - 2.88) * &lt;span style=&quot; font-weight:700;&quot;&gt;Formamide Concentration&lt;/span&gt;&lt;/p&gt;&lt;p&gt;The &lt;span style=&quot; font-weight:700;&quot;&gt;Formamide Concentration&lt;/span&gt; correction was suggested by Blake and Delcourt (R D Blake and S G Delcourt. Nucleic Acids Research, 24, 11:2095–2103, 1996).&lt;/p&gt;&lt;p&gt;Convert % into mol/l:&lt;/p&gt;&lt;p&gt;[DMSO in mol/l] = [DMSO in % weight] * 10 / 45.04 g/mol&lt;/p&gt;&lt;p&gt;[DMSO in mol/l] = [DMSO in % volume] * 10 * 1.13 g/cm3 / 45.04 g/mol&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Концентрация формамида в моль/л. Температуру плавления праймеров можно приблизительно скорректировать на формамид:&lt;/p&gt;&lt;p&gt;Tm = Tm (без формамида) +(0.453 * [GC-состав в процентах для правого/внутреннего/левого праймера] / 100 - 2.88) * &lt;span style=&quot; font-weight:700;&quot;&gt;Концентрация формамида&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Значение &lt;span style=&quot; font-weight:700;&quot;&gt;Концентрация формамида&lt;/span&gt; было предложено Blake and Delcourt (R D Blake and S G Delcourt. Nucleic Acids Research, 24, 11:2095–2103, 1996).&lt;/p&gt;&lt;p&gt;Преобразовать % в моль/л:&lt;/p&gt;&lt;p&gt;[ДМСО в моль/л] = [массДМСО в %] * 10 / 45.04 г/моль&lt;/p&gt;&lt;p&gt;[ДМСО в моль/л] = [объем ДМСО в %] * 10 * 1.13 г/см3 / 45.04 г/моль&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3287"/>
        <source>Use formatted output</source>
        <translation>Использовать форматированный вывод</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3297"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If this flag is true, produce PRIMER_LEFT_EXPLAIN, PRIMER_RIGHT_EXPLAIN, PRIMER_INTERNAL_EXPLAIN and/or PRIMER_PAIR_EXPLAIN output tags as appropriate.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если этот флаг установлен, то выходные тэги PRIMER_LEFT_EXPLAIN, PRIMER_RIGHT_EXPLAIN, PRIMER_INTERNAL_EXPLAIN и/или PRIMER_PAIR_EXPLAIN будут сгенерированы по возможности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3300"/>
        <source>Print Statistics</source>
        <translation>Статистика паймеров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3310"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the associated value = 1, then Primer3 will print out the calculated secondary structures&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если отмечено, то Primer3 распечатает рассчитанные вторичные структуры.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3313"/>
        <source>Print secondary structures</source>
        <translation>Напечатать вторичные структуры</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3331"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If set to 1, treat ambiguity codes as if they were consensus codes when matching oligos to mispriming or mishyb libraries. For example, if this flag is set, then a C in an oligo will be scored as a perfect match to an S in a library sequence, as will a G in the oligo. More importantly, though, any base in an oligo will be scored as a perfect match to an N in the library. This is very bad if the library contains strings of Ns, as no oligo will be legal (and it will take a long time to find this out). So unless you know for sure that your library does not have runs of Ns (or Xs), then set this flag to 0.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если задано, то обрабатывайте неоднозначности кода так, как если бы они были согласованными кодами при сопоставлении олигонуклеотидов с библиотеками mispriming или mishyb. Например, если этот флаг установлен, то C в олигонуклеотиде будет считаться полным совпадением с S в библиотечной последовательности, как и G в олигонуклеотиде.Однако, что еще более важно, любое основание в олигонуклеотида будет оцениваться как идеальное совпадение с N в библиотеке. Это очень плохо, если в библиотеке есть строки из N, так как ни один олигонуклеотид не будет легальным (и выяснять это придется долго). Поэтому, если вы точно не знаете, что в вашей библиотеке нет серий N (или X), установите этот флаг на 0.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3334"/>
        <source>Treat ambiguity codes in libraries as consensus</source>
        <translation>Относить неоднозначности кода в библиотеке к консенсусу</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3344"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If true use primer provided in left, right, or internal primer even if it violates specific constraints.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если true, используйте праймер, указанный в левом, правом или внутреннем праймере, даже если он нарушает определенные ограничения.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3347"/>
        <source>Pick anyway</source>
        <translation>Выбрать в любом случае</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3357"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This option allows for intelligent design of primers in sequence in which masked regions (for example repeat-masked regions) are lower-cased. (New in v. 1.1.0, added by Maido Remm and Triinu Koressaar)&lt;/p&gt;&lt;p&gt;A value of 1 directs Primer3 to reject primers overlapping lowercase a base exactly at the 3&apos; end.&lt;/p&gt;&lt;p&gt;This property relies on the assumption that masked features (e.g. repeats) can partly overlap primer, but they cannot overlap the 3&apos;-end of the primer. In other words, lowercase bases at other positions in the primer are accepted, assuming that the masked features do not influence the primer performance if they do not overlap the 3&apos;-end of primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эта опция позволяет интеллектуально проектировать праймеры в последовательности, в которой маскированные области (например, повторяющиеся маскированные области) имеют нижний регистр.&lt;/p&gt;&lt;p&gt;Выставленное значение указывает Primer3 отклонять праймеры, перекрывающие основание в нижнем регистре точно на 3&apos;-конце.&lt;/p&gt;&lt;p&gt;Это свойство основано на предположении, что маскированные признаки (например, повторы) могут частично перекрывать праймер, но не могут перекрывать 3&apos;-конец праймера. Другими словами, символы нижнего регистра в других позициях в праймере допускаются, предполагая, что маскированные признаки не влияют на эффективность праймера, если они не перекрывают 3&apos;-конец праймера.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3367"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This parameter provides a quick-and-dirty way to get Primer3 to accept IUB / IUPAC codes for ambiguous bases (i.e. by changing all unrecognized bases to N). If you wish to include an ambiguous base in an oligo, you must set &lt;span style=&quot; font-weight:700;&quot;&gt;Max #N&apos;s accepted&lt;/span&gt; to a 1 (non-0) value.&lt;/p&gt;&lt;p&gt;Perhaps &apos;-&apos; and &apos;* &apos; should be squeezed out rather than changed to &apos;N&apos;, but currently they simply get converted to N&apos;s. The authors invite user comments.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Этот параметр обеспечивает быстрый и грязный способ заставить Primer3 принимать коды IUB / IUPAC для неоднозначных оснований. (т.е. заменив все нераспознанные базы на N). Если вы хотите включить неоднозначное основание в олигонуклеотид, вы должны выставить &lt;span style=&quot; font-weight:700;&quot;&gt;Максимально допустимое #N&lt;/span&gt; на не-нулевое значение.&lt;/p&gt;&lt;p&gt;Возможно, «-» и «*» следует убрать, а не заменить на «N», но в настоящее время они просто преобразуются в N.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3370"/>
        <source>Liberal base</source>
        <translation>Свободный символ</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3412"/>
        <source>Internal Oligo Excluded Region</source>
        <translation>Исключаемые регионы внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3409"/>
        <location filename="../src/Primer3Dialog.ui" line="3419"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Middle oligos may not overlap any region specified by this tag. The associated value must be a space-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs, where &lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt; is the index of the first base of an excluded region, and &lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt; is its length. Often one would make Target regions excluded regions for internal oligos.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Средние олигонуклеотиды не могут перекрывать какую-либо область, указанную этим тегом. Связанное значение должно быть списком разделенных пробелом пар&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;начало&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt;&lt;/p&gt;&lt;p&gt;, где &lt;span style=&quot; font-weight:700;&quot;&gt;начало&lt;/span&gt; индекс первого символа исключенной области, а &lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt; - это её длина. Достаточно частая задача - сделать &quot;Целевые рерионы&quot; исключенными регионами для внутренних олигонуклеотидов.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3429"/>
        <source>Internal Oligo Overlap Positions</source>
        <translation>Позиции перекрывания внутренних олигонуклеотидов</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3426"/>
        <location filename="../src/Primer3Dialog.ui" line="3436"/>
        <location filename="../src/Primer3Dialog.ui" line="4198"/>
        <location filename="../src/Primer3Dialog.ui" line="4272"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The 5&apos; end of the middle oligo / probe must overlap one of the junctions in &lt;span style=&quot; font-weight:700;&quot;&gt;Internal Oligo Overlap Positions&lt;/span&gt; by this amount. See details in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;5&apos;-конец среднего олигонуклеотида должен перекрывать одно из соединений в &lt;span style=&quot; font-weight:700;&quot;&gt;Позициях перекрывания внутреннего олигонуклеотида&lt;/span&gt; на это значение. Смотри детальное описание параметра &lt;span style=&quot; font-weight:700;&quot;&gt;Список перекрывающихся соединений&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3457"/>
        <source>Internal Oligo Bound</source>
        <translation>Связь внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3541"/>
        <source>Internal Oligo GC%</source>
        <translation>GC% внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3605"/>
        <source>Internal Oligo Tm</source>
        <translation>Т. плав. внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3615"/>
        <source>Internal Oligo Size</source>
        <translation>Размер внутренного олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3454"/>
        <location filename="../src/Primer3Dialog.ui" line="3655"/>
        <location filename="../src/Primer3Dialog.ui" line="3761"/>
        <location filename="../src/Primer3Dialog.ui" line="3777"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer Bound for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Параметр, эквивалентный Связи праймера для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3817"/>
        <location filename="../src/Primer3Dialog.ui" line="3932"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max Self Complementarity&lt;/span&gt; for the internal oligo&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Макс. самокомплементарность&lt;/span&gt; для внутреннего олигонуклеотида&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3839"/>
        <location filename="../src/Primer3Dialog.ui" line="3868"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Same as &lt;span style=&quot; font-weight:700;&quot;&gt;Internal Oligo Max 3&apos; End Self Complementarity&lt;/span&gt; but for calculating the score (melting temperature of structure) thermodynamical approach is used&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;То же что и  &lt;span style=&quot; font-weight:700;&quot;&gt;Макс. самокомплементарность на 3&apos; для внутреннего олигонуклеотида&lt;/span&gt; но с использованием термодинамического подхода при расчете&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3871"/>
        <source>TH: Internal Oligo Max 3&apos; End Self Complementarity	</source>
        <translation>TH: Макс. самокомплементарность на 3&apos; для внутреннего олигонуклеотида	</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3884"/>
        <location filename="../src/Primer3Dialog.ui" line="3922"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer &lt;span style=&quot; font-weight:700;&quot;&gt;Max Self Complementarity&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Макс. самокомплементарность&lt;/span&gt; для внутреннего олигонуклеотида&lt;/p&gt;&lt;/body&gt;&lt;/html</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3906"/>
        <location filename="../src/Primer3Dialog.ui" line="3987"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer &lt;span style=&quot; font-weight:700;&quot;&gt;Max 3&apos; Self Complementarity&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Макс. самокомплементарность на 3&apos;&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3925"/>
        <location filename="../src/Primer3Dialog.ui" line="5916"/>
        <source>Internal Oligo Self Complementarity</source>
        <translation>Самокомплементарность внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3935"/>
        <location filename="../src/Primer3Dialog.ui" line="5829"/>
        <source>TH: Internal Oligo Self Complementarity</source>
        <translation>TH: Макс. самокомплементарность внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3961"/>
        <location filename="../src/Primer3Dialog.ui" line="3977"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The most stable monomer structure of internal oligo calculated by thermodynamic approach. See &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max Primer Hairpin&lt;/span&gt;	 for details&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Наиболее стабильная мономерная структура внутреннего олигонуклеотида, рассчитанная термодинамическим подходом. Смотри &lt;span style=&quot; font-weight:700;&quot;&gt;TH: макс. шпилечаная структура&lt;/span&gt; для деталей&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3980"/>
        <location filename="../src/Primer3Dialog.ui" line="5887"/>
        <source>TH: Internal Oligo Hairpin</source>
        <translation>TH: Шпилечная структура внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3990"/>
        <source>Internal Oligo Max 3&apos; End Self Complementarity</source>
        <translation>Самокомплементарность внутреннего олигонуклеотида на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4011"/>
        <source>Internal Oligo Max Poly-X</source>
        <translation>Макс. poly-x внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4021"/>
        <source>Internal Oligo DNA Concentration</source>
        <translation>Концентрация ДНК внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4031"/>
        <source>Internal Oligo Formamide Concentration</source>
        <translation>Концентрация формамида внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4044"/>
        <location filename="../src/Primer3Dialog.ui" line="4243"/>
        <location filename="../src/Primer3Dialog.ui" line="4288"/>
        <location filename="../src/Primer3Dialog.ui" line="4308"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Similar to &lt;span style=&quot; font-weight:700;&quot;&gt;Max Library Mispriming&lt;/span&gt; except that this parameter applies to the similarity of candidate internal oligos to the library specified in &lt;span style=&quot; font-weight:700;&quot;&gt;Internal Oligo Mishyb Library&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;То же что и &lt;span style=&quot; font-weight:700;&quot;&gt;Максимальное несоответствие библиотеке праймеров&lt;/span&gt; за исключением того, что этот параметр применяется к сходству внутренних олигонуклеотидов-кандидатов с библиотекой, указанной в &lt;span style=&quot; font-weight:700;&quot;&gt;Библиотека Mishyb Внутреннего Олиго&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4054"/>
        <source>Internal Oligo Min Sequence Quality</source>
        <translation>Минимальное качество последовательности внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4067"/>
        <location filename="../src/Primer3Dialog.ui" line="4090"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Parameter for internal oligos analogous to &lt;span style=&quot; font-weight:700;&quot;&gt;Concentration of dNTPs&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Параметр для внутренних олигонуклеотидов, аналогичный &lt;span style=&quot; font-weight:700;&quot;&gt;Концентрации dNTPs&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/ HTML&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4093"/>
        <source>Internal Oligo [dNTP]</source>
        <translation>Внутренние олигонуклеотиды [dNTP]</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4018"/>
        <location filename="../src/Primer3Dialog.ui" line="4106"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Annealing Oligo Concentration&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Концентрация олигонуклеотидов при отжиге&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt; /html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4051"/>
        <location filename="../src/Primer3Dialog.ui" line="4128"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Min sequence quality&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Минимальное качество последовательности&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt; /html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4147"/>
        <location filename="../src/Primer3Dialog.ui" line="4208"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Concentration of Monovalent Cations&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Концентрация одновалентных катионов&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt; &lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4166"/>
        <source>Internal Oligo DMSO Concentration</source>
        <translation>Концнтрация ДМСО для внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4179"/>
        <location filename="../src/Primer3Dialog.ui" line="4318"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Concentration of Divalent Cations&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Концентрация двухвалентных катионов&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt; &lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4201"/>
        <source>Internal Oligo 5 Prime Junction Overlap</source>
        <translation>5&apos; перекрытие основного соединения для внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4211"/>
        <source>Internal Oligo Conc of monovalent cations</source>
        <translation>Концентрация одновалентных катионов внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4080"/>
        <location filename="../src/Primer3Dialog.ui" line="4224"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer &lt;span style=&quot; font-weight:700;&quot;&gt;Max #Ns&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр праймера &lt;span style=&quot; font-weight:700;&quot;&gt;Мксимально допустимое к #Ns&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt; &lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4028"/>
        <location filename="../src/Primer3Dialog.ui" line="4259"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Formamide Concentration&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Концентрация формамида&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/ HTML&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4291"/>
        <source>Internal Oligo Mishyb Library</source>
        <translation>Библиотека mishyb внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4301"/>
        <source>Internal Oligo 3 Prime Junction Overlap</source>
        <translation>3&apos; перекрытие основного соединения для внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4311"/>
        <source>Internal Oligo Max Library Mishyb	</source>
        <translation>Макс. библиотеки Mishyb внутреннего олигонуклеотида	</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4321"/>
        <source>Internal Oligo conc of divalent cations</source>
        <translation>Концентрация двухвалентных катионов внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4163"/>
        <location filename="../src/Primer3Dialog.ui" line="4328"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Concentration&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр праймера &lt;span style=&quot; font-weight:700;&quot;&gt;Концентрация ДМСО&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body &gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4008"/>
        <location filename="../src/Primer3Dialog.ui" line="4347"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer &lt;span style=&quot; font-weight:700;&quot;&gt;Max poly-X&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр праймера &lt;span style=&quot; font-weight:700;&quot;&gt;Макс. poly-X&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body &gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4363"/>
        <location filename="../src/Primer3Dialog.ui" line="4382"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Factor&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;фактор ДМСО&lt;/span&gt; для внутреннего олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/ HTML&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4385"/>
        <source>Internal Oligo DMSO Factor</source>
        <translation>Фактор ДМСО внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4298"/>
        <location filename="../src/Primer3Dialog.ui" line="4392"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The 3&apos; end of the middle oligo / probe must overlap one of the junctions in &lt;span style=&quot; font-weight:700;&quot;&gt;Internal Oligo Overlap Positions&lt;/span&gt; by this amount. See details in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;3&apos;-конец среднего олигонуклеотида должен перекрывать одно из соединений в &lt;span style=&quot; font-weight:700;&quot;&gt;Позициях перекрывания внутреннего олигонуклеотида&lt;/span&gt; на это значение. Смотри детальное описание параметра &lt;span style=&quot; font-weight:700;&quot;&gt;Список перекрывающихся соединений&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4408"/>
        <location filename="../src/Primer3Dialog.ui" line="4424"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Analogous to &lt;span style=&quot; font-weight:700;&quot;&gt;3&apos; End Distance Between Left Primers&lt;/span&gt;, but for internal primer / probe.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Аналогично &lt;span style=&quot; font-weight:700;&quot;&gt;Расстояние Между 3&apos; концами Левых Праймеров&lt;/span&gt;, но для внутреннего праймера.&lt;/p &gt;&lt;/тело&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4427"/>
        <source>Min Internal Oligo End Distance</source>
        <translation>Мин. расстояние между концами внутренних олигонуклеотидов</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4453"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section describes &amp;quot;penalty weights&amp;quot;, which allow the user to modify the criteria that Primer3 uses to select the &amp;quot;best&amp;quot; primers.&lt;/p&gt;&lt;p&gt;There are two classes of weights: for some parameters there is a &apos;Lt&apos; (less than) and a &apos;Gt&apos; (greater than) weight. These are the weights that Primer3 uses when the value is less or greater than (respectively) the specified optimum.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эта секция описывает &amp;quot;веса штрафов&amp;quot;, которые позволяют пользователю модифицировать критерии, которые Primer3 использует при выборе &amp;quot;лучших&amp;quot; праймеров.&lt;/p&gt;&lt;p&gt;Существует два класса параметров весов - &apos;Мч&apos; (менее чем) и &apos;Бч&apos; (более чем). Эти веса Primer3 использует в случаях, когда значение менее или более чем (соответственно) заданное оптимальное значение.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4477"/>
        <location filename="../src/Primer3Dialog.ui" line="4500"/>
        <location filename="../src/Primer3Dialog.ui" line="4531"/>
        <location filename="../src/Primer3Dialog.ui" line="4554"/>
        <location filename="../src/Primer3Dialog.ui" line="4570"/>
        <location filename="../src/Primer3Dialog.ui" line="4586"/>
        <location filename="../src/Primer3Dialog.ui" line="4609"/>
        <location filename="../src/Primer3Dialog.ui" line="4619"/>
        <location filename="../src/Primer3Dialog.ui" line="4635"/>
        <location filename="../src/Primer3Dialog.ui" line="4645"/>
        <location filename="../src/Primer3Dialog.ui" line="4655"/>
        <location filename="../src/Primer3Dialog.ui" line="4665"/>
        <location filename="../src/Primer3Dialog.ui" line="5795"/>
        <location filename="../src/Primer3Dialog.ui" line="5805"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for corressponding parameter of a primer less than (Lt) or greater than (Gt) the optimal value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для соответствующего параметра праймера меньше чем (Lt) или больше чем (Gt), что является оптимальным значением.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4668"/>
        <source>Bound</source>
        <translation>Связь</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4703"/>
        <source>TH: Self Complementarity</source>
        <translation>TH: Самокомплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4726"/>
        <source>TH: 3&apos; End Self Complementarity	</source>
        <translation>TH: Самокомплементарность на 3&apos;	</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4749"/>
        <source>TH: Hairpin</source>
        <translation>TH: Шпильки</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4775"/>
        <source>TH: Template Mispriming</source>
        <translation>TH: Шаблон ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4801"/>
        <source>Self Complementarity</source>
        <translation>Самокомплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4700"/>
        <location filename="../src/Primer3Dialog.ui" line="4710"/>
        <location filename="../src/Primer3Dialog.ui" line="4723"/>
        <location filename="../src/Primer3Dialog.ui" line="4733"/>
        <location filename="../src/Primer3Dialog.ui" line="4746"/>
        <location filename="../src/Primer3Dialog.ui" line="4756"/>
        <location filename="../src/Primer3Dialog.ui" line="4772"/>
        <location filename="../src/Primer3Dialog.ui" line="4785"/>
        <location filename="../src/Primer3Dialog.ui" line="4798"/>
        <location filename="../src/Primer3Dialog.ui" line="4814"/>
        <location filename="../src/Primer3Dialog.ui" line="4827"/>
        <location filename="../src/Primer3Dialog.ui" line="4843"/>
        <location filename="../src/Primer3Dialog.ui" line="4856"/>
        <location filename="../src/Primer3Dialog.ui" line="4872"/>
        <location filename="../src/Primer3Dialog.ui" line="4896"/>
        <location filename="../src/Primer3Dialog.ui" line="4912"/>
        <location filename="../src/Primer3Dialog.ui" line="4925"/>
        <location filename="../src/Primer3Dialog.ui" line="4935"/>
        <location filename="../src/Primer3Dialog.ui" line="4948"/>
        <location filename="../src/Primer3Dialog.ui" line="4964"/>
        <location filename="../src/Primer3Dialog.ui" line="4977"/>
        <location filename="../src/Primer3Dialog.ui" line="4993"/>
        <location filename="../src/Primer3Dialog.ui" line="5035"/>
        <location filename="../src/Primer3Dialog.ui" line="5051"/>
        <location filename="../src/Primer3Dialog.ui" line="5064"/>
        <location filename="../src/Primer3Dialog.ui" line="5080"/>
        <location filename="../src/Primer3Dialog.ui" line="5256"/>
        <location filename="../src/Primer3Dialog.ui" line="5272"/>
        <location filename="../src/Primer3Dialog.ui" line="5285"/>
        <location filename="../src/Primer3Dialog.ui" line="5301"/>
        <location filename="../src/Primer3Dialog.ui" line="5317"/>
        <location filename="../src/Primer3Dialog.ui" line="5336"/>
        <location filename="../src/Primer3Dialog.ui" line="5826"/>
        <location filename="../src/Primer3Dialog.ui" line="5842"/>
        <location filename="../src/Primer3Dialog.ui" line="5855"/>
        <location filename="../src/Primer3Dialog.ui" line="5871"/>
        <location filename="../src/Primer3Dialog.ui" line="5884"/>
        <location filename="../src/Primer3Dialog.ui" line="5900"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for a primer parameter different from predefined optimum value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для параметра праймера отличается от оптимального предопределенного значения.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4830"/>
        <source>3&apos; End Self Complementarity</source>
        <translation>Самокомплементарность на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4859"/>
        <source>Template Mispriming</source>
        <translation>Шаблон ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4928"/>
        <location filename="../src/Primer3Dialog.ui" line="5479"/>
        <source>Library Mispriming</source>
        <translation>Библиотека ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5006"/>
        <location filename="../src/Primer3Dialog.ui" line="5022"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Determines the overall weight of the position penalty in calculating the penalty for a primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Позиция штрафа.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5067"/>
        <source>Primer failure rate</source>
        <translation>Величина ошибки праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5146"/>
        <location filename="../src/Primer3Dialog.ui" line="5162"/>
        <location filename="../src/Primer3Dialog.ui" line="5178"/>
        <location filename="../src/Primer3Dialog.ui" line="5188"/>
        <location filename="../src/Primer3Dialog.ui" line="5204"/>
        <location filename="../src/Primer3Dialog.ui" line="5221"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for corressponding parameter of a primer pair less than (Lt) or greater than (Gt) the optimal value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для соответствующего параметра пары праймеров меньше чем (Lt) или больше чем (Gt), что является оптимальным значением.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5259"/>
        <source>TH: Any Complementarity</source>
        <translation>TH: Любая комплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5288"/>
        <source>TH: 3&apos; End Complementarity</source>
        <translation>TH: Комплементарность на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5320"/>
        <source>TH: Template Mispriming Weight</source>
        <translation>TH: Вес шаблона ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5352"/>
        <source>Any Complementarity</source>
        <translation>Любая комплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5349"/>
        <location filename="../src/Primer3Dialog.ui" line="5365"/>
        <location filename="../src/Primer3Dialog.ui" line="5378"/>
        <location filename="../src/Primer3Dialog.ui" line="5394"/>
        <location filename="../src/Primer3Dialog.ui" line="5407"/>
        <location filename="../src/Primer3Dialog.ui" line="5423"/>
        <location filename="../src/Primer3Dialog.ui" line="5447"/>
        <location filename="../src/Primer3Dialog.ui" line="5463"/>
        <location filename="../src/Primer3Dialog.ui" line="5476"/>
        <location filename="../src/Primer3Dialog.ui" line="5492"/>
        <location filename="../src/Primer3Dialog.ui" line="5505"/>
        <location filename="../src/Primer3Dialog.ui" line="5521"/>
        <location filename="../src/Primer3Dialog.ui" line="5534"/>
        <location filename="../src/Primer3Dialog.ui" line="5550"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for a primer pair parameter different from predefined optimum value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для параметра пары праймеров отличается от оптимального предопределенного значения.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5381"/>
        <source>3&apos; End Complementarity</source>
        <translation>Комплементарность на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5410"/>
        <source>Template Mispriming Weight</source>
        <translation>Вес шаблона ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5450"/>
        <source>Tm Difference</source>
        <translation>Разница температуры плавления</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5508"/>
        <source>Primer Penalty Weight</source>
        <translation>Вес штрафа праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5537"/>
        <source>Internal Oligo Penalty Weight	</source>
        <translation>Вес штрафа внутреннего олигонуклеотида	</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5594"/>
        <source>Internal Oligos</source>
        <translation>Внутренние олигонуклеотиды</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5606"/>
        <source>Internal oligo Size</source>
        <translation>Размер внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5603"/>
        <location filename="../src/Primer3Dialog.ui" line="5633"/>
        <location filename="../src/Primer3Dialog.ui" line="5649"/>
        <location filename="../src/Primer3Dialog.ui" line="5665"/>
        <location filename="../src/Primer3Dialog.ui" line="5688"/>
        <location filename="../src/Primer3Dialog.ui" line="5718"/>
        <location filename="../src/Primer3Dialog.ui" line="5728"/>
        <location filename="../src/Primer3Dialog.ui" line="5744"/>
        <location filename="../src/Primer3Dialog.ui" line="5754"/>
        <location filename="../src/Primer3Dialog.ui" line="5771"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for corressponding parameter of a internal oligo less than (Lt) or greater than (Gt) the optimal value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для соответствующего параметра внутреннего олигонуклеотида меньше чем (Lt) или больше чем (Gt), что является оптимальным значением.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2561"/>
        <location filename="../src/Primer3Dialog.ui" line="2878"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Non-default values are valid only for sequences with 0 or 1 target regions.&lt;/p&gt;&lt;p&gt;If the primer is part of a pair that spans a target and does not overlap the target, then multiply this value times the number of nucleotide positions from the 3&apos; end to the (unique) target to get the &apos;position penalty&apos;. The effect of this parameter is to allow Primer3 to include nearness to the target as a term in the objective function.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Значения, отличные от значений по умолчанию, допустимы только для последовательностей с целевым регионом 0 или 1.&lt;/p&gt;&lt;p&gt;Если праймер является частью пары, которая охватывает цель и не перекрывает мишень, то умножьте это значение на количество позиций нуклеотидов от 3&apos;-конца до (уникальной) мишени, чтобы получить «штраф за позицию». Эффект этого параметра заключается в том, чтобы позволить Primer3 включить ближайшие к цели в качестве члена целевой функции.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="142"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This tag tells Primer3 what task to perform. Legal values are:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;generic&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Design a primer pair (the classic Primer3 task) if the &lt;span style=&quot; font-weight:700;&quot;&gt;Pick left primer &lt;/span&gt;is checked and &lt;span style=&quot; font-weight:700;&quot;&gt;Pick right primer&lt;/span&gt; is checked. In addition, pick an internal hybridization oligo if &lt;span style=&quot; font-weight:700;&quot;&gt;Pick internal oligo&lt;/span&gt; is checked.&lt;/p&gt;&lt;p&gt;NOTE: If &lt;span style=&quot; font-weight:700;&quot;&gt;Pick left primer&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt; Pick right primer&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;Pick internal oligo &lt;/span&gt;are checked, then behaves similarly to &lt;span style=&quot; font-weight:700;&quot;&gt;Primer task &lt;/span&gt;is&lt;span style=&quot; font-weight:700;&quot;&gt; pick_primer_list&lt;/span&gt;.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;check_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 only checks the primers provided in &lt;span style=&quot; font-weight:700;&quot;&gt;Left primer&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;Internal oligo&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;Right primer&lt;/span&gt;. It is the only task that does not require a sequence.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_primer_list&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick all primers in sequence (possibly limited by &lt;span style=&quot; font-weight:700;&quot;&gt;Include region&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;Exclude region&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;Pair OK Region List&lt;/span&gt;, etc.). Returns the primers sorted by quality starting with the best primers. If &lt;span style=&quot; font-weight:700;&quot;&gt;Pick left primer&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;Pick right primer &lt;/span&gt;are selected, Primer3 does not to pick primer pairs but generates independent lists of left primers, right primers, and, if requested, internal oligos.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick primers suited to sequence a region. The position of each primer is calculated for optimal sequencing results.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;pick_cloning_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick primers suited to clone a gene were the start nucleotide and the end nucleotide of the PCR fragment must be fixed, for example to clone an ORF. &lt;span style=&quot; font-weight:700;&quot;&gt;Included region&lt;/span&gt; must be used to indicate the first and the last nucleotide. Due to these limitations Primer3 can only vary the length of the primers. Set &lt;span style=&quot; font-weight:700;&quot;&gt;Pick anyway&lt;/span&gt; to obtain primers even if they violate specific constraints.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_discriminative_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick primers suited to select primers which bind with their end at a specific position. This can be used to force the end of a primer to a polymorphic site, with the goal of discriminating between sequence variants. &lt;span style=&quot; font-weight:700;&quot;&gt;Targets&lt;/span&gt; must be used to provide a single target indicating the last nucleotide of the left (nucleotide before the first nucleotide of the target) and the right primer (nucleotide following the target). The primers border the &lt;span style=&quot; font-weight:700;&quot;&gt;Targets&lt;/span&gt;. Due to these limitations Primer3 can only vary the length of the primers. Set &lt;span style=&quot; font-weight:700;&quot;&gt;Pick anyway&lt;/span&gt; to obtain primers even if they violate specific constraints.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Этот параметр сообщает Primer3, какую задачу выполнять. Возможные значения:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;generic&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Дизайн праймеров (классическая задача Primer3) если &lt;span style=&quot; font-weight:700;&quot;&gt;Выбрать левый праймер &lt;/span&gt;отмечен и &lt;span style=&quot; font-weight:700;&quot;&gt;Выбрать правый праймер&lt;/span&gt; отмечен. Кроме того, выберите внутренний гибридизационный олиго, если &lt;span style=&quot; font-weight:700;&quot;&gt;Выбрать внутренний олигонуклеотид&lt;/span&gt; отмечен.&lt;/p&gt;&lt;p&gt;ЗАМЕЧАНИЕ: Если &lt;span style=&quot; font-weight:700;&quot;&gt;Выбрать левый праймер&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt; Выбрать правый праймер&lt;/span&gt; и &lt;span style=&quot; font-weight:700;&quot;&gt;Выбрать внутренний олигонуклеотид &lt;/span&gt;отмечены, тогда результат такой же как и для &lt;span style=&quot; font-weight:700;&quot;&gt;Задачи&lt;/span&gt; &lt;span style=&quot; font-weight:700;&quot;&gt; pick_primer_list&lt;/span&gt;.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;check_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 только проверяет праймеры, представленные в &lt;span style=&quot; font-weight:700;&quot;&gt;Левом праймере&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;Внутренном олигонуклеотиде&lt;/span&gt; и &lt;span style=&quot; font-weight:700;&quot;&gt;Правом праймере&lt;/span&gt;. Это единственная задача, которая не требует последовательности.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_primer_list&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Выбрать все праймеры в последовательности (возможно ограничение по праметрам &lt;span style=&quot; font-weight:700;&quot;&gt;Включенные регионы&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;Исключаемые регионы&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;Список ОК парных регионов&lt;/span&gt;, и т.д.). Возвращает праймеры, отсортированные по качеству, начиная с лучших праймеров. Если &lt;span style=&quot; font-weight:700;&quot;&gt;Выбрать левый праймер&lt;/span&gt; и &lt;span style=&quot; font-weight:700;&quot;&gt;Выбрать правый праймер &lt;/span&gt;выбраны, Primer3 не выбирает пары праймеров, а генерирует независимые списки левых праймеров, правых праймеров и, если требуется, внутренних олигонуклеотидов.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;ыберите праймеры, подходящие для секвенирования области. Положение каждого праймера рассчитывается для оптимальных результатов секвенирования.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;pick_cloning_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Выберите праймеры, подходящие для клонирования гена, где начальный нуклеотид и конечный нуклеотид фрагмента ПЦР должны быть зафиксированы, например, для клонирования ORF. Параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Включенные регионы&lt;/span&gt; должен использоваться для обозначения первого и последнего нуклеотидов. Из-за этих ограничений Primer3 может изменять только длину праймеров. Задайте &lt;span style=&quot; font-weight:700;&quot;&gt;Выбрать в любом случае&lt;/span&gt; для получения праймеров, даже если они нарушают определенные ограничения.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_discriminative_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Подбор таких праймеров, которые связываются своим концом в определенном положении. Это может быть использовано для принудительного прикрепления конца праймера к полиморфному сайту с целью различения вариантов последовательности. Параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Целевые регионы&lt;/span&gt; должен использоваться для получения единственной цели, указывающей последний нуклеотид слева (нуклеотид перед первым целевым нуклеотидом) и правый праймер (нуклеотид, следующий за целевым). Праймеры граничат с &lt;span style=&quot; font-weight:700;&quot;&gt;Целевыми регионами&lt;/span&gt;. Из-за этих ограничений Primer3 может изменять только длину праймеров. Задайте &lt;span style=&quot; font-weight:700;&quot;&gt;Выбрать в любом случае&lt;/span&gt; для получения праймеров, даже если они нарушают определенные ограничения.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="65"/>
        <source>Choose preset:</source>
        <translation>Выберите пресет:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="85"/>
        <source>Default</source>
        <translation>По-умолчанию</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="90"/>
        <source>Recombinase Polymerase Amplification</source>
        <translation>Рекомбиназная полимеразная амплификация</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="206"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the associated value is checked, then Primer3 will attempt to pick left primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если соответствующее значение отмечено флажком, Primer3 попытается выбрать левые праймеры.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="225"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the associated value is checked, then Primer3 will attempt to pick an internal oligo (hybridization probe to detect the PCR product).&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если соответствующее значение отмечено флажком, Primer3 попытается выбрать внутренний олигонуклеотид (зонд гибридизации для обнаружения продукта ПЦР).&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="228"/>
        <source>Pick internal oligo</source>
        <translation>Выберите внутренний олигонуклеотид</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="247"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the associated value is checked, then Primer3 will attempt to pick a right primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если выбрано соответствующее значение, Primer3 попытается выбрать правый праймер.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="290"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The sequence of a left primer to check and around which to design right primers and optional internal oligos.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Последовательность левого праймера, которую нужно проверить и вокруг которой нужно подобрать правые праймеры и, если задано, внутренние олигонуклеотиды.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="300"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The sequence of an internal oligo to check and around which to design left and right primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Последовательность внутреннего олигонуклеотида, который нужно проверить и вокруг которого подобрать левый и правый праймеры.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="310"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The sequence of a right primer to check and around which to design left primers and optional internal oligos.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Последовательность правого праймера, которую необходимо проверить и вокруг которой подобрать левые праймеры и необязательные внутренние олигонуклеотиды.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="405"/>
        <location filename="../src/Primer3Dialog.ui" line="537"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Primer oligos may not overlap any region specified in this tag. The associated value must be a space-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs where &lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt; is the index of the first base of the excluded region, and &lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt; is its length. This tag is useful for tasks such as excluding regions of low sequence quality or for excluding regions containing repetitive elements such as ALUs or LINEs.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Олигонуклеотиды праймеров не могут перекрывать какую-либо область, указанную в этом параметре. Значение должно быть списком разделенных пробелом пар&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;начало&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt;&lt;/p&gt;&lt;p&gt;где &lt;span style=&quot; font-weight:700;&quot;&gt;начало&lt;/span&gt; — это индекс первого символа исключаемой области, а &lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt; е длина. Этот тег полезен для таких задач, как исключение областей с низким качеством последовательности или для исключения областей, содержащих повторяющиеся элементы, такие как ALU или LINE.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="415"/>
        <location filename="../src/Primer3Dialog.ui" line="530"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This tag allows detailed specification of possible locations of left and right primers in primer pairs.&lt;/p&gt;&lt;p&gt;The associated value must be a semicolon-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;left_start&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;left_length&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;right_start&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;right_length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;quadruples. The left primer must be in the region specified by &lt;span style=&quot; font-weight:700;&quot;&gt;left_start&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;left_length&lt;/span&gt; and the right primer must be in the region specified by &lt;span style=&quot; font-weight:700;&quot;&gt;right_start&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;right_length&lt;/span&gt;. &lt;span style=&quot; font-weight:700;&quot;&gt;left_start&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;left_length&lt;/span&gt; specify the location of the left primer in terms of the index of the first base in the region and the length of the region. &lt;span style=&quot; font-weight:700;&quot;&gt;right_start&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;right_length&lt;/span&gt; specify the location of the right primer in analogous fashion. As seen in the example below, if no integers are specified for a region then the location of the corresponding primer is not constrained.&lt;/p&gt;&lt;p&gt;Example:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;Pair OK Region List&lt;/span&gt;=100,50,300,50 ; 900,60,, ; ,,930,100&lt;/p&gt;&lt;p&gt;Specifies that there are three choices:&lt;/p&gt;&lt;p&gt;Left primer in the 50 bp region starting at position 100 AND right primer in the 50 bp region starting at position 300&lt;/p&gt;&lt;p&gt;OR&lt;/p&gt;&lt;p&gt;Left primer in the 60 bp region starting at position 900 (and right primer anywhere)&lt;/p&gt;&lt;p&gt;OR&lt;/p&gt;&lt;p&gt;Right primer in the 100 bp region starting at position 930 (and left primer anywhere)&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Этот параметр позволяет подробно указать возможные положения левого и правого праймеров в парах праймеров.&lt;/p&gt;&lt;p&gt;Значение должно быть списком четверок, разделенных точкой с запятой: &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;левое_начало&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;левая_длина&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;правое_начало&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;правая_длина&lt;/span&gt;&lt;/p&gt;&lt;p&gt;. Левый праймер должен находиться в области, указанной в значении &lt;span style=&quot; font-weight:700;&quot;&gt;левое_начало&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;левая_длина&lt;/span&gt; и правый праймер должен быть в области, указанной в паре &lt;span style=&quot; font-weight:700;&quot;&gt;правое_начало&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;правая_длина&lt;/span&gt;. &lt;span style=&quot; font-weight:700;&quot;&gt;левое_начало&lt;/span&gt; и &lt;span style=&quot; font-weight:700;&quot;&gt;левая_длина&lt;/span&gt; определяют расположение левого праймера с точки зрения индекса первого символа в регионе и длины региона. &lt;span style=&quot; font-weight:700;&quot;&gt;правое_начало&lt;/span&gt; и &lt;span style=&quot; font-weight:700;&quot;&gt;правая_длина&lt;/span&gt; аналогичным образом указывают расположение правого праймера. Как видно из приведенного ниже примера, если для региона не указаны целые числа, то расположение соответствующего праймера не ограничено.&lt;/p&gt;&lt;p&gt;Пример:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;Список ОК парных регионов&lt;/span&gt;=100,50,300,50 ; 900,60,, ; ,,930,100&lt;/p&gt;&lt;p&gt;Указывает, что есть три варианта:&lt;/p&gt;&lt;p&gt;Левый праймер находится в области длиной 100 нуклеотидов начиная с 50-го нуклеотида, а правый праймер - в области в 50 нуклеотидов начиная с 300-го нуклеотида&lt;/p&gt;&lt;p&gt;ИЛИ&lt;/p&gt;&lt;p&gt;Левый праймер находится в области длиной 60 нуклеотидов начиная с 900-го нуклеотида (а правый праймер - где угодно)&lt;/p&gt;&lt;p&gt;ИЛИ&lt;/p&gt;&lt;p&gt;Правый праймер - в области в 100 нуклеотидов начиная с 930-го нуклеотида (а левый праймер - где угодно)&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="479"/>
        <location filename="../src/Primer3Dialog.ui" line="717"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Discards all primers which do not match this match sequence at the 5&apos; end. (New in v. 2.3.6, added by A. Untergasser.)&lt;br/&gt;&lt;br/&gt;The match sequence must be 5 nucleotides long and can contain the following letters:&lt;/p&gt;&lt;p&gt;N Any nucleotide&lt;br/&gt;A Adenine&lt;br/&gt;G Guanine&lt;br/&gt;C Cytosine&lt;br/&gt;T Thymine&lt;br/&gt;R Purine (A or G)&lt;br/&gt;Y Pyrimidine (C or T)&lt;br/&gt;W Weak (A or T)&lt;br/&gt;S Strong (G or C)&lt;br/&gt;M Amino (A or C)&lt;br/&gt;K Keto (G or T)&lt;br/&gt;B Not A (G or C or T)&lt;br/&gt;H Not G (A or C or T)&lt;br/&gt;D Not C (A or G or T)&lt;br/&gt;V Not T (A or G or C)&lt;/p&gt;&lt;p&gt;Any primer which will not match the entire match sequence at the 5&apos; end will be discarded and not evaluated. Setting strict requirements here will result in low quality primers due to the high numbers of primers discarded at this step.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Отбрасывает все праймеры, которые не соответствуют этой последовательности соответствия на 5&apos; конце. (Новый с v. 2.3.6, автор A. Untergasser.)&lt;br/&gt;&lt;br/&gt;Последовательность должна состоять из 5 нуклеотидов и может содержать следующие буквы:&lt;/p&gt;&lt;p&gt;N Любой нуклеотид&lt;br/&gt;A Аденин&lt;br/&gt;G Гуанин&lt;br/&gt;C Цитозин&lt;br/&gt;T Тимин&lt;br/&gt;R Пурин (A или G)&lt;br/&gt;Y Пиримидин (C или T)&lt;br/&gt;W Слабый (A или T)&lt;br/&gt;S Сильный (G или C)&lt;br/&gt;M Амино (A или C)&lt;br/&gt;K Кето (G или T)&lt;br/&gt;B не A (G или C или T)&lt;br/&gt;H не G (A или C или T)&lt;br/&gt;D не C (A или G или T)&lt;br/&gt;V не T (A или G или C)&lt;/p&gt;&lt;p&gt;Любой праймер, который не будет соответствовать всей последовательности соответствия на 5&apos; конце, будет отброшен и не будет оцениваться. Установление здесь строгих требований приведет к получению праймеров низкого качества из-за большого количества праймеров, отбрасываемых на этом этапе.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="707"/>
        <location filename="../src/Primer3Dialog.ui" line="797"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Discards all primers which do not match this match sequence at the 3&apos; end. Similar parameter to &lt;span style=&quot; font-weight:700;&quot;&gt;Five Matches on Primer&apos;s 5&apos;&lt;/span&gt;, but limits the 3&apos; end. (New in v. 2.3.6, added by A. Untergasser.)&lt;br/&gt;&lt;br/&gt;The match sequence must be 5 nucleotides long and can contain the following letters:&lt;/p&gt;&lt;p&gt;N Any nucleotide&lt;br/&gt;A Adenine&lt;br/&gt;G Guanine&lt;br/&gt;C Cytosine&lt;br/&gt;T Thymine&lt;br/&gt;R Purine (A or G)&lt;br/&gt;Y Pyrimidine (C or T)&lt;br/&gt;W Weak (A or T)&lt;br/&gt;S Strong (G or C)&lt;br/&gt;M Amino (A or C)&lt;br/&gt;K Keto (G or T)&lt;br/&gt;B Not A (G or C or T)&lt;br/&gt;H Not G (A or C or T)&lt;br/&gt;D Not C (A or G or T)&lt;br/&gt;V Not T (A or G or C)&lt;/p&gt;&lt;p&gt;Any primer which will not match the entire match sequence at the 3&apos; end will be discarded and not evaluated. Setting strict requirements here will result in low quality primers due to the high numbers of primers discarded at this step.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Отбрасывает все праймеры, которые не соответствуют этой последовательности соответствия на 3&apos; конце. Схоже с параметром &lt;span style=&quot; font-weight:700;&quot;&gt;Пять совпадений на 5&apos; конце&lt;/span&gt;, но для 3&apos; конца. (Новый с v. 2.3.6, автор A. Untergasser.)&lt;br/&gt;&lt;br/&gt;Последовательность должна состоять из 5 нуклеотидов и может содержать следующие буквы:&lt;/p&gt;&lt;p&gt;N Любой нуклеотид&lt;br/&gt;A Аденин&lt;br/&gt;G Гуанин&lt;br/&gt;C Цитозин&lt;br/&gt;T Тимин&lt;br/&gt;R Пурин (A или G)&lt;br/&gt;Y Пиримидин (C или T)&lt;br/&gt;W Слабый (A или T)&lt;br/&gt;S Сильный (G или C)&lt;br/&gt;M Амино (A или C)&lt;br/&gt;K Кето (G или T)&lt;br/&gt;B не A (G или C или T)&lt;br/&gt;H не G (A или C или T)&lt;br/&gt;D не C (A или G или T)&lt;br/&gt;V не T (A или G или C)&lt;/p&gt;&lt;p&gt;Любой праймер, который не будет соответствовать всей последовательности соответствия на 3&apos; конце, будет отброшен и не будет оцениваться. Установление здесь строгих требований приведет к получению праймеров низкого качества из-за большого количества праймеров, отбрасываемых на этом этапе.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1845"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Use thermodynamic models to calculate the propensity of oligos to form hairpins and dimers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Используйте термодинамические модели для расчета склонности олигонуклеотидов к образованию шпилек и димеров.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1918"/>
        <location filename="../src/Primer3Dialog.ui" line="2084"/>
        <location filename="../src/Primer3Dialog.ui" line="2113"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Tries to bind the 3&apos;-END of the left primer to the right primer and scores the best binding it can find. It is similar to &lt;span style=&quot; font-weight:700;&quot;&gt;Max 3&apos; Self Complementarity&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Пытается связать 3&apos;-КОНЕЦ левого праймера с правым праймером и оценивает лучшее связывание, которое может найти. То же самое, что и &lt;span style=&quot; font-weight:700;&quot;&gt;Макс. самокомплементарность на 3&apos;&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5731"/>
        <source>Internal oligo Tm</source>
        <translation>Т плав. внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5757"/>
        <source>Internal oligo GC%</source>
        <translation>GC% внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5774"/>
        <source>Internal oligo Bound</source>
        <translation>Связь внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5858"/>
        <source>TH: Internal Oligo 3&apos; End Complementarity</source>
        <translation>TH: Комплементарность на 3&apos; внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5913"/>
        <location filename="../src/Primer3Dialog.ui" line="5929"/>
        <location filename="../src/Primer3Dialog.ui" line="5942"/>
        <location filename="../src/Primer3Dialog.ui" line="5958"/>
        <location filename="../src/Primer3Dialog.ui" line="5982"/>
        <location filename="../src/Primer3Dialog.ui" line="5998"/>
        <location filename="../src/Primer3Dialog.ui" line="6011"/>
        <location filename="../src/Primer3Dialog.ui" line="6027"/>
        <location filename="../src/Primer3Dialog.ui" line="6040"/>
        <location filename="../src/Primer3Dialog.ui" line="6056"/>
        <location filename="../src/Primer3Dialog.ui" line="6069"/>
        <location filename="../src/Primer3Dialog.ui" line="6085"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for an internal oligo parameter different from predefined optimum value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для параметра внутреннего олигонуклеотида отличается от оптимального предопределенного значения.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5945"/>
        <source>Internal Oligo 3&apos; End Complementarity</source>
        <translation>Комплементарность на 3&apos; внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5985"/>
        <source>Internal Oligo #N&apos;s</source>
        <translation>#N внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6014"/>
        <source>Internal Oligo Library Mishybing</source>
        <translation>Библиотечный mishybing внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6043"/>
        <source>Internal Oligo Sequence Quality</source>
        <translation>Качество последовательности внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6072"/>
        <source>Internal Oligo Sequence End Quality</source>
        <translation>Качество конца последовательности внутреннего олигонуклеотида</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6241"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;In this section user can specify if primer product must overlap exon-exon junction or span intron. This only applies when designing primers for a cDNA (mRNA) sequence with annotated exons.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;В этой секции пользователь может определить должен ли праймер иметь экзон-экзон перекрытие или протяжный интрон. Работает только в случае дизайна праймеров для cDNA (mRNA) последовательностей, аннотированных экзонами.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6247"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This feature allows to specify if primer product must span intron-exon boundaries.&lt;/p&gt;&lt;p&gt;Checking this option will result in ignoring &lt;span style=&quot; font-style:italic;&quot;&gt;Excluded&lt;/span&gt; and &lt;span style=&quot; font-style:italic;&quot;&gt;Target Regions&lt;/span&gt; from Main section.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эта опция позволяет указать если праймер должен охватывать интрон-экзон границы.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6277"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The name of the annotation which defines the exons in the mRNA sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Имя экзон аннотации.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6382"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the range is set, primer search will be restricted to selected exons. For example:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;1-5&lt;/span&gt;&lt;/p&gt;&lt;p&gt;If the range is larger than actual exon range or the starting exon number exceeds number of exons, error message is shown. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если диапазон установлен, поиск праймеров будет ограничен выбранными экзонами. Например:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;1-5&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Если диапазон больше фактического диапазона экзонов или начальный номер экзона превышает количество экзонов, отображается сообщение об ошибке. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6423"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This number of found primer pairs wil bel queried to check if they fullfill the requirements for RTPCR.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимальное число пар для запроса.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6121"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;In this section user can specify sequence quality of target sequence and related parameters.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;В данной секции пользователь может задать качество целевой последовательности и связанные параметры.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6134"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A list of space separated integers. There must be exactly one integer for each base in the Source Sequence if this argument is non-empty. High numbers indicate high confidence in the base call at that position and low numbers indicate low confidence in the base call at that position.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Список разделенных пробелами целых чисел.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6156"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum sequence quality allowed within a primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальное качество последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6179"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum sequence quality allowed within the 3&apos; pentamer of a primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальное 3&amp;apos;качество последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6202"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum legal sequence quality.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимум интервала качества последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6225"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum legal sequence quality.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Макс. интервала качества последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6643"/>
        <source>Result Settings</source>
        <translation>Параметры результатов</translation>
    </message>
</context>
<context>
    <name>Primer3TmCalculatorFactory</name>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorFactory.cpp" line="29"/>
        <source>Primer 3</source>
        <translation>Primer 3</translation>
    </message>
</context>
<context>
    <name>Primer3TmCalculatorSettingsWidget</name>
    <message>
        <location filename="../src/temperature/Primer3TmCalculatorSettingsWidget.ui" line="14"/>
        <source>Form</source>
        <translation>Form</translation>
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
        <location filename="../src/Primer3Query.cpp" line="170"/>
        <source>Primer</source>
        <translation>Праймер</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="171"/>
        <source>PCR primer design</source>
        <translation>Конструкция PCR праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="173"/>
        <source>Excluded regions</source>
        <translation>Исключенные регионы</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="174"/>
        <source>Targets</source>
        <translation>Целевые регионы</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="175"/>
        <source>Product size ranges</source>
        <translation>Диапазоны размера продукта</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="176"/>
        <source>Number to return</source>
        <translation>Результатов не более</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="178"/>
        <source>Max repeat mispriming</source>
        <translation>Максимальный повтор ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="179"/>
        <source>Max template mispriming</source>
        <translation>Максимальный шаблон ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="180"/>
        <source>Max 3&apos; stability</source>
        <translation>Макс. 3&apos; стабильность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="181"/>
        <source>Pair max repeat mispriming</source>
        <translation>Парный максимальный повтор ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="182"/>
        <source>Pair max template mispriming</source>
        <translation>Парный максимальный шаблон ошибочного праймирования</translation>
    </message>
</context>
<context>
    <name>U2::CheckComplementTask</name>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="37"/>
        <source>Check complement task</source>
        <translation>Задача проверки комплементарности</translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="44"/>
        <source>Sequence object has been closed, abort</source>
        <translation>Объект последовательности был закрыт, отмена</translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="70"/>
        <source>Check complement</source>
        <translation>Проверить комплементарность</translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="71"/>
        <source>The following filtering settings have been used</source>
        <translation>Следующие настройки фильрации были использованы</translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="73"/>
        <source>Max base pairs in dimers</source>
        <translation>Максимальное количество пар нуклеотидов в димере</translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="73"/>
        <source>bp</source>
        <translation>пн</translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="76"/>
        <source>Max dimer GC-content</source>
        <translation>Максимальный GC-состав димера</translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="78"/>
        <source>The following primers have been found and processed (red - filtered, green - passed)</source>
        <translation>Следующие праймеры были найдены и обработаны (красные - отильрованы, зеленые - прошли)</translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="83"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="138"/>
        <source>No.</source>
        <translation>Ном.</translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="84"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="139"/>
        <source>Strand</source>
        <translation>Направление</translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="85"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="140"/>
        <source>Primer</source>
        <translation>Праймер</translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="86"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="141"/>
        <source>Self-dimer</source>
        <translation>Гомодимер</translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="87"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="92"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="142"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="147"/>
        <source>Delta G (kcal/mol)</source>
        <translation>Энергия Гиббса (ккал/моль)</translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="88"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="93"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="143"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="148"/>
        <source>Base Pairs (bp)</source>
        <translation>Пары нуклеотидов (пн)</translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="89"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="94"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="144"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="149"/>
        <source>G/C pairs (bp)</source>
        <translation>Пары G/C (пн)</translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="90"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="95"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="145"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="150"/>
        <source>G/C-content (%)</source>
        <translation>GC-состав (%)</translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="91"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="146"/>
        <source>Hetero-dimer</source>
        <translation>Гетеродимер</translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="102"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="165"/>
        <source>Forward</source>
        <translation>Прямой</translation>
    </message>
    <message>
        <location filename="../src/task/CheckComplementTask.cpp" line="118"/>
        <location filename="../src/task/CheckComplementTask.cpp" line="179"/>
        <source>Reverse</source>
        <translation>Обратный</translation>
    </message>
</context>
<context>
    <name>U2::FindExonRegionsTask</name>
    <message>
        <location filename="../src/task/FindExonRegionsTask.cpp" line="93"/>
        <source>Failed to search for exon annotations. The sequence %1 doesn&apos;t have any related annotations.</source>
        <translation>Failed to search for exon annotations. The sequence %1 doesn&apos;t have any related annotations.</translation>
    </message>
</context>
<context>
    <name>U2::GTest</name>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="566"/>
        <source>Error in sequence quality data</source>
        <translation>Error in sequence quality data</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="570"/>
        <source>Sequence quality data missing</source>
        <translation>Sequence quality data missing</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="97"/>
        <location filename="../src/Primer3Tests.cpp" line="103"/>
        <source>Illegal SEQUENCE_TARGET value: %1</source>
        <translation>Недопустимое значение SEQUENCE_TARGET: %1</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="141"/>
        <source>Illegal SEQUENCE_OVERLAP_JUNCTION_LIST value: %1</source>
        <translation>Недопустимое значение SEQUENCE_OVERLAP_JUNCTION_LIST: %1</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="156"/>
        <source>Illegal SEQUENCE_INTERNAL_OVERLAP_JUNCTION_LIST value: %1</source>
        <translation>Недопустимое значение SEQUENCE_INTERNAL_OVERLAP_JUNCTION_LIST: %1</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="171"/>
        <source>Illegal SEQUENCE_EXCLUDED_REGION value: %1</source>
        <translation>Недопустимое значение SEQUENCE_EXCLUDED_REGION: %1</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="197"/>
        <source>Illegal SEQUENCE_PRIMER_PAIR_OK_REGION_LIST value: %1</source>
        <translation>Недопустимое значение SEQUENCE_PRIMER_PAIR_OK_REGION_LIST: %1</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="211"/>
        <source>Illegal SEQUENCE_INCLUDED_REGION value: %1</source>
        <translation>Недопустимое значение SEQUENCE_INCLUDED_REGION: %1</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="229"/>
        <location filename="../src/Primer3Tests.cpp" line="236"/>
        <source>Illegal SEQUENCE_INTERNAL_EXCLUDED_REGION value: %1</source>
        <translation>Недопустимое значение SEQUENCE_INTERNAL_EXCLUDED_REGION: %1</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="251"/>
        <source>Illegal PRIMER_PRODUCT_SIZE_RANGE value: %1</source>
        <translation>Недопустимое значение PRIMER_PRODUCT_SIZE_RANGE: %1</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="402"/>
        <source>Incorrect results num. Pairs: %1, left: %2, right: %3, inernal: %4</source>
        <translation>Некорректное количество результатов. Пары: %1, слева: %2, справа: %3, внутренние: %4</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="417"/>
        <location filename="../src/Primer3Tests.cpp" line="428"/>
        <location filename="../src/Primer3Tests.cpp" line="439"/>
        <source>Incorrect parameter: %1</source>
        <translation>Некорректный параметр: %1</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="593"/>
        <source>No error, but expected: %1</source>
        <translation>Нет ошибки, но ожидалось: %1</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="596"/>
        <source>An unexpected error. Expected: %1, but Actual: %2</source>
        <translation>Неожиданная ошибка. Ожидалось: %1, но фактическое значение: %2</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="605"/>
        <source>No warning, but expected: %1</source>
        <translation>Нет предупреждения, но ожидалось: %1</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="609"/>
        <source>An unexpected warning. Expected: %1, but Actual: %2</source>
        <translation>Неожиданное предупреждение. Ожидалось: %1, но фактическое значение: %2</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="892"/>
        <location filename="../src/Primer3Tests.cpp" line="900"/>
        <source>%1 is incorrect. Expected:%2, but Actual:%3</source>
        <translation>%1 is incorrect. Expected:%2, but Actual:%3</translation>
    </message>
</context>
<context>
    <name>U2::Primer3ADVContext</name>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="97"/>
        <source>Primer3...</source>
        <translation>Primer3: Подбор праймеров...</translation>
    </message>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="132"/>
        <location filename="../src/Primer3Plugin.cpp" line="145"/>
        <source>Error</source>
        <translation>Ошибка</translation>
    </message>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="133"/>
        <source>Cannot create an annotation object. Please check settings</source>
        <translation>Невозможно создать аннотацию. Проверьте настройки</translation>
    </message>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="146"/>
        <source>Result file path is empty. Please, set this value on the &quot;Result Settings&quot; tab</source>
        <translation>Не задан путь к файлу с результатом. Пожалуйста, задайте данное значение на вкладке &quot;Параметры результатов&quot;</translation>
    </message>
</context>
<context>
    <name>U2::Primer3Dialog</name>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="160"/>
        <source>NONE</source>
        <translation>нет</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="161"/>
        <source>HUMAN</source>
        <translation>HUMAN</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="162"/>
        <source>RODENT_AND_SIMPLE</source>
        <translation>RODENT_AND_SIMPLE</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="163"/>
        <source>RODENT</source>
        <translation>RODENT</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="164"/>
        <source>DROSOPHILA</source>
        <translation>DROSOPHILA</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="521"/>
        <source>The &quot;Include region&quot; should be the only one</source>
        <translation>&quot;Включенный регион&quot; должен быть только один</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="734"/>
        <source>Task &quot;pick_discriminative_primers&quot; requires exactly one &quot;Targets&quot; region.</source>
        <translation>Опция &quot;pick_discriminative_primers&quot; Требует ровно один &quot;Целевой регион&quot;.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="637"/>
        <source>Left primer</source>
        <translation>Левый праймер</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="628"/>
        <source>%1 sequence has incorrect alphabet, should be be simple DNA</source>
        <translation>%1 имеет некорректный алфавит, ожидается ДНК</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="146"/>
        <source>Save result to file</source>
        <translation>Сохранить рузльтат в файл</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="155"/>
        <source>Save to...</source>
        <translation>Сохранить в...</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="643"/>
        <source>Left 5&apos; overhang</source>
        <translation>Левое 5&apos; перекрытие</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="657"/>
        <source>Internal oligo</source>
        <translation>Внутренний олигонуклеотид</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="669"/>
        <source>Right primer</source>
        <translation>Правый праймер</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="675"/>
        <source>Right 5&apos; overhang</source>
        <translation>Правое 5&apos; перекрытие</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="702"/>
        <source>Sequence quality list length must be equal to the sequence length. Sequence length = %1, quality list length = %2.</source>
        <translation>Длина списка качества последовательности должна быть равна длине последовательности. Длина последовательности = %1ь длина списка качетсва = %2.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="750"/>
        <source>At least one primer on the &quot;Main&quot; settings page should be enabled - this is required by the &quot;check_primers&quot; task.</source>
        <translation>Как минимум один праймер на странице &quot;Основные параметры&quot; должен быть активен - это требование задачи подбора &quot;check_primers&quot;.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="753"/>
        <source>The %1 primer on the &quot;Main&quot; settings page is enabled, but not set.</source>
        <translatorcomment>Не нужно удалять этот пробел хоть тут и указвается неточность, он нужен из-за различий грамматики при переводе</translatorcomment>
        <translation> %1 праймер на странице &quot;Основные параметры&quot; активен, но не задан.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="760"/>
        <source>left</source>
        <translation>Левый</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="761"/>
        <source>internal</source>
        <translation>Внутренний</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="762"/>
        <source>right</source>
        <translation>Правый</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="785"/>
        <source>Primer Size Ranges should have at least one range</source>
        <translation>Диапазон размеров продукта должен иметь как миниум один диапазон</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="797"/>
        <source>Sequence range region is too small for current product size ranges</source>
        <translation>Регион последовательности слишком мал для текущего диапазона размера продукта</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="815"/>
        <source>The priming sequence is too long, please, decrease the region</source>
        <translation>Последовательность слишком длинная, пожалуйста, уменьшите регион</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="824"/>
        <source>Incorrect sum &quot;Included Region Start + First Base Index&quot; - should be more or equal than 0</source>
        <translation>Некорректная сумма по параметрам &quot;Начало включенного региона + Индекс первого символа&quot; - значение должно быть больше либо равно 0</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="829"/>
        <source>The priming sequence is out of range.
Either make the priming region end &quot;%1&quot; less or equal than the sequence size &quot;%2&quot; plus the first base index value &quot;%3&quot;or mark the sequence as circular</source>
        <translation>Последовательность праймирования все заданного региона.
Либо сделайте конец праймируемого региона %1 меньше либо равным размеру последовательности %2 плюс индекс первого сиимвола %3, либо отметьте последовательность круговой</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="840"/>
        <source>Result file path is empty. Please, set this value on the &quot;Result Settings&quot; tab</source>
        <translation>Не задан путь к файлу с результатом. Пожалуйста, задайте данное значение на вкладке &quot;Параметры результатов&quot;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="854"/>
        <source>%1 parameter(s) have an incorrect value(s), pay attention on red widgets. </source>
        <translation>%1 параметр(ов) имеют некорректное(ые) значение(я), обратите внимание на подсвеченные красным виджеты. </translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="856"/>
        <source>The following errors are possible: 

</source>
        <translation>Возможны следующие ошибки: 

</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="859"/>
        <source>

Click OK to continue calculation, but the incorrect values will be ignored.</source>
        <translation>

Нажмите ОК чтобы продолжить расчет, некорректные значения будут проигнорированы.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="867"/>
        <source>The &quot;%1&quot; parameter has incorrect value, please, read the tooltip of this parameter to find out how the correct one looks like.</source>
        <translation>Параметру &quot;%1&quot; установлено некорректное значение. Пожалуйста, прочитайте подсказку, всплывающую при наведении на данный параметр, чтобы понять, как откорректировать значение.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="974"/>
        <source>Default</source>
        <translation>По-умолчанию</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="978"/>
        <source>Recombinase Polymerase Amplification</source>
        <translation>Рекомбиназная полимеразная амплификация</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="982"/>
        <source>Info: &quot;Check complementary&quot; has been enabled (see the &quot;Posterior Actions&quot; tab)</source>
        <translation>Информация: &quot;Проверить комплементарность&quot; задана (см. вкладку &quot;Действия после расчета&quot;)</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="994"/>
        <source>Warning: &quot;Check complementary&quot; requires left and right primers enabled (&quot;Main&quot; page).</source>
        <translation>Внимание: параметр &quot;Проверить комплементарность&quot; требует параметры левого и правого праймера активными (Основные параметры).</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="996"/>
        <source>Warning: &quot;Check complementary&quot; requires any task but &quot;pick_primer_list&quot; (&quot;Main&quot; page).</source>
        <translation>Внимание: параметр &quot;Проверить комплементарность&quot; требует любую задачу кроме &quot;pick_primer_list&quot; (Основные параметры).</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="1012"/>
        <source>CSV report</source>
        <translation>Отчет CSV</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="1014"/>
        <source>Save CSV report to...</source>
        <translation>Сохранить CSV отчет в...</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="1121"/>
        <location filename="../src/Primer3Dialog.cpp" line="1130"/>
        <location filename="../src/Primer3Dialog.cpp" line="1139"/>
        <location filename="../src/Primer3Dialog.cpp" line="1151"/>
        <location filename="../src/Primer3Dialog.cpp" line="1204"/>
        <location filename="../src/Primer3Dialog.cpp" line="1210"/>
        <location filename="../src/Primer3Dialog.cpp" line="1245"/>
        <source>Can&apos;t parse &quot;%1&quot; value: &quot;%2&quot;</source>
        <translation>Невозможно распознать &quot;%1&quot; значение: &quot;%2&quot;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="1140"/>
        <source>Incorrect value for &quot;%1&quot; value: &quot;%2&quot;</source>
        <translation>Неверное значение для &quot;%1&quot; значение: &quot;%2&quot;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="1223"/>
        <source>PRIMER_MISPRIMING_LIBRARY value should points to the file from the &quot;%1&quot; directory</source>
        <translation>Значение PRIMER_MISPRIMING_LIBRARY должно указывать на файл из директории &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="1236"/>
        <source>PRIMER_INTERNAL_MISHYB_LIBRARY value should points to the file from the &quot;%1&quot; directory</source>
        <translation>Значение PRIMER_INTERNAL_MISHYB_LIBRARY должно указывать на файл из директории &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="1240"/>
        <source>PRIMER_MIN_THREE_PRIME_DISTANCE is unused in the UGENE GUI interface. We may either skip it or set PRIMER_MIN_LEFT_THREE_PRIME_DISTANCE and PRIMER_MIN_RIGHT_THREE_PRIME_DISTANCE to %1. Do you want to set?</source>
        <translation>PRIMER_MIN_THREE_PRIME_DISTANCE не используется в UGENE. Можно либо пропустить его, либо задать одновременно PRIMER_MIN_LEFT_THREE_PRIME_DISTANCE и PRIMER_MIN_RIGHT_THREE_PRIME_DISTANCE в %1. Хотите задать?</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="900"/>
        <source>Save primer settings</source>
        <translation>Сохранить настройки праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="911"/>
        <source>Text files</source>
        <translation>Текстовый файл</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="912"/>
        <source>Load settings</source>
        <translation>Загрузить настройки</translation>
    </message>
</context>
<context>
    <name>U2::Primer3Plugin</name>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="60"/>
        <source>Primer3</source>
        <translation>Primer3</translation>
    </message>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="60"/>
        <source>Integrated tool for PCR primers design.</source>
        <translation>Инструмент для дизайна праймеров.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="65"/>
        <source>Primer3 (no target sequence)...</source>
        <translation>Primer3 (без целевой последовательности)...</translation>
    </message>
</context>
<context>
    <name>U2::Primer3Task</name>
    <message>
        <location filename="../src/task/Primer3Task.cpp" line="45"/>
        <source>Pick primers task</source>
        <translation>Выбор праймеров</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3Task.cpp" line="59"/>
        <source>Incorrect sum &quot;Included Region Start + First Base Index&quot; - should be more or equal than 0</source>
        <translation>Некорректная сумма по параметрам &quot;Начало включенного региона + Индекс первого символа&quot; - значение должно быть больше либо равно 0</translation>
    </message>
</context>
<context>
    <name>U2::Primer3TopLevelTask</name>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="60"/>
        <source>Find primers with target sequence task</source>
        <translation>Поиск праймеров с целевой последовательностью</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="71"/>
        <source>Find primers without target sequence task</source>
        <translation>ПОиск праймеров без целевой последовательности</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="132"/>
        <source>High any compl</source>
        <translation>Выс any компл</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="133"/>
        <source>High 3&apos; compl</source>
        <translation>Выс 3&apos; компл</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="134"/>
        <source>High hairpins</source>
        <translation>Выс шпильки</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="135"/>
        <source>Considered</source>
        <translation>Рассмотрено</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="136"/>
        <source>Too many Ns</source>
        <translation>Сл много N</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="137"/>
        <source>In target</source>
        <translation>В цел рег</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="138"/>
        <source>In exclude</source>
        <translation>В искл рег</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="139"/>
        <source>Not OK reg</source>
        <translation>Не ОК рег</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="140"/>
        <source>Bad GC%</source>
        <translation>Плох GC%</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="141"/>
        <source>No GC clamp</source>
        <translation>Нет зажима GC</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="142"/>
        <source>Tm too low</source>
        <translation>Tm низкая</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="143"/>
        <source>Tm too high</source>
        <translation>Tm высокая</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="147"/>
        <source>Poly X</source>
        <translation>Poly X</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="148"/>
        <source>High end stab</source>
        <translation>Выс конц стаб</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="149"/>
        <source>OK</source>
        <translation>ОК</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="153"/>
        <source>th</source>
        <translation>th</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="200"/>
        <source>Left</source>
        <translation>Левый</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="203"/>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="212"/>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="221"/>
        <source>&lt;th&gt; %1 &lt;/th&gt;</source>
        <translation>&lt;th&gt; %1 &lt;/th&gt;</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="209"/>
        <source>Right</source>
        <translation>Правый</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="218"/>
        <source>Internal</source>
        <translation>Внутренний</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="227"/>
        <source>Pair stats</source>
        <translation>Статистика пар</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="229"/>
        <source>considered %1, unacceptable product size %2, high end compl %3, ok %4.</source>
        <translation>рассмотрено %1, неподходящий размер продукта %2, высокая комплементарность на концах %3, ok %4.</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="249"/>
        <source>Failed to find any exon annotations associated with the sequence %1.Make sure the provided sequence is cDNA and has exonic structure annotated</source>
        <translation>Не получается найти экзон, связанный с последовательностью %1. Убедитесь, что текущая последовательность cDNA и имеет экзонную структуру с аннотациями</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="261"/>
        <source>The first exon from the selected range [%1,%2] is larger the number of exons (%3). Please set correct exon range.</source>
        <translation>Первый экзон из выбранного диапазона [%1,%2] больше общего количества экзонов(%3). Пожалуйста, задайте правильный диапазон экзонов.</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="270"/>
        <source>The the selected exon range [%1,%2] is larger the number of exons (%3). Please set correct exon range.</source>
        <translation>Выбраный диапазон экзонов [%1,%2] больше общего количества экзонов(%3). Пожалуйста, задайте правильный диапазон экзонов.</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="317"/>
        <source>No primers has been found due to the parameters you&apos;ve set up</source>
        <translation>Не было найдено праймеров, соответствующих заданным параметрам</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="319"/>
        <source>All found primers has been filtered due to the &quot;Check complement&quot; parameters</source>
        <translation>Все найденные праймеры были отфильтрованы согласно параметру &quot;Проверить комплементарность&quot;</translation>
    </message>
    <message>
        <location filename="../src/task/Primer3TopLevelTask.cpp" line="331"/>
        <source>Object with annotations was removed</source>
        <translation>Объект с аннотациями удален</translation>
    </message>
</context>
<context>
    <name>U2::ProcessPrimer3ResultsToAnnotationsTask</name>
    <message>
        <location filename="../src/task/ProcessPrimer3ResultsToAnnotationsTask.cpp" line="49"/>
        <source>Search primers to annotations</source>
        <translation>Найденный праймеры в аннотации</translation>
    </message>
</context>
<context>
    <name>U2::QDPrimerActor</name>
    <message>
        <location filename="../src/Primer3Query.cpp" line="83"/>
        <source>%1 invalid input. Excluded regions.</source>
        <translation>%1 invalid input. Excluded regions.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="92"/>
        <source>%1 invalid input. Targets.</source>
        <translation>%1 invalid input. Targets.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Query.cpp" line="101"/>
        <source>%1 invalid input. Product size ranges.</source>
        <translation>%1 invalid input. Product size ranges.</translation>
    </message>
</context>
</TS>
