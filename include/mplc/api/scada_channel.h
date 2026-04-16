#pragma once
#include <mplc/libs/optional.hpp>
#include <drivers/drv_user.h>
#include <mplc/date_time.h>
#include <mplc/libs/tslist.hpp>
#include <mplc/vm/node_typese.h>

#include <mplc/lua/lua_function.h>
#include <share/lua_data_provider.h>

#include "scada_parameter.h"
namespace mplc { namespace api {

    struct ScadaChannel {
        struct input_t : ScadaParameter {
            lib::tslist<OpcUa_VariantHlp> buf;
            /*void PushBuf(OpcUa_VariantHlp&& val) {
                buf.push_back(val);
            }*/
            MPLCSHARE_API OpcUa_StatusCode WriteBuf(LuaDataProvider* provider);
        };
        struct output_t : ScadaParameter {
            DateTime LastWrite;
            ProtocolWriteCondition WriteCondition;
            output_t(): WriteCondition(pwcByChange) {}
        };

        lib::optional<input_t> InVar;
        lib::optional<output_t> OutVar;
        const vm::Channel* m_channel{};
        enum { Input = 0, Output = 1, InOut = 2 };
        uint8_t Direction;
        ScadaChannel() = default;
        ScadaChannel(const ScadaChannel& ch) = delete;
        ScadaChannel& operator=(const ScadaChannel& ch) = delete;
        ScadaChannel(ScadaChannel&& ch) noexcept
            : InVar(std::move(ch.InVar)), OutVar(std::move(ch.OutVar)), Direction(ch.Direction) {}
        ScadaChannel& operator=(ScadaChannel&& ch) noexcept {
            InVar = std::move(ch.InVar);
            OutVar = std::move(ch.OutVar);
            Direction = ch.Direction;
            return *this;
        }
        virtual ~ScadaChannel() = default;

        /**
         * \brief Читает текущее значение канала в OutVar из задачи протокола и конвертирует в тип T
         * \tparam T Тип в который необходимо преобразовать OpcUa_VariantHlp
         * \param provider Контекст задачи протокола, доступен из модуля и протокола по вызову LuaProvider()
         * \param def_val Значение которое вернёт функция в случае ошибки чтения
         * \return Значение параметра
         */
        template<class T>
        T Read(LuaDataProvider* provider, T&& def_val = {}) {
            if (OutVar && OpcUa_IsGood(OutVar->Read(provider))) {
                return OutVar->Get<T>(def_val);
            }
            return def_val;
        }

        /**
         * \brief Читает текущее значение канала из задачи протокола в переданный по ссылке аргумент val
         * \param provider Контекст задачи протокола, доступен из модуля и протокола по вызову LuaProvider()
         * \return Возвращает OpcUa_Good в случае успех и OpcUa_Bad если при чтении возникли проблемы
         */
        OpcUa_StatusCode ReadVariant(LuaDataProvider* provider, OpcUa_VariantHlp& val) {
            if (OutVar)
                return OutVar->Read(provider, val);
            return OpcUa_Bad;
        }

        /**
         * \brief Читает текущее значение канала в OutVar из задачи протокола
         * \param provider Контекст задачи протокола, доступен из модуля и протокола по вызову LuaProvider()
         * \return Возвращает ссылку на новое значение в OutVar
         */
        const OpcUa_VariantHlp& ReadVariant(LuaDataProvider* provider) {
            static const OpcUa_VariantHlp empty;
            if (OutVar) {
                OutVar->Read(provider);
                return OutVar->Get();
            }
            return empty;
        }

        /**
         * \brief Записывает произвольный тип в Input переменную канала.
         *    Позволяет записаьт любой тип для которого определён set_lua_value
         * \tparam T Для нестандартных типов должна быть реализована функция:
         *   void set_lua_value(const T& ch, lua_State* L);
         * \param provider Ссылка на LuaDataProvider доступен из модуля и протокола по вызову LuaProvider()
         * \param val Данные
         */
        template<class T>
        void Write(LuaDataProvider* provider, T&& val) {
            if (InVar && InVar->m_var) {
                // 1 - itemId, 2 - typeHash, 3 - itemPath, 4 - operation, 5 - blocked, 6 - value
                lua::lua_function WriteVarSimpleValue = provider->LuaFunction("WriteVarSimpleValue");
                WriteVarSimpleValue(InVar->m_var->id, 0, std::string(), 0, 0, val);
            }
        }
        


        /**
         * \brief Записывает текущее значение Input переменной.
         * \param provider Контекст задачи протокола, доступен из модуля и протокола по вызову LuaProvider()
         */
        void Write(LuaDataProvider* provider) {
            if (InVar) {
                InVar->Write(provider);
            }
        }

        /**
         * \brief Записывает текущий буфер Input канала.
         * \param provider Контекст задачи протокола, доступен из модуля и протокола по вызову LuaProvider()
         */
        void WriteBuf(LuaDataProvider* provider) {
            if (!InVar)
                return;
            InVar->WriteBuf(provider);
        }

        /**
         * \brief Проверяет выполнены ли условия записи и обновилось ли значение по сравнению с предыдущим
         *   если да то обновляет последнее значение в Out параметре.
         * \param value Новое значение
         * \param condition Режимы способа записи
         * переменных драйвера
         * \return Возвращает true если выполнены все условия для записи значения в протокол
         */
        MPLCSHARE_API bool IsNeedWrite(const OpcUa_VariantHlp& value, ProtocolWriteCondition condition);
        /**
         * \brief Инициализация служебных настроек канала,  Direction (In,Out,InOut) и параметры для чтения и записи
         * \param channel Описание канала из дерева проекта
         * \param provider Контекст задачи протокола, доступен из модуля и протокола по вызову LuaProvider()
         */
        MPLCSHARE_API virtual void BaseInit(const vm::Channel* channel, LuaDataProvider* provider);

        MPLCSHARE_API void Unscale(OpcUa_VariantHlp& val) const;
        MPLCSHARE_API void Scale(OpcUa_VariantHlp& val) const;

    protected:
        /**
         * \brief Вызвается после инициализация Direction и Input/Output параметров канала
         * \param channel Описание канала в дереве проекта
         */
        virtual void Init(const vm::Channel* channel) {}

    private:
    };

}}  // namespace mplc::api
