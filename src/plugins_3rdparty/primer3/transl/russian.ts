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
        <location filename="../src/Primer3Dialog.ui" line="282"/>
        <source>Targets</source>
        <translation>Целевые регионы</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Primer oligos may not overlap any region specified in this tag. The associated value must be a space-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;start,length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs where start is the index of the first base of the excluded region, and length is its length. This tag is useful for tasks such as excluding regions of low sequence quality or for excluding regions containing repetitive elements such as ALUs or LINEs.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Олиго праймера не могут перекрывать регионы, указанные в этом теге.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If one or more Targets is specified then a legal primer pair must flank at least one of them. A Target might be a simple sequence repeat site (for example a CA repeat) or a single-base-pair polymorphism. The value should be a space-separated list of &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;start,length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs where start is the index of the first base of a Target, and length is its length.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если указаны один или больше целевых регионов то пара праймеров должна обрамлять как минимум один из них.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="754"/>
        <location filename="../src/Primer3Dialog.ui" line="793"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A list of product size ranges, for example:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot;font-weight:600;&quot;&gt; 150-250 100-300 301-400 &lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 first tries to pick primers in the first range. If that is not possible, it goes to the next range and tries again. It continues in this way until it has either picked all necessary primers or until there are no more ranges. For technical reasons this option makes much lighter computational demands than the Product Size option.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Список диапазонов размера продукта, например:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot;font-weight:600;&quot;&gt; 150-250 100-300 301-400 &lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="800"/>
        <location filename="../src/Primer3Dialog.ui" line="813"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This selection indicates what mispriming library (if any) Primer3 should use to screen for interspersed repeats or for other sequence to avoid as a location for primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Этот параметр указывает какую библиотеку будет использовать Primer3.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="764"/>
        <location filename="../src/Primer3Dialog.ui" line="830"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum number of primer pairs to return.&lt;/p&gt;&lt;p&gt;Primer pairs returned are sorted by their &amp;quot;quality&amp;quot;, in other words by the value of the objective function (where a lower number indicates a better primer pair).&lt;/p&gt;&lt;p&gt;Caution: setting this parameter to a large value will increase running time.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимальное число пар праймеров.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="846"/>
        <location filename="../src/Primer3Dialog.ui" line="862"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum stability for the last five 3&apos; bases of a left or right primer.&lt;/p&gt;&lt;p&gt;Bigger numbers mean more stable 3&apos; ends. The value is the maximum delta G (kcal/mol) for duplex disruption for the five 3&apos; bases as calculated using the Nearest-Neighbor parameter values specified by the option of &apos;Table of thermodynamic parameters&apos;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимум стабильности для последних пяти 3&apos; оснований левого или правого праймеров. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="777"/>
        <location filename="../src/Primer3Dialog.ui" line="897"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed weighted similarity with any sequence in Mispriming Library.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимально допустимое взвешенное сходство с любой последовательностью из библиотеки ошибочного праймирования.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="913"/>
        <location filename="../src/Primer3Dialog.ui" line="929"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed sum of similarities of a primer pair (one similarity for each primer) with any single sequence in Mispriming Library. &lt;/p&gt;&lt;p&gt;Library sequence weights are not used in computing the sum of similarities.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимально допустимая сумма сходств пар праймеров с любой последовательностью из библиотеки ошибочного праймирования.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed similarity to ectopic sites in the sequence from which you are designing the primers.&lt;/p&gt;&lt;p&gt;The scoring system is the same as used for Max Mispriming, except that an ambiguity code is never treated as a consensus.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимально допустимое сходство со смещенными сайтами в последовательности из которой конструируются праймеры.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed summed similarity of both primers to ectopic sites in the sequence from which you are designing the primers.&lt;/p&gt;&lt;p&gt;The scoring system is the same as used for Max Mispriming, except that an ambiguity code is never treated as a consensus.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимально разрешенное суммарное сходство обоих праймеров со смещенными сайтами в последовательности из которой конструируются праймеры. &lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Index of the first base of a start codon.&lt;/p&gt;&lt;p&gt;This parameter allows Primer3 to select primer pairs to create in-frame amplicons e.g. to create a template for a fusion protein. Primer3 will attempt to select an in-frame left primer, ideally starting at or to the left of the start codon, or to the right if necessary. Negative values of this parameter are legal if the actual start codon is to the left of available sequence. If this parameter is non-negative Primer3 signals an error if the codon at the position specified by this parameter is not an ATG. A value less than or equal to -10^6 indicates that Primer3 should ignore this parameter. Primer3 selects the position of the right primer by scanning right from the left primer for a stop codon. Ideally the right primer will end at or after the stop codon.&lt;/p&gt;&lt;p&gt;This parameter should be considered EXPERIMENTAL. Please check the output carefully; some erroneous inputs might cause an error in Primer3.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Индекс первого основания стартового кодона.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="118"/>
        <source>Pick left primer</source>
        <translation>Выбрать левый праймер</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="166"/>
        <source>or use left primer below</source>
        <translation>или использовать этот левый праймер</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="978"/>
        <source>General Settings</source>
        <translation>Общие настройки</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="981"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section covers such settings as primer size, temperature, GC and other general settings.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&gt;В этом разделе рассматриваются такие параметры, как размер праймера, температура, GC и другие общие настройки.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1033"/>
        <location filename="../src/Primer3Dialog.ui" line="1075"/>
        <location filename="../src/Primer3Dialog.ui" line="1203"/>
        <location filename="../src/Primer3Dialog.ui" line="1240"/>
        <location filename="../src/Primer3Dialog.ui" line="1257"/>
        <source>Min</source>
        <translation>Минимум</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1333"/>
        <location filename="../src/Primer3Dialog.ui" line="1410"/>
        <location filename="../src/Primer3Dialog.ui" line="1544"/>
        <location filename="../src/Primer3Dialog.ui" line="1666"/>
        <location filename="../src/Primer3Dialog.ui" line="1673"/>
        <source>Opt</source>
        <translation>Оптимум</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1118"/>
        <location filename="../src/Primer3Dialog.ui" line="1164"/>
        <location filename="../src/Primer3Dialog.ui" line="1226"/>
        <location filename="../src/Primer3Dialog.ui" line="1233"/>
        <location filename="../src/Primer3Dialog.ui" line="1603"/>
        <source>Max</source>
        <translation>Максимум</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1147"/>
        <source>Primer Tm</source>
        <translation>Т. плав. праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1317"/>
        <location filename="../src/Primer3Dialog.ui" line="1362"/>
        <location filename="../src/Primer3Dialog.ui" line="1384"/>
        <location filename="../src/Primer3Dialog.ui" line="1400"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum lengths (in bases) of a primer oligo.&lt;/p&gt;&lt;p&gt;Primer3 will not pick primers shorter than Min or longer than Max, and with default arguments will attempt to pick primers close with size close to Opt.&lt;/p&gt;&lt;p&gt;Min cannot be smaller than 1. Max cannot be larger than 36. (This limit is governed by maximum oligo size for which melting-temperature calculations are valid.)&lt;/p&gt;&lt;p&gt;Min cannot be greater than Max.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальная, оптимальная и максимальная длины олиго праймера.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1020"/>
        <location filename="../src/Primer3Dialog.ui" line="1046"/>
        <location filename="../src/Primer3Dialog.ui" line="1131"/>
        <location filename="../src/Primer3Dialog.ui" line="1144"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum melting temperatures (Celsius) for a primer oligo.&lt;/p&gt;&lt;p&gt;Primer3 will not pick oligos with temperatures smaller than Min or larger than Max, and with default conditions will try to pick primers with melting temperatures close to Opt.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальная, оптимальная и максимальная температуры плавления олиго праймера.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1274"/>
        <location filename="../src/Primer3Dialog.ui" line="1652"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum percentage of Gs and Cs in any primer or oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;
</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальное, оптимальное и максимальное процентное содержание Gs и Cs в праймере или олиго.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;
</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1343"/>
        <location filename="../src/Primer3Dialog.ui" line="1531"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum percentage of Gs and Cs in any primer or oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальное, оптимальное и максимальное процентное содержание Gs и Cs в праймере или олиго.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1247"/>
        <location filename="../src/Primer3Dialog.ui" line="1455"/>
        <location filename="../src/Primer3Dialog.ui" line="1487"/>
        <location filename="../src/Primer3Dialog.ui" line="1509"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum, optimum, and maximum melting temperature of the amplicon.&lt;/p&gt;&lt;p&gt;Primer3 will not pick a product with melting temperature less than min or greater than max. If Opt is supplied and the Penalty Weights for Product Size are non-0 Primer3 will attempt to pick an amplicon with melting temperature close to Opt.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальная, оптимальная и максимальная температуры плавления ампликона.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="991"/>
        <location filename="../src/Primer3Dialog.ui" line="1177"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Maximum acceptable (unsigned) difference between the melting temperatures of the left and right primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимально допустимое различие между температурами плавления левого и правого праймеров.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1111"/>
        <source>Table of thermodynamic parameters</source>
        <translation>Таблица термодинамических параметров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4287"/>
        <source>Primers</source>
        <translation>Праймеры</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4943"/>
        <source>Primer Pairs</source>
        <translation>Пары праймеров</translation>
    </message>
    <message>
        <source>Hyb Oligos</source>
        <translation type="vanished">Гибридизация олиго</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6312"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section allows to set output annotations&apos; settings.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;В этой секции можно задать настройки для аннотаций.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1617"/>
        <source>Breslauer et. al 1986</source>
        <translation>Breslauer et. al 1986</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1622"/>
        <location filename="../src/Primer3Dialog.ui" line="2546"/>
        <source>SantaLucia 1998</source>
        <translation>SantaLucia 1998</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="20"/>
        <source>Primer Designer</source>
        <translation>Дизайнер праймеров</translation>
    </message>
    <message>
        <source>Excluded regions</source>
        <translation type="vanished">Исключенные регионы</translation>
    </message>
    <message>
        <source>Product size ranges</source>
        <translation type="vanished">Диапазоны размера продукта</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="803"/>
        <source>Mispriming/Repeat library</source>
        <translation>Библиотека повторов/ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="767"/>
        <source>Number to return</source>
        <translation>Результатов не более</translation>
    </message>
    <message>
        <source>Max repeat mispriming</source>
        <translation type="vanished">Максимальный повтор ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2103"/>
        <source>Max template mispriming</source>
        <translation>Максимальный шаблон ошибочного праймирования</translation>
    </message>
    <message>
        <source>Max 3&apos; stability</source>
        <translation type="vanished">Максимальная 3&apos; стабильность</translation>
    </message>
    <message>
        <source>Pair max repeat mispriming</source>
        <translation type="vanished">Парный максимальный повтор ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2180"/>
        <source>Pair max template mispriming</source>
        <translation>Парный максимальный шаблон ошибочного праймирования</translation>
    </message>
    <message>
        <source>Start codon position</source>
        <translation type="vanished">Позиция стартового кодона</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="134"/>
        <source>Pick hybridization probe (internal oligo)</source>
        <translation>Выбрать гибридизационную пробу (internal oligo)</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="176"/>
        <source>or use oligo below</source>
        <translation>или использовать эту oligo</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="153"/>
        <source>Pick right primer</source>
        <translation>Выбрать правый праймер</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="183"/>
        <source>or use right primer below (5&apos; to 3&apos; on opposite strand)</source>
        <translation>или использовать этот правый праймер (5&apos; в 3&apos; в противоположном направлении)</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1403"/>
        <source>Primer size</source>
        <translation>Размер праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1250"/>
        <location filename="../src/Primer3Dialog.ui" line="5010"/>
        <source>Product Tm</source>
        <translation>Т. плав. продукта</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1278"/>
        <source>Primer GC%</source>
        <translation>GC состав %</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="994"/>
        <source>Max Tm difference</source>
        <translation>Максимальная разница Т. плав</translation>
    </message>
    <message>
        <source>Max self complementarity</source>
        <translation type="vanished">Максимальная комплементарность</translation>
    </message>
    <message>
        <source>Max 3&apos; self complementarity</source>
        <translation type="vanished">Максимальная 3&apos; комплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2849"/>
        <source>Inside target penalty</source>
        <translation>Внутренний целевой штраф</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2404"/>
        <source>Outside target penalty</source>
        <translation>Внешний целевой штраф</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2491"/>
        <source>Max poly-X</source>
        <translation>Максимум poly-X</translation>
    </message>
    <message>
        <source>First base index</source>
        <translation type="vanished">Индекс первого основания</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2872"/>
        <source>CG clamp</source>
        <translation>Зажим CG</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2534"/>
        <location filename="../src/Primer3Dialog.ui" line="2745"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Specifies the salt correction formula for the melting temperature calculation.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Задает коррекционную формулу для расчета температуры плавления.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>Annealing oligo concentration</source>
        <translation type="vanished">Концентрация отжига олиго</translation>
    </message>
    <message>
        <source> Liberal base</source>
        <translation type="vanished"> Разрешить неоднозначные символы</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3096"/>
        <source>Show debugging info</source>
        <translation>Показывать отладочную информацию</translation>
    </message>
    <message>
        <source>Hyb oligo excluded region</source>
        <translation type="vanished">Исключаемые регионы гибридизации олиго</translation>
    </message>
    <message>
        <source>Hyb oligo size</source>
        <translation type="vanished">Размер гибридизации олиго</translation>
    </message>
    <message>
        <source>Hyb oligo Tm</source>
        <translation type="vanished">Tm гибридизации олиго</translation>
    </message>
    <message>
        <source>Hyb oligo GC%</source>
        <translation type="vanished">GC% гибридизации олиго</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3289"/>
        <location filename="../src/Primer3Dialog.ui" line="3431"/>
        <location filename="../src/Primer3Dialog.ui" line="3496"/>
        <location filename="../src/Primer3Dialog.ui" line="3534"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer Size for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр размера праймера для внутреннего олиго.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3325"/>
        <location filename="../src/Primer3Dialog.ui" line="3373"/>
        <location filename="../src/Primer3Dialog.ui" line="3421"/>
        <location filename="../src/Primer3Dialog.ui" line="3567"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer Tm for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Tm олиго.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3357"/>
        <location filename="../src/Primer3Dialog.ui" line="3461"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer GC% for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;GC% олиго.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3344"/>
        <location filename="../src/Primer3Dialog.ui" line="3518"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer GC% for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;GC% олиго.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>Hyb oligo self complementarity</source>
        <translation type="vanished">Комплементарность гибридизации олиго</translation>
    </message>
    <message>
        <source>Hyb oligo mishyb library</source>
        <translation type="vanished">Библиотека гибридизации олиго</translation>
    </message>
    <message>
        <source>Hyb oligo min sequence quality</source>
        <translation type="vanished">Минимальное качество последовательности гибридизации олиго</translation>
    </message>
    <message>
        <source>Hyb oligo conc of monovalent cations</source>
        <translation type="vanished">Концентрация одновалентных катионов гибридизации олиго</translation>
    </message>
    <message>
        <source>Hyb oligo conc of divalent cations</source>
        <translation type="vanished">Концентрация двухвалентных катионов гибридизации олиго</translation>
    </message>
    <message>
        <source>Hyb oligo max 3&apos; self complementarity</source>
        <translation type="vanished">Максимальная 3&apos; комплементарность гибридизации олиго</translation>
    </message>
    <message>
        <source>Hyb oligo max poly-X</source>
        <translation type="vanished">Максимум poly-X гибридизации олиго</translation>
    </message>
    <message>
        <source>Hyb oligo max mishyb</source>
        <translation type="vanished">Максимум ошибки гибридизации олиго</translation>
    </message>
    <message>
        <source>Hyb oligo [dNTP]</source>
        <translation type="vanished">Гибридизация олиго [dNTP]</translation>
    </message>
    <message>
        <source>Hyb oligo DNA concentration</source>
        <translation type="vanished">ДНК концентрация гибридизации олиго</translation>
    </message>
    <message>
        <source>Self complementarity</source>
        <translation type="vanished">Комплементарность</translation>
    </message>
    <message>
        <source>3&apos; self complementarity</source>
        <translation type="vanished">3&apos; комплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6069"/>
        <source>RT-PCR</source>
        <translation>RT-PCR</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6081"/>
        <source>Design primers for RT-PCR analysis</source>
        <translation>Дизайн праймеров для анализа RT-PCR</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6101"/>
        <source>Exon annotation name:</source>
        <translation>Имя экзон аннотации:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6111"/>
        <source>exon</source>
        <translation>exon</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6136"/>
        <source>Minimum exon junction overlap size</source>
        <translation>Минимальный размер перекрытия соединения экзона</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6200"/>
        <source>Exon range:</source>
        <translation>Диапазон экзона:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6247"/>
        <source>Max number of pairs to query:</source>
        <translation>Максимальное число пар для запроса:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5965"/>
        <source>Min sequence quality:</source>
        <translation>Минимальное качество последовательности:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5991"/>
        <source>Min 3&apos; sequence quality:</source>
        <translation>Минимальное 3&apos;качество последовательности:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6017"/>
        <source>Sequence quality range min:</source>
        <translation>Минимум интервала качества последовательности:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6043"/>
        <source>Sequence quality range max:</source>
        <translation>Максимум интервала качества последовательности:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6360"/>
        <source>Help</source>
        <translation>Помощь</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6380"/>
        <source>Save settings</source>
        <translation>Сохранить настройки</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6387"/>
        <source>Load settings</source>
        <translation>Загрузить настройки</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6394"/>
        <source>Reset form</source>
        <translation>Сброс параметров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6401"/>
        <source>Pick primers</source>
        <translation>Подобрать</translation>
    </message>
    <message>
        <source>Concentration of monovalent cations</source>
        <translation type="vanished">Концентрация одновалентных катионов</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1108"/>
        <location filename="../src/Primer3Dialog.ui" line="1613"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Specifies details of melting temperature calculation.&lt;/p&gt;&lt;p&gt;First method uses the table of thermodynamic parameters from the paper [Breslauer KJ, et. al (1986), Proc Natl Acad Sci 83:4746-50 http://dx.doi.org/10.1073/pnas.83.11.3746]&lt;/p&gt;&lt;p&gt;Second method (recommended) was suggested in the paper [SantaLucia JR (1998), Proc Natl Acad Sci 95:1460-65 http://dx.doi.org/10.1073/pnas.95.4.1460].&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Указывает детали расчета температуры плавления.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>Max #N&apos;s</source>
        <translation type="vanished">Максимум N</translation>
    </message>
    <message>
        <source>Concentration of divalent cations</source>
        <translation type="vanished">Концентрация двухвалентных катионов</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2748"/>
        <source>Salt correction formula</source>
        <translation>Коррекционная формула</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2537"/>
        <location filename="../src/Primer3Dialog.ui" line="2541"/>
        <source>Schildkraut and Lifson 1965</source>
        <translation>Schildkraut and Lifson 1965</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2551"/>
        <source>Owczarzy et. 2004</source>
        <translation>Owczarzy et. 2004</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2882"/>
        <source>Concentration of dNTPs</source>
        <translation>Концентрация dNTP</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowable 3&apos;-anchored global alignment score when testing a single primer for self-complementarity, and the maximum allowable 3&apos;-anchored global alignment score when testing for complementarity between left and right primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимальная 3&amp;apos;-комплементарность.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowable local alignment score when testing a single primer for (local) self-complementarity and the maximum allowable local alignment score when testing for complementarity between left and right primers.&lt;/p&gt;&lt;p&gt;Local self-complementarity is taken to predict the tendency of primers to anneal to each other without necessarily causing self-priming in the PCR. The scoring system gives 1.00 for complementary bases, -0.25 for a match of any base (or N) with an N, -1.00 for a mismatch, and -2.00 for a gap. Only single-base-pair gaps are allowed.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимальная комплементарность.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2778"/>
        <location filename="../src/Primer3Dialog.ui" line="2895"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Maximum number of unknown bases (N) allowable in any primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимальное число неизвестных оснований (N), допустимых в праймере.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2488"/>
        <location filename="../src/Primer3Dialog.ui" line="2810"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowable length of a mononucleotide repeat, for example AAAAAA.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимальная допустимая для мононуклеотидного повтора, например AAAAAA.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2401"/>
        <location filename="../src/Primer3Dialog.ui" line="2703"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Non-default values valid only for sequences with 0 or 1 target regions.&lt;/p&gt;&lt;p&gt;If the primer is part of a pair that spans a target and does not overlap the target, then multiply this value times the number of nucleotide positions from the 3&apos; end to the (unique) target to get the &apos;position penalty&apos;. The effect of this parameter is to allow Primer3 to include nearness to the target as a term in the objective function.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Внешний целевой штраф.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2609"/>
        <location filename="../src/Primer3Dialog.ui" line="2732"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This parameter is the index of the first base in the input sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Индекс первого основания во входной последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2869"/>
        <location filename="../src/Primer3Dialog.ui" line="2914"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Require the specified number of consecutive Gs and Cs at the 3&apos; end of both the left and right primer.&lt;/p&gt;&lt;p&gt;This parameter has no effect on the internal oligo if one is requested.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Зажим CG.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2303"/>
        <location filename="../src/Primer3Dialog.ui" line="2985"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The millimolar (mM) concentration of monovalent salt cations (usually KCl) in the PCR.&lt;/p&gt;&lt;p&gt;Primer3 uses this argument to calculate oligo and primer melting temperatures.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Концентрация одновалентных катионов.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2378"/>
        <location filename="../src/Primer3Dialog.ui" line="2565"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The millimolar concentration of divalent salt cations (usually MgCl^(2+)) in the PCR.&lt;/p&gt;&lt;p&gt;Primer3 converts concentration of divalent cations to concentration of monovalent cations using following formula:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;[Monovalent cations] = [Monovalent cations] + 120*(([divalent cations] - [dNTP])^0.5)&lt;/span&gt;&lt;/p&gt;&lt;p&gt;In addition, if the specified concentration of dNTPs is larger than the concentration of divalent cations then the effect of the divalent cations is not considered.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Концентрация двухвалентных катионов.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2590"/>
        <location filename="../src/Primer3Dialog.ui" line="2879"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The millimolar concentration of the sum of all deoxyribonucleotide triphosphates.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Концентрация dNTP.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2342"/>
        <location filename="../src/Primer3Dialog.ui" line="3001"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A value to use as nanomolar (nM) concentration of each annealing oligo over the course the PCR.&lt;/p&gt;&lt;p&gt;Primer3 uses this argument to esimate oligo melting temperatures.&lt;/p&gt;&lt;p&gt;The default (50nM) works well with the standard protocol used at the Whitehead/MIT Center for Genome Research --0.5 microliters of 20 micromolar concentration for each primer in a 20 microliter reaction with 10 nanograms template, 0.025 units/microliter Taq polymerase in 0.1 mM each dNTP, 1.5mM MgCl2, 50mM KCl, 10mM Tris-HCL (pH 9.3) using 35 cycles with an annealing temperature of 56 degrees Celsius.&lt;/p&gt;&lt;p&gt;The value of this parameter is less than the actual concentration of oligos in the initial reaction mix because  it is the concentration of annealing oligos, which in turn depends on the amount of template (including PCR product) in a given cycle. This concentration increases a great deal during a PCR; fortunately PCR seems quite robust for a variety of oligo melting temperatures.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Концентрация отжига олиго.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Non-default values valid only for sequences with 0 or 1 target regions.&lt;/p&gt;&lt;p&gt;If the primer is part of a pair that spans a target and overlaps the target, then multiply this value times the number of nucleotide positions by which the primer overlaps the (unique) target to get the &apos;position penalty&apos;.&lt;/p&gt;&lt;p&gt;The effect of this parameter is to allow Primer3 to include overlap with the target as a term in the objective function.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Внешний целевой штраф.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>Do not treat ambiguity codes in libraries as consensus</source>
        <translation type="vanished">Не относить неоднозначности кода в библиотеке к консенсусу</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3179"/>
        <source>Lowercase masking</source>
        <translation>Строчная маскировка</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3217"/>
        <source>Internal Oligo</source>
        <translation>Внутреннее олиго</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3220"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Parameters governing choice of internal oligos are analogous to the parameters governing choice of primer pairs.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Параметры, регулирующие выбор внутренних олиго аналогичны параметрам, регулирующих выбор пар праймеров.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Middle oligos may not overlap any region specified by this tag. The associated value must be a space-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;&amp;lt;start&amp;gt;,&amp;lt;length&amp;gt;&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs, where &amp;lt;start&amp;gt; is the index of the first base of an excluded region, and &amp;lt;length&amp;gt; is its length. Often one would make Target regions excluded regions for internal oligos.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Исключаемые олиго регионы.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3386"/>
        <location filename="../src/Primer3Dialog.ui" line="3441"/>
        <location filename="../src/Primer3Dialog.ui" line="3448"/>
        <location filename="../src/Primer3Dialog.ui" line="3547"/>
        <source>Min:</source>
        <translation>Мин:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3305"/>
        <location filename="../src/Primer3Dialog.ui" line="3393"/>
        <location filename="../src/Primer3Dialog.ui" line="3414"/>
        <location filename="../src/Primer3Dialog.ui" line="3612"/>
        <source>Opt:</source>
        <translation>Опт:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3312"/>
        <location filename="../src/Primer3Dialog.ui" line="3400"/>
        <location filename="../src/Primer3Dialog.ui" line="3407"/>
        <location filename="../src/Primer3Dialog.ui" line="3554"/>
        <source>Max:</source>
        <translation>Макс:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3902"/>
        <source>Max #Ns</source>
        <translation>Макс N</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer max self complemntarity for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Комплементарность олиго.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer max #Ns for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимум N символов.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Similar to primer mispriming library, except that the event we seek to avoid is hybridization of the internal oligo to sequences in this library rather than priming from them.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Библиотека олиго.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer minimum quality for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальное качество последовательности олиго.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer concentration of monovalent cations for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Концентрация одновалентных катионов олиго.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer concentration of divalent cations for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Концентрация двухвалентных катионов олиго.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer max 3&apos; self complementarity for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимальная 3&apos; комплементарность олиго.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer max poly-X for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимум poly-X олиго.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Similar to primer max library mispriming.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимум ошибки олиго.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer concentration of the dNTPs for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer concentration of the dNTPs for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer DNA concentration for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer DNA concentration for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4269"/>
        <source>Penalty Weights</source>
        <translation>Веса штрафов</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4441"/>
        <source>Tm</source>
        <translation>Tm</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4353"/>
        <source>Size  </source>
        <translation>Размер  </translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4299"/>
        <source>GC%</source>
        <translation>GC%</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4306"/>
        <location filename="../src/Primer3Dialog.ui" line="4343"/>
        <location filename="../src/Primer3Dialog.ui" line="4360"/>
        <location filename="../src/Primer3Dialog.ui" line="4494"/>
        <location filename="../src/Primer3Dialog.ui" line="4952"/>
        <location filename="../src/Primer3Dialog.ui" line="5057"/>
        <location filename="../src/Primer3Dialog.ui" line="5432"/>
        <location filename="../src/Primer3Dialog.ui" line="5439"/>
        <location filename="../src/Primer3Dialog.ui" line="5517"/>
        <location filename="../src/Primer3Dialog.ui" line="5600"/>
        <source>Lt:</source>
        <translation>Lt:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4329"/>
        <location filename="../src/Primer3Dialog.ui" line="4336"/>
        <location filename="../src/Primer3Dialog.ui" line="4415"/>
        <location filename="../src/Primer3Dialog.ui" line="4501"/>
        <location filename="../src/Primer3Dialog.ui" line="5033"/>
        <location filename="../src/Primer3Dialog.ui" line="5050"/>
        <location filename="../src/Primer3Dialog.ui" line="5494"/>
        <location filename="../src/Primer3Dialog.ui" line="5524"/>
        <location filename="../src/Primer3Dialog.ui" line="5583"/>
        <location filename="../src/Primer3Dialog.ui" line="5607"/>
        <source>Gt:</source>
        <translation>Gt:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4718"/>
        <source>#N&apos;s</source>
        <translation>N</translation>
    </message>
    <message>
        <source>Mispriming</source>
        <translation type="vanished">Ошибочное праймирование</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4770"/>
        <location filename="../src/Primer3Dialog.ui" line="5946"/>
        <source>Sequence quality</source>
        <translation>Качество последовательности</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4799"/>
        <source>End sequence quality</source>
        <translation>Качество конца последовательности</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4828"/>
        <source>Position penalty</source>
        <translation>Позиция штрафа</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4857"/>
        <source>End stability</source>
        <translation>Стабильность конца</translation>
    </message>
    <message>
        <source>Template mispriming</source>
        <translation type="vanished">Шаблон ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5043"/>
        <source>Product size</source>
        <translation>Размер продукта</translation>
    </message>
    <message>
        <source>Tm difference</source>
        <translation type="vanished">Максимальная разница Tm</translation>
    </message>
    <message>
        <source>Any complementarity</source>
        <translation type="vanished">Любая комплементарность</translation>
    </message>
    <message>
        <source>3&apos; complementarity</source>
        <translation type="vanished">3&apos; комплементарность</translation>
    </message>
    <message>
        <source>Pair mispriming</source>
        <translation type="vanished">Ошибочное парное праймирование</translation>
    </message>
    <message>
        <source>Primer penalty weight</source>
        <translation type="vanished">Вес штрафа праймера</translation>
    </message>
    <message>
        <source>Hyb oligo penalty weight</source>
        <translation type="vanished">Вес штрафа гибридизации олиго</translation>
    </message>
    <message>
        <source>Primer pair template mispriming weight</source>
        <translation type="vanished">Вес шаблона ошибочного праймирования</translation>
    </message>
    <message>
        <source>Hyb oligo #N&apos;s</source>
        <translation type="vanished">N гибридизации олиго</translation>
    </message>
    <message>
        <source>Hyb oligo mishybing</source>
        <translation type="vanished">Ошибочная гибридизация олиго</translation>
    </message>
    <message>
        <source>Hyb oligo sequence quality</source>
        <translation type="vanished">Качество последовательности гибридизации олиго</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6093"/>
        <source>mRNA sequence</source>
        <translation>Последовательность mRNA</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6148"/>
        <source>At 5&apos; side (bp):</source>
        <translation>Со стороны 5&apos; (нк):</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6178"/>
        <source>At 3&apos;side (bp)</source>
        <translation>Со стороны 3&apos; (нк)</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6235"/>
        <source>Primer product must span at least one intron on the corresponding genomic DNA</source>
        <translation>Продук праймера должен охватывать по крайней мере один интрон на соответствующей геномной ДНК</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5937"/>
        <source>Sequence Quality</source>
        <translation>Качество последовательности</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="47"/>
        <source>Select the Task for primer selection</source>
        <translation>Выберите Задачу подбора праймеров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="44"/>
        <location filename="../src/Primer3Dialog.ui" line="54"/>
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
        <location filename="../src/Primer3Dialog.ui" line="216"/>
        <location filename="../src/Primer3Dialog.ui" line="250"/>
        <source>5&apos; Overhang:</source>
        <translation>5&apos; Перекрытие:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="213"/>
        <location filename="../src/Primer3Dialog.ui" line="223"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The provided sequence is added to the 5&apos; end of the left primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Предоставленная последовательность добавляется к 5&apos;-концу левого праймера.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="247"/>
        <location filename="../src/Primer3Dialog.ui" line="257"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The provided sequence is added to the 5&apos; end of the right primer. It is reverse complementary to the template sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Предоставленная последовательность добавляется к 5&apos;-концу правого праймера. Она обратно комплементарна последовательности шаблона.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="292"/>
        <source>Overlap Junction List</source>
        <translation>Список перекрывающихся соединений</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="302"/>
        <source>Excluded Regions</source>
        <translation>Исключаемые регионы</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="312"/>
        <source>Pair OK Region List</source>
        <translation>Список ОК парных регионов</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="322"/>
        <source>Included region</source>
        <translation>Включенные регион</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="334"/>
        <source>Start Codon Position</source>
        <translation>Позиция стартового кодона</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="346"/>
        <source>Start Codon Sequence</source>
        <translation>Последовательность стартового кодона</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="356"/>
        <source>Force Left Primer Start</source>
        <translation>Принудительное начало левого праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="366"/>
        <source>Force Left Primer End</source>
        <translation>Принудительный конец левого праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="376"/>
        <source>Five Matches on Primer&apos;s 5&apos;</source>
        <translation>Пять совпадений на 5&apos; конце</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="386"/>
        <source>Five Matches on Internal Oligo&apos;s  5&apos;</source>
        <translation>Пять совпадений на 5&apos; конце Внутреннего олиго</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="279"/>
        <location filename="../src/Primer3Dialog.ui" line="397"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If one or more Targets is specified then a legal primer pair must flank at least one of them. A Target might be a simple sequence repeat site (for example a CA repeat) or a single-base-pair polymorphism. The value should be a space-separated list of &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs where &lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt; is the index of the first base of a Target, and length is its &lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если указан один или несколько Целевых регионов, то пара праймеров должна обрамлять как минимум один из них. Целевой регион может быть простым сайтом повтора последовательности (например, повтором CA) или полиморфизмом с одной парой оснований. Значение должно представлять собой разделенный пробелами список &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;начало&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt;&lt;/p&gt;&lt;p&gt;пар, где &lt;span style=&quot; font-weight:700;&quot;&gt;начало&lt;/span&gt; является порядковым номером первого нуклеотида Целевого региона, а его длина - это &lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="289"/>
        <location filename="../src/Primer3Dialog.ui" line="404"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If this list is not empty, then the forward OR the reverse primer must overlap one of these junction points by at least &lt;span style=&quot; font-weight:700;&quot;&gt;3 Prime Junction Overlap&lt;/span&gt; nucleotides at the 3&apos; end and at least &lt;span style=&quot; font-weight:700;&quot;&gt;5 Prime Junction Overlap&lt;/span&gt; nucleotides at the 5&apos; end.&lt;/p&gt;&lt;p&gt;In more detail: The junction associated with a given position is the space immediately to the right of that position in the template sequence on the strand given as input.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Если этот список не пуст, то прямой ИЛИ обратный праймер должен перекрывать одну из этих точек из Списка Перекрывающихся Соединений как минимум на &lt;span style=&quot; font-weight:700;&quot;&gt;Перекрытие Первичного Соединения 3&lt;/span&gt; на 3&apos;-конце и не менее &lt;span style=&quot; font-weight:700;&quot;&gt;Перекрытие Первичного Соединения 5&lt;/span&gt; на 5&apos;-конце.&lt;/p&gt;&lt;p&gt;Более подробно: соединение, связанное с данной позицией, представляет собой пространство непосредственно справа от этой позиции в шаблонной последовательности на цепи, заданной в качестве входных данных.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="309"/>
        <location filename="../src/Primer3Dialog.ui" line="424"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This tag allows detailed specification of possible locations of left and right primers in primer pairs.&lt;/p&gt;&lt;p&gt;The associated value must be a semicolon-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;left_start&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;left_length&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;right_start&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;right_length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;quadruples. The left primer must be in the region specified by &lt;span style=&quot; font-weight:700;&quot;&gt;left_start&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;left_length&lt;/span&gt; and the right primer must be in the region specified by &lt;span style=&quot; font-weight:700;&quot;&gt;right_start&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;right_length&lt;/span&gt;. &lt;span style=&quot; font-weight:700;&quot;&gt;left_start&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;left_length&lt;/span&gt; specify the location of the left primer in terms of the index of the first base in the region and the length of the region. &lt;span style=&quot; font-weight:700;&quot;&gt;right_start&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;right_length&lt;/span&gt; specify the location of the right primer in analogous fashion. As seen in the example below, if no integers are specified for a region then the location of the corresponding primer is not constrained.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Этот тег позволяет подробно указать возможное расположение левого и правого праймеров в парах праймеров.&lt;/p&gt;&lt;p&gt;Это значение должно быть списком разделенных точкой с запятой четверок&lt;/ p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;левый_старт&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;левая_длина&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;правый_старт&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;правая,длина&lt;/span&gt;&lt;/p&gt;&lt;p&gt;. Левый праймер должен находиться в области &lt;span style=&quot; font-weight:700;&quot;&gt;левый_старт&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;левая_длина&lt;/span&gt;, а правый праймер должен находиться в области &lt;span style=&quot; font-weight:700;&quot;&gt;правый_старт&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt;. &lt;span style=&quot; font-weight:700;&quot;&gt;левый_старт&lt;/span&gt; и &lt;span style=&quot; font-weight:700;&quot;&gt;левая_длина&lt;/span&gt; укажите расположение левого праймера с точки зрения индекса первого символа в регионе и длины региона. &lt;span style=&quot; font-weight:700;&quot;&gt;правый_старт&lt;/span&gt; и &lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt; аналогичным образом задают расположение правого праймера.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="299"/>
        <location filename="../src/Primer3Dialog.ui" line="431"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Primer oligos may not overlap any region specified in this tag. The associated value must be a space-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs where start is the index of the first base of the excluded region, and length is its length. This tag is useful for tasks such as excluding regions of low sequence quality or for excluding regions containing repetitive elements such as ALUs or LINEs.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Олигонуклеотиды праймеров не могут перекрывать какую-либо область, указанную в этом параметре. Значение должно быть списком разделенных пробелом&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;начало&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt;&lt;/p&gt;&lt;p&gt;пар, где начало — это индекс первого символа исключаемой области, а длина — ее длина.
Этот тег полезен для таких задач, как исключение областей с низким качеством последовательности или для исключения областей, содержащих повторяющиеся элементы, такие как ALU или LINE.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="319"/>
        <location filename="../src/Primer3Dialog.ui" line="477"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A sub-region of the given sequence in which to pick primers. For example, often the first dozen or so bases of a sequence are vector, and should be excluded from consideration. The value for this parameter has the form&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;where &lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt; is the index of the first base to consider, and &lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt; is the number of subsequent bases in the primer-picking region.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Подобласть данной последовательности, в которой нужно выбрать праймеры. Например, часто первая дюжина или около того символов последовательности является вектором и должна быть исключена из рассмотрения. Значение этого параметра имеет вид &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;начало&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt;&lt;/p&gt;&lt;p&gt;где &lt;span style=&quot; font-weight:700;&quot;&gt;начало&lt;/span&gt; это индекс первого символа для рассмотрения, а &lt;span style=&quot; font-weight:700;&quot;&gt;длина&lt;/span&gt; - количество последующих символов в области выбора праймера.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="602"/>
        <source>Force Right Primer End</source>
        <translation>Принудительный конец правого праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="363"/>
        <location filename="../src/Primer3Dialog.ui" line="583"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Forces the 3&apos; end of the left primer to be at the indicated position. Primers are also picked if they violate certain constraints. The default value indicates that the end of the left primer can be anywhere.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Заставляет 3&apos;-конец левого праймера находиться в указанной позиции. Праймеры также выбираются, если они нарушают определенные ограничения. Значение по умолчанию указывает, что конец левого праймера может быть где угодно.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="353"/>
        <location filename="../src/Primer3Dialog.ui" line="650"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Forces the 5&apos; end of the left primer to be at the indicated position. Primers are also picked if they violate certain constraints. The default value indicates that the start of the left primer can be anywhere.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Заставляет 5&apos;-конец левого праймера находиться в указанной позиции. Праймеры также выбираются, если они нарушают определенные ограничения. Значение по умолчанию указывает, что конец левого праймера может быть где угодно.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="383"/>
        <location filename="../src/Primer3Dialog.ui" line="732"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Five Matches on Primer&apos;s 5&apos;&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр &lt;span style=&quot; font-weight:700;&quot;&gt;Пять совпадений на 5&apos; конце&lt;/span&gt; для внутреннего олиго.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="725"/>
        <source>Five Matches on Primer&apos;s 3&apos;</source>
        <translation>Пять совпадений на 3&apos; конце</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="570"/>
        <location filename="../src/Primer3Dialog.ui" line="708"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Five Matches on Primer&apos;s 3&apos;&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Five Matches on Primer&apos;s 3&apos;&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="711"/>
        <source>Five Matches on Internal Oligo&apos;s 3&apos;</source>
        <translation>Пять совпадений на 3&apos; конце Внутренного олиго</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="329"/>
        <location filename="../src/Primer3Dialog.ui" line="539"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This parameter should be considered EXPERIMENTAL at this point. Please check the output carefully; some erroneous inputs might cause an error in Primer3.&lt;br&gt;&lt;br&gt;
Index of the first base of a start codon. This parameter allows Primer3 to select primer pairs to create in-frame amplicons e.g. to create a template for a fusion protein. Primer3 will attempt to select an in-frame left primer, ideally starting at or to the left of the start codon, or to the right if necessary. Negative values of this parameter are legal if the actual start codon is to the left of available sequence. If this parameter is non-negative Primer3 signals an error if the codon at the position specified by this parameter is not an ATG. A value less than or equal to -10^6 indicates that Primer3 should ignore this parameter.&lt;br&gt;&lt;br&gt;
Primer3 selects the position of the right primer by scanning right from the left primer for a stop codon. Ideally the right primer will end at or after the stop codon.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;На данный момент этот параметр следует считать ЭКСПЕРИМЕНТАЛЬНЫМ. Пожалуйста, внимательно проверьте выходные данные; некоторые ошибочные входные данные могут вызвать ошибку в Primer3.&lt;br&gt;&lt;br&gt;
Индекс первого символа стартового кодона. Этот параметр позволяет Primer3 выбирать пары праймеров для создания внутрирамочных ампликонов, например, для создания шаблона для слитого белка. Primer3 попытается выбрать левый праймер в рамке, в идеале начиная с начального кодона или слева от него, или справа, если необходимо. Отрицательные значения этого параметра допустимы, если фактический стартовый кодон находится слева от доступной последовательности. Если этот параметр неотрицательный, Primer3 сигнализирует об ошибке, если кодон в позиции, указанной этим параметром, не является ATG. Значение, меньшее или равное -10^6, указывает на то, что Primer3 должен игнорировать этот параметр.&lt;br&gt;&lt;br&gt;
Primer3 выбирает положение правого праймера, сканируя справа от левого праймера стоп-кодон. В идеале правильный праймер должен заканчиваться на стоп-кодоне или после него.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="628"/>
        <location filename="../src/Primer3Dialog.ui" line="698"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Forces the 5&apos; end of the right primer to be at the indicated position. Primers are also picked if they violate certain constraints. The default value indicates that the start of the right primer can be anywhere.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Заставляет 5&apos;-конец правого праймера находиться в указанной позиции. Праймеры также выбираются, если они нарушают определенные ограничения. Значение по умолчанию указывает, что конец левого праймера может быть где угодно.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="718"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Discards all primers which do not match this match sequence at the 3&apos; end. Similar parameter to &lt;span style=&quot; font-weight:700;&quot;&gt;Five Matches on Primer&apos;s 5&apos;&lt;/span&gt;, but limits the 3&apos; end. (New in v. 2.3.6, added by A. Untergasser.)&lt;br/&gt;&lt;br/&gt;The match sequence must be 5 nucletides long and can contain the following letters:&lt;/p&gt;&lt;p&gt;N Any nucleotide&lt;br/&gt;A Adenine&lt;br/&gt;G Guanine&lt;br/&gt;C Cytosine&lt;br/&gt;T Thymine&lt;br/&gt;R Purine (A or G)&lt;br/&gt;Y Pyrimidine (C or T)&lt;br/&gt;W Weak (A or T)&lt;br/&gt;S Strong (G or C)&lt;br/&gt;M Amino (A or C)&lt;br/&gt;K Keto (G or T)&lt;br/&gt;B Not A (G or C or T)&lt;br/&gt;H Not G (A or C or T)&lt;br/&gt;D Not C (A or G or T)&lt;br/&gt;V Not T (A or G or C)&lt;/p&gt;&lt;p&gt;Any primer which will not match the entire match sequence at the 5&apos; end will be discarded and not evaluated. Setting strict requirements here will result in low quality primers due to the high numbers of primers discarded at this step.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Отбрасывает все праймеры, которые не соответствуют этой последовательности соответствия на 3&apos;-конце. Аналогичный параметр для &lt;span style=&quot; font-weight:700;&quot;&gt;Пять совпадений на 5&apos; конце&lt;/span&gt;, но ограничивает 3&apos;-конец. (Новый с v. 2.3.6, автор A. Untergasser.)&lt;br/&gt;&lt;br/&gt;Последовательность должна состоять из 5 нуклеотидов и может содержать следующие буквы:&lt;/p&gt;&lt;p&gt;N Любой нуклеотид&lt;br/&gt;A Аденин&lt;br/&gt;G Гуанин&lt;br/&gt;C Цитозин&lt;br/&gt;T Тимин&lt;br/&gt;R Пурин (A или G)&lt;br/&gt;Y Пиримидин (C или T)&lt;br/&gt;W Слабый (A или T)&lt;br/&gt;S Сильный (G или C)&lt;br/&gt;M Амино (A или C)&lt;br/&gt;K Кето (G или T)&lt;br/&gt;B не A (G или C или T)&lt;br/&gt;H не G (A или C или T)&lt;br/&gt;D не C (A или G или T)&lt;br/&gt;V не T (A или G или C)&lt;/p&gt;&lt;p&gt;Любой праймер, который не будет соответствовать всей последовательности соответствия на 5&apos;-конце, будет отброшен и не будет оцениваться. Установление здесь строгих требований приведет к получению праймеров низкого качества из-за большого количества праймеров, отбрасываемых на этом этапе.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="701"/>
        <source>Force Right Primer Start</source>
        <translation>Принудительный конец правого праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="507"/>
        <location filename="../src/Primer3Dialog.ui" line="599"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Forces the 3&apos; end of the right primer to be at the indicated position. Primers are also picked if they violate certain constraints. The default value indicates that the end of the right primer can be anywhere.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Заставляет 3&apos;-конец правого праймера находиться в указанной позиции. Праймеры также выбираются, если они нарушают определенные ограничения. Значение по умолчанию указывает, что конец левого праймера может быть где угодно.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="341"/>
        <location filename="../src/Primer3Dialog.ui" line="679"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The sequence of the start codon, by default ATG. Some bacteria use different start codons, this tag allows to specify alternative start codons.

Any triplet can be provided as start codon.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Последовательность стартового кодона, по умолчанию ATG. Некоторые бактерии используют разные стартовые кодоны, этот тег позволяет указать альтернативные стартовые кодоны.

Любой триплет может быть использован в качестве стартового кодона.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="684"/>
        <source>ATG</source>
        <translation></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="757"/>
        <source>Product Size Ranges</source>
        <translation>Диапазоны размеров продукта</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="780"/>
        <source>Max Library Mispriming</source>
        <translation>Максимальное несоответствие бибилотеке праймеров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="849"/>
        <source>Max 3&apos; Stability</source>
        <translation>Максимальная стабильность на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="916"/>
        <source>Pair Max Library Mispriming</source>
        <translation>Максимальное несоответствие библиотеке праймеров (пары)</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1735"/>
        <location filename="../src/Primer3Dialog.ui" line="1822"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This is the most stable monomer structure of internal oligo calculated by thermodynamic approach. The hairpin loops, bulge loops, internal loops, internal single mismatches, dangling ends, terminal mismatches have been considered. This parameter is calculated only if &lt;span style=&quot; font-weight:700;&quot;&gt;Use Thermodynamic Oligo Alignment&lt;/span&gt; is checked. The default value is 10 degrees lower than the default value of &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Tm Min&lt;/span&gt;. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Это наиболее стабильная мономерная структура внутреннего олиго, рассчитанная термодинамическим подходом. Были рассмотрены петли-шпильки, петли-выпуклости, внутренние петли, внутренние одиночные несовпадения, свисающие концы, концевые несовпадения. Этот параметр рассчитывается только в том случае, если &lt;span style=&quot; font-weight:700;&quot;&gt;Использовать Термодинамическое Олигонуклеотидное Выравнивание&lt;/span&gt; отмечено. Значение по умолчанию на 10 градусов ниже, чем значение по умолчанию &lt;span style=&quot; font-weight:700;&quot;&gt;Миннимамльная Т. плав. праймера&lt;/span&gt;. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1095"/>
        <location filename="../src/Primer3Dialog.ui" line="1264"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The annealing temperature (Celsius) used in the PCR reaction. Usually it is chosen up to 10°C below the melting temperature of the primers. If provided, Primer3 will calculate the fraction of primers bound at the provided annealing temperature for each oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Температура отжига (по Цельсию), используемая в реакции ПЦР. Обычно ее выбирают на 10°С ниже температуры плавления праймеров. Если задано, Primer3 вычислит долю праймеров, связанных при указанной температуре отжига для каждого олигонуклеотида.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1157"/>
        <source>Primer Bound</source>
        <translation>Связь праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1154"/>
        <location filename="../src/Primer3Dialog.ui" line="1210"/>
        <location filename="../src/Primer3Dialog.ui" line="1417"/>
        <location filename="../src/Primer3Dialog.ui" line="1630"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum acceptable fraction of primer bound at &lt;span style=&quot; font-weight:700;&quot;&gt;Annealing Temperature&lt;/span&gt; for a primer oligo in percent.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальная, оптимальная и максимальная допустимая доля Связи праймера при &lt;span style=&quot; font-weight:700;&quot;&gt;Температуре отжига&lt;/span&gt; для олигонуклеотида в процентах.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1267"/>
        <source>Annealing Temperature</source>
        <translation>Температура Отжига</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1691"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Use thermodynamic models to calculate the the propensity of oligos to form hairpins and dimers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Используйте термодинамические модели для расчета склонности олигонуклеотидов к образованию шпилек и димеров.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1694"/>
        <source>Use Thermodynamic Oligo Alignment</source>
        <translation>Использовать Термодинамическое Олигонуклеотидное Выравнивание</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1706"/>
        <location filename="../src/Primer3Dialog.ui" line="1845"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Same as&lt;span style=&quot; font-weight:700;&quot;&gt; Max 3&apos; Self Complementarity&lt;/span&gt; but is based on thermodynamical approach - the stability of structure is analyzed.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;То же, что и &lt;span style=&quot; font-weight:700;&quot;&gt; Максимальная Самокомплементарность на 3&apos;&lt;/span&gt;, но основанная на термодинамическом подходе - анализируется стабильность структуры.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1748"/>
        <location filename="../src/Primer3Dialog.ui" line="2004"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Same as &lt;span style=&quot; font-weight:700;&quot;&gt;Max 3&apos; Pair Complementarity&lt;/span&gt; but for calculating the score (melting temperature of structure) thermodynamical approach is used.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;То же, что и &lt;span style=&quot; font-weight:700;&quot;&gt;Максимальная Парная Комплементарность на 3&apos;&lt;/span&gt; но для расчета значения (температуры плавления структуры) используется термодинамический подход&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1767"/>
        <source>Max 3&apos; Pair Complementarity</source>
        <translation>Максимальная Парная Комплементарность на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1774"/>
        <location filename="../src/Primer3Dialog.ui" line="2014"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Describes the tendency of a primer to bind to itself (interfering with target sequence binding). It will score ANY binding occurring within the entire primer sequence.&lt;/p&gt;&lt;p&gt;It is the maximum allowable local alignment score when testing a single primer for (local) self-complementarity. Local self-complementarity is taken to predict the tendency of primers to anneal to each other without necessarily causing self-priming in the PCR. The scoring system gives 1.00 for complementary bases, -0.25 for a match of any base (or N) with an N, -1.00 for a mismatch, and -2.00 for a gap. Only single-base-pair gaps are allowed.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Описывает тенденцию праймера связываться с самим собой (препятствуя связыванию целевой последовательности). Будет оцениваться ЛЮБОЕ связывание, происходящее во всей последовательности праймера.
&lt;/p&gt;&lt;p&gt;Это максимально допустимая оценка локального выравнивания при тестировании одного праймера на (локальную) самокомплементарность. Локальная самокомплементарность используется для предсказания тенденции праймеров отжигаться друг с другом, не обязательно вызывая самозаполнение в ПЦР. Система подсчета очков дает 1,00 за комплементарные символы, -0,25 за совпадение любого символа (или N) с N, -1,00 за несоответствие и -2,00 за гэп. Разрешаются только одинарные пары гэпов.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1793"/>
        <source>TH: Max Self Complementarity</source>
        <translation>TH: Максимальная Самокомплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1790"/>
        <location filename="../src/Primer3Dialog.ui" line="1806"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The same as &lt;span style=&quot; font-weight:700;&quot;&gt;Max Self Complementarity&lt;/span&gt; but all calculations are based on thermodynamical approach.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;То же, что и &lt;span style=&quot; font-weight:700;&quot;&gt;Максимальная Самокомплементарность&lt;/span&gt; но все расчеты основаны на термодинамическом подходе.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1825"/>
        <source>TH: Max Primer Hairpin	</source>
        <translation>TH: Максимальная шпилечная структура	</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1848"/>
        <source>TH: Max 3&apos; Self Complementarity</source>
        <translation>TH: Максимальная Самокомплементарность на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1871"/>
        <source>Max Pair Complementarity</source>
        <translation>Максимальная Парная Комплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1891"/>
        <location filename="../src/Primer3Dialog.ui" line="1920"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Describes the tendency of the left primer to bind to the right primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Описывает тенденцию левого праймера связываться с правым праймером.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1923"/>
        <source>TH: Max Pair Complementarity</source>
        <translation>TH: Максимальная Парная Комплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1868"/>
        <location filename="../src/Primer3Dialog.ui" line="1975"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Describes the tendency of the left primer to bind to the right primer. It is the maximum allowable local alignment score when testing for complementarity between left and right primers. It is similar to &lt;span style=&quot; font-weight:700;&quot;&gt;Max Self Complementarity&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Описывает тенденцию левого праймера связываться с правым праймером. Это максимально допустимая оценка локального выравнивания при тестировании на комплементарность между левым и правым праймерами. Это похоже на &lt;span style=&quot; font-weight:700;&quot;&gt;Максимальную Самокомплементарность&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2024"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Tries to bind the 3&apos;-END to a identical primer and scores the best binding it can find. This is critical for primer quality because it allows primers use itself as a target and amplify a short piece (forming a primer-dimer). These primers are then unable to bind and amplify the target sequence.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;Max 3&apos; Self Complementarity&lt;/span&gt; is the maximum allowable 3&apos;-anchored global alignment score when testing a single primer for self-complementarity. The 3&apos;-anchored global alignment score is taken to predict the likelihood of PCR-priming primer-dimers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Пытается связать 3&apos;-КОНЕЦ с идентичным праймером и оценивает лучшее связывание, которое может найти. Это имеет решающее значение для качества праймеров, поскольку позволяет праймерам использовать себя в качестве цели и амплифицировать короткий фрагмент (образуя праймер-димер). Затем эти праймеры не могут связываться и амплифицировать целевую последовательность.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;Максимальная Самокомплементарность на 3&apos;&lt;/span&gt; это максимально допустимая оценка глобального выравнивания с 3&apos;-привязкой при тестировании одного праймера на самодополняемость. Оценка глобального выравнивания с 3&apos;-привязкой используется для прогнозирования вероятности ПЦР-праймирования димеров праймеров.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2007"/>
        <source>TH: Max 3&apos; Pair Complementarity</source>
        <translation>TH: Максимальная Парная Комплементарность на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2017"/>
        <source>Max Self Complementarity</source>
        <translation>Максимальная Самокомплементарность</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2027"/>
        <source>Max 3&apos; Self Complementarity</source>
        <translation>Максимальная Самокомплементарность на 3&apos;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2047"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Use thermodynamic models to calculate the the propensity of oligos to anneal to undesired sites in the template sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Используйте термодинамические модели для расчета склонности олигонуклеотидов к отжигу с нежелательными сайтами в шаблонной  последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2050"/>
        <source>Use Thermodynamic Template Alignment	</source>
        <translation>Использовать Термодинамическое Шаблонное Выранивание	</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2068"/>
        <source>TH: Max Template Mispriming</source>
        <translation>TH: Максимальный шаблон ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2065"/>
        <location filename="../src/Primer3Dialog.ui" line="2084"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Similar to &lt;span style=&quot; font-weight:700;&quot;&gt;Max template mispriming&lt;/span&gt; but assesses alternative binding sites in the template using thermodynamic models. This parameter specifies the maximum allowed melting temperature of an oligo (primer) at an &amp;quot;ectopic&amp;quot; site within the template sequence; 47.0 would be a reasonable choice if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Tm Min&lt;/span&gt; is 57.0.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2100"/>
        <location filename="../src/Primer3Dialog.ui" line="2116"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed similarity to ectopic sites in the template. A negative value means do not check.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2151"/>
        <source>TH: Pair Max Template Mispriming</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2148"/>
        <location filename="../src/Primer3Dialog.ui" line="2161"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed summed melting temperatures of both primers at ectopic sites within the template (with the two primers in an orientation that would allow PCR amplification.) The melting temperatures are calculated as for &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max Template Mispriming&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2177"/>
        <location filename="../src/Primer3Dialog.ui" line="2193"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed summed similarity of both primers to ectopic sites in the template. A negative value means do not check.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2242"/>
        <source>Advanced Settings</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2245"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section covers more accurate settings, which are related to &lt;span style=&quot; font-weight:700;&quot;&gt;General Settings&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2255"/>
        <location filename="../src/Primer3Dialog.ui" line="2498"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The 5&apos; end of the left OR the right primer must overlap one of the junctions in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt; by this amount. See details in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2274"/>
        <source>Sequencing Interval</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2281"/>
        <location filename="../src/Primer3Dialog.ui" line="2508"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Defines the space from the calculated position of the 3&apos;end to both sides in which Primer3Plus picks the best primer. Value only used if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Task&lt;/span&gt; is &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2322"/>
        <source>Max GC in primer 3&apos; end</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2345"/>
        <source>Annealing Oligo Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2381"/>
        <source>Concentration of Divalent Cations</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2319"/>
        <location filename="../src/Primer3Dialog.ui" line="2411"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum number of Gs or Cs allowed in the last five 3&apos; bases of a left or right primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2427"/>
        <location filename="../src/Primer3Dialog.ui" line="2768"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Analogous to &lt;span style=&quot; font-weight:700;&quot;&gt;3&apos; End Distance Between Left Primers&lt;/span&gt;, but for right primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2443"/>
        <location filename="../src/Primer3Dialog.ui" line="2632"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Defines the space from the 3&apos;end of the primer to the point were the trace signals are readable. Value only used if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Task&lt;/span&gt; is &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2459"/>
        <location filename="../src/Primer3Dialog.ui" line="2716"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The 3&apos; end of the left OR the right primer must overlap one of the junctions in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt; by this amount. See details in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2501"/>
        <source>5 Prime Junction Overlap</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2511"/>
        <source>Sequencing Accuracy</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2612"/>
        <source>First Base Index</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2635"/>
        <source>Sequencing Lead</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2642"/>
        <location filename="../src/Primer3Dialog.ui" line="2826"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;When returning multiple primer pairs, the minimum number of base pairs between the 3&apos; ends of any two left primers.&lt;/p&gt;&lt;p&gt;Primers with 3&apos; ends at positions e.g. 30 and 31 in the template sequence have a three-prime distance of 1.&lt;/p&gt;&lt;p&gt;In addition to positive values, the values -1 and 0 are acceptable and have special interpretations:&lt;/p&gt;&lt;p&gt;-1 indicates that a given left primer can appear in multiple primer pairs returned by Primer3. This is the default behavior.&lt;/p&gt;&lt;p&gt;0 indicates that a left primer is acceptable if it was not already used. In other words, two left primers are allowed to have the same 3&apos; position provided their 5&apos; positions differ.&lt;/p&gt;&lt;p&gt;For n &amp;gt; 0: A left primer is acceptable if:&lt;/p&gt;&lt;p&gt;NOT(3&apos; end of left primer closer than n to the 3&apos; end of a previously used left primer)&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2661"/>
        <source>Sequencing Spacing</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2271"/>
        <location filename="../src/Primer3Dialog.ui" line="2668"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Defines the space from the 3&apos;end of the primer to the 3&apos;end of the next primer on the reverse strand. Value only used if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Task&lt;/span&gt; is &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2719"/>
        <source>3 Prime Junction Overlap</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2771"/>
        <source>3&apos; End Distance Between Right Primers	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2781"/>
        <source>Max #N&apos;s accepted</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2788"/>
        <location filename="../src/Primer3Dialog.ui" line="2846"/>
        <source>Non-default values are valid only for sequences with 0 or 1 target regions. If the primer is part of a pair that spans a target and overlaps the target, then multiply this value times the number of nucleotide positions by which the primer overlaps the (unique) target to get the &apos;position penalty&apos;. The effect of this parameter is to allow Primer3 to include overlap with the target as a term in the objective function.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2829"/>
        <source>3&apos; End Distance Between Left Primers</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2839"/>
        <source>DMSO Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2658"/>
        <location filename="../src/Primer3Dialog.ui" line="2969"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Defines the space from the 3&apos;end of the primer to the 3&apos;end of the next primer on the same strand. Value only used if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Task&lt;/span&gt; is &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2988"/>
        <source>Concentration of Monovalent Cations</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3020"/>
        <source>Formamide Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3030"/>
        <source>DMSO Factor</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2836"/>
        <location filename="../src/Primer3Dialog.ui" line="3037"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The concentration of DMSO in percent. See PRIMER_DMSO_FACTOR for details of Tm correction.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3027"/>
        <location filename="../src/Primer3Dialog.ui" line="3050"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The melting temperature of primers can be approximately corrected for DMSO:&lt;/p&gt;&lt;p&gt;Tm = Tm (without DMSO) - &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Factor&lt;/span&gt; * &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Concentration&lt;/span&gt;&lt;/p&gt;&lt;p&gt;The &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Concentration&lt;/span&gt; concentration must be given in %.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3017"/>
        <location filename="../src/Primer3Dialog.ui" line="3069"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The concentration of formamide in mol/l. The melting temperature of primers can be approximately corrected for formamide:&lt;/p&gt;&lt;p&gt;Tm = Tm (without formamide) +(0.453 * PRIMER_[LEFT/INTERNAL/RIGHT]_4_GC_PERCENT / 100 - 2.88) * &lt;span style=&quot; font-weight:700;&quot;&gt;Formamide Concentration&lt;/span&gt;&lt;/p&gt;&lt;p&gt;The &lt;span style=&quot; font-weight:700;&quot;&gt;Formamide Concentration&lt;/span&gt; correction was suggested by Blake and Delcourt (R D Blake and S G Delcourt. Nucleic Acids Research, 24, 11:2095–2103, 1996).&lt;/p&gt;&lt;p&gt;Convert % into mol/l:&lt;/p&gt;&lt;p&gt;[DMSO in mol/l] = [DMSO in % weight] * 10 / 45.04 g/mol&lt;/p&gt;&lt;p&gt;[DMSO in mol/l] = [DMSO in % volume] * 10 * 1.13 g/cm3 / 45.04 g/mol&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3109"/>
        <source>Use formatted output</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3119"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If this flag is true, produce PRIMER_LEFT_EXPLAIN, PRIMER_RIGHT_EXPLAIN, PRIMER_INTERNAL_EXPLAIN and/or PRIMER_PAIR_EXPLAIN output tags as appropriate.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3122"/>
        <source>Print Statistics</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3132"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the associated value = 1, then Primer3 will print out the calculated secondary structures&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3135"/>
        <source>Print secondary structures</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3153"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If set to 1, treat ambiguity codes as if they were consensus codes when matching oligos to mispriming or mishyb libraries. For example, if this flag is set, then a C in an oligo will be scored as a perfect match to an S in a library sequence, as will a G in the oligo. More importantly, though, any base in an oligo will be scored as a perfect match to an N in the library. This is very bad if the library contains strings of Ns, as no oligo will be legal (and it will take a long time to find this out). So unless you know for sure that your library does not have runs of Ns (or Xs), then set this flag to 0.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3156"/>
        <source>Treat ambiguity codes in libraries as consensus</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3166"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If true use primer provided in left, right, or internal primer even if it violates specific constraints.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3169"/>
        <source>Pick anyway</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3176"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This option allows for intelligent design of primers in sequence in which masked regions (for example repeat-masked regions) are lower-cased. (New in v. 1.1.0, added by Maido Remm and Triinu Koressaar)&lt;/p&gt;&lt;p&gt;A value of 1 directs Primer3 to reject primers overlapping lowercase a base exactly at the 3&apos; end.&lt;/p&gt;&lt;p&gt;This property relies on the assumption that masked features (e.g. repeats) can partly overlap primer, but they cannot overlap the 3&apos;-end of the primer. In other words, lowercase bases at other positions in the primer are accepted, assuming that the masked features do not influence the primer performance if they do not overlap the 3&apos;-end of primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3186"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This parameter provides a quick-and-dirty way to get Primer3 to accept IUB / IUPAC codes for ambiguous bases (i.e. by changing all unrecognized bases to N). If you wish to include an ambiguous base in an oligo, you must set &lt;span style=&quot; font-weight:700;&quot;&gt;Max #N&apos;s accepted&lt;/span&gt; to a 1 (non-0) value.&lt;/p&gt;&lt;p&gt;Perhaps &apos;-&apos; and &apos;* &apos; should be squeezed out rather than changed to &apos;N&apos;, but currently they simply get converted to N&apos;s. The authors invite user comments.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3189"/>
        <source>Liberal base</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3231"/>
        <source>Internal Oligo Excluded Region</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3228"/>
        <location filename="../src/Primer3Dialog.ui" line="3238"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Middle oligos may not overlap any region specified by this tag. The associated value must be a space-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs, where &lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt; is the index of the first base of an excluded region, and &lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt; is its length. Often one would make Target regions excluded regions for internal oligos.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3248"/>
        <source>Internal Oligo Overlap Positions</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3245"/>
        <location filename="../src/Primer3Dialog.ui" line="3255"/>
        <location filename="../src/Primer3Dialog.ui" line="4017"/>
        <location filename="../src/Primer3Dialog.ui" line="4091"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The 5&apos; end of the middle oligo / probe must overlap one of the junctions in &lt;span style=&quot; font-weight:700;&quot;&gt;Internal Oligo Overlap Positions&lt;/span&gt; by this amount. See details in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3276"/>
        <source>Internal Oligo Bound</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3360"/>
        <source>Internal Oligo GC%</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3424"/>
        <source>Internal Oligo Tm</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3434"/>
        <source>Internal Oligo Size</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3273"/>
        <location filename="../src/Primer3Dialog.ui" line="3474"/>
        <location filename="../src/Primer3Dialog.ui" line="3580"/>
        <location filename="../src/Primer3Dialog.ui" line="3596"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer Bound for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="373"/>
        <location filename="../src/Primer3Dialog.ui" line="691"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Discards all primers which do not match this match sequence at the 5&apos; end. (New in v. 2.3.6, added by A. Untergasser.)&lt;/p&gt;&lt;p&gt;The match sequence must be 5 nucletides long and can contain the following letters:&lt;/p&gt;&lt;p&gt;N Any nucleotide&lt;br/&gt;A Adenine&lt;br/&gt;G Guanine&lt;br/&gt;C Cytosine&lt;br/&gt;T Thymine&lt;br/&gt;R Purine (A or G)&lt;br/&gt;Y Pyrimidine (C or T)&lt;br/&gt;W Weak (A or T)&lt;br/&gt;S Strong (G or C)&lt;br/&gt;M Amino (A or C)&lt;br/&gt;K Keto (G or T)&lt;br/&gt;B Not A (G or C or T)&lt;br/&gt;H Not G (A or C or T)&lt;br/&gt;D Not C (A or G or T)&lt;br/&gt;V Not T (A or G or C)&lt;/p&gt;&lt;p&gt;Any primer which will not match the entire match sequence at the 5&apos; end will be discarded and not evaluated. Setting strict requirements here will result in low quality primers due to the high numbers of primers discarded at this step.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1764"/>
        <location filename="../src/Primer3Dialog.ui" line="1930"/>
        <location filename="../src/Primer3Dialog.ui" line="1959"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Tries to bind the 3&apos;-END of the left primer to the right primer and scores the best binding it can find. It is similar to &lt;span style=&quot; font-weight:700;&quot;&gt;Max 3&apos; Self Complementarity&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3636"/>
        <location filename="../src/Primer3Dialog.ui" line="3751"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max Self Complementarity&lt;/span&gt; for the internal oligo&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3658"/>
        <location filename="../src/Primer3Dialog.ui" line="3687"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Same as &lt;span style=&quot; font-weight:700;&quot;&gt;Internal Oligo Max 3&apos; End Self Complementarity&lt;/span&gt; but for calculating the score (melting temperature of structure) thermodynamical approach is used&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3690"/>
        <source>TH: Internal Oligo Max 3&apos; End Self Complementarity	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3703"/>
        <location filename="../src/Primer3Dialog.ui" line="3741"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer &lt;span style=&quot; font-weight:700;&quot;&gt;Max Self Complementarity&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3725"/>
        <location filename="../src/Primer3Dialog.ui" line="3806"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer &lt;span style=&quot; font-weight:700;&quot;&gt;Max 3&apos; Self Complementarity&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3744"/>
        <location filename="../src/Primer3Dialog.ui" line="5735"/>
        <source>Internal Oligo Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3754"/>
        <location filename="../src/Primer3Dialog.ui" line="5648"/>
        <source>TH: Internal Oligo Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3780"/>
        <location filename="../src/Primer3Dialog.ui" line="3796"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The most stable monomer structure of internal oligo calculated by thermodynamic approach. See &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max Primer Hairpin&lt;/span&gt;	 for details&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3799"/>
        <location filename="../src/Primer3Dialog.ui" line="5706"/>
        <source>TH: Internal Oligo Hairpin</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3809"/>
        <source>Internal Oligo Max 3&apos; End Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3830"/>
        <source>Internal Oligo Max Poly-X</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3840"/>
        <source>Internal Oligo DNA Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3850"/>
        <source>Internal Oligo Formamide Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3863"/>
        <location filename="../src/Primer3Dialog.ui" line="4062"/>
        <location filename="../src/Primer3Dialog.ui" line="4107"/>
        <location filename="../src/Primer3Dialog.ui" line="4127"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Similar to &lt;span style=&quot; font-weight:700;&quot;&gt;Max Library Mispriming&lt;/span&gt; except that this parameter applies to the similarity of candidate internal oligos to the library specified in &lt;span style=&quot; font-weight:700;&quot;&gt;Internal Oligo Mishyb Library&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3873"/>
        <source>Internal Oligo Min Sequence Quality</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3886"/>
        <location filename="../src/Primer3Dialog.ui" line="3909"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Parameter for internal oligos analogous to &lt;span style=&quot; font-weight:700;&quot;&gt;Concentration of dNTPs&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3912"/>
        <source>Internal Oligo [dNTP]</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3837"/>
        <location filename="../src/Primer3Dialog.ui" line="3925"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Annealing Oligo Concentration&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3870"/>
        <location filename="../src/Primer3Dialog.ui" line="3947"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Min sequence quality&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3966"/>
        <location filename="../src/Primer3Dialog.ui" line="4027"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Concentration of Monovalent Cations&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3985"/>
        <source>Internal Oligo DMSO Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3998"/>
        <location filename="../src/Primer3Dialog.ui" line="4137"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Concentration of Divalent Cations&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4020"/>
        <source>Internal Oligo 5 Prime Junction Overlap</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4030"/>
        <source>Internal Oligo Conc of monovalent cations</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3899"/>
        <location filename="../src/Primer3Dialog.ui" line="4043"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer &lt;span style=&quot; font-weight:700;&quot;&gt;Max #Ns&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3847"/>
        <location filename="../src/Primer3Dialog.ui" line="4078"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Formamide Concentration&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4110"/>
        <source>Internal Oligo Mishyb Library</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4120"/>
        <source>Internal Oligo 3 Prime Junction Overlap</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4130"/>
        <source>Internal Oligo Max Library Mishyb	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4140"/>
        <source>Internal Oligo conc of divalent cations</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3982"/>
        <location filename="../src/Primer3Dialog.ui" line="4147"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Concentration&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3827"/>
        <location filename="../src/Primer3Dialog.ui" line="4166"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer &lt;span style=&quot; font-weight:700;&quot;&gt;Max poly-X&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4182"/>
        <location filename="../src/Primer3Dialog.ui" line="4201"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Factor&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4204"/>
        <source>Internal Oligo DMSO Factor</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4117"/>
        <location filename="../src/Primer3Dialog.ui" line="4211"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The 3&apos; end of the middle oligo / probe must overlap one of the junctions in &lt;span style=&quot; font-weight:700;&quot;&gt;Internal Oligo Overlap Positions&lt;/span&gt; by this amount. See details in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4227"/>
        <location filename="../src/Primer3Dialog.ui" line="4243"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Analogous to &lt;span style=&quot; font-weight:700;&quot;&gt;3&apos; End Distance Between Left Primers&lt;/span&gt;, but for internal primer / probe.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4246"/>
        <source>Min Internal Oligo End Distance</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4272"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section describes &amp;quot;penalty weights&amp;quot;, which allow the user to modify the criteria that Primer3 uses to select the &amp;quot;best&amp;quot; primers.&lt;/p&gt;&lt;p&gt;There are two classes of weights: for some parameters there is a &apos;Lt&apos; (less than) and a &apos;Gt&apos; (greater than) weight. These are the weights that Primer3 uses when the value is less or greater than (respectively) the specified optimum.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section describes &amp;quot;penalty weights&amp;quot;, which allow the user to modify the criteria that Primer3 uses to select the &amp;quot;best&amp;quot; primers.&lt;/p&gt;&lt;p&gt;There are two classes of weights: for some parameters there is a &apos;Lt&apos; (less than) and a &apos;Gt&apos; (greater than) weight. These are the weights that Primer3 uses when the value is less or greater than (respectively) the specified optimum.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4296"/>
        <location filename="../src/Primer3Dialog.ui" line="4319"/>
        <location filename="../src/Primer3Dialog.ui" line="4350"/>
        <location filename="../src/Primer3Dialog.ui" line="4373"/>
        <location filename="../src/Primer3Dialog.ui" line="4389"/>
        <location filename="../src/Primer3Dialog.ui" line="4405"/>
        <location filename="../src/Primer3Dialog.ui" line="4428"/>
        <location filename="../src/Primer3Dialog.ui" line="4438"/>
        <location filename="../src/Primer3Dialog.ui" line="4454"/>
        <location filename="../src/Primer3Dialog.ui" line="4464"/>
        <location filename="../src/Primer3Dialog.ui" line="4474"/>
        <location filename="../src/Primer3Dialog.ui" line="4484"/>
        <location filename="../src/Primer3Dialog.ui" line="5590"/>
        <location filename="../src/Primer3Dialog.ui" line="5614"/>
        <location filename="../src/Primer3Dialog.ui" line="5624"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for corressponding parameter of a primer less than (Lt) or greater than (Gt) the optimal value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для соответствующего параметра праймера меньше чем (Lt) или больше чем (Gt), что является оптимальным значением.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4487"/>
        <source>Bound</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4522"/>
        <source>TH: Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4519"/>
        <location filename="../src/Primer3Dialog.ui" line="4529"/>
        <location filename="../src/Primer3Dialog.ui" line="5645"/>
        <location filename="../src/Primer3Dialog.ui" line="5661"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Self Complementarity&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4545"/>
        <source>TH: 3&apos; End Self Complementarity	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4542"/>
        <location filename="../src/Primer3Dialog.ui" line="4552"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for the individual primer self binding value as in &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max 3&apos; Self Complementarity&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4568"/>
        <source>TH: Hairpin</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4565"/>
        <location filename="../src/Primer3Dialog.ui" line="4575"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for the individual primer hairpin structure value as in &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max Primer Hairpin&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4594"/>
        <source>TH: Template Mispriming</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4591"/>
        <location filename="../src/Primer3Dialog.ui" line="4604"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty for a single primer binding to the template sequence (thermodynamic approach, when &lt;span style=&quot; font-weight:700;&quot;&gt;Use Thermodynamic Template Alignment&lt;/span&gt; is true).&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4620"/>
        <source>Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4617"/>
        <location filename="../src/Primer3Dialog.ui" line="4633"/>
        <location filename="../src/Primer3Dialog.ui" line="4646"/>
        <location filename="../src/Primer3Dialog.ui" line="4662"/>
        <location filename="../src/Primer3Dialog.ui" line="4675"/>
        <location filename="../src/Primer3Dialog.ui" line="4691"/>
        <location filename="../src/Primer3Dialog.ui" line="4715"/>
        <location filename="../src/Primer3Dialog.ui" line="4731"/>
        <location filename="../src/Primer3Dialog.ui" line="4744"/>
        <location filename="../src/Primer3Dialog.ui" line="4754"/>
        <location filename="../src/Primer3Dialog.ui" line="4767"/>
        <location filename="../src/Primer3Dialog.ui" line="4783"/>
        <location filename="../src/Primer3Dialog.ui" line="4796"/>
        <location filename="../src/Primer3Dialog.ui" line="4812"/>
        <location filename="../src/Primer3Dialog.ui" line="4854"/>
        <location filename="../src/Primer3Dialog.ui" line="4870"/>
        <location filename="../src/Primer3Dialog.ui" line="4883"/>
        <location filename="../src/Primer3Dialog.ui" line="4899"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for a primer parameter different from predefined optimum value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для параметра праймера отличается от оптимального предопределенного значения.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4649"/>
        <source>3&apos; End Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4678"/>
        <source>Template Mispriming</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4747"/>
        <location filename="../src/Primer3Dialog.ui" line="5298"/>
        <source>Library Mispriming</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4825"/>
        <location filename="../src/Primer3Dialog.ui" line="4841"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Determines the overall weight of the position penalty in calculating the penalty for a primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Позиция штрафа.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4886"/>
        <source>Primer failure rate</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4965"/>
        <location filename="../src/Primer3Dialog.ui" line="4981"/>
        <location filename="../src/Primer3Dialog.ui" line="4997"/>
        <location filename="../src/Primer3Dialog.ui" line="5007"/>
        <location filename="../src/Primer3Dialog.ui" line="5023"/>
        <location filename="../src/Primer3Dialog.ui" line="5040"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for corressponding parameter of a primer pair less than (Lt) or greater than (Gt) the optimal value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для соответствующего параметра пары праймеров меньше чем (Lt) или больше чем (Gt), что является оптимальным значением.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5078"/>
        <source>TH: Any Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5075"/>
        <location filename="../src/Primer3Dialog.ui" line="5091"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for the binding value of the primer pair as in &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max Self Complementarity&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5107"/>
        <source>TH: 3&apos; End Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5104"/>
        <location filename="../src/Primer3Dialog.ui" line="5120"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for the binding value of the primer pair as in &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max 3&apos; Self Complementarity&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5139"/>
        <source>TH: Template Mispriming Weight</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5136"/>
        <location filename="../src/Primer3Dialog.ui" line="5155"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty for a primer pair binding to the template sequence (thermodynamic approach, when &lt;span style=&quot; font-weight:700;&quot;&gt;Use Thermodynamic Template Alignment&lt;/span&gt; is true).&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5171"/>
        <source>Any Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5168"/>
        <location filename="../src/Primer3Dialog.ui" line="5184"/>
        <location filename="../src/Primer3Dialog.ui" line="5197"/>
        <location filename="../src/Primer3Dialog.ui" line="5213"/>
        <location filename="../src/Primer3Dialog.ui" line="5226"/>
        <location filename="../src/Primer3Dialog.ui" line="5242"/>
        <location filename="../src/Primer3Dialog.ui" line="5266"/>
        <location filename="../src/Primer3Dialog.ui" line="5282"/>
        <location filename="../src/Primer3Dialog.ui" line="5295"/>
        <location filename="../src/Primer3Dialog.ui" line="5311"/>
        <location filename="../src/Primer3Dialog.ui" line="5324"/>
        <location filename="../src/Primer3Dialog.ui" line="5340"/>
        <location filename="../src/Primer3Dialog.ui" line="5353"/>
        <location filename="../src/Primer3Dialog.ui" line="5369"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for a primer pair parameter different from predefined optimum value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для параметра пары праймеров отличается от оптимального предопределенного значения.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5200"/>
        <source>3&apos; End Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5229"/>
        <source>Template Mispriming Weight</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5269"/>
        <source>Tm Difference</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5327"/>
        <source>Primer Penalty Weight</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5356"/>
        <source>Internal Oligo Penalty Weight	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5413"/>
        <source>Internal Oligos</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5425"/>
        <source>Internal oligo Size</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5422"/>
        <location filename="../src/Primer3Dialog.ui" line="5452"/>
        <location filename="../src/Primer3Dialog.ui" line="5468"/>
        <location filename="../src/Primer3Dialog.ui" line="5484"/>
        <location filename="../src/Primer3Dialog.ui" line="5507"/>
        <location filename="../src/Primer3Dialog.ui" line="5537"/>
        <location filename="../src/Primer3Dialog.ui" line="5547"/>
        <location filename="../src/Primer3Dialog.ui" line="5563"/>
        <location filename="../src/Primer3Dialog.ui" line="5573"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for corressponding parameter of a internal oligo less than (Lt) or greater than (Gt) the optimal value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для соответствующего параметра внутреннего олиго меньше чем (Lt) или больше чем (Gt), что является оптимальным значением.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5550"/>
        <source>Internal oligo Tm</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5576"/>
        <source>Internal oligo GC%</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5593"/>
        <source>Internal oligo Bound</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5677"/>
        <source>TH: Internal Oligo 3&apos; End Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5674"/>
        <location filename="../src/Primer3Dialog.ui" line="5690"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;TH: 3&apos; End Self Complementarity&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5703"/>
        <location filename="../src/Primer3Dialog.ui" line="5719"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for the most stable primer hairpin structure value as in &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Internal Oligo Hairpin&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5732"/>
        <location filename="../src/Primer3Dialog.ui" line="5748"/>
        <location filename="../src/Primer3Dialog.ui" line="5761"/>
        <location filename="../src/Primer3Dialog.ui" line="5777"/>
        <location filename="../src/Primer3Dialog.ui" line="5801"/>
        <location filename="../src/Primer3Dialog.ui" line="5817"/>
        <location filename="../src/Primer3Dialog.ui" line="5830"/>
        <location filename="../src/Primer3Dialog.ui" line="5846"/>
        <location filename="../src/Primer3Dialog.ui" line="5859"/>
        <location filename="../src/Primer3Dialog.ui" line="5875"/>
        <location filename="../src/Primer3Dialog.ui" line="5888"/>
        <location filename="../src/Primer3Dialog.ui" line="5904"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for an internal oligo parameter different from predefined optimum value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для параметра внутреннего олиго отличается от оптимального предопределенного значения.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5764"/>
        <source>Internal Oligo 3&apos; End Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5804"/>
        <source>Internal Oligo #N&apos;s</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5833"/>
        <source>Internal Oligo Library Mishybing</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5862"/>
        <source>Internal Oligo Sequence Quality</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5891"/>
        <source>Internal Oligo Sequence End Quality</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6072"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;In this section user can specify if primer product must overlap exon-exon junction or span intron. This only applies when designing primers for a cDNA (mRNA) sequence with annotated exons.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;In this section user can specify if primer product must overlap exon-exon junction or span intron. This only applies when designing primers for a cDNA (mRNA) sequence with annotated exons.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6078"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This feature allows to specify if primer product must span intron-exon boundaries.&lt;/p&gt;&lt;p&gt;Checking this option will result in ignoring &lt;span style=&quot; font-style:italic;&quot;&gt;Excluded&lt;/span&gt; and &lt;span style=&quot; font-style:italic;&quot;&gt;Target Regions&lt;/span&gt; from Main section.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эта опция позволяет указать если праймер должен охватывать интрон-экзон границы.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6108"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The name of the annotation which defines the exons in the mRNA sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Имя экзон аннотации.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6213"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the range is set, primer search will be restricted to selected exons. For example:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;1-5&lt;/span&gt;&lt;/p&gt;&lt;p&gt;If the range is larger than actual exon range or the starting exon number exceeds number of exons, error message is shown. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the range is set, primer search will be restricted to selected exons. For example:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;1-5&lt;/span&gt;&lt;/p&gt;&lt;p&gt;If the range is larger than actual exon range or the starting exon number exceeds number of exons, error message is shown. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6254"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This number of found primer pairs wil bel queried to check if they fullfill the requirements for RTPCR.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимальное число пар для запроса.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5940"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;In this section user can specify sequence quality of target sequence and related parameters.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;In this section user can specify sequence quality of target sequence and related parameters.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5953"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A list of space separated integers. There must be exactly one integer for each base in the Source Sequence if this argument is non-empty. High numbers indicate high confidence in the base call at that position and low numbers indicate low confidence in the base call at that position.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Список разделенных пробелами целых чисел.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5962"/>
        <location filename="../src/Primer3Dialog.ui" line="5978"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum sequence quality allowed within a primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальное качество последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5988"/>
        <location filename="../src/Primer3Dialog.ui" line="6004"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum sequence quality allowed within the 3&apos; pentamer of a primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальное 3&amp;apos;качество последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6014"/>
        <location filename="../src/Primer3Dialog.ui" line="6030"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum legal sequence quality.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимум интервала качества последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6040"/>
        <location filename="../src/Primer3Dialog.ui" line="6056"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum legal sequence quality.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимум интервала качества последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="6309"/>
        <source>Result Settings</source>
        <translation>Параметры результатов</translation>
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
        <translation>Максимальная 3&apos; стабильность</translation>
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
        <source>Illegal TARGET value: %1</source>
        <translation type="vanished">Illegal TARGET value: %1</translation>
    </message>
    <message>
        <source>Illegal PRIMER_DEFAULT_PRODUCT value: %1</source>
        <translation type="vanished">Illegal PRIMER_DEFAULT_PRODUCT value: %1</translation>
    </message>
    <message>
        <source>Illegal PRIMER_INTERNAL_OLIGO_EXCLUDED_REGION value: %1</source>
        <translation type="vanished">Illegal PRIMER_INTERNAL_OLIGO_EXCLUDED_REGION value: %1</translation>
    </message>
    <message>
        <source>Illegal INCLUDED_REGION value: %1</source>
        <translation type="vanished">Illegal INCLUDED_REGION value: %1</translation>
    </message>
    <message>
        <source>Illegal EXCLUDED_REGION value: %1</source>
        <translation type="vanished">Illegal EXCLUDED_REGION value: %1</translation>
    </message>
    <message>
        <source>Unrecognized PRIMER_TASK</source>
        <translation type="vanished">Unrecognized PRIMER_TASK</translation>
    </message>
    <message>
        <source>Contradiction in primer_task definition</source>
        <translation type="vanished">Contradiction in primer_task definition</translation>
    </message>
    <message>
        <source>Missing SEQUENCE tag</source>
        <translation type="vanished">Missing SEQUENCE tag</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="503"/>
        <source>Error in sequence quality data</source>
        <translation>Error in sequence quality data</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="507"/>
        <source>Sequence quality data missing</source>
        <translation>Sequence quality data missing</translation>
    </message>
    <message>
        <source>PRIMER_PAIRS_NUMBER is incorrect. Expected:%2, but Actual:%3</source>
        <translation type="vanished">PRIMER_PAIRS_NUMBER is incorrect. Expected:%2, but Actual:%3</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="36"/>
        <location filename="../src/Primer3Tests.cpp" line="319"/>
        <source>QDomNode isn&apos;t element</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="83"/>
        <location filename="../src/Primer3Tests.cpp" line="89"/>
        <source>Illegal SEQUENCE_TARGET value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="103"/>
        <source>Illegal SEQUENCE_OVERLAP_JUNCTION_LIST value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="118"/>
        <source>Illegal SEQUENCE_INTERNAL_OVERLAP_JUNCTION_LIST value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="133"/>
        <source>Illegal SEQUENCE_EXCLUDED_REGION value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="159"/>
        <source>Illegal SEQUENCE_PRIMER_PAIR_OK_REGION_LIST value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="173"/>
        <source>Illegal SEQUENCE_INCLUDED_REGION value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="191"/>
        <location filename="../src/Primer3Tests.cpp" line="198"/>
        <source>Illegal SEQUENCE_INTERNAL_EXCLUDED_REGION value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="213"/>
        <source>Illegal PRIMER_PRODUCT_SIZE_RANGE value: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="364"/>
        <source>Incorrect results num. Pairs: %1, left: %2, right: %3, inernal: %4</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="376"/>
        <location filename="../src/Primer3Tests.cpp" line="387"/>
        <location filename="../src/Primer3Tests.cpp" line="398"/>
        <source>Incorrect parameter: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="518"/>
        <source>No error, but expected: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="521"/>
        <source>An unexpected error. Expected: %1, but Actual: %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="530"/>
        <source>No warning, but expected: %1</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="534"/>
        <source>An unexpected warning. Expected: %1, but Actual: %2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="546"/>
        <source>PRIMER_PAIR_NUM_RETURNED is incorrect. Expected:%1, but Actual:%2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="562"/>
        <source>Incorrect single primers num. Expected:%1, but Actual:%2</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="573"/>
        <source>Incorrect single primer type, num: %1. Expected:%2, but Actual:%3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="729"/>
        <source>PRIMER_PAIR%1_LIBRARY_MISPRIMING_NAME name is incorrect. Expected:%2, but Actual:%3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="733"/>
        <source>PRIMER_PAIR%1_COMPL_ANY_STUCT name is incorrect. Expected:%2, but Actual:%3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="737"/>
        <source>PRIMER_PAIR%1_COMPL_END_STUCT name is incorrect. Expected:%2, but Actual:%3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="750"/>
        <source>%1 is incorrect. Expected:%2,%3, but Actual:NULL</source>
        <translation>%1 is incorrect. Expected:%2,%3, but Actual:NULL</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="758"/>
        <source>%1 is incorrect. Expected:NULL, but Actual:%2,%3</source>
        <translation>%1 is incorrect. Expected:NULL, but Actual:%2,%3</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="767"/>
        <source>%1 is incorrect. Expected:%2,%3, but Actual:%4,%5</source>
        <translation>%1 is incorrect. Expected:%2,%3, but Actual:%4,%5</translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="792"/>
        <source>%1_LIBRARY_MISPRIMING_NAME name is incorrect. Expected:%2, but Actual:%3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="796"/>
        <source>%1_SELF_ANY_STUCT name is incorrect. Expected:%2, but Actual:%3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="800"/>
        <source>%1_SELF_END_STUCT name is incorrect. Expected:%2, but Actual:%3</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Tests.cpp" line="814"/>
        <location filename="../src/Primer3Tests.cpp" line="822"/>
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
        <location filename="../src/Primer3Plugin.cpp" line="125"/>
        <source>Error</source>
        <translation>Ошибка</translation>
    </message>
    <message>
        <location filename="../src/Primer3Plugin.cpp" line="126"/>
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
        <source>Start Codon Position</source>
        <translation type="vanished">Позиция стартового кодона</translation>
    </message>
    <message>
        <source>Product Min Tm</source>
        <translation type="vanished">Минимальная Tm продукта</translation>
    </message>
    <message>
        <source>Product Opt Tm</source>
        <translation type="vanished">Оптимальная Tm продукта</translation>
    </message>
    <message>
        <source>Product Max Tm</source>
        <translation type="vanished">Максимальная Tm продукта</translation>
    </message>
    <message>
        <source>Opt GC%</source>
        <translation type="vanished">Оптимальный GC%</translation>
    </message>
    <message>
        <source>Inside Penalty</source>
        <translation type="vanished">Внутренний штраф</translation>
    </message>
    <message>
        <source>Internal Oligo Opt Tm</source>
        <translation type="vanished">Оптимальная внутренняя Oligo Tm</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="506"/>
        <source>Excluded Regions</source>
        <translation>Исключаемые регионы</translation>
    </message>
    <message>
        <source>Internal Oligo Excluded Regions</source>
        <translation type="vanished">Внутренние исключенные Oligo регионы</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="479"/>
        <source>Targets</source>
        <translation>Целевые регионы</translation>
    </message>
    <message>
        <source>Included region is too small for current product size ranges</source>
        <translation type="vanished">Включенный регион слишком малений для текущих диапазонов размера продукта</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="711"/>
        <source>Product Size Ranges</source>
        <translation>Диапазоны размеров продукта</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="617"/>
        <source>Sequence Quality</source>
        <translation>Качество последовательности</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="488"/>
        <source>Overlap Junction List</source>
        <translation type="unfinished">Список перекрывающихся соединений</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="497"/>
        <source>Internal Oligo Overlap Positions</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="514"/>
        <source>The &quot;Include region&quot; should be the only one</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="521"/>
        <source>Include Regions</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="530"/>
        <source>Pair OK Region List</source>
        <translation type="unfinished">Список ОК парных регионов</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="540"/>
        <source>Start Codon Sequence</source>
        <translation type="unfinished">Последовательность стартового кодона</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="551"/>
        <source>Five Matches on Primer&apos;s 5&apos;</source>
        <translation type="unfinished">Пять совпадений на 5&apos; конце</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="562"/>
        <source>Five Matches on Primer&apos;s 3&apos;</source>
        <translation type="unfinished">Пять совпадений на 5&apos; конце {3&apos;?}</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="573"/>
        <source>Five Matches on Internal Oligo&apos;s  5&apos;</source>
        <translation type="unfinished">Пять совпадений на 5&apos; конце Внутреннего олиго</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="584"/>
        <source>Five Matches on Internal Oligo&apos;s 3&apos;</source>
        <translation type="unfinished">Пять совпадений на 3&apos; конце Внутренного олиго</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="624"/>
        <source>Sequence quality list length must be equal to the sequence length</source>
        <translation>Длина списка качества последовательности должна быть равна длине последовательности</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="641"/>
        <source>Task &quot;pick_discriminative_primers&quot; requires exactly one &quot;Targets&quot; region.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="652"/>
        <source>At least one primer on the &quot;Main&quot; settings page should be enabled.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="657"/>
        <source>Primer3 task</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="674"/>
        <source>Primer Size Ranges should have at least one range</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="686"/>
        <source>Sequence range region is too small for current product size ranges</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="690"/>
        <source>The priming sequence is too long, please, decrease the region</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="698"/>
        <source>Incorrect summ &quot;Included Region Start + First Base Index&quot; - should be more or equal than 0</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="703"/>
        <source>The priming sequence is out of range.
Either make the priming region end &quot;%1&quot; less or equal than the sequence size &quot;%2&quot; plus the first base index value &quot;%3&quot;or mark the sequence as circular</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="827"/>
        <source>Can&apos;t read to &quot;%1&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="847"/>
        <location filename="../src/Primer3Dialog.cpp" line="856"/>
        <location filename="../src/Primer3Dialog.cpp" line="865"/>
        <location filename="../src/Primer3Dialog.cpp" line="877"/>
        <location filename="../src/Primer3Dialog.cpp" line="930"/>
        <location filename="../src/Primer3Dialog.cpp" line="936"/>
        <location filename="../src/Primer3Dialog.cpp" line="971"/>
        <source>Can&apos;t parse &quot;%1&quot; value: &quot;%2&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="866"/>
        <source>Incorrect value for &quot;%1&quot; value: &quot;%2&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="949"/>
        <source>PRIMER_MISPRIMING_LIBRARY value should points to the file from the &quot;%1&quot; directory</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="962"/>
        <source>PRIMER_INTERNAL_MISHYB_LIBRARY value should points to the file from the &quot;%1&quot; directory</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="966"/>
        <source>PRIMER_MIN_THREE_PRIME_DISTANCE is unused in the UGENE GUI interface. We may either skip it or set PRIMER_MIN_LEFT_THREE_PRIME_DISTANCE and PRIMER_MIN_RIGHT_THREE_PRIME_DISTANCE to %1. Do you want to set?</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <source>Can&apos;t pick hyb oligo and only one primer</source>
        <translation type="vanished">Can&apos;t pick hyb oligo and only one primer</translation>
    </message>
    <message>
        <source>Nothing to pick</source>
        <translation type="vanished">Nothing to pick</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="721"/>
        <source>The field &apos;%1&apos; has invalid value</source>
        <translation>The field &apos;%1&apos; has invalid value</translation>
    </message>
    <message>
        <source>Cannot create an annotation object. Please check settings.</source>
        <translation type="vanished">Невозможно создать аннотацию. Проверьте настройки.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="738"/>
        <source>Save primer settings</source>
        <translation>Сохранить настройки праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="745"/>
        <source>Can&apos;t write to &quot;%1&quot;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="819"/>
        <source>Text files</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="820"/>
        <source>Load settings</source>
        <translation>Загрузить настройки</translation>
    </message>
    <message>
        <source>Can not load settings file: invalid format.</source>
        <translation type="vanished">Невозможно загрузить настройки: неверный формат.</translation>
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
        <location filename="../src/Primer3Plugin.cpp" line="66"/>
        <source>Primer3 lock</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::Primer3SWTask</name>
    <message>
        <location filename="../src/Primer3Task.cpp" line="611"/>
        <source>Incorrect summ &quot;Included Region Start + First Base Index&quot; - should be more or equal than 0</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>U2::Primer3Task</name>
    <message>
        <location filename="../src/Primer3Task.cpp" line="388"/>
        <source>Pick primers task</source>
        <translation>Выбор праймеров</translation>
    </message>
</context>
<context>
    <name>U2::Primer3ToAnnotationsTask</name>
    <message>
        <location filename="../src/Primer3Task.cpp" line="643"/>
        <source>Search primers to annotations</source>
        <translation>Search primers to annotations</translation>
    </message>
    <message>
        <location filename="../src/Primer3Task.cpp" line="676"/>
        <source>Failed to find any exon annotations associated with the sequence %1.Make sure the provided sequence is cDNA and has exonic structure annotated</source>
        <translation>Failed to find any exon annotations associated with the sequence %1.Make sure the provided sequence is cDNA and has exonic structure annotated</translation>
    </message>
    <message>
        <location filename="../src/Primer3Task.cpp" line="687"/>
        <source>The first exon from the selected range [%1,%2] is larger the number of exons (%3). Please set correct exon range.</source>
        <translation type="unfinished">The first exon from the selected range [%1,%2] is larger the number of exons (%2). Please set correct exon range. {1,%2]?} {3)?}</translation>
    </message>
    <message>
        <location filename="../src/Primer3Task.cpp" line="696"/>
        <source>The the selected exon range [%1,%2] is larger the number of exons (%3). Please set correct exon range.</source>
        <translation type="unfinished">The the selected exon range [%1,%2] is larger the number of exons (%2). Please set correct exon range. {1,%2]?} {3)?}</translation>
    </message>
    <message>
        <source>The first exon from the selected range [%1,%2] is larger the number of exons (%2). Please set correct exon range.</source>
        <translation type="vanished">The first exon from the selected range [%1,%2] is larger the number of exons (%2). Please set correct exon range.</translation>
    </message>
    <message>
        <source>The the selected exon range [%1,%2] is larger the number of exons (%2). Please set correct exon range.</source>
        <translation type="vanished">The the selected exon range [%1,%2] is larger the number of exons (%2). Please set correct exon range.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Task.cpp" line="794"/>
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
