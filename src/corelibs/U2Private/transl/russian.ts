<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru">
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/CredentialsAskerCli.cpp" line="87"/>
        <source>Enter password: </source>
        <translation>Пароль: </translation>
    </message>
    <message>
        <location filename="../src/CredentialsAskerCli.cpp" line="88"/>
        <source>Enter user name: </source>
        <translation>Имя пользователя: </translation>
    </message>
    <message>
        <location filename="../src/CredentialsAskerCli.cpp" line="90"/>
        <source>Would you like UGENE to remember the password?</source>
        <translation>Запомнить пароль?</translation>
    </message>
    <message>
        <location filename="../src/CredentialsAskerCli.cpp" line="91"/>
        <source>Connect to the &apos;%1&apos; ...
</source>
        <translation>Подключение к &apos;%1&apos; ...
</translation>
    </message>
    <message>
        <location filename="../src/CredentialsAskerCli.cpp" line="92"/>
        <source>You are going to log in as &apos;%1&apos;.
</source>
        <translation>Вы собираетесь войти как &apos;%1&apos;.
</translation>
    </message>
    <message>
        <location filename="../src/CredentialsAskerCli.cpp" line="93"/>
        <source>Would you like to log in as another user?</source>
        <translation>Хотите войти под другим пользователем?</translation>
    </message>
</context>
<context>
    <name>U2::AddPluginTask</name>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="408"/>
        <source>Add plugin task: %1</source>
        <translation>Загрузка внешнего модуля: %1</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="423"/>
        <source>Plugin is already loaded: %1</source>
        <translation>Модуль уже загружен: %1</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="431"/>
        <source>Plugin %1 depends on %2 which is not loaded</source>
        <translation>Модуль %1 зависит от модуля %2, который не был загружен</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="435"/>
        <source>Plugin %1 depends on %2 which is available, but the version is too old</source>
        <translation>Модуль %1 зависит от модуля %2, который доступен, но версия которого устарела</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="446"/>
        <source>Plugin loading error: %1, Error string %2</source>
        <translation>Ошибка загрузки модуля: %1, строка %2</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="452"/>
        <source>Settings is NULL</source>
        <translation>Settings is NULL</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="507"/>
        <source>Plugin loading error: %1. Verification failed.</source>
        <translation>Plugin loading error: %1. Verification failed.</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="527"/>
        <source>Plugin initialization routine was not found: %1</source>
        <translation>Не найдена входная функция библиотеки модуля: %1</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="533"/>
        <source>Plugin initialization failed: %1</source>
        <translation>Ошибка инициализации модуля: %1</translation>
    </message>
</context>
<context>
    <name>U2::ConsoleLogDriver</name>
    <message>
        <location filename="../src/ConsoleLogDriver.cpp" line="80"/>
        <source>Specifies the format of a log line.</source>
        <translation>Задает формат строки лога.</translation>
    </message>
    <message>
        <location filename="../src/ConsoleLogDriver.cpp" line="81"/>
        <source>Specifies the format of a log line.

Use the following notations: L - level, C - category, YYYY or YY - year, MM - month, dd - day, hh - hour, mm - minutes, ss - seconds, zzz - milliseconds. 

By default, logformat=&quot;[L][hh:mm]&quot;.</source>
        <translation>Задает формат строки лога.

Используются следующие обозначения L - уровень, C - категория, YYYY или YY - год, MM - месяц, dd - день, hh - час, mm - минуты, ss - секунды, zzz - миллисекунды. 

По умолчанию формат лога=&quot;[L][hh:mm]&quot;.</translation>
    </message>
    <message>
        <location filename="../src/ConsoleLogDriver.cpp" line="86"/>
        <source>&quot;&lt;format_string&gt;&quot;</source>
        <translation>&quot;&lt;format_string&gt;&quot;</translation>
    </message>
    <message>
        <location filename="../src/ConsoleLogDriver.cpp" line="90"/>
        <source>Sets the log level.</source>
        <translation>Устанавливает уровень лога.</translation>
    </message>
    <message>
        <location filename="../src/ConsoleLogDriver.cpp" line="91"/>
        <source>Sets the log level per category. If a category is not specified, the log level is applied to all categories.

The following categories are available: 
&quot;Algorithms&quot;, &quot;Console&quot;, &quot;Core Services&quot;, &quot;Input/Output&quot;, &quot;Performance&quot;, &quot;Remote Service&quot;, &quot;Scripts&quot;, &quot;Tasks&quot;.

The following log levels are available: TRACE, DETAILS, INFO, ERROR or NONE.

By default, loglevel=&quot;ERROR&quot;.</source>
        <translation>Устанавливает уровень лога для категории. Если категория не задана, уровень лога применяется для всех категорий.

Следующие категории доступны: 
&quot;Algorithms&quot;, &quot;Console&quot;, &quot;Core Services&quot;, &quot;Input/Output&quot;, &quot;Performance&quot;, &quot;Remote Service&quot;, &quot;Scripts&quot;, &quot;Tasks&quot;.

Следующие уровни доступны: TRACE, DETAILS, INFO, ERROR or NONE.

По умолчанию , уровень=&quot;ERROR&quot;.</translation>
    </message>
    <message>
        <location filename="../src/ConsoleLogDriver.cpp" line="98"/>
        <source>&quot;&lt;category1&gt;=&lt;level1&gt; [&lt;category2&gt;=&lt;level2&gt; ...]&quot; | &lt;level&gt;</source>
        <translation>&quot;&lt;category1&gt;=&lt;level1&gt; [&lt;category2&gt;=&lt;level2&gt; ...]&quot; | &lt;level&gt;</translation>
    </message>
    <message>
        <location filename="../src/ConsoleLogDriver.cpp" line="101"/>
        <source>Enables colored output.</source>
        <translation>Включает цветовой вывод.</translation>
    </message>
</context>
<context>
    <name>U2::DisableServiceTask</name>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="287"/>
        <source>Disable &apos;%1&apos; service</source>
        <translation>Остановка сервиса: %1</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="301"/>
        <source>Service is not registered</source>
        <translation>Сервис не зарегистрирован</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="302"/>
        <source>Service is not enabled</source>
        <translation>Сервис недоступен</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="308"/>
        <source>Active top-level task name: %1</source>
        <translation>Активная задача: %1</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="310"/>
        <source>Active task was found</source>
        <translation>Имеются неоконченные задачи</translation>
    </message>
</context>
<context>
    <name>U2::DocumentFormatRegistryImpl</name>
    <message>
        <location filename="../src/DocumentFormatRegistryImpl.cpp" line="233"/>
        <source>UGENE Database</source>
        <translation>База данных UGENE</translation>
    </message>
</context>
<context>
    <name>U2::EnableServiceTask</name>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="191"/>
        <source>Service is enabled</source>
        <translation>Сервис доступен</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="192"/>
        <source>Service is not registered</source>
        <translation>Сервис не зарегистрирован</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="197"/>
        <source>Circular service dependency: %1</source>
        <translation>Циркулярная зависимость модуля: %1</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="203"/>
        <source>Required service is not enabled: %1</source>
        <translation>Не запущен обеспечивающий зависимость сервис: %1</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="181"/>
        <source>Enable &apos;%1&apos; service</source>
        <translation>Запуск сервиса: %1</translation>
    </message>
</context>
<context>
    <name>U2::LoadAllPluginsTask</name>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="82"/>
        <source>Loading start up plugins</source>
        <translation>Загрузка подключаемых модулей</translation>
    </message>
    <message>
        <source>Problem occurred loading the OpenCL driver. Please try to update drivers if                                    you&apos;re going to make calculations on your video card. For details see this page:                                    &lt;a href=&quot;%1&quot;&gt;%1&lt;/a&gt;</source>
        <translation type="vanished">Problem occurred loading the OpenCL driver. Please try to update drivers if                                    you&apos;re going to make calculations on your video card. For details see this page:                                    &lt;a href=&quot;%1&quot;&gt;%1&lt;/a&gt;</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="114"/>
        <source>File not found: %1</source>
        <translation>Не найден внешний модуль: %1</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="119"/>
        <source>Invalid file format: %1</source>
        <translation>Некорректный файл библиотеки: %1</translation>
    </message>
</context>
<context>
    <name>U2::LogCategories</name>
    <message>
        <location filename="../src/LogSettings.cpp" line="37"/>
        <source>TRACE</source>
        <translation>Трассировка</translation>
    </message>
    <message>
        <location filename="../src/LogSettings.cpp" line="38"/>
        <source>DETAILS</source>
        <translation>Детали</translation>
    </message>
    <message>
        <location filename="../src/LogSettings.cpp" line="39"/>
        <source>INFO</source>
        <translation>Информация</translation>
    </message>
    <message>
        <location filename="../src/LogSettings.cpp" line="40"/>
        <source>ERROR</source>
        <translation>Ошибка</translation>
    </message>
</context>
<context>
    <name>U2::PluginDescriptorHelper</name>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="110"/>
        <location filename="../src/PluginDescriptor.cpp" line="116"/>
        <location filename="../src/PluginDescriptor.cpp" line="122"/>
        <location filename="../src/PluginDescriptor.cpp" line="128"/>
        <source>Required attribute not found %1</source>
        <translation>Required attribute not found %1</translation>
    </message>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="139"/>
        <location filename="../src/PluginDescriptor.cpp" line="150"/>
        <location filename="../src/PluginDescriptor.cpp" line="156"/>
        <source>Required element not found %1</source>
        <translation>Required element not found %1</translation>
    </message>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="163"/>
        <source>Not valid value: &apos;%1&apos;, plugin: %2</source>
        <translation>Not valid value: &apos;%1&apos;, plugin: %2</translation>
    </message>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="171"/>
        <source>Platform arch is unknown: %1</source>
        <translation>Platform arch is unknown: %1</translation>
    </message>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="178"/>
        <source>Platform bits is unknown: %1</source>
        <translation>Platform bits is unknown: %1</translation>
    </message>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="197"/>
        <source>Invalid depends token: %1</source>
        <translation>Invalid depends token: %1</translation>
    </message>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="263"/>
        <source>Plugin circular dependency detected: %1 &lt;-&gt; %2</source>
        <translation>Обнаружен модуль циркулярной зависимости: %1 &lt;-&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="352"/>
        <source>Can&apos;t satisfy dependencies for %1 !</source>
        <translation>Can&apos;t satisfy dependencies for %1 !</translation>
    </message>
</context>
<context>
    <name>U2::RegisterServiceTask</name>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="152"/>
        <source>Register &apos;%1&apos; service</source>
        <translation>Регистрация сервиса: %1</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="158"/>
        <source>Service has already registered</source>
        <translation>Сервис уже зарегистрирован</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="159"/>
        <source>Illegal service state: %1</source>
        <translation>Ошибка сервиса: %1</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="163"/>
        <source>Only one service of specified type is allowed: %1</source>
        <translation>Разрешён только один экземпляр сервиса: %1</translation>
    </message>
</context>
<context>
    <name>U2::TaskSchedulerImpl</name>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="82"/>
        <location filename="../src/TaskSchedulerImpl.cpp" line="680"/>
        <source>New</source>
        <translation>Новая</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="82"/>
        <location filename="../src/TaskSchedulerImpl.cpp" line="682"/>
        <source>Prepared</source>
        <translation>Инициализирована</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="82"/>
        <location filename="../src/TaskSchedulerImpl.cpp" line="684"/>
        <source>Running</source>
        <translation>Выполняется</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="82"/>
        <location filename="../src/TaskSchedulerImpl.cpp" line="686"/>
        <source>Finished</source>
        <translation>Завершена</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="101"/>
        <source>Canceling task: %1</source>
        <translation>Отменяется задача &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="118"/>
        <source>There is not enough memory to finish the task.</source>
        <translation>There is not enough memory to finish the task.</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="140"/>
        <source>Subtask {%1} is failed: %2</source>
        <translation>Подзадача {%1} завершена с ошибкой: %2</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="378"/>
        <source>Waiting for resource &apos;%1&apos;, count: %2</source>
        <translation>Ожидание ресурса &quot;%1&quot;, требуется: %2</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="401"/>
        <source>Not enough resources for the task, resource name: &apos;%1&apos; max: %2%3 requested: %4%5</source>
        <translation>Не достаточно ресурсов для выполнения задачи: &apos;%1&apos; макс: %2%3	запрошено: %4%5</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="409"/>
        <source>Waiting for resource &apos;%1&apos;, count: %2%3</source>
        <translation>Ожидание ресурса &quot;%1&quot;, требуется: %2%3</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="542"/>
        <source>Registering new task: %1</source>
        <translation>Добавлена задача &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="689"/>
        <source>Invalid name</source>
        <translation>Неизвестное состояние</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="913"/>
        <source>Deleting task: %1</source>
        <translation>Удаляется задача &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="763"/>
        <source>Promoting task {%1} to &apos;%2&apos;</source>
        <translation>Задача {%1} %2</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="132"/>
        <source>Subtask {%1} is canceled %2</source>
        <translation>Подзадача {%1} отменена %2</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="393"/>
        <source>No required resources for the task, resource id: &apos;%1&apos;</source>
        <translation>No required resources for the task, resource id: &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="394"/>
        <source>Unable to run test because required resource not found</source>
        <translation>Unable to run test because required resource not found</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="604"/>
        <source>Unregistering task: %1</source>
        <translation>Незарегистрированная задача: %1</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="765"/>
        <source>Promoting task {%1} to &apos;%2&apos;, error &apos;%3&apos;</source>
        <translation>Задача {%1} %2; ошибка: %3</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="775"/>
        <source>Starting {%1} task</source>
        <translation>Старт задачи {%1}</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="803"/>
        <source>Task {%1} finished with error: %2</source>
        <translation>Задача {%1} завершена с ошибкой: %2</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="807"/>
        <source>Task {%1} canceled</source>
        <translation>Отменяется задача {%1}</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="810"/>
        <source>Task {%1} finished</source>
        <translation>Задача {%1} завершена</translation>
    </message>
</context>
<context>
    <name>U2::UnregisterServiceTask</name>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="260"/>
        <source>Unregister &apos;%1&apos; service</source>
        <translation>Дерегистрация сервиса &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="266"/>
        <source>Service is not registered</source>
        <translation>Сервис не зарегистрирован</translation>
    </message>
</context>
<context>
    <name>U2::VerifyPluginTask</name>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="560"/>
        <source>Verify plugin task: %1</source>
        <translation>Задача верификации модуля: %1</translation>
    </message>
</context>
</TS>
