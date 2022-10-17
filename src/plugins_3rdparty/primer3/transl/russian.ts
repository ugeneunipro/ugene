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
        <location filename="../src/Primer3Dialog.ui" line="270"/>
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
        <location filename="../src/Primer3Dialog.ui" line="729"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A list of product size ranges, for example:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot;font-weight:600;&quot;&gt; 150-250 100-300 301-400 &lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 first tries to pick primers in the first range. If that is not possible, it goes to the next range and tries again. It continues in this way until it has either picked all necessary primers or until there are no more ranges. For technical reasons this option makes much lighter computational demands than the Product Size option.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Список диапазонов размера продукта, например:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot;font-weight:600;&quot;&gt; 150-250 100-300 301-400 &lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="746"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This selection indicates what mispriming library (if any) Primer3 should use to screen for interspersed repeats or for other sequence to avoid as a location for primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Этот параметр указывает какую библиотеку будет использовать Primer3.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="763"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum number of primer pairs to return.&lt;/p&gt;&lt;p&gt;Primer pairs returned are sorted by their &amp;quot;quality&amp;quot;, in other words by the value of the objective function (where a lower number indicates a better primer pair).&lt;/p&gt;&lt;p&gt;Caution: setting this parameter to a large value will increase running time.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимальное число пар праймеров.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="792"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum stability for the last five 3&apos; bases of a left or right primer.&lt;/p&gt;&lt;p&gt;Bigger numbers mean more stable 3&apos; ends. The value is the maximum delta G (kcal/mol) for duplex disruption for the five 3&apos; bases as calculated using the Nearest-Neighbor parameter values specified by the option of &apos;Table of thermodynamic parameters&apos;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимум стабильности для последних пяти 3&apos; оснований левого или правого праймеров. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="827"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed weighted similarity with any sequence in Mispriming Library.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимально допустимое взвешенное сходство с любой последовательностью из библиотеки ошибочного праймирования.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="856"/>
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
        <location filename="../src/Primer3Dialog.ui" line="115"/>
        <source>Pick left primer</source>
        <translation>Выбрать левый праймер</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="163"/>
        <source>or use left primer below</source>
        <translation>или использовать этот левый праймер</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="905"/>
        <source>General Settings</source>
        <translation>Общие настройки</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="908"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section covers such settings as primer size, temperature, GC and other general settings.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;&gt;В этом разделе рассматриваются такие параметры, как размер праймера, температура, GC и другие общие настройки.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="957"/>
        <location filename="../src/Primer3Dialog.ui" line="999"/>
        <location filename="../src/Primer3Dialog.ui" line="1118"/>
        <location filename="../src/Primer3Dialog.ui" line="1155"/>
        <location filename="../src/Primer3Dialog.ui" line="1169"/>
        <source>Min</source>
        <translation>Минимум</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1238"/>
        <location filename="../src/Primer3Dialog.ui" line="1312"/>
        <location filename="../src/Primer3Dialog.ui" line="1446"/>
        <location filename="../src/Primer3Dialog.ui" line="1568"/>
        <location filename="../src/Primer3Dialog.ui" line="1575"/>
        <source>Opt</source>
        <translation>Оптимум</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1039"/>
        <location filename="../src/Primer3Dialog.ui" line="1079"/>
        <location filename="../src/Primer3Dialog.ui" line="1141"/>
        <location filename="../src/Primer3Dialog.ui" line="1148"/>
        <location filename="../src/Primer3Dialog.ui" line="1505"/>
        <source>Max</source>
        <translation>Максимум</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1065"/>
        <source>Primer Tm</source>
        <translation>Т. плав. праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1222"/>
        <location filename="../src/Primer3Dialog.ui" line="1267"/>
        <location filename="../src/Primer3Dialog.ui" line="1289"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum lengths (in bases) of a primer oligo.&lt;/p&gt;&lt;p&gt;Primer3 will not pick primers shorter than Min or longer than Max, and with default arguments will attempt to pick primers close with size close to Opt.&lt;/p&gt;&lt;p&gt;Min cannot be smaller than 1. Max cannot be larger than 36. (This limit is governed by maximum oligo size for which melting-temperature calculations are valid.)&lt;/p&gt;&lt;p&gt;Min cannot be greater than Max.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальная, оптимальная и максимальная длины олиго праймера.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="944"/>
        <location filename="../src/Primer3Dialog.ui" line="1052"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum melting temperatures (Celsius) for a primer oligo.&lt;/p&gt;&lt;p&gt;Primer3 will not pick oligos with temperatures smaller than Min or larger than Max, and with default conditions will try to pick primers with melting temperatures close to Opt.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальная, оптимальная и максимальная температуры плавления олиго праймера.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1554"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum percentage of Gs and Cs in any primer or oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;
</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальное, оптимальное и максимальное процентное содержание Gs и Cs в праймере или олиго.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;
</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1248"/>
        <location filename="../src/Primer3Dialog.ui" line="1433"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum percentage of Gs and Cs in any primer or oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальное, оптимальное и максимальное процентное содержание Gs и Cs в праймере или олиго.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1357"/>
        <location filename="../src/Primer3Dialog.ui" line="1389"/>
        <location filename="../src/Primer3Dialog.ui" line="1411"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum, optimum, and maximum melting temperature of the amplicon.&lt;/p&gt;&lt;p&gt;Primer3 will not pick a product with melting temperature less than min or greater than max. If Opt is supplied and the Penalty Weights for Product Size are non-0 Primer3 will attempt to pick an amplicon with melting temperature close to Opt.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальная, оптимальная и максимальная температуры плавления ампликона.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1092"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Maximum acceptable (unsigned) difference between the melting temperatures of the left and right primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимально допустимое различие между температурами плавления левого и правого праймеров.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1032"/>
        <source>Table of thermodynamic parameters</source>
        <translation>Таблица термодинамических параметров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4012"/>
        <source>Primers</source>
        <translation>Праймеры</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4614"/>
        <source>Primer Pairs</source>
        <translation>Пары праймеров</translation>
    </message>
    <message>
        <source>Hyb Oligos</source>
        <translation type="vanished">Гибридизация олиго</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5896"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section allows to set output annotations&apos; settings.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;В этой секции можно задать настройки для аннотаций.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1519"/>
        <source>Breslauer et. al 1986</source>
        <translation>Breslauer et. al 1986</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1524"/>
        <location filename="../src/Primer3Dialog.ui" line="2388"/>
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
        <location filename="../src/Primer3Dialog.ui" line="736"/>
        <source>Mispriming/Repeat library</source>
        <translation>Библиотека повторов/ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="706"/>
        <source>Number to return</source>
        <translation>Результатов не более</translation>
    </message>
    <message>
        <source>Max repeat mispriming</source>
        <translation type="vanished">Максимальный повтор ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1975"/>
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
        <location filename="../src/Primer3Dialog.ui" line="2046"/>
        <source>Pair max template mispriming</source>
        <translation>Парный максимальный шаблон ошибочного праймирования</translation>
    </message>
    <message>
        <source>Start codon position</source>
        <translation type="vanished">Позиция стартового кодона</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="131"/>
        <source>Pick hybridization probe (internal oligo)</source>
        <translation>Выбрать гибридизационную пробу (internal oligo)</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="173"/>
        <source>or use oligo below</source>
        <translation>или использовать эту oligo</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="150"/>
        <source>Pick right primer</source>
        <translation>Выбрать правый праймер</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="180"/>
        <source>or use right primer below (5&apos; to 3&apos; on opposite strand)</source>
        <translation>или использовать этот правый праймер (5&apos; в 3&apos; в противоположном направлении)</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1305"/>
        <source>Primer size</source>
        <translation>Размер праймера</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1162"/>
        <location filename="../src/Primer3Dialog.ui" line="4678"/>
        <source>Product Tm</source>
        <translation>Т. плав. продукта</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1183"/>
        <source>Primer GC%</source>
        <translation>GC состав %</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="918"/>
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
        <location filename="../src/Primer3Dialog.ui" line="2667"/>
        <source>Inside target penalty</source>
        <translation>Внутренний целевой штраф</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2255"/>
        <source>Outside target penalty</source>
        <translation>Внешний целевой штраф</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2339"/>
        <source>Max poly-X</source>
        <translation>Максимум poly-X</translation>
    </message>
    <message>
        <source>First base index</source>
        <translation type="vanished">Индекс первого основания</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2687"/>
        <source>CG clamp</source>
        <translation>Зажим CG</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2376"/>
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
        <location filename="../src/Primer3Dialog.ui" line="2899"/>
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
        <location filename="../src/Primer3Dialog.ui" line="3083"/>
        <location filename="../src/Primer3Dialog.ui" line="3281"/>
        <location filename="../src/Primer3Dialog.ui" line="3319"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer Size for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эквивалентный параметр размера праймера для внутреннего олиго.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3119"/>
        <location filename="../src/Primer3Dialog.ui" line="3164"/>
        <location filename="../src/Primer3Dialog.ui" line="3352"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer Tm for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Tm олиго.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3246"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer GC% for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;GC% олиго.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3138"/>
        <location filename="../src/Primer3Dialog.ui" line="3303"/>
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
        <location filename="../src/Primer3Dialog.ui" line="5653"/>
        <source>RT-PCR</source>
        <translation>RT-PCR</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5665"/>
        <source>Design primers for RT-PCR analysis</source>
        <translation>Дизайн праймеров для анализа RT-PCR</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5685"/>
        <source>Exon annotation name:</source>
        <translation>Имя экзон аннотации:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5695"/>
        <source>exon</source>
        <translation>exon</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5720"/>
        <source>Minimum exon junction overlap size</source>
        <translation>Минимальный размер перекрытия соединения экзона</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5784"/>
        <source>Exon range:</source>
        <translation>Диапазон экзона:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5831"/>
        <source>Max number of pairs to query:</source>
        <translation>Максимальное число пар для запроса:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5558"/>
        <source>Min sequence quality:</source>
        <translation>Минимальное качество последовательности:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5581"/>
        <source>Min 3&apos; sequence quality:</source>
        <translation>Минимальное 3&apos;качество последовательности:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5604"/>
        <source>Sequence quality range min:</source>
        <translation>Минимум интервала качества последовательности:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5627"/>
        <source>Sequence quality range max:</source>
        <translation>Максимум интервала качества последовательности:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5944"/>
        <source>Help</source>
        <translation>Помощь</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5964"/>
        <source>Save settings</source>
        <translation>Сохранить настройки</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5971"/>
        <source>Load settings</source>
        <translation>Загрузить настройки</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5978"/>
        <source>Reset form</source>
        <translation>Сброс параметров</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5985"/>
        <source>Pick primers</source>
        <translation>Подобрать</translation>
    </message>
    <message>
        <source>Concentration of monovalent cations</source>
        <translation type="vanished">Концентрация одновалентных катионов</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1515"/>
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
        <location filename="../src/Primer3Dialog.ui" line="2575"/>
        <source>Salt correction formula</source>
        <translation>Коррекционная формула</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2379"/>
        <location filename="../src/Primer3Dialog.ui" line="2383"/>
        <source>Schildkraut and Lifson 1965</source>
        <translation>Schildkraut and Lifson 1965</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2393"/>
        <source>Owczarzy et. 2004</source>
        <translation>Owczarzy et. 2004</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2694"/>
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
        <location filename="../src/Primer3Dialog.ui" line="2707"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Maximum number of unknown bases (N) allowable in any primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимальное число неизвестных оснований (N), допустимых в праймере.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2634"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowable length of a mononucleotide repeat, for example AAAAAA.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимальная допустимая для мононуклеотидного повтора, например AAAAAA.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2536"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Non-default values valid only for sequences with 0 or 1 target regions.&lt;/p&gt;&lt;p&gt;If the primer is part of a pair that spans a target and does not overlap the target, then multiply this value times the number of nucleotide positions from the 3&apos; end to the (unique) target to get the &apos;position penalty&apos;. The effect of this parameter is to allow Primer3 to include nearness to the target as a term in the objective function.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Внешний целевой штраф.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2562"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This parameter is the index of the first base in the input sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Индекс первого основания во входной последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2726"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Require the specified number of consecutive Gs and Cs at the 3&apos; end of both the left and right primer.&lt;/p&gt;&lt;p&gt;This parameter has no effect on the internal oligo if one is requested.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Зажим CG.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2166"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The millimolar (mM) concentration of monovalent salt cations (usually KCl) in the PCR.&lt;/p&gt;&lt;p&gt;Primer3 uses this argument to calculate oligo and primer melting temperatures.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Концентрация одновалентных катионов.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2407"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The millimolar concentration of divalent salt cations (usually MgCl^(2+)) in the PCR.&lt;/p&gt;&lt;p&gt;Primer3 converts concentration of divalent cations to concentration of monovalent cations using following formula:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;[Monovalent cations] = [Monovalent cations] + 120*(([divalent cations] - [dNTP])^0.5)&lt;/span&gt;&lt;/p&gt;&lt;p&gt;In addition, if the specified concentration of dNTPs is larger than the concentration of divalent cations then the effect of the divalent cations is not considered.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Концентрация двухвалентных катионов.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2432"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The millimolar concentration of the sum of all deoxyribonucleotide triphosphates.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Концентрация dNTP.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2810"/>
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
        <location filename="../src/Primer3Dialog.ui" line="2982"/>
        <source>Lowercase masking</source>
        <translation>Строчная маскировка</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3020"/>
        <source>Internal Oligo</source>
        <translation>Внутреннее олиго</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3023"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Parameters governing choice of internal oligos are analogous to the parameters governing choice of primer pairs.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Параметры, регулирующие выбор внутренних олиго аналогичны параметрам, регулирующих выбор пар праймеров.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Middle oligos may not overlap any region specified by this tag. The associated value must be a space-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;&amp;lt;start&amp;gt;,&amp;lt;length&amp;gt;&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs, where &amp;lt;start&amp;gt; is the index of the first base of an excluded region, and &amp;lt;length&amp;gt; is its length. Often one would make Target regions excluded regions for internal oligos.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="vanished">&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Исключаемые олиго регионы.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3177"/>
        <location filename="../src/Primer3Dialog.ui" line="3226"/>
        <location filename="../src/Primer3Dialog.ui" line="3233"/>
        <location filename="../src/Primer3Dialog.ui" line="3332"/>
        <source>Min:</source>
        <translation>Мин:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3099"/>
        <location filename="../src/Primer3Dialog.ui" line="3184"/>
        <location filename="../src/Primer3Dialog.ui" line="3205"/>
        <location filename="../src/Primer3Dialog.ui" line="3397"/>
        <source>Opt:</source>
        <translation>Опт:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3106"/>
        <location filename="../src/Primer3Dialog.ui" line="3191"/>
        <location filename="../src/Primer3Dialog.ui" line="3198"/>
        <location filename="../src/Primer3Dialog.ui" line="3339"/>
        <source>Max:</source>
        <translation>Макс:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3657"/>
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
        <location filename="../src/Primer3Dialog.ui" line="3994"/>
        <source>Penalty Weights</source>
        <translation>Веса штрафов</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4157"/>
        <source>Tm</source>
        <translation>Tm</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4072"/>
        <source>Size  </source>
        <translation>Размер  </translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4021"/>
        <source>GC%</source>
        <translation>GC%</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4028"/>
        <location filename="../src/Primer3Dialog.ui" line="4065"/>
        <location filename="../src/Primer3Dialog.ui" line="4079"/>
        <location filename="../src/Primer3Dialog.ui" line="4207"/>
        <location filename="../src/Primer3Dialog.ui" line="4623"/>
        <location filename="../src/Primer3Dialog.ui" line="4722"/>
        <location filename="../src/Primer3Dialog.ui" line="5064"/>
        <location filename="../src/Primer3Dialog.ui" line="5071"/>
        <location filename="../src/Primer3Dialog.ui" line="5149"/>
        <location filename="../src/Primer3Dialog.ui" line="5223"/>
        <source>Lt:</source>
        <translation>Lt:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4051"/>
        <location filename="../src/Primer3Dialog.ui" line="4058"/>
        <location filename="../src/Primer3Dialog.ui" line="4134"/>
        <location filename="../src/Primer3Dialog.ui" line="4214"/>
        <location filename="../src/Primer3Dialog.ui" line="4701"/>
        <location filename="../src/Primer3Dialog.ui" line="4715"/>
        <location filename="../src/Primer3Dialog.ui" line="5126"/>
        <location filename="../src/Primer3Dialog.ui" line="5156"/>
        <location filename="../src/Primer3Dialog.ui" line="5209"/>
        <location filename="../src/Primer3Dialog.ui" line="5230"/>
        <source>Gt:</source>
        <translation>Gt:</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4407"/>
        <source>#N&apos;s</source>
        <translation>N</translation>
    </message>
    <message>
        <source>Mispriming</source>
        <translation type="vanished">Ошибочное праймирование</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4453"/>
        <location filename="../src/Primer3Dialog.ui" line="5542"/>
        <source>Sequence quality</source>
        <translation>Качество последовательности</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4479"/>
        <source>End sequence quality</source>
        <translation>Качество конца последовательности</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4505"/>
        <source>Position penalty</source>
        <translation>Позиция штрафа</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4531"/>
        <source>End stability</source>
        <translation>Стабильность конца</translation>
    </message>
    <message>
        <source>Template mispriming</source>
        <translation type="vanished">Шаблон ошибочного праймирования</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4708"/>
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
        <location filename="../src/Primer3Dialog.ui" line="5677"/>
        <source>mRNA sequence</source>
        <translation>Последовательность mRNA</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5732"/>
        <source>At 5&apos; side (bp):</source>
        <translation>Со стороны 5&apos; (нк):</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5762"/>
        <source>At 3&apos;side (bp)</source>
        <translation>Со стороны 3&apos; (нк)</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5819"/>
        <source>Primer product must span at least one intron on the corresponding genomic DNA</source>
        <translation>Продук праймера должен охватывать по крайней мере один интрон на соответствующей геномной ДНК</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5533"/>
        <source>Sequence Quality</source>
        <translation>Качество последовательности</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="44"/>
        <source>Select the Task for primer selection</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="51"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This tag tells Primer3 what task to perform. Legal values are:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;generic&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Design a primer pair (the classic Primer3 task) if the &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_LEFT_PRIMER=1&lt;/span&gt;, and &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_RIGHT_PRIMER=1&lt;/span&gt;. In addition, pick an internal hybridization oligo if &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_INTERNAL_OLIGO=1&lt;/span&gt;.&lt;/p&gt;&lt;p&gt;NOTE: If &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_LEFT_PRIMER=1&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_RIGHT_PRIMER=0&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_INTERNAL_OLIGO=1&lt;/span&gt;, then behaves similarly to &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_TASK=pick_primer_list&lt;/span&gt;.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;check_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Primer3 only checks the primers provided in &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_PRIMER&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_INTERNAL_OLIGO&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_PRIMER_REVCOMP&lt;/span&gt;. It is the only task that does not require a sequence. However, if &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_TEMPLATE&lt;/span&gt; is provided, it is used.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_primer_list&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick all primers in &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_TEMPLATE&lt;/span&gt; (possibly limited by &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_INCLUDED_REGION&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_EXCLUDED_REGION&lt;/span&gt;, &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_PRIMER_PAIR_OK_REGION_LIST&lt;/span&gt;, etc.). Returns the primers sorted by quality starting with the best primers. If &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_LEFT_PRIMER&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_RIGHT_PRIMER&lt;/span&gt; is selected Primer3 does not to pick primer pairs but generates independent lists of left primers, right primers, and, if requested, internal oligos.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick primers suited to sequence a region. &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_TARGET&lt;/span&gt; can be used to indicate several targets. The position of each primer is calculated for optimal sequencing results.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;pick_cloning_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick primers suited to clone a gene were the start nucleotide and the end nucleotide of the PCR fragment must be fixed, for example to clone an ORF. &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_INCLUDED_REGION&lt;/span&gt; must be used to indicate the first and the last nucleotide. Due to these limitations Primer3 can only vary the length of the primers. Set &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_ANYWAY=1&lt;/span&gt; to obtain primers even if they violate specific constraints.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_discriminative_primers&lt;/span&gt;&lt;/p&gt;&lt;p&gt;Pick primers suited to select primers which bind with their end at a specific position. This can be used to force the end of a primer to a polymorphic site, with the goal of discriminating between sequence variants. &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_TARGET&lt;/span&gt; must be used to provide a single target indicating the last nucleotide of the left (nucleotide before the first nucleotide of the target) and the right primer (nucleotide following the target). The primers border the &lt;span style=&quot; font-weight:700;&quot;&gt;SEQUENCE_TARGET&lt;/span&gt;. Due to these limitations Primer3 can only vary the length of the primers. Set &lt;span style=&quot; font-weight:700;&quot;&gt;PRIMER_PICK_ANYWAY=1&lt;/span&gt; to obtain primers even if they violate specific constraints.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="55"/>
        <source>generic</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="60"/>
        <source>pick_sequencing_primers</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="65"/>
        <source>pick_primer_list</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="70"/>
        <source>check_primers</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="75"/>
        <source>pick_cloning_primers</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="80"/>
        <source>pick_discriminative_primers</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="210"/>
        <location filename="../src/Primer3Dialog.ui" line="241"/>
        <source>5&apos; Overhang:</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="217"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The provided sequence is added to the 5&apos; end of the left primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="248"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The provided sequence is added to the 5&apos; end of the right primer. It is reverse complementary to the template sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="277"/>
        <source>Overlap Junction List</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="284"/>
        <source>Excluded Regions</source>
        <translation type="unfinished">Исключаемые регионы</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="291"/>
        <source>Pair OK Region List</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="298"/>
        <source>Included region</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="305"/>
        <source>Start Codon Position</source>
        <translation type="unfinished">Позиция стартового кодона</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="312"/>
        <source>Start Codon Sequence</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="319"/>
        <source>Force Left Primer Start</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="326"/>
        <source>Force Left Primer End</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="333"/>
        <source>Five Matches on Primer&apos;s 5&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="340"/>
        <source>Five Matches on Internal Oligo&apos;s  5&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="351"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If one or more Targets is specified then a legal primer pair must flank at least one of them. A Target might be a simple sequence repeat site (for example a CA repeat) or a single-base-pair polymorphism. The value should be a space-separated list of &lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs where &lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt; is the index of the first base of a Target, and length is its &lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="358"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If this list is not empty, then the forward OR the reverse primer must overlap one of these junction points by at least &lt;span style=&quot; font-weight:700;&quot;&gt;3 Prime Junction Overlap&lt;/span&gt; nucleotides at the 3&apos; end and at least &lt;span style=&quot; font-weight:700;&quot;&gt;5 Prime Junction Overlap&lt;/span&gt; nucleotides at the 5&apos; end.&lt;/p&gt;&lt;p&gt;In more detail: The junction associated with a given position is the space immediately to the right of that position in the template sequence on the strand given as input.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="378"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This tag allows detailed specification of possible locations of left and right primers in primer pairs.&lt;/p&gt;&lt;p&gt;The associated value must be a semicolon-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;left_start&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;left_length&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;right_start&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;right_length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;quadruples. The left primer must be in the region specified by &lt;span style=&quot; font-weight:700;&quot;&gt;left_start&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;left_length&lt;/span&gt; and the right primer must be in the region specified by &lt;span style=&quot; font-weight:700;&quot;&gt;right_start&lt;/span&gt;,&lt;span style=&quot; font-weight:700;&quot;&gt;right_length&lt;/span&gt;. &lt;span style=&quot; font-weight:700;&quot;&gt;left_start&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;left_length&lt;/span&gt; specify the location of the left primer in terms of the index of the first base in the region and the length of the region. &lt;span style=&quot; font-weight:700;&quot;&gt;right_start&lt;/span&gt; and &lt;span style=&quot; font-weight:700;&quot;&gt;right_length&lt;/span&gt; specify the location of the right primer in analogous fashion. As seen in the example below, if no integers are specified for a region then the location of the corresponding primer is not constrained.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="385"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Primer oligos may not overlap any region specified in this tag. The associated value must be a space-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs where start is the index of the first base of the excluded region, and length is its length. This tag is useful for tasks such as excluding regions of low sequence quality or for excluding regions containing repetitive elements such as ALUs or LINEs.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="431"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A sub-region of the given sequence in which to pick primers. For example, often the first dozen or so bases of a sequence are vector, and should be excluded from consideration. The value for this parameter has the form&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;where &lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt; is the index of the first base to consider, and &lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt; is the number of subsequent bases in the primer-picking region.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="455"/>
        <source>Force Right Primer End</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="468"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Forces the 3&apos; end of the left primer to be at the indicated position. Primers are also picked if they violate certain constraints. The default value indicates that the end of the left primer can be anywhere.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="503"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Forces the 5&apos; end of the left primer to be at the indicated position. Primers are also picked if they violate certain constraints. The default value indicates that the start of the left primer can be anywhere.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="545"/>
        <location filename="../src/Primer3Dialog.ui" line="559"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Five Matches on Primer&apos;s 5&apos;&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="552"/>
        <source>Five Matches on Primer&apos;s 3&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="566"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Five Matches on Primer&apos;s 3&apos;&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="573"/>
        <source>Five Matches on Internal Oligo&apos;s 3&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="580"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This parameter should be considered EXPERIMENTAL at this point. Please check the output carefully; some erroneous inputs might cause an error in Primer3.&lt;br&gt;&lt;br&gt;
Index of the first base of a start codon. This parameter allows Primer3 to select primer pairs to create in-frame amplicons e.g. to create a template for a fusion protein. Primer3 will attempt to select an in-frame left primer, ideally starting at or to the left of the start codon, or to the right if necessary. Negative values of this parameter are legal if the actual start codon is to the left of available sequence. If this parameter is non-negative Primer3 signals an error if the codon at the position specified by this parameter is not an ATG. A value less than or equal to -10^6 indicates that Primer3 should ignore this parameter.&lt;br&gt;&lt;br&gt;
Primer3 selects the position of the right primer by scanning right from the left primer for a stop codon. Ideally the right primer will end at or after the stop codon.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="620"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Forces the 5&apos; end of the right primer to be at the indicated position. Primers are also picked if they violate certain constraints. The default value indicates that the start of the right primer can be anywhere.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="636"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Discards all primers which do not match this match sequence at the 3&apos; end. Similar parameter to &lt;span style=&quot; font-weight:700;&quot;&gt;Five Matches on Primer&apos;s 5&apos;&lt;/span&gt;, but limits the 3&apos; end. (New in v. 2.3.6, added by A. Untergasser.)&lt;br/&gt;&lt;br/&gt;The match sequence must be 5 nucletides long and can contain the following letters:&lt;/p&gt;&lt;p&gt;N Any nucleotide&lt;br/&gt;A Adenine&lt;br/&gt;G Guanine&lt;br/&gt;C Cytosine&lt;br/&gt;T Thymine&lt;br/&gt;R Purine (A or G)&lt;br/&gt;Y Pyrimidine (C or T)&lt;br/&gt;W Weak (A or T)&lt;br/&gt;S Strong (G or C)&lt;br/&gt;M Amino (A or C)&lt;br/&gt;K Keto (G or T)&lt;br/&gt;B Not A (G or C or T)&lt;br/&gt;H Not G (A or C or T)&lt;br/&gt;D Not C (A or G or T)&lt;br/&gt;V Not T (A or G or C)&lt;/p&gt;&lt;p&gt;Any primer which will not match the entire match sequence at the 5&apos; end will be discarded and not evaluated. Setting strict requirements here will result in low quality primers due to the high numbers of primers discarded at this step.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="643"/>
        <source>Force Right Primer Start</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="656"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Forces the 3&apos; end of the right primer to be at the indicated position. Primers are also picked if they violate certain constraints. The default value indicates that the end of the right primer can be anywhere.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="672"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The sequence of the start codon, by default ATG. Some bacteria use different start codons, this tag allows to specify alternative start codons.

Any triplet can be provided as start codon.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="677"/>
        <source>ATG</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="699"/>
        <source>Product Size Ranges</source>
        <translation type="unfinished">Диапазоны размеров продукта</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="716"/>
        <source>Max Library Mispriming</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="779"/>
        <source>Max 3&apos; Stability</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="843"/>
        <source>Pair Max Library Mispriming</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="970"/>
        <location filename="../src/Primer3Dialog.ui" line="1637"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This is the most stable monomer structure of internal oligo calculated by thermodynamic approach. The hairpin loops, bulge loops, internal loops, internal single mismatches, dangling ends, terminal mismatches have been considered. This parameter is calculated only if &lt;span style=&quot; font-weight:700;&quot;&gt;Use Thermodynamic Oligo Alignment&lt;/span&gt; is checked. The default value is 10 degrees lower than the default value of &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Tm Min&lt;/span&gt;. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="996"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Similar to PRIMER_MAX_TEMPLATE_MISPRIMING but assesses alternative binding sites in the template using thermodynamic models. This parameter specifies the maximum allowed melting temperature of an oligo (primer) at an &amp;quot;ectopic&amp;quot; site within the template sequence; 47.0 would be a reasonable choice if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Tm Min&lt;/span&gt; is 57.0.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1019"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The annealing temperature (Celsius) used in the PCR reaction. Usually it is chosen up to 10°C below the melting temperature of the primers. If provided, Primer3 will calculate the fraction of primers bound at the provided annealing temperature for each oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1072"/>
        <source>Primer Bound</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1125"/>
        <location filename="../src/Primer3Dialog.ui" line="1319"/>
        <location filename="../src/Primer3Dialog.ui" line="1532"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Minimum, Optimum, and Maximum acceptable fraction of primer bound at &lt;span style=&quot; font-weight:700;&quot;&gt;Annealing Temperature&lt;/span&gt; for a primer oligo in percent.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1176"/>
        <source>Annealing Temperature</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1593"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Use thermodynamic models to calculate the the propensity of oligos to form hairpins and dimers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1596"/>
        <source>Use Thermodynamic Oligo Alignment</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1608"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Same as&lt;span style=&quot; font-weight:700;&quot;&gt; Max 3&apos; Self Complementarity&lt;/span&gt; but is based on thermodynamical approach - the stability of structure is analyzed.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1650"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Same as &lt;span style=&quot; font-weight:700;&quot;&gt;Max 3&apos; Pair Complementarity&lt;/span&gt; but for calculating the score (melting temperature of structure) thermodynamical approach is used.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1666"/>
        <source>Max 3&apos; Pair Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1673"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Describes the tendency of a primer to bind to itself (interfering with target sequence binding). It will score ANY binding occurring within the entire primer sequence.&lt;/p&gt;&lt;p&gt;It is the maximum allowable local alignment score when testing a single primer for (local) self-complementarity. Local self-complementarity is taken to predict the tendency of primers to anneal to each other without necessarily causing self-priming in the PCR. The scoring system gives 1.00 for complementary bases, -0.25 for a match of any base (or N) with an N, -1.00 for a mismatch, and -2.00 for a gap. Only single-base-pair gaps are allowed.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1689"/>
        <source>TH: Max Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1702"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The same as &lt;span style=&quot; font-weight:700;&quot;&gt;Max Self Complementarity&lt;/span&gt; but all calculations are based on thermodynamical approach.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1718"/>
        <source>TH: Max Primer Hairpin	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1738"/>
        <source>TH: Max 3&apos; Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1758"/>
        <source>Max Pair Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1778"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Describes the tendency of the left primer to bind to the right primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1807"/>
        <source>TH: Max Pair Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1814"/>
        <location filename="../src/Primer3Dialog.ui" line="1859"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Describes the tendency of the left primer to bind to the right primer. It is the maximum allowable local alignment score when testing for complementarity between left and right primers. It is similar to &lt;span style=&quot; font-weight:700;&quot;&gt;Max Self Complementarity&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1843"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Tries to bind the 3&apos;-END to a identical primer and scores the best binding it can find. This is critical for primer quality because it allows primers use itself as a target and amplify a short piece (forming a primer-dimer). These primers are then unable to bind and amplify the target sequence.&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;Max 3&apos; Self Complementarity&lt;/span&gt; is the maximum allowable 3&apos;-anchored global alignment score when testing a single primer for self-complementarity. The 3&apos;-anchored global alignment score is taken to predict the likelihood of PCR-priming primer-dimers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1888"/>
        <source>TH: Max 3&apos; Pair Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1895"/>
        <source>Max Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1902"/>
        <source>Max 3&apos; Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1922"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Use thermodynamic models to calculate the the propensity of oligos to anneal to undesired sites in the template sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1925"/>
        <source>Use Thermodynamic Template Alignment	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1943"/>
        <source>TH: Max Template Mispriming</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1959"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Similar to &lt;span style=&quot; font-weight:700;&quot;&gt;Max template mispriming&lt;/span&gt; but assesses alternative binding sites in the template using thermodynamic models. This parameter specifies the maximum allowed melting temperature of an oligo (primer) at an &amp;quot;ectopic&amp;quot; site within the template sequence; 47.0 would be a reasonable choice if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Tm Min&lt;/span&gt; is 57.0.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="1988"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed similarity to ectopic sites in the template. A negative value means do not check.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2020"/>
        <source>TH: Pair Max Template Mispriming</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2030"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed summed melting temperatures of both primers at ectopic sites within the template (with the two primers in an orientation that would allow PCR amplification.) The melting temperatures are calculated as for &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max Template Mispriming&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2059"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum allowed summed similarity of both primers to ectopic sites in the template. A negative value means do not check.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2108"/>
        <source>Advanced Settings</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2111"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section covers more accurate settings, which are related to &lt;span style=&quot; font-weight:700;&quot;&gt;General Settings&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2121"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The 5&apos; end of the left OR the right primer must overlap one of the junctions in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt; by this amount. See details in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2137"/>
        <source>Sequencing Interval</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2144"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Defines the space from the calculated position of the 3&apos;end to both sides in which Primer3Plus picks the best primer. Value only used if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Task&lt;/span&gt; is &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2182"/>
        <source>Max GC in primer 3&apos; end</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2202"/>
        <source>Annealing Oligo Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2235"/>
        <source>Concentration of Divalent Cations</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2262"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum number of Gs or Cs allowed in the last five 3&apos; bases of a left or right primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2278"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Analogous to &lt;span style=&quot; font-weight:700;&quot;&gt;3&apos; End Distance Between Left Primers&lt;/span&gt;, but for right primers.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2294"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Defines the space from the 3&apos;end of the primer to the point were the trace signals are readable. Value only used if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Task&lt;/span&gt; is &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2310"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The 3&apos; end of the left OR the right primer must overlap one of the junctions in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt; by this amount. See details in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2346"/>
        <source>5 Prime Junction Overlap</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2353"/>
        <source>Sequencing Accuracy</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2451"/>
        <source>First Base Index</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2471"/>
        <source>Sequencing Lead</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2478"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;When returning multiple primer pairs, the minimum number of base pairs between the 3&apos; ends of any two left primers.&lt;/p&gt;&lt;p&gt;Primers with 3&apos; ends at positions e.g. 30 and 31 in the template sequence have a three-prime distance of 1.&lt;/p&gt;&lt;p&gt;In addition to positive values, the values -1 and 0 are acceptable and have special interpretations:&lt;/p&gt;&lt;p&gt;-1 indicates that a given left primer can appear in multiple primer pairs returned by Primer3. This is the default behavior.&lt;/p&gt;&lt;p&gt;0 indicates that a left primer is acceptable if it was not already used. In other words, two left primers are allowed to have the same 3&apos; position provided their 5&apos; positions differ.&lt;/p&gt;&lt;p&gt;For n &amp;gt; 0: A left primer is acceptable if:&lt;/p&gt;&lt;p&gt;NOT(3&apos; end of left primer closer than n to the 3&apos; end of a previously used left primer)&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2494"/>
        <source>Sequencing Spacing</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2501"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Defines the space from the 3&apos;end of the primer to the 3&apos;end of the next primer on the reverse strand. Value only used if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Task&lt;/span&gt; is &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2549"/>
        <source>3 Prime Junction Overlap</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2595"/>
        <source>3&apos; End Distance Between Right Primers	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2602"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This parameter provides a quick-and-dirty way to get Primer3 to accept IUB / IUPAC codes for ambiguous bases (i.e. by changing all unrecognized bases to N). If you wish to include an ambiguous base in an oligo, you must set &lt;span style=&quot; font-weight:700;&quot;&gt;Max #N&apos;s accepted&lt;/span&gt; to a 1 (non-0) value.&lt;/p&gt;&lt;p&gt;&lt;br/&gt;&lt;/p&gt;&lt;p&gt;Perhaps &apos;-&apos; and &apos;* &apos; should be squeezed out rather than changed to &apos;N&apos;, but currently they simply get converted to N&apos;s. The authors invite user comments.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2605"/>
        <source>Max #N&apos;s accepted</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2612"/>
        <location filename="../src/Primer3Dialog.ui" line="2664"/>
        <source>Non-default values are valid only for sequences with 0 or 1 target regions. If the primer is part of a pair that spans a target and overlaps the target, then multiply this value times the number of nucleotide positions by which the primer overlaps the (unique) target to get the &apos;position penalty&apos;. The effect of this parameter is to allow Primer3 to include overlap with the target as a term in the objective function.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2650"/>
        <source>3&apos; End Distance Between Left Primers</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2657"/>
        <source>DMSO Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2781"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Defines the space from the 3&apos;end of the primer to the 3&apos;end of the next primer on the same strand. Value only used if &lt;span style=&quot; font-weight:700;&quot;&gt;Primer Task&lt;/span&gt; is &lt;span style=&quot; font-weight:700; font-style:italic;&quot;&gt;pick_sequencing_primers&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2797"/>
        <source>Concentration of Monovalent Cations</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2826"/>
        <source>Formamide Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2833"/>
        <source>DMSO Factor</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2840"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The concentration of DMSO in percent. See PRIMER_DMSO_FACTOR for details of Tm correction.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2853"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The melting temperature of primers can be approximately corrected for DMSO:&lt;/p&gt;&lt;p&gt;Tm = Tm (without DMSO) - &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Factor&lt;/span&gt; * &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Concentration&lt;/span&gt;&lt;/p&gt;&lt;p&gt;The &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Concentration&lt;/span&gt; concentration must be given in %.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2872"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The concentration of formamide in mol/l. The melting temperature of primers can be approximately corrected for formamide:&lt;/p&gt;&lt;p&gt;Tm = Tm (without formamide) +(0.453 * PRIMER_[LEFT/INTERNAL/RIGHT]_4_GC_PERCENT / 100 - 2.88) * &lt;span style=&quot; font-weight:700;&quot;&gt;Formamide Concentration&lt;/span&gt;&lt;/p&gt;&lt;p&gt;The &lt;span style=&quot; font-weight:700;&quot;&gt;Formamide Concentration&lt;/span&gt; correction was suggested by Blake and Delcourt (R D Blake and S G Delcourt. Nucleic Acids Research, 24, 11:2095–2103, 1996).&lt;/p&gt;&lt;p&gt;Convert % into mol/l:&lt;/p&gt;&lt;p&gt;[DMSO in mol/l] = [DMSO in % weight] * 10 / 45.04 g/mol&lt;/p&gt;&lt;p&gt;[DMSO in mol/l] = [DMSO in % volume] * 10 * 1.13 g/cm3 / 45.04 g/mol&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2912"/>
        <source>Use formatted output</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2922"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If this flag is true, produce PRIMER_LEFT_EXPLAIN, PRIMER_RIGHT_EXPLAIN, PRIMER_INTERNAL_EXPLAIN and/or PRIMER_PAIR_EXPLAIN output tags as appropriate.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2925"/>
        <source>Print Statistics</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2935"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the associated value = 1, then Primer3 will print out the calculated secondary structures&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2938"/>
        <source>Print secondary structures</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2956"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If set to 1, treat ambiguity codes as if they were consensus codes when matching oligos to mispriming or mishyb libraries. For example, if this flag is set, then a C in an oligo will be scored as a perfect match to an S in a library sequence, as will a G in the oligo. More importantly, though, any base in an oligo will be scored as a perfect match to an N in the library. This is very bad if the library contains strings of Ns, as no oligo will be legal (and it will take a long time to find this out). So unless you know for sure that your library does not have runs of Ns (or Xs), then set this flag to 0.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2959"/>
        <source>Treat ambiguity codes in libraries as consensus</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2969"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If true use primer provided in left, right, or internal primer even if it violates specific constraints.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2972"/>
        <source>Pick anyway</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2979"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This option allows for intelligent design of primers in sequence in which masked regions (for example repeat-masked regions) are lower-cased. (New in v. 1.1.0, added by Maido Remm and Triinu Koressaar)&lt;/p&gt;&lt;p&gt;A value of 1 directs Primer3 to reject primers overlapping lowercase a base exactly at the 3&apos; end.&lt;/p&gt;&lt;p&gt;This property relies on the assumption that masked features (e.g. repeats) can partly overlap primer, but they cannot overlap the 3&apos;-end of the primer. In other words, lowercase bases at other positions in the primer are accepted, assuming that the masked features do not influence the primer performance if they do not overlap the 3&apos;-end of primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2989"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This parameter provides a quick-and-dirty way to get Primer3 to accept IUB / IUPAC codes for ambiguous bases (i.e. by changing all unrecognized bases to N). If you wish to include an ambiguous base in an oligo, you must set &lt;span style=&quot; font-weight:700;&quot;&gt;Max #N&apos;s accepted&lt;/span&gt; to a 1 (non-0) value.&lt;/p&gt;&lt;p&gt;Perhaps &apos;-&apos; and &apos;* &apos; should be squeezed out rather than changed to &apos;N&apos;, but currently they simply get converted to N&apos;s. The authors invite user comments.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="2992"/>
        <source>Liberal base</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3031"/>
        <source>Internal Oligo Excluded Region</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3038"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Middle oligos may not overlap any region specified by this tag. The associated value must be a space-separated list of&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;,&lt;/span&gt;&lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt;&lt;/p&gt;&lt;p&gt;pairs, where &lt;span style=&quot; font-weight:700;&quot;&gt;start&lt;/span&gt; is the index of the first base of an excluded region, and &lt;span style=&quot; font-weight:700;&quot;&gt;length&lt;/span&gt; is its length. Often one would make Target regions excluded regions for internal oligos.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3045"/>
        <source>Internal Oligo Overlap Positions</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3052"/>
        <location filename="../src/Primer3Dialog.ui" line="3834"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The 5&apos; end of the middle oligo / probe must overlap one of the junctions in &lt;span style=&quot; font-weight:700;&quot;&gt;Internal Oligo Overlap Positions&lt;/span&gt; by this amount. See details in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3070"/>
        <source>Internal Oligo Bound</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3151"/>
        <source>Internal Oligo GC%</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3212"/>
        <source>Internal Oligo Tm</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3219"/>
        <source>Internal Oligo Size</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3259"/>
        <location filename="../src/Primer3Dialog.ui" line="3365"/>
        <location filename="../src/Primer3Dialog.ui" line="3381"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of Primer Bound for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3421"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max Self Complementarity&lt;/span&gt; for the internal oligo&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3443"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Same as &lt;span style=&quot; font-weight:700;&quot;&gt;Internal Oligo Max 3&apos; End Self Complementarity&lt;/span&gt; but for calculating the score (melting temperature of structure) thermodynamical approach is used&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3472"/>
        <source>TH: Internal Oligo Max 3&apos; End Self Complementarity	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3485"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer &lt;span style=&quot; font-weight:700;&quot;&gt;Max Self Complementarity&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3507"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer &lt;span style=&quot; font-weight:700;&quot;&gt;Max 3&apos; Self Complementarity&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3523"/>
        <location filename="../src/Primer3Dialog.ui" line="5346"/>
        <source>Internal Oligo Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3530"/>
        <location filename="../src/Primer3Dialog.ui" line="5268"/>
        <source>TH: Internal Oligo Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3556"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The most stable monomer structure of internal oligo calculated by thermodynamic approach. See &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max Primer Hairpin&lt;/span&gt;	 for details&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3572"/>
        <location filename="../src/Primer3Dialog.ui" line="5320"/>
        <source>TH: Internal Oligo Hairpin</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3579"/>
        <source>Internal Oligo Max 3&apos; End Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3597"/>
        <source>Internal Oligo Max Poly-X</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3604"/>
        <source>Internal Oligo DNA Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3611"/>
        <source>Internal Oligo Formamide Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3624"/>
        <location filename="../src/Primer3Dialog.ui" line="3805"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Similar to &lt;span style=&quot; font-weight:700;&quot;&gt;Max Library Mispriming&lt;/span&gt; except that this parameter applies to the similarity of candidate internal oligos to the library specified in &lt;span style=&quot; font-weight:700;&quot;&gt;Internal Oligo Mishyb Library&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3631"/>
        <source>Internal Oligo Min Sequence Quality</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3644"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Parameter for internal oligos analogous to &lt;span style=&quot; font-weight:700;&quot;&gt;Concentration of dNTPs&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3664"/>
        <source>Internal Oligo [dNTP]</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3677"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Annealing Oligo Concentration&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3699"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Min sequence quality&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3718"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Concentration of Monovalent Cations&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3734"/>
        <source>Internal Oligo DMSO Concentration</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3747"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Concentration of Divalent Cations&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3766"/>
        <source>Internal Oligo 5 Prime Junction Overlap</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3773"/>
        <source>Internal Oligo Conc of monovalent cations</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3786"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer &lt;span style=&quot; font-weight:700;&quot;&gt;Max #Ns&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3821"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;Formamide Concentration&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3850"/>
        <source>Internal Oligo Mishyb Library</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3857"/>
        <source>Internal Oligo 3 Prime Junction Overlap</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3864"/>
        <source>Internal Oligo Max Library Mishyb	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3871"/>
        <source>Internal Oligo conc of divalent cations</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3878"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Concentration&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3897"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of primer &lt;span style=&quot; font-weight:700;&quot;&gt;Max poly-X&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3913"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;DMSO Factor&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3932"/>
        <source>Internal Oligo DMSO Factor</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3939"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The 3&apos; end of the middle oligo / probe must overlap one of the junctions in &lt;span style=&quot; font-weight:700;&quot;&gt;Internal Oligo Overlap Positions&lt;/span&gt; by this amount. See details in &lt;span style=&quot; font-weight:700;&quot;&gt;Overlap Junction List&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3955"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Analogous to &lt;span style=&quot; font-weight:700;&quot;&gt;3&apos; End Distance Between Left Primers&lt;/span&gt;, but for internal primer / probe.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3971"/>
        <source>Min Internal Oligo End Distance</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="3997"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section describes &amp;quot;penalty weights&amp;quot;, which allow the user to modify the criteria that Primer3 uses to select the &amp;quot;best&amp;quot; primers.&lt;/p&gt;&lt;p&gt;There are two classes of weights: for some parameters there is a &apos;Lt&apos; (less than) and a &apos;Gt&apos; (greater than) weight. These are the weights that Primer3 uses when the value is less or greater than (respectively) the specified optimum.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This section describes &amp;quot;penalty weights&amp;quot;, which allow the user to modify the criteria that Primer3 uses to select the &amp;quot;best&amp;quot; primers.&lt;/p&gt;&lt;p&gt;There are two classes of weights: for some parameters there is a &apos;Lt&apos; (less than) and a &apos;Gt&apos; (greater than) weight. These are the weights that Primer3 uses when the value is less or greater than (respectively) the specified optimum.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4041"/>
        <location filename="../src/Primer3Dialog.ui" line="4092"/>
        <location filename="../src/Primer3Dialog.ui" line="4108"/>
        <location filename="../src/Primer3Dialog.ui" line="4124"/>
        <location filename="../src/Primer3Dialog.ui" line="4147"/>
        <location filename="../src/Primer3Dialog.ui" line="4170"/>
        <location filename="../src/Primer3Dialog.ui" line="4180"/>
        <location filename="../src/Primer3Dialog.ui" line="4190"/>
        <location filename="../src/Primer3Dialog.ui" line="5237"/>
        <location filename="../src/Primer3Dialog.ui" line="5247"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for corressponding parameter of a primer less than (Lt) or greater than (Gt) the optimal value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для соответствующего параметра праймера меньше чем (Lt) или больше чем (Gt), что является оптимальным значением.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4200"/>
        <source>Bound</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4232"/>
        <source>TH: Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4239"/>
        <location filename="../src/Primer3Dialog.ui" line="5281"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Self Complementarity&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4252"/>
        <source>TH: 3&apos; End Self Complementarity	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4259"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for the individual primer self binding value as in &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max 3&apos; Self Complementarity&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4272"/>
        <source>TH: Hairpin</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4279"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for the individual primer hairpin structure value as in &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max Primer Hairpin&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4295"/>
        <source>TH: Template Mispriming</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4305"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty for a single primer binding to the template sequence (thermodynamic approach, when &lt;span style=&quot; font-weight:700;&quot;&gt;Use Thermodynamic Template Alignment&lt;/span&gt; is true).&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4318"/>
        <source>Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4331"/>
        <location filename="../src/Primer3Dialog.ui" line="4357"/>
        <location filename="../src/Primer3Dialog.ui" line="4383"/>
        <location filename="../src/Primer3Dialog.ui" line="4420"/>
        <location filename="../src/Primer3Dialog.ui" line="4440"/>
        <location filename="../src/Primer3Dialog.ui" line="4466"/>
        <location filename="../src/Primer3Dialog.ui" line="4492"/>
        <location filename="../src/Primer3Dialog.ui" line="4544"/>
        <location filename="../src/Primer3Dialog.ui" line="4570"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for a primer parameter different from predefined optimum value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для параметра праймера отличается от оптимального предопределенного значения.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4344"/>
        <source>3&apos; End Self Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4370"/>
        <source>Template Mispriming</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4433"/>
        <location filename="../src/Primer3Dialog.ui" line="4939"/>
        <source>Library Mispriming</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4518"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Determines the overall weight of the position penalty in calculating the penalty for a primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Позиция штрафа.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4557"/>
        <source>Primer failure rate</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4636"/>
        <location filename="../src/Primer3Dialog.ui" line="4652"/>
        <location filename="../src/Primer3Dialog.ui" line="4668"/>
        <location filename="../src/Primer3Dialog.ui" line="4691"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for corressponding parameter of a primer pair less than (Lt) or greater than (Gt) the optimal value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для соответствующего параметра пары праймеров меньше чем (Lt) или больше чем (Gt), что является оптимальным значением.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4740"/>
        <source>TH: Any Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4753"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for the binding value of the primer pair as in &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max Self Complementarity&lt;/span&gt;&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4766"/>
        <source>TH: 3&apos; End Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4779"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for the binding value of the primer pair as in &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Max 3&apos; Self Complementarity&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4795"/>
        <source>TH: Template Mispriming Weight</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4811"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty for a primer pair binding to the template sequence (thermodynamic approach, when &lt;span style=&quot; font-weight:700;&quot;&gt;Use Thermodynamic Template Alignment&lt;/span&gt; is true).&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4824"/>
        <source>Any Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4837"/>
        <location filename="../src/Primer3Dialog.ui" line="4863"/>
        <location filename="../src/Primer3Dialog.ui" line="4889"/>
        <location filename="../src/Primer3Dialog.ui" line="4926"/>
        <location filename="../src/Primer3Dialog.ui" line="4952"/>
        <location filename="../src/Primer3Dialog.ui" line="4978"/>
        <location filename="../src/Primer3Dialog.ui" line="5004"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for a primer pair parameter different from predefined optimum value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для параметра пары праймеров отличается от оптимального предопределенного значения.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4850"/>
        <source>3&apos; End Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4876"/>
        <source>Template Mispriming Weight</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4913"/>
        <source>Tm Difference</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4965"/>
        <source>Primer Penalty Weight</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="4991"/>
        <source>Internal Oligo Penalty Weight	</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5048"/>
        <source>Internal Oligos</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5057"/>
        <source>Internal oligo Size</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5084"/>
        <location filename="../src/Primer3Dialog.ui" line="5100"/>
        <location filename="../src/Primer3Dialog.ui" line="5116"/>
        <location filename="../src/Primer3Dialog.ui" line="5139"/>
        <location filename="../src/Primer3Dialog.ui" line="5169"/>
        <location filename="../src/Primer3Dialog.ui" line="5192"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for corressponding parameter of a internal oligo less than (Lt) or greater than (Gt) the optimal value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для соответствующего параметра внутреннего олиго меньше чем (Lt) или больше чем (Gt), что является оптимальным значением.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5179"/>
        <source>Internal oligo Tm</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5202"/>
        <source>Internal oligo GC%</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5216"/>
        <source>Internal oligo Bound</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5294"/>
        <source>TH: Internal Oligo 3&apos; End Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5307"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Equivalent parameter of &lt;span style=&quot; font-weight:700;&quot;&gt;TH: 3&apos; End Self Complementarity&lt;/span&gt; for the internal oligo.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5333"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for the most stable primer hairpin structure value as in &lt;span style=&quot; font-weight:700;&quot;&gt;TH: Internal Oligo Hairpin&lt;/span&gt;.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5359"/>
        <location filename="../src/Primer3Dialog.ui" line="5385"/>
        <location filename="../src/Primer3Dialog.ui" line="5422"/>
        <location filename="../src/Primer3Dialog.ui" line="5448"/>
        <location filename="../src/Primer3Dialog.ui" line="5474"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Penalty weight for an internal oligo parameter different from predefined optimum value.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Вес штрафа для параметра внутреннего олиго отличается от оптимального предопределенного значения.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5372"/>
        <source>Internal Oligo 3&apos; End Complementarity</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5409"/>
        <source>Internal Oligo #N&apos;s</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5435"/>
        <source>Internal Oligo Library Mishybing</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5461"/>
        <source>Internal Oligo Sequence Quality</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5487"/>
        <source>Internal Oligo Sequence End Quality</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5656"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;In this section user can specify if primer product must overlap exon-exon junction or span intron. This only applies when designing primers for a cDNA (mRNA) sequence with annotated exons.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;In this section user can specify if primer product must overlap exon-exon junction or span intron. This only applies when designing primers for a cDNA (mRNA) sequence with annotated exons.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5662"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This feature allows to specify if primer product must span intron-exon boundaries.&lt;/p&gt;&lt;p&gt;Checking this option will result in ignoring &lt;span style=&quot; font-style:italic;&quot;&gt;Excluded&lt;/span&gt; and &lt;span style=&quot; font-style:italic;&quot;&gt;Target Regions&lt;/span&gt; from Main section.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Эта опция позволяет указать если праймер должен охватывать интрон-экзон границы.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5692"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The name of the annotation which defines the exons in the mRNA sequence.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Имя экзон аннотации.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5797"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the range is set, primer search will be restricted to selected exons. For example:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;1-5&lt;/span&gt;&lt;/p&gt;&lt;p&gt;If the range is larger than actual exon range or the starting exon number exceeds number of exons, error message is shown. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;If the range is set, primer search will be restricted to selected exons. For example:&lt;/p&gt;&lt;p&gt;&lt;span style=&quot; font-weight:600;&quot;&gt;1-5&lt;/span&gt;&lt;/p&gt;&lt;p&gt;If the range is larger than actual exon range or the starting exon number exceeds number of exons, error message is shown. &lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5838"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;This number of found primer pairs wil bel queried to check if they fullfill the requirements for RTPCR.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимальное число пар для запроса.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5536"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;In this section user can specify sequence quality of target sequence and related parameters.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;In this section user can specify sequence quality of target sequence and related parameters.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5549"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;A list of space separated integers. There must be exactly one integer for each base in the Source Sequence if this argument is non-empty. High numbers indicate high confidence in the base call at that position and low numbers indicate low confidence in the base call at that position.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Список разделенных пробелами целых чисел.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5571"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum sequence quality allowed within a primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальное качество последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5594"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum sequence quality allowed within the 3&apos; pentamer of a primer.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимальное 3&amp;apos;качество последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5617"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The minimum legal sequence quality.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Минимум интервала качества последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5640"/>
        <source>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;The maximum legal sequence quality.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</source>
        <translation>&lt;html&gt;&lt;head/&gt;&lt;body&gt;&lt;p&gt;Максимум интервала качества последовательности.&lt;/p&gt;&lt;/body&gt;&lt;/html&gt;</translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.ui" line="5893"/>
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
        <translation type="unfinished"></translation>
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
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="540"/>
        <source>Start Codon Sequence</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="551"/>
        <source>Five Matches on Primer&apos;s 5&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="562"/>
        <source>Five Matches on Primer&apos;s 3&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="573"/>
        <source>Five Matches on Internal Oligo&apos;s  5&apos;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="../src/Primer3Dialog.cpp" line="584"/>
        <source>Five Matches on Internal Oligo&apos;s 3&apos;</source>
        <translation type="unfinished"></translation>
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
        <source>The first exon from the selected range [%1,%2] is larger the number of exons (%2). Please set correct exon range.</source>
        <translation>The first exon from the selected range [%1,%2] is larger the number of exons (%2). Please set correct exon range.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Task.cpp" line="695"/>
        <source>The the selected exon range [%1,%2] is larger the number of exons (%2). Please set correct exon range.</source>
        <translation>The the selected exon range [%1,%2] is larger the number of exons (%2). Please set correct exon range.</translation>
    </message>
    <message>
        <location filename="../src/Primer3Task.cpp" line="792"/>
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
