<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="tr">
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/CredentialsAskerCli.cpp" line="126"/>
        <source>Enter password: </source>
        <translation>Parolayı girin: </translation>
    </message>
    <message>
        <location filename="../src/CredentialsAskerCli.cpp" line="177"/>
        <source>Enter user name: </source>
        <translation>Kullanıcı adını girin: </translation>
    </message>
    <message>
        <location filename="../src/CredentialsAskerCli.cpp" line="159"/>
        <location filename="../src/CredentialsAskerCli.cpp" line="184"/>
        <source>Would you like UGENE to remember the password?</source>
        <translation>UGENE&apos;nin şifreyi hatırlamasını ister misiniz?</translation>
    </message>
    <message>
        <location filename="../src/CredentialsAskerCli.cpp" line="155"/>
        <location filename="../src/CredentialsAskerCli.cpp" line="171"/>
        <source>Connect to the &apos;%1&apos; ...
</source>
        <translation>&apos;%1&apos;e bağlanın ...
</translation>
    </message>
    <message>
        <location filename="../src/CredentialsAskerCli.cpp" line="156"/>
        <location filename="../src/CredentialsAskerCli.cpp" line="172"/>
        <source>You are going to log in as &apos;%1&apos;.
</source>
        <translation>&apos;%1&apos; olarak giriş yapacaksınız.
</translation>
    </message>
    <message>
        <location filename="../src/CredentialsAskerCli.cpp" line="173"/>
        <source>Would you like to log in as another user?</source>
        <translation>Başka bir kullanıcı olarak giriş yapmak ister misiniz?</translation>
    </message>
</context>
<context>
    <name>U2::AddPluginTask</name>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="336"/>
        <source>Add plugin task: %1</source>
        <translation>Eklenti görevi ekle: %1</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="350"/>
        <source>Plugin is already loaded: %1</source>
        <translation>Eklenti zaten yüklendi: %1</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="358"/>
        <source>Plugin %1 depends on %2 which is not loaded</source>
        <translation>Eklenti %1, yüklenmemiş %2&apos;ye bağlıdır</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="362"/>
        <source>Plugin %1 depends on %2 which is available, but the version is too old</source>
        <translation>Eklenti %1, mevcut olan %2&apos;ye bağlıdır, ancak sürüm çok eski</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="373"/>
        <source>Plugin loading error: %1, Error string %2</source>
        <translation>Eklenti yükleme hatası: %1, Hata dizesi %2</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="379"/>
        <source>Settings is NULL</source>
        <translation>Ayarlar BOŞ</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="434"/>
        <source>Plugin loading error: %1. Verification failed.</source>
        <translation>Eklenti yükleme hatası: %1. Doğrulama başarısız oldu.</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="454"/>
        <source>Plugin initialization routine was not found: %1</source>
        <translation>Eklenti başlatma rutini bulunamadı: %1</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="460"/>
        <source>Plugin initialization failed: %1</source>
        <translation>Eklenti başlatılamadı: %1</translation>
    </message>
</context>
<context>
    <name>U2::ConsoleLogDriver</name>
    <message>
        <location filename="../src/ConsoleLogDriver.cpp" line="81"/>
        <source>Specifies the format of a log line.</source>
        <translation>Günlük satırının formatını belirtir.</translation>
    </message>
    <message>
        <location filename="../src/ConsoleLogDriver.cpp" line="82"/>
        <source>Specifies the format of a log line.

Use the following notations: L - level, C - category, YYYY or YY - year, MM - month, dd - day, hh - hour, mm - minutes, ss - seconds, zzz - milliseconds.

By default, logformat=&quot;[L][hh:mm]&quot;.</source>
        <translation>Günlük satırının formatını belirtir.

Aşağıdaki gösterimleri kullanın: L - düzey, C - kategorisi, YYYY veya YY - yıl, AA - ay, gg - gün, ss - saat, mm - dakika, ss - saniye, zzz - milisaniye.

Varsayılan olarak, logformat = &quot;[L] [hh: mm]&quot;.</translation>
    </message>
    <message>
        <location filename="../src/ConsoleLogDriver.cpp" line="87"/>
        <source>&quot;&lt;format_string&gt;&quot;</source>
        <translation>&quot;&lt;biçim_dizesi&gt;&quot;</translation>
    </message>
    <message>
        <location filename="../src/ConsoleLogDriver.cpp" line="91"/>
        <source>Sets the log level.</source>
        <translation>Günlük seviyesini ayarlar.</translation>
    </message>
    <message>
        <location filename="../src/ConsoleLogDriver.cpp" line="92"/>
        <source>Sets the log level per category. If a category is not specified, the log level is applied to all categories.

The following categories are available:
&quot;Algorithms&quot;, &quot;Console&quot;, &quot;Core Services&quot;, &quot;Input/Output&quot;, &quot;Performance&quot;, &quot;Remote Service&quot;, &quot;Scripts&quot;, &quot;Tasks&quot;.

The following log levels are available: TRACE, DETAILS, INFO, ERROR or NONE.

By default, loglevel=&quot;ERROR&quot;.</source>
        <translation>Kategori başına günlük düzeyini ayarlar. Bir kategori belirtilmezse, günlük düzeyi tüm kategorilere uygulanır.

Aşağıdaki kategoriler mevcuttur:
&quot;Algoritmalar&quot;, &quot;Konsol&quot;, &quot;Temel Hizmetler&quot;, &quot;Giriş / Çıkış&quot;, &quot;Performans&quot;, &quot;Uzaktan Hizmet&quot;, &quot;Komut Dosyaları&quot;, &quot;Görevler&quot;.

Aşağıdaki günlük seviyeleri mevcuttur: İZLEME, DETAYLAR, BİLGİ, HATA veya HİÇBİRİ.

Varsayılan olarak, loglevel = &quot;HATA&quot;.</translation>
    </message>
    <message>
        <location filename="../src/ConsoleLogDriver.cpp" line="99"/>
        <source>&quot;&lt;category1&gt;=&lt;level1&gt; [&lt;category2&gt;=&lt;level2&gt; ...]&quot; | &lt;level&gt;</source>
        <translation>&quot;&lt;category1&gt;=&lt;level1&gt; [&lt;category2&gt;=&lt;level2&gt; ...]&quot; | &lt;level&gt;</translation>
    </message>
    <message>
        <location filename="../src/ConsoleLogDriver.cpp" line="102"/>
        <source>Enables colored output.</source>
        <translation>Renkli çıktıyı etkinleştirir.</translation>
    </message>
</context>
<context>
    <name>U2::DisableServiceTask</name>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="275"/>
        <source>Disable &apos;%1&apos; service</source>
        <translation>&apos;%1&apos; hizmetini devre dışı bırakın</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="288"/>
        <source>Service is not registered</source>
        <translation>Hizmet kayıtlı değil</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="289"/>
        <source>Service is not enabled</source>
        <translation>Hizmet etkinleştirilmedi</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="295"/>
        <source>Active top-level task name: %1</source>
        <translation>Etkin üst düzey görev adı: %1</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="297"/>
        <source>Active task was found</source>
        <translation>Aktif görev bulundu</translation>
    </message>
</context>
<context>
    <name>U2::DocumentFormatRegistryImpl</name>
    <message>
        <location filename="../src/DocumentFormatRegistryImpl.cpp" line="235"/>
        <source>UGENE Database</source>
        <translation>UGENE Veritabanı</translation>
    </message>
</context>
<context>
    <name>U2::EnableServiceTask</name>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="214"/>
        <source>Service is enabled</source>
        <translation>Hizmet etkinleştirildi</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="215"/>
        <source>Service is not registered</source>
        <translation>Hizmet kayıtlı değil</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="220"/>
        <source>Circular service dependency: %1</source>
        <translation>Döngüsel hizmet bağımlılığı: %1</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="226"/>
        <source>Required services are not enabled: %1</source>
        <translation>Gerekli hizmetler etkinleştirilmedi: %1</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="174"/>
        <source>Enable &apos;%1&apos; service</source>
        <translation>&apos;%1&apos; hizmetini etkinleştir</translation>
    </message>
    <message>
        <source>Required service is not enabled: %1</source>
        <translation>Gerekli hizmet etkinleştirilmedi: %1</translation>
    </message>
</context>
<context>
    <name>U2::LoadAllPluginsTask</name>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="78"/>
        <source>Loading start up plugins</source>
        <translation>Başlangıç eklentileri yükleniyor</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="108"/>
        <source>File not found: %1</source>
        <translation>Dosya bulunamadı: %1</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="113"/>
        <source>Invalid file format: %1</source>
        <translation>Geçersiz dosya biçimi: %1</translation>
    </message>
</context>
<context>
    <name>U2::LogCategories</name>
    <message>
        <location filename="../src/LogSettings.cpp" line="37"/>
        <source>TRACE</source>
        <translation>İZLEME</translation>
    </message>
    <message>
        <location filename="../src/LogSettings.cpp" line="38"/>
        <source>DETAILS</source>
        <translation>AYRINTILAR</translation>
    </message>
    <message>
        <location filename="../src/LogSettings.cpp" line="39"/>
        <source>INFO</source>
        <translation>BİLGİ</translation>
    </message>
    <message>
        <location filename="../src/LogSettings.cpp" line="40"/>
        <source>ERROR</source>
        <translation>HATA</translation>
    </message>
</context>
<context>
    <name>U2::PluginDescriptorHelper</name>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="109"/>
        <location filename="../src/PluginDescriptor.cpp" line="115"/>
        <location filename="../src/PluginDescriptor.cpp" line="121"/>
        <location filename="../src/PluginDescriptor.cpp" line="127"/>
        <source>Required attribute not found %1</source>
        <translation>Gerekli öznitelik bulunamadı %1</translation>
    </message>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="138"/>
        <location filename="../src/PluginDescriptor.cpp" line="149"/>
        <location filename="../src/PluginDescriptor.cpp" line="155"/>
        <source>Required element not found %1</source>
        <translation>Gerekli eleman %1 bulunamadı</translation>
    </message>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="162"/>
        <source>Not valid value: &apos;%1&apos;, plugin: %2</source>
        <translation>Değer geçersiz: &apos;%1&apos;, eklenti: %2</translation>
    </message>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="170"/>
        <source>Platform arch is unknown: %1</source>
        <translation>Platform arkı bilinmiyor: %1</translation>
    </message>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="177"/>
        <source>Platform bits is unknown: %1</source>
        <translation>Platform bitleri bilinmiyor: %1</translation>
    </message>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="196"/>
        <source>Invalid depends token: %1</source>
        <translation>Geçersiz simge: %1</translation>
    </message>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="255"/>
        <source>Plugin circular dependency detected: %1 &lt;-&gt; %2</source>
        <translation>Eklenti döngüsel bağımlılığı algılandı: %1 &lt;-&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="344"/>
        <source>Can&apos;t satisfy dependencies for %1 !</source>
        <translation>%1 için bağımlılıkları karşılayamıyor!</translation>
    </message>
</context>
<context>
    <name>U2::RegisterServiceTask</name>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="147"/>
        <source>Register &apos;%1&apos; service</source>
        <translation>&apos;%1&apos; hizmetini kaydet</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="152"/>
        <source>Service has already registered</source>
        <translation>Hizmet zaten kayıtlı</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="153"/>
        <source>Illegal service state: %1</source>
        <translation>Yasadışı hizmet durumu: %1</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="157"/>
        <source>Only one service of specified type is allowed: %1</source>
        <translation>Belirtilen türde yalnızca bir hizmete izin verilir: %1</translation>
    </message>
</context>
<context>
    <name>U2::TaskSchedulerImpl</name>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="77"/>
        <location filename="../src/TaskSchedulerImpl.cpp" line="694"/>
        <source>New</source>
        <translation>Yeni</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="77"/>
        <location filename="../src/TaskSchedulerImpl.cpp" line="696"/>
        <source>Prepared</source>
        <translation>Hazırlanmış</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="77"/>
        <location filename="../src/TaskSchedulerImpl.cpp" line="698"/>
        <source>Running</source>
        <translation>Çalışıyor</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="77"/>
        <location filename="../src/TaskSchedulerImpl.cpp" line="700"/>
        <source>Finished</source>
        <translation>Tamamlandı</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="96"/>
        <source>Canceling task: %1</source>
        <translation>Görev iptal ediliyor: %1</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="113"/>
        <source>There is not enough memory to finish the task.</source>
        <translation>Görevi bitirmek için yeterli hafıza yok.</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="135"/>
        <source>Subtask {%1} is failed: %2</source>
        <translation>{%1} alt görevi başarısız oldu: %2</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="376"/>
        <source>Waiting for resource &apos;%1&apos;, count: %2</source>
        <translation>&apos;%1&apos; kaynağı bekleniyor, sayı: %2</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="401"/>
        <source>Not enough resources for the task, resource name: &apos;%1&apos; max: %2%3 requested: %4%5</source>
        <translation>Görev için yeterli kaynak yok, kaynak adı: &apos;%1&apos; maks: %2%3 talep edildi: %4%5</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="413"/>
        <source>Waiting for resource &apos;%1&apos;, count: %2%3</source>
        <translation>&apos;%1&apos; kaynağı bekleniyor, sayı: %2%3</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="547"/>
        <source>Registering new task: %1</source>
        <translation>Yeni görev kaydediliyor: %1</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="704"/>
        <source>Invalid name</source>
        <translation>Geçersiz ad</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="926"/>
        <source>Deleting task: %1</source>
        <translation>Görev siliniyor: %1</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="778"/>
        <source>Promoting task {%1} to &apos;%2&apos;</source>
        <translation>{%1} görevi &apos;%2&apos; olarak yükseltiliyor</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="127"/>
        <source>Subtask {%1} is canceled %2</source>
        <translation>Alt görev {%1} iptal edildi %2</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="393"/>
        <source>No required resources for the task, resource id: &apos;%1&apos;</source>
        <translation>Görev için gerekli kaynak yok, kaynak kimliği: &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="394"/>
        <source>Unable to run test because required resource not found</source>
        <translation>Gerekli kaynak bulunamadığı için test çalıştırılamıyor</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="613"/>
        <source>Unregistering task: %1</source>
        <translation>Kaydı iptal edilen görev: %1</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="780"/>
        <source>Promoting task {%1} to &apos;%2&apos;, error &apos;%3&apos;</source>
        <translation>{%1} görevi &apos;%2&apos; olarak yükseltiliyor, &apos;%3&apos; hatası</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="790"/>
        <source>Starting {%1} task</source>
        <translation>{%1} görev başlatılıyor</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="818"/>
        <source>Task {%1} finished with error: %2</source>
        <translation>{%1} görevi şu hatayla tamamlandı: %2</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="821"/>
        <source>Task {%1} canceled</source>
        <translation>{%1} görevi iptal edildi</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="824"/>
        <source>Task {%1} finished</source>
        <translation>{%1} görevi tamamlandı</translation>
    </message>
</context>
<context>
    <name>U2::UnregisterServiceTask</name>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="250"/>
        <source>Unregister &apos;%1&apos; service</source>
        <translation>&apos;%1&apos; hizmetinin kaydını sil</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="255"/>
        <source>Service is not registered</source>
        <translation>Hizmet kayıtlı değil</translation>
    </message>
</context>
<context>
    <name>U2::VerifyPluginTask</name>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="487"/>
        <source>Verify plugin task: %1</source>
        <translation>Eklenti görevini doğrulayın: %1</translation>
    </message>
</context>
</TS>
