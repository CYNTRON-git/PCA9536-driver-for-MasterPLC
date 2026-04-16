#pragma once
#include <mplc/macros/mplc_expand_as.h>
#include "macros_fwd.h"

/**
 * \brief Генерирует необходимые метаклассы для рефлексии
 * \param Name Публичное имя для получение типа по имени в рантайме
 * \param Class Полное имя типа включая namespace в котором он объявлен
 * \warning Должен находится в cpp файле
 */
#define MPLC_DECLARE_PUBLIC_API(Name, Class) META_PP_DECLARE_PUBLIC_API(Name, Class)

/**
 * \brief Объеденяет MPLC_DEFINE_PUBLIC_API и MPLC_DECLARE_PUBLIC_API для случая когда не требуется доступ к типу из
 * других димаческих библиотек
 * \param Name Публичное имя для получение типа по имени в рантайме
 * \param Class Полное имя типа включая namespace в котором он объявлен
 * \warning Должен находится в cpp файле
 */
#define MPLC_DECLARE_API(Name, Class)                                                                                  \
    MPLC_DEFINE_PUBLIC_API(Class);                                                                                     \
    MPLC_DECLARE_PUBLIC_API(Name, Class)

/**
 * \brief Регистрация произвольного типа в mplc::api::ApiStorage
 * \param Class Полное имя типа включая namespace в котором он объявлен
 * \warning Должен находится в cpp файле
 */
#define MPLC_REGISTER_TYPE(Class) _MPLC_REGISTER_(CreateApi, Class)

/**
 * \brief Регистрация FB в mplc::api::ApiStorage
 * \param Class Полное имя FB включая namespace в котором он объявлен
 * \warning Должен находится в cpp файле
 */
#define MPLC_REGISTER_FB(Class)                                                                                        \
    static_assert(!boost::is_abstract<Class>::value, "Forbidden to register an abstract class as a FB");               \
    _MPLC_REGISTER_(CreateApi, Class)

/**
 * \brief Регистрация нового протокола в mplc::api::ApiStorage
 * \param Class Полное имя протокола включая namespace в котором он объявлен
 * \warning Должен находится в cpp файле
 */
#define MPLC_REGISTER_PROTOCOL(Class)                                                                                  \
    static_assert(!boost::is_abstract<Class>::value, "Forbidden to register an abstract class as a Protocol");         \
    _MPLC_REGISTER_(CreateApi, Class)

/**
 * \brief Сокращённый вариант для регистрации типа как FB
 * \param Name Публичное имя для получение типа по имени в рантайме
 * \param Class Полное имя типа включая namespace в котором он объявлен
 * \warning Должен находится в cpp файле
 */
#define MPLC_FB_TYPE(Name, Class)                                                                                      \
    MPLC_DECLARE_API(Name, Class);                                                                                     \
    MPLC_REGISTER_FB(Class)

/**
 * \brief Сокращённый вариант для регистрации типа как протокол
 * \param Name Публичное имя для получение типа по имени в рантайме
 * \param Class Полное имя протокола включая namespace в котором он объявлен
 * \warning Должен находится в cpp файле
 */
#define MPLC_PROTOCOL_TYPE(Name, Class)                                                                                \
    MPLC_DECLARE_API(Name, Class);                                                                                     \
    MPLC_REGISTER_PROTOCOL(Class)

/**
 * \brief Объявление таблицы с полями типа для доступа к ним по именам
 * \param Class Полное имя типа включая namespace в котором он объявлен
 * \warning Должен находится в cpp файле
 */
#define MPLC_DECLARE_PROPERTIES(Class) META_PP_DECLARE(Property, Class)

// #define MPLC_DECLARE_METHODS(Class) META_PP_DECLARE(Method, Class)

/**
 * \brief Описания параметра доступного только на запись
 *
 * Возможные перегрузки:\n
 * MPLC_In(name)\n
 * MPLC_In(name, field)
 * \param name Имя по которому будет осуществляться доступ
 * \param field Параметр которому будет соответсвовать заданное имя, может быть как член класса так и
 * функция с сигнатурой void (*)(const T&). Необязательное аргумент если публичное имя соответсвует имени параметра в
 * классе.
 * \warning Должен находится в секции MPLC_DECLARE_PROPERTIES(Class){}
 */
#define MPLC_In(/* <name>  [field] */...) MPLC_PP_EXPAND_AS(_MPLC_IN_, __VA_ARGS__)

/**
 * \brief Описания параметра доступного только на чтение
 *
 * Возможные перегрузки:\n
 * MPLC_Out(name)\n
 * MPLC_Out(name, field)
 * \param name Имя по которому будет осуществляться доступ
 * \param field Параметр которому будет соответсвовать заданное имя, может быть как член класса так и
 * функция с сигнатурой T (*)(). Необязательное аргумент если публичное имя соответсвует имени параметра в
 * классе.
 * \warning Должен находится в секции MPLC_DECLARE_PROPERTIES(Class){}
 */
#define MPLC_Out(/* <name> [field] */...) MPLC_PP_EXPAND_AS(_MPLC_OUT_, __VA_ARGS__)

/**
 * \brief Макрос для описания параметра доступного для чтения и записи. Возможно объвить как один и тот же параметр для
 * чтения и записи, так и два разных.
 *
 * Возможные перегрузки:\n
 * MPLC_InOut(name)\n
 * MPLC_InOut(name, field)\n
 * MPLC_InOut(name, InputField, OutputField)
 * \param name Имя по которому будет осуществляться доступ
 * \param InOut Параметр которому будет соответсвовать заданное имя. Необязательное аргумент
 * если name соответсвует имени параметра в классе.
 * \param Input Параметр или метод который будет использоваться для записи значения.
 * \param Output Параметр или метод который будет использоваться для чтения значения
 * \warning Должен находится в секции MPLC_DECLARE_PROPERTIES(Class){}
 */
#define MPLC_InOut(/* <name>  [InOut | <In> <Out>]*/...) MPLC_PP_EXPAND_AS(_MPLC_INOUT_, __VA_ARGS__)

/* Служебные макросы */
#define _MPLC_IN_1(name) META_PP_PROPERTY(name, META_PP_READ(name), META_PP_WRITE(name))
#define _MPLC_IN_2(name, Field) META_PP_PROPERTY(name, META_PP_READ(Field), META_PP_WRITE(Field))
#define _MPLC_OUT_1(name) META_PP_PROPERTY(name, META_PP_READ_ONLY(name))
#define _MPLC_OUT_2(name, Field) META_PP_PROPERTY(name, META_PP_READ_ONLY(Field))
#define _MPLC_INOUT_1(name) META_PP_PROPERTY(name, META_PP_MEMBER(name))
#define _MPLC_INOUT_2(name, InOut) META_PP_PROPERTY(name, META_PP_MEMBER(InOut))
#define _MPLC_INOUT_3(name, Input, Output) META_PP_PROPERTY(name, META_PP_READ(Output), META_PP_WRITE(Input))

#define _MPLC_REGISTER_(Type, Class)                                                                                   \
    template<>                                                                                                         \
    const mplc::meta::Object& mplc::api::detail::ApiHolder<Class>::type = mplc::api::ApiStorage::instance().Type<Class>()
