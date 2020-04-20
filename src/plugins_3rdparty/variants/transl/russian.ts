<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru" sourcelanguage="en">
<context>
    <name>U2::LocalWorkflow::CallVariantsPrompter</name>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="691"/>
        <source>unset</source>
        <translation>не указан</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="699"/>
        <source>For reference sequence from &lt;u&gt;%1&lt;/u&gt;,</source>
        <translation>Для референсной последовательности из &lt;u&gt;%1&lt;/u&gt;,</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="702"/>
        <source>with assembly data provided by &lt;u&gt;%1&lt;/u&gt;</source>
        <translation>с данными сборки, произведенными &lt;u&gt;%1&lt;/u&gt;</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="704"/>
        <source>%1 call variants %2.</source>
        <translation>%1 вызов вариантов %2.</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::CallVariantsTask</name>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="53"/>
        <source>Call variants for %1</source>
        <translation>Вызов вариантов для %1</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="66"/>
        <source>reference</source>
        <translation>референс</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="68"/>
        <source>assembly</source>
        <translation>сборка</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="76"/>
        <source>The %1 file does not exist: %2</source>
        <translation>The %1 file does not exist: %2</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="89"/>
        <source>No assembly files</source>
        <translation>No assembly files</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="94"/>
        <source>No dbi storage</source>
        <translation>No dbi storage</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="98"/>
        <source>No sequence URL</source>
        <translation>No sequence URL</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="136"/>
        <source>No document loaded</source>
        <translation>No document loaded</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="140"/>
        <source>Incorrect variant track object in %1</source>
        <translation>Incorrect variant track object in %1</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::CallVariantsWorker</name>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="135"/>
        <source>Empty input slot: %1</source>
        <translation>Входной слот пуст: %1</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="155"/>
        <source>Input sequences</source>
        <translation>Входные последовательности</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="156"/>
        <source>A nucleotide reference sequence.</source>
        <translation>Нуклеотидная референсная последовательность.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="163"/>
        <source>Input assembly</source>
        <translation>Входная сборка</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="164"/>
        <source>Position sorted alignment file</source>
        <translation>Сортированный файл выравнивания</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="171"/>
        <source>Output variations</source>
        <translation>Выходные вариации</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="172"/>
        <source>Output tracks with SNPs and short INDELs</source>
        <translation>Выходные треки с SNP и  короткими INDEL</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="177"/>
        <source>Call Variants with SAMtools</source>
        <translation>Поиск вариабельных позиций с помощью SAMtools</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="178"/>
        <source>Calls SNPs and INDELS with SAMtools mpileup and bcftools.</source>
        <translation>этот элемент ищет однонуклеотидные вариации (SNP) и короткие вставки или выпадения (indels) при помощи SAMtools mpileup и bcftools.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="186"/>
        <source>Output variants file</source>
        <translation>Выходной файл</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="187"/>
        <source>The url to the file with the extracted variations.</source>
        <translation>Путь до файла с извлеченными вариациями.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="190"/>
        <source>Use reference from</source>
        <translation>Использовать референс из</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="191"/>
        <source>&lt;p&gt;Specify &quot;File&quot; to set a single reference sequence for all input NGS assemblies. The reference should be set in the &quot;Reference&quot; parameter.&lt;/p&gt;&lt;p&gt;Specify &quot;Input port&quot; to be able to set different references for difference NGS assemblies. The references should be input via the &quot;Input sequences&quot; port (e.g. use datasets in the &quot;Read Sequence&quot; element).&lt;/p&gt;</source>
        <translation>&lt;p&gt;Укажите &quot;Файл&quot; с референсной последовательностью для всех входных NGS сборок. Референсная последовательность будет установлена в параметр &quot;Референс&quot;.&lt;/p&gt;&lt;p&gt;Укажите &quot;Входной порт&quot; чтобы иметь возможность указать разные референсные последовательности для разных NGS сборок. Референсные последовательности будут передаваться через входной порт &quot;Входные последовательности&quot; (датасет в элементе &quot;Чтение последовательности&quot;).&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="197"/>
        <source>Reference</source>
        <translation>Референс</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="198"/>
        <source>&lt;p&gt;Specify a file with the reference sequence.&lt;/p&gt;&lt;p&gt;The sequence will be used as reference for all datasets with NGS assemblies.&lt;/p&gt;</source>
        <translation>&lt;p&gt;Укажите файл с референсной последовательностью.&lt;/p&gt;&lt;p&gt;Последовательность будет использована в качестве референса для всех входных NGS сборок.&lt;/p&gt;</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="202"/>
        <source>Illumina-1.3+ encoding</source>
        <translation>Кодирование Illumina-1.3+</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="203"/>
        <source>Assume the quality is in the Illumina 1.3+ encoding (mpileup)(-6).</source>
        <translation>значения качества в кодировке Illumina 1.3+. Соответствует опции mpileup -6.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="206"/>
        <source>Count anomalous read pairs</source>
        <translation>Учитывать аномальные пары прочтений</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="207"/>
        <source>Do not skip anomalous read pairs in variant calling(mpileup)(-A).</source>
        <translation>не пропускать аномальные пары прочтений при поиске вариаций. Соответствует опции mpileup -A.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="210"/>
        <source>Disable BAQ computation</source>
        <translation>Отключить расчет BAQ</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="211"/>
        <source>Disable probabilistic realignment for the computation of base alignment quality (BAQ). BAQ is the Phred-scaled probability of a read base being misaligned. Applying this option greatly helps to reduce false SNPs caused by misalignments. (mpileup)(-B).</source>
        <translation>не выполнять выравнивание для расчета качества выравнивания каждого нуклеотида (base alignment quality, BAQ). BAQ - это приведенная к шкале Phred вероятность того, что данный нуклеотид выровнен неверно. Расчет BAQ сильно снижает вероятность ложного нахождения SNP из-за неверного выравнивания. Соответствует опции mpileup -B.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="216"/>
        <source>Mapping quality downgrading coefficient</source>
        <translation>Коэффициент снижения качества выравнивания</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="217"/>
        <source>Coefficient for downgrading mapping quality for reads containing excessive mismatches. Given a read with a phred-scaled mapping quality q of being generated from the mapped position, the new mapping quality is about sqrt((INT-q)/INT)*INT. A zero value disables this functionality; if enabled, the recommended value for BWA is 50 (mpileup)(-C).</source>
        <translation>применить коэффициент, отражающий снижение качества выравнивания прочтений, содержащих много замен. Если прочтение обладает качеством выравнивания q, новое значение качества выравнивания составит около sqrt[(INT-q)/INT]*INT. Значение 0 отключает эту опцию. Рекомендуемым значением для BWA является 50. Cjjndtncndetn jgwbb ьзшдугз -C INT.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="222"/>
        <source>Max number of reads per input BAM</source>
        <translation>Максимальное число прочтений для входного BAM-файла</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="223"/>
        <source>At a position, read maximally the number of reads per input BAM (mpileup)(-d).</source>
        <translation>максимальное число прочтений, которое следует прочитать для каждой позиции из входного BAM-файла. Соответствует опции mpileup -d.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="226"/>
        <source>Extended BAQ computation</source>
        <translation>Расширенный расчет BAQ</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="227"/>
        <source>Extended BAQ computation. This option helps sensitivity especially for MNPs, but may hurt specificity a little bit (mpileup)(-E).</source>
        <translation>Расширенные расчет BAQ. Эта опция увеличивает чувствительность, особенно в случае множественных замен (MNPs), однако может снижать специфичность.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="231"/>
        <source>BED or position list file</source>
        <translation>BED-файл или файл со списком позиций</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="232"/>
        <source>BED or position list file containing a list of regions or sites where pileup or BCF should be generated (mpileup)(-l).</source>
        <translation>BED-файл или файл со списком позиций или участков, для которых необходимо сгенерировать pileup или BCF. Соответствует опции mpileup -l.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="236"/>
        <source>Pileup region</source>
        <translation>Pileup для участка</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="237"/>
        <source>Only generate pileup in region STR (mpileup)(-r).</source>
        <translation>генерировать pileup только для участка STR. Соответствует опции mpileup -r STR.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="240"/>
        <source>Minimum mapping quality</source>
        <translation>Минимальное качество выравнивания</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="241"/>
        <source>Minimum mapping quality for an alignment to be used (mpileup)(-q).</source>
        <translation>использовать выравнивания только с таким или более хорошим качеством. Соответствует опции mpileup -q.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="244"/>
        <source>Minimum base quality</source>
        <translation>Минимальное качество нуклеотида</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="245"/>
        <source>Minimum base quality for a base to be considered (mpileup)(-Q).</source>
        <translation>учитывать нуклеотиды только с таким или более хорошим качеством. Соответствует опции mpileup -Q.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="248"/>
        <source>Gap extension error</source>
        <translation>Ошибка расширения пробела</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="249"/>
        <source>Phred-scaled gap extension sequencing error probability. Reducing INT leads to longer indels (mpileup)(-e).</source>
        <translation>Вероятность ошибки расширения пробела. (-e).</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="252"/>
        <source>Homopolymer errors coefficient</source>
        <translation>Коэффициент ошибок в гомополимерных участках</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="253"/>
        <source>Coefficient for modeling homopolymer errors. Given an l-long homopolymer run, the sequencing error of an indel of size s is modeled as INT*s/l (mpileup)(-h).</source>
        <translation>Коэффициент для моделирования ошибок в участках гомополимеров. Для гомополимера длиной l ошибка секвенирования для вставки/выпадения длиной s рассчитывается как INT*s/l. Соответствует опции mpileup -h.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="256"/>
        <source>No INDELs</source>
        <translation>Не искать INDELs</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="257"/>
        <source>Do not perform INDEL calling (mpileup)(-I).</source>
        <translation>не выполнять поиск вставок/выпадений. Соответствует опции mpileup -l.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="260"/>
        <source>Max INDEL depth</source>
        <translation>Максимальное покрытие для поиска INDELs</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="261"/>
        <source>Skip INDEL calling if the average per-sample depth is above INT (mpileup)(-L).</source>
        <translation>не выполнять поиск вставок/выпадений если среднее покрытие образца превышает INT. Соответствует опции mpileup -L INT.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="264"/>
        <source>Gap open error</source>
        <translation>Ошибка открытия пробела</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="265"/>
        <source>Phred-scaled gap open sequencing error probability. Reducing INT leads to more indel calls (mpileup)(-o).</source>
        <translation>вероятность ошибки секвенирования, приводящей к открытию пробела, в шкале Phred. Снижение значения INT приводит к увеличению числа найденных вставок/выпадений. Соответствует опции mpileup -o INT.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="268"/>
        <source>List of platforms for indels</source>
        <translation>Список платформ для поиска вставок/выпадений</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="269"/>
        <source>Comma dilimited list of platforms (determined by @RG-PL) from which indel candidates are obtained.It is recommended to collect indel candidates from sequencing technologies that have low indel error rate such as ILLUMINA (mpileup)(-P).</source>
        <translation>список через запятую названий платформ в формате @RG-PL, для которых необходимо проводить поиск возможных вставок/выпадений. Этот анализ рекомендуется проводить для данных платформ секвенирования, обладающих низким уровнем ошибок типа вставка/выпадение (например, ILLUMINA). Соответствует опции mpileup -P.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="274"/>
        <source>Retain all possible alternate</source>
        <translation>Сохранять все альтернативные варианты</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="275"/>
        <source>Retain all possible alternate alleles at variant sites. By default, the view command discards unlikely alleles (bcf view)(-A).</source>
        <translation>сохранять все аллельные варианты в вариабельных позициях. По умолчанию команда view отбрасывает аллели с низкой вероятностью. Соответствует опции bcftools view -A.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="278"/>
        <source>Indicate PL</source>
        <translation>Указать PL</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="279"/>
        <source>Indicate PL is generated by r921 or before (ordering is different) (bcf view)(-F).</source>
        <translation>указать, получены ли данные с помощью r921 или более ранней версии платформы (отличаются сортировкой). Соответствует опции bcftools view -F.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="282"/>
        <source>No genotype information</source>
        <translation>Не отображать информацию о генотипе</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="283"/>
        <source>Suppress all individual genotype information (bcf view)(-G).</source>
        <translation>отключить отображение всей информации о генотипе индивидуума. Соответствует опции bcftools view -G.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="286"/>
        <source>A/C/G/T only</source>
        <translation>Только A/C/G/T</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="287"/>
        <source>Skip sites where the REF field is not A/C/G/T (bcf view)(-N).</source>
        <translation>отбрасывать сайты, где поле REF содержит значение не из списка A/C/G/T. Соответствует опции bcftools view -N.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="290"/>
        <source>List of sites</source>
        <translation>Список позиций</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="291"/>
        <source>List of sites at which information are outputted (bcf view)(-l).</source>
        <translation>список позиций, для которых необходимо вывести информацию. Соответствует опции bcftools view -l STR.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="294"/>
        <source>QCALL likelihood</source>
        <translation>Вероятность в QCALL</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="295"/>
        <source>Output the QCALL likelihood format (bcf view)(-Q).</source>
        <translation>выводить значения вероятности в формате QCALL. Соответствует опции bcftools view -Q.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="298"/>
        <source>List of samples</source>
        <translation>Список образцов</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="299"/>
        <source>List of samples to use. The first column in the input gives the sample names and the second gives the ploidy, which can only be 1 or 2. When the 2nd column is absent, the sample ploidy is assumed to be 2. In the output, the ordering of samples will be identical to the one in FILE (bcf view)(-s).</source>
        <translation>список образцов, которые необходимо использовать. В первом столбце необходимо перечислить названия образцов, во втором - плоидность. Если второй столбец отсутствует, плоидность считается равной 2. Порядок образцов в выходном файле будет соответствовать порядку во входном файле. Соответствует опции bcftools view -s.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="305"/>
        <source>Min samples fraction</source>
        <translation>Минимальная доля образцов</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="306"/>
        <source>skip loci where the fraction of samples covered by reads is below FLOAT (bcf view)(-d).</source>
        <translation>пропустить участок если доля образцов, у которых этот участок покрыт прочтениями, ниже указанного значения. Соответствует опции bcftools view -d FLOAT.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="309"/>
        <source>Per-sample genotypes</source>
        <translation>Генотип для каждого образца</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="310"/>
        <source>Call per-sample genotypes at variant sites (bcf view)(-g).</source>
        <translation>отобразить генотип для каждого образца и каждой вариабельной позиции. Соответствует опции bcftools view -g.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="313"/>
        <source>INDEL-to-SNP Ratio</source>
        <translation>Соотношение INDEL-to-SNP</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="314"/>
        <source>Ratio of INDEL-to-SNP mutation rate (bcf view)(-i).</source>
        <translation>указать отношение скоростей появления вставок/выпадений и замен. Соответствует опции bcftools view -i FLOAT.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="317"/>
        <source>Max P(ref|D)</source>
        <translation>Максимальное P(ref|D)</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="318"/>
        <source>A site is considered to be a variant if P(ref|D)&lt;FLOAT (bcf view)(-p).</source>
        <translation>считать позицию вариабельной, если вероятность совпадения с референсом (P(ref|D)) меньше указанного значения. Соответствует опции bcftools view -p FLOAT.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="321"/>
        <source>Prior allele frequency spectrum</source>
        <translation>Априорное распределение частот аллелей</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="322"/>
        <source>If STR can be full, cond2, flat or the file consisting of error output from a previous variant calling run (bcf view)(-P).</source>
        <translation>значение из следующего списка: full, cond2, flat - или файл, содержащий список ошибок в предыдущем запуске. Соответствует опции bcftools view -P STR.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="325"/>
        <source>Mutation rate</source>
        <translation>Скорость накоплений мутаций</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="326"/>
        <source>Scaled mutation rate for variant calling (bcf view)(-t).</source>
        <translation>нормализованная скорость накопления мутаций для поиска вариаций. Соответствует опции bcftools view -t.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="329"/>
        <source>Pair/trio calling</source>
        <translation>Поиск вариаций в парных образцах или трио</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="330"/>
        <source>Enable pair/trio calling. For trio calling, option -s is usually needed to be applied to configure the trio members and their ordering. In the file supplied to the option -s, the first sample must be the child, the second the father and the third the mother. The valid values of STR are &apos;pair&apos;, &apos;trioauto&apos;, &apos;trioxd&apos; and &apos;trioxs&apos;, where &apos;pair&apos; calls differences between two input samples, and &apos;trioxd&apos; (&apos;trioxs&apos;)specifies that the input is from the X chromosome non-PAR regions and the child is a female (male) (bcf view)(-T).</source>
        <translation>включить поиск вариаций в парах или трио образцов. Для работы с трио необходимо также применить опцию -s для настройки входящих в трио образцов и порядка их сортировки. В файле, переданном опции -s, первому образцу должны соответствовать данные для ребенка, второму - для отца, третьему - для матери. Корректными значениями этого параметра являются &apos;pair&apos;, &apos;trioauto&apos;, &apos;trioxd&apos; и &apos;trioxs&apos;, где &apos;pair&apos; позволяет найти различия между двумя образцами, а &apos;trioxd&apos; или &apos;trioxs&apos; указывает на то, что на вход поданы данные для X - хромосомы за вычетом псевдоаутосомного участка, а ребенок женского (мужского) пола. Соответствует опции bcftools view -T.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="337"/>
        <source>N group-1 samples</source>
        <translation>Число образцов в группе 1</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="338"/>
        <source>Number of group-1 samples. This option is used for dividing the samples into two groups for contrast SNP calling or association test. When this option is in use, the followingVCF INFO will be outputted: PC2, PCHI2 and QCHI2 (bcf view)(-1).</source>
        <translation>число образцов в первой группе. Эта опция используется для разделения образцов на две группы для поиска различающихся SNP или проведения анализа ассоциаций. Если эта опция включена, в выходном файле будут добавлены следующие значения VCF INFO: PC2, PCHI2 и QCHI2. Соответствует опции bcftools view -1.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="343"/>
        <source>N permutations</source>
        <translation>Число перестановок</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="344"/>
        <source>Number of permutations for association test (effective only with -1) (bcf view)(-U).</source>
        <translation>число перестановок для анализа ассоциаций. Работает только в том случае, если включена опция -1. Соответствует опции bcftools view -U.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="347"/>
        <source>Max P(chi^2)</source>
        <translation>Максимальное P(chi^2)</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="348"/>
        <source>Only perform permutations for P(chi^2)&lt;FLOAT (N permutations) (bcf view)(-X).</source>
        <translation>выполнять перестановки только для меньших  значений P(chi^2), чем указанное значение. Соответствует опции bcftools view -X FLOAT.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="352"/>
        <source>Minimum RMS quality</source>
        <translation>Минимальное качество RMS</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="353"/>
        <source>Minimum RMS mapping quality for SNPs (varFilter) (-Q).</source>
        <translation>минимальное качество выравнивания RMS для SNP. Соответствует опции varFilter -Q.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="355"/>
        <source>Minimum read depth</source>
        <translation>Минимальная глубина покрытия</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="356"/>
        <source>Minimum read depth (varFilter) (-d).</source>
        <translation>Минимальная глубина покрытия для прочтения. Соответствует опции varFilter -d.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="358"/>
        <source>Maximum read depth</source>
        <translation>Максимальная глубина покрытия</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="359"/>
        <source>Maximum read depth (varFilter) (-D).</source>
        <translation>Максимальная глубина покрытия для прочтения. Соответствует опции varFilter -d.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="361"/>
        <source>Alternate bases</source>
        <translation>Альтернативные нуклеотиды</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="362"/>
        <source>Minimum number of alternate bases (varFilter) (-a).</source>
        <translation>минимальное число прочтений, содержащих в этой позиции не соответствующий референсному вариант. Соответствует опции varFilter -a.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="364"/>
        <source>Gap size</source>
        <translation>Длина пробела</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="365"/>
        <source>SNP within INT bp around a gap to be filtered (varFilter) (-w).</source>
        <translation>длина участка, включающего вставку/выпадение, на котором следует отбросить SNP. Соответствует опции varFilter -w.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="367"/>
        <source>Window size</source>
        <translation>Размер окна</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="368"/>
        <source>Window size for filtering adjacent gaps (varFilter) (-W).</source>
        <translation>Размер окна для фильтрации смежных пробелов. Соответствует опции varFilter -W.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="370"/>
        <source>Strand bias</source>
        <translation>Смещение по разным цепям</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="371"/>
        <source>Minimum P-value for strand bias (given PV4) (varFilter) (-1).</source>
        <translation>минимальное P-значение для добавления информации о неравномерном распределении вариаций по двум цепям. Соответствует опции varFilter -1.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="373"/>
        <source>BaseQ bias</source>
        <translation>Смещение BaseQ</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="374"/>
        <source>Minimum P-value for baseQ bias (varFilter) (-2).</source>
        <translation>минимальное P-значение для добавления информации о неравномерном распределении качества нуклеотида (baseQ). Соответствует опции varFilter -2.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="376"/>
        <source>MapQ bias</source>
        <translation>Смещение MapQ</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="377"/>
        <source>Minimum P-value for mapQ bias (varFilter) (-3).</source>
        <translation>минимальное P-значение для добавления информации о неравномерном распределении качества выравнивания (mapQ). Соответствует опции varFilter -3.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="379"/>
        <source>End distance bias</source>
        <translation>Смещение расстояния до конца прочтения</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="380"/>
        <source>Minimum P-value for end distance bias (varFilter) (-4).</source>
        <translation>минимальное P-значение для добавления информации о неравномерном распределении расстояния от вариабельного сайта до конца прочтения. Соответствует опции varFilter -4.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="382"/>
        <source>HWE</source>
        <translation>HWE</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="383"/>
        <source>Minimum P-value for HWE (plus F&lt;0) (varFilter) (-e).</source>
        <translation>минимальное P-значение для добавления информации о распределении Харди-Вайнберга (HWE). Соответствует опции varFilter -e.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="385"/>
        <source>Log filtered</source>
        <translation>Лог фильтрации</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="386"/>
        <source>Print filtered variants into the log (varFilter) (-p).</source>
        <translation>записывать отфильтрованные вариации в лог-файл. Соответствует опции varFilter -p.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="658"/>
        <source>Input port</source>
        <translation>Входной порт</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="659"/>
        <source>File</source>
        <translation>Файл</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="843"/>
        <source>Assembly URL slot is empty. Please, specify the URL slot</source>
        <translation>Assembly URL slot is empty. Please, specify the URL slot</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="868"/>
        <source>Ref sequence URL slot is empty. Please, specify the URL slot</source>
        <translation>Ref sequence URL slot is empty. Please, specify the URL slot</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="947"/>
        <source>Not enough references</source>
        <translation>Not enough references</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="952"/>
        <source>The dataset slot is not binded, only the first reference sequence against all assemblies was processed.</source>
        <translation>The dataset slot is not binded, only the first reference sequence against all assemblies was processed.</translation>
    </message>
    <message>
        <location filename="../src/SamtoolMpileupWorker.cpp" line="955"/>
        <source>Not enough assemblies</source>
        <translation>Not enough assemblies</translation>
    </message>
</context>
<context>
    <name>U2::LocalWorkflow::SamtoolsMpileupTask</name>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="167"/>
        <source>Samtool mpileup for %1 </source>
        <translation>Samtool mpileup для %1 </translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="174"/>
        <source>No reference sequence URL to do pileup</source>
        <translation>No reference sequence URL to do pileup</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="179"/>
        <source>No assembly URL to do pileup</source>
        <translation>No assembly URL to do pileup</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="185"/>
        <source>There is an assembly with an empty path</source>
        <translation>There is an assembly with an empty path</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="194"/>
        <source>Can not create the folder: </source>
        <translation>Can not create the folder: </translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="249"/>
        <source>Can not run %1 tool</source>
        <translation>Can not run %1 tool</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="255"/>
        <source>%1 tool exited with code %2</source>
        <translation>%1 tool exited with code %2</translation>
    </message>
    <message>
        <location filename="../src/AssemblySamtoolsMpileup.cpp" line="257"/>
        <source>Tool %1 finished successfully</source>
        <translation>Tool %1 finished successfully</translation>
    </message>
</context>
<context>
    <name>U2::SamtoolsPlugin</name>
    <message>
        <location filename="../src/SamtoolsPlugin.cpp" line="36"/>
        <source>Samtools plugin</source>
        <translation>Samtools plugin</translation>
    </message>
    <message>
        <location filename="../src/SamtoolsPlugin.cpp" line="36"/>
        <source>Samtools plugin for NGS data analysis</source>
        <translation>Samtools плагин для анализа данных NGS</translation>
    </message>
</context>
</TS>
