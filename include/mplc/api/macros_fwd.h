#pragma once

#include <mplc/meta/macros_fwd.h>
#include <boost/preprocessor/empty.hpp>
/**
 * \brief Макрос MPLC_OBJECT должен быть объявлен в секции public определения класса
 * для досутупа к его полям, методам и родителям
 * \param Class Имя класса
 */
#define MPLC_OBJECT(Class) META_PP_OBJECT(Class)

/**
 * \brief Для доступа к API объкта из других cpp файлов в нутри одной библиотеки необходимо объявить этот макрос в
 * публичном h файле
 * \param Class Полное имя типа включая namespace в котором он объявлен
 */
#define MPLC_DEFINE_PUBLIC_API(Class) META_PP_DEFINE_PUBLIC_API(BOOST_PP_EMPTY(), Class)

/**
 * \brief Для доступа к API объкта из другой динамической библиотеки необходимо объявить этот макрос в публичном h файле
 * \param ExportMacros имя макроса который должен использоваться для экспорта/импорта символов
 * \param Class Полное имя типа включая namespace в котором он объявлен
 */
#define MPLC_EXPORT_PUBLIC_API(ExportMacros, Class) META_PP_DEFINE_PUBLIC_API(ExportMacros, Class)
