# Collect translations for Russian.
file(GLOB PROJECT_TRANSLATIONS_RU transl/russian.ts)
set(GLOBAL_TRANSLATIONS_RU ${GLOBAL_TRANSLATIONS_RU} ${PROJECT_TRANSLATIONS_RU} PARENT_SCOPE)

# Collect translations for Turkish.
file(GLOB PROJECT_TRANSLATIONS_TR transl/turkish.ts)
set(GLOBAL_TRANSLATIONS_TR ${GLOBAL_TRANSLATIONS_TR} ${PROJECT_TRANSLATIONS_TR} PARENT_SCOPE)
