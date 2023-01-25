<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="ru">
<context>
    <name>QObject</name>
    <message>
        <location filename="../src/CredentialsAskerCli.cpp" line="126"/>
        <source>Enter password: </source>
        <translation>Пароль: </translation>
    </message>
    <message>
        <location filename="../src/CredentialsAskerCli.cpp" line="177"/>
        <source>Enter user name: </source>
        <translation>Имя пользователя: </translation>
    </message>
    <message>
        <location filename="../src/CredentialsAskerCli.cpp" line="159"/>
        <location filename="../src/CredentialsAskerCli.cpp" line="184"/>
        <source>Would you like UGENE to remember the password?</source>
        <translation>Запомнить пароль?</translation>
    </message>
    <message>
        <location filename="../src/CredentialsAskerCli.cpp" line="155"/>
        <location filename="../src/CredentialsAskerCli.cpp" line="171"/>
        <source>Connect to the &apos;%1&apos; ...
</source>
        <translation>Подключение к &apos;%1&apos; ...
</translation>
    </message>
    <message>
        <location filename="../src/CredentialsAskerCli.cpp" line="156"/>
        <location filename="../src/CredentialsAskerCli.cpp" line="172"/>
        <source>You are going to log in as &apos;%1&apos;.
</source>
        <translation>Вы собираетесь войти как &apos;%1&apos;.
</translation>
    </message>
    <message>
        <location filename="../src/CredentialsAskerCli.cpp" line="173"/>
        <source>Would you like to log in as another user?</source>
        <translation>Хотите войти под другим пользователем?</translation>
    </message>
</context>
<context>
    <name>U2::AddPluginTask</name>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="342"/>
        <source>Add plugin task: %1</source>
        <translation>Загрузка внешнего модуля: %1</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="356"/>
        <source>Plugin is already loaded: %1</source>
        <translation>Модуль уже загружен: %1</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="364"/>
        <source>Plugin %1 depends on %2 which is not loaded</source>
        <translation>Модуль %1 зависит от модуля %2, который не был загружен</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="368"/>
        <source>Plugin %1 depends on %2 which is available, but the version is too old</source>
        <translation>Модуль %1 зависит от модуля %2, который доступен, но версия которого устарела</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="379"/>
        <source>Plugin loading error: %1, Error string %2</source>
        <translation>Ошибка загрузки модуля: %1, строка %2</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="385"/>
        <source>Settings is NULL</source>
        <translation>Settings is NULL</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="440"/>
        <source>Plugin loading error: %1. Verification failed.</source>
        <translation>Plugin loading error: %1. Verification failed.</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="460"/>
        <source>Plugin initialization routine was not found: %1</source>
        <translation>Не найдена входная функция библиотеки модуля: %1</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="466"/>
        <source>Plugin initialization failed: %1</source>
        <translation>Ошибка инициализации модуля: %1</translation>
    </message>
</context>
<context>
    <name>U2::ConsoleLogDriver</name>
    <message>
        <location filename="../src/ConsoleLogDriver.cpp" line="81"/>
        <source>Specifies the format of a log line.</source>
        <translation>Задает формат строки лога.</translation>
    </message>
    <message>
        <location filename="../src/ConsoleLogDriver.cpp" line="82"/>
        <source>Specifies the format of a log line.

Use the following notations: L - level, C - category, YYYY or YY - year, MM - month, dd - day, hh - hour, mm - minutes, ss - seconds, zzz - milliseconds. 

By default, logformat=&quot;[L][hh:mm]&quot;.</source>
        <translation>Задает формат строки лога.

Используются следующие обозначения L - уровень, C - категория, YYYY или YY - год, MM - месяц, dd - день, hh - час, mm - минуты, ss - секунды, zzz - миллисекунды. 

По умолчанию формат лога=&quot;[L][hh:mm]&quot;.</translation>
    </message>
    <message>
        <location filename="../src/ConsoleLogDriver.cpp" line="87"/>
        <source>&quot;&lt;format_string&gt;&quot;</source>
        <translation>&quot;&lt;format_string&gt;&quot;</translation>
    </message>
    <message>
        <location filename="../src/ConsoleLogDriver.cpp" line="91"/>
        <source>Sets the log level.</source>
        <translation>Устанавливает уровень лога.</translation>
    </message>
    <message>
        <location filename="../src/ConsoleLogDriver.cpp" line="92"/>
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
        <location filename="../src/ConsoleLogDriver.cpp" line="99"/>
        <source>&quot;&lt;category1&gt;=&lt;level1&gt; [&lt;category2&gt;=&lt;level2&gt; ...]&quot; | &lt;level&gt;</source>
        <translation>&quot;&lt;category1&gt;=&lt;level1&gt; [&lt;category2&gt;=&lt;level2&gt; ...]&quot; | &lt;level&gt;</translation>
    </message>
    <message>
        <location filename="../src/ConsoleLogDriver.cpp" line="102"/>
        <source>Enables colored output.</source>
        <translation>Включает цветовой вывод.</translation>
    </message>
</context>
<context>
    <name>U2::DisableServiceTask</name>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="277"/>
        <source>Disable &apos;%1&apos; service</source>
        <translation>Остановка сервиса: %1</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="290"/>
        <source>Service is not registered</source>
        <translation>Сервис не зарегистрирован</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="291"/>
        <source>Service is not enabled</source>
        <translation>Сервис недоступен</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="297"/>
        <source>Active top-level task name: %1</source>
        <translation>Активная задача: %1</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="299"/>
        <source>Active task was found</source>
        <translation>Имеются неоконченные задачи</translation>
    </message>
</context>
<context>
    <name>U2::DocumentFormatRegistryImpl</name>
    <message>
        <location filename="../src/DocumentFormatRegistryImpl.cpp" line="169"/>
        <source>UGENE Database</source>
        <translation>База данных UGENE</translation>
    </message>
</context>
<context>
    <name>U2::EnableServiceTask</name>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="216"/>
        <source>Service is enabled</source>
        <translation>Сервис доступен</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="217"/>
        <source>Service is not registered</source>
        <translation>Сервис не зарегистрирован</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="222"/>
        <source>Circular service dependency: %1</source>
        <translation>Циркулярная зависимость модуля: %1</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="228"/>
        <source>Required services are not enabled: %1</source>
        <translation>Необходимые сервисы недоступны: %1</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="176"/>
        <source>Enable &apos;%1&apos; service</source>
        <translation>Запуск сервиса: %1</translation>
    </message>
</context>
<context>
    <name>U2::LoadAllPluginsTask</name>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="78"/>
        <source>Loading start up plugins</source>
        <translation>Загрузка подключаемых модулей</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="108"/>
        <source>File not found: %1</source>
        <translation>Не найден внешний модуль: %1</translation>
    </message>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="113"/>
        <source>Invalid file format: %1</source>
        <translation>Некорректный файл библиотеки: %1</translation>
    </message>
</context>
<context>
    <name>U2::LogCategories</name>
    <message>
        <location filename="../src/LogSettings.cpp" line="50"/>
        <source>TRACE</source>
        <translation>Трассировка</translation>
    </message>
    <message>
        <location filename="../src/LogSettings.cpp" line="51"/>
        <source>DETAILS</source>
        <translation>Детали</translation>
    </message>
    <message>
        <location filename="../src/LogSettings.cpp" line="52"/>
        <source>INFO</source>
        <translation>Информация</translation>
    </message>
    <message>
        <location filename="../src/LogSettings.cpp" line="53"/>
        <source>ERROR</source>
        <translation>Ошибка</translation>
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
        <translation>Required attribute not found %1</translation>
    </message>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="138"/>
        <location filename="../src/PluginDescriptor.cpp" line="149"/>
        <location filename="../src/PluginDescriptor.cpp" line="155"/>
        <source>Required element not found %1</source>
        <translation>Required element not found %1</translation>
    </message>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="162"/>
        <source>Not valid value: &apos;%1&apos;, plugin: %2</source>
        <translation>Not valid value: &apos;%1&apos;, plugin: %2</translation>
    </message>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="170"/>
        <source>Platform arch is unknown: %1</source>
        <translation>Platform arch is unknown: %1</translation>
    </message>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="177"/>
        <source>Platform bits is unknown: %1</source>
        <translation>Platform bits is unknown: %1</translation>
    </message>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="196"/>
        <source>Invalid depends token: %1</source>
        <translation>Invalid depends token: %1</translation>
    </message>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="255"/>
        <source>Plugin circular dependency detected: %1 &lt;-&gt; %2</source>
        <translation>Обнаружен модуль циркулярной зависимости: %1 &lt;-&gt; %2</translation>
    </message>
    <message>
        <location filename="../src/PluginDescriptor.cpp" line="344"/>
        <source>Can&apos;t satisfy dependencies for %1 !</source>
        <translation>Can&apos;t satisfy dependencies for %1 !</translation>
    </message>
</context>
<context>
    <name>U2::RegisterServiceTask</name>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="149"/>
        <source>Register &apos;%1&apos; service</source>
        <translation>Регистрация сервиса: %1</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="154"/>
        <source>Service has already registered</source>
        <translation>Сервис уже зарегистрирован</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="155"/>
        <source>Illegal service state: %1</source>
        <translation>Ошибка сервиса: %1</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="159"/>
        <source>Only one service of specified type is allowed: %1</source>
        <translation>Разрешён только один экземпляр сервиса: %1</translation>
    </message>
</context>
<context>
    <name>U2::TaskSchedulerImpl</name>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="77"/>
        <location filename="../src/TaskSchedulerImpl.cpp" line="659"/>
        <source>New</source>
        <translation>Новая</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="77"/>
        <location filename="../src/TaskSchedulerImpl.cpp" line="661"/>
        <source>Prepared</source>
        <translation>Инициализирована</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="77"/>
        <location filename="../src/TaskSchedulerImpl.cpp" line="663"/>
        <source>Running</source>
        <translation>Выполняется</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="77"/>
        <location filename="../src/TaskSchedulerImpl.cpp" line="665"/>
        <source>Finished</source>
        <translation>Завершена</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="95"/>
        <source>Canceling task: %1</source>
        <translation>Отменяется задача &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="111"/>
        <source>There is not enough memory to finish the task.</source>
        <translation>There is not enough memory to finish the task.</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="132"/>
        <source>Subtask {%1} is failed: %2</source>
        <translation>Подзадача {%1} завершена с ошибкой: %2</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="375"/>
        <source>Waiting for resource &apos;%1&apos;, count: %2</source>
        <translation>Ожидание ресурса &quot;%1&quot;, требуется: %2</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="400"/>
        <source>Not enough resources for the task, resource name: &apos;%1&apos; max: %2%3 requested: %4%5</source>
        <translation>Не достаточно ресурсов для выполнения задачи: &apos;%1&apos; макс: %2%3	запрошено: %4%5</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="412"/>
        <source>Waiting for resource &apos;%1&apos;, count: %2%3</source>
        <translation>Ожидание ресурса &quot;%1&quot;, требуется: %2%3</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="538"/>
        <source>Registering new task: %1</source>
        <translation>Добавлена задача &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="669"/>
        <source>Invalid name</source>
        <translation>Неизвестное состояние</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="894"/>
        <source>Deleting task: %1</source>
        <translation>Удаляется задача &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="738"/>
        <source>Promoting task {%1} to &apos;%2&apos;</source>
        <translation>Задача {%1} %2</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="124"/>
        <source>Subtask {%1} is canceled %2</source>
        <translation>Подзадача {%1} отменена %2</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="392"/>
        <source>No required resources for the task, resource id: &apos;%1&apos;</source>
        <translation>No required resources for the task, resource id: &apos;%1&apos;</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="393"/>
        <source>Unable to run test because required resource not found</source>
        <translation>Unable to run test because required resource not found</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="599"/>
        <source>Unregistering task: %1</source>
        <translation>Незарегистрированная задача: %1</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="740"/>
        <source>Promoting task {%1} to &apos;%2&apos;, error &apos;%3&apos;</source>
        <translation>Задача {%1} %2; ошибка: %3</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="750"/>
        <source>Starting {%1} task</source>
        <translation>Старт задачи {%1}</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="778"/>
        <source>Task {%1} finished with error: %2</source>
        <translation>Задача {%1} завершена с ошибкой: %2</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="781"/>
        <source>Task {%1} canceled</source>
        <translation>Отменяется задача {%1}</translation>
    </message>
    <message>
        <location filename="../src/TaskSchedulerImpl.cpp" line="784"/>
        <source>Task {%1} finished</source>
        <translation>Задача {%1} завершена</translation>
    </message>
</context>
<context>
    <name>U2::UnregisterServiceTask</name>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="252"/>
        <source>Unregister &apos;%1&apos; service</source>
        <translation>Дерегистрация сервиса &quot;%1&quot;</translation>
    </message>
    <message>
        <location filename="../src/ServiceRegistryImpl.cpp" line="257"/>
        <source>Service is not registered</source>
        <translation>Сервис не зарегистрирован</translation>
    </message>
</context>
<context>
    <name>U2::VerifyPluginTask</name>
    <message>
        <location filename="../src/PluginSupportImpl.cpp" line="493"/>
        <source>Verify plugin task: %1</source>
        <translation>Задача верификации модуля: %1</translation>
    </message>
</context>
</TS>
