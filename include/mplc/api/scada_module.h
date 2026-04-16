#pragma once

#include "scada_channel.h"

namespace mplc { namespace api {

    /**
     * \brief Базовый классы для реализации модуля в протоколе
     */
    struct ScadaModule {
        MPLCSHARE_API ScadaModule();

        virtual ~ScadaModule() = default;

        /**
         * \brief  Обновляет состояние модуля
         * \param fault Содержит текущее состояние ошибки, true если произошёл сбой
         * \param errorText Текст ошибки
         */
        MPLCSHARE_API void SetFaultState(bool fault, const std::string& errorText = "");

        /**
         * \brief  Обновляет состояние модуля В отличие от SetFaultState errorText устанавливается
         * независимо от последнего состояния модуля
         * \param fault Содержит текущее состояние ошибки, true если произошёл сбой
         * \param errorText Текст ошибки
         */
        MPLCSHARE_API void UpdateFaultState(bool fault, const std::string& errorText = "");

        /**
         * \brief Проверяет текущее состояние пареметра "Опрос по условию"
         * \return Возвращает true если настройка не включена или значение параметра == true
         */
        MPLCSHARE_API bool isExecute();

        /**
         * \brief Проверяет текущее состояние пареметра "Подключение по условию"
         * \return Возвращает true если настройка не включена или значение параметра == true
         */
        MPLCSHARE_API bool isConnect();

        MPLCSHARE_API bool isWrite();

        /**
         * \brief Переопределяемый пользователем метод для добавления нового канала
         * \warning За удаление созданного объекта отвечает пользовательский код
         * \param channel Ссылка на описание канала в дереве системы
         * \param provider Передаётся для чтения начальных значений на входе канала
         * \return Должен вернуть либо nullptr, либо указатель на созданный экземпляр ScadaChannel или его наследника.
         */
        virtual ScadaChannel* Create(const mplc::vm::Channel* channel, LuaDataProvider* provider) = 0;

        /**
         * \brief Переопределяемый пользователем метод для добавления новой группы каналов
         * \param channel Ссылка на описание группы каналов в дереве системы
         * \param provider Передаётся для чтения начальных значений в каналах группы
         */
        MPLCSHARE_API virtual void InitChannelGroup(const mplc::vm::ChannelGroup* group, LuaDataProvider* provider);
        /**
         * \brief Переопределяемый пользователем метод для добавления нового параметра
         * \warning За удаление созданного объекта отвечает пользовательский код
         * \param var Ссылка на описание параметра в дереве системы
         * \param provider Передаётся для чтения начальных значений
         * \return Должен вернуть либо nullptr, либо указатель на созданный экземпляр ScadaParameter или его
         * наследника.
         */
        virtual ScadaParameter* Create(const mplc::vm::Variable* var, LuaDataProvider* provider) {
            return nullptr;
        }

        /**
         * \brief Проверяет результат последнего вызова модуля
         * \warning для корректной работы необходимо на каждом цикле обновлять статус через: SetFaultState
         * \return Возвращет true если произошёл сбой
         */
        bool isFailed() const {
            return _last_fault;
        }

        /**
         * \brief LuaDataProvider для обращеения к виртуальной машине Lua
         * \return Возвращает укзатель на связанный с протоколом DataProvider
         */
        LuaDataProvider* LuaProvider() const {
            return _lua_provider;
        }

        /**
         * \brief Входная точка инициализации модуля
         *
         * Инициализирует параметры модуля такие как "Опрос по услови",
         * "Подключение по условию" и "Способ записи". После чего вызывает InitChannels и Init
         *
         * \param mod Указатель на описание модуля в дереве системы
         * \param provider DataProvider для чтения начальных значений
         */
        MPLCSHARE_API void BaseInit(const mplc::vm::IOModule* mod, LuaDataProvider* provider);

        /*Сеттеры для установки свойств*/

        MPLCSHARE_API void SetFaultItem(int64_t Id);

        MPLCSHARE_API void SetErrorTextItem(int64_t Id);

        MPLCSHARE_API void SetExecuteItem(int64_t Id);

        MPLCSHARE_API void SetConnectItem(int64_t Id);

        MPLCSHARE_API void SetWriteItem(int64_t Id);

        MPLCSHARE_API void SetLuaProvider(LuaDataProvider* LuaProviderIn);

        ProtocolWriteCondition GetWriteCondition() const {
            return WriteCondition;
        }

    protected:
        /**
         * \brief Функция инициализация модуля, вызывается после создания всех каналов этог модуля
         * \param modl Указатель на представление модуля в дереве системы
         */
        virtual void Init(const mplc::vm::IOModule* modl) {}

        /**
         * \brief В зависимости от настройки "Способ записи" проверяет нужно ли обновить значение на
         * выходе канала и обновляет его
         * \param ch Проверяемый канал
         * \param value Новое значение на входе канала
         * \return Возвращет true если значение на выходе обновилось
         */
        bool IsNeedWrite(ScadaChannel& ch, const OpcUa_VariantHlp& value) {
            return ch.IsNeedWrite(value, WriteCondition);
        }

        /**
         * \brief Производит обход дочерние элементы и инициализирует все каналы и параметры объявленные в этом модуле
         * \param mod Указатель на ноду принадлежащую этому модулю в дереве системы
         * \param provider DataProvider для чтения начальных значений
         */
        MPLCSHARE_API virtual void InitChilds(const mplc::vm::PtNode* mod, LuaDataProvider* provider);

    private:
        LuaDataProvider* _lua_provider;

        bool _last_fault;
        bool _last_execute;
        bool _last_write;

        ProtocolWriteCondition WriteCondition;
        lib::optional<ScadaParameter> ExecuteItem, ConnectItem, WriteItem, FaultItem, ErrorTextItem;
    };
}}  // namespace mplc::api
