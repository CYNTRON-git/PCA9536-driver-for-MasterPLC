#pragma once
#include "events_archive_share.h"
struct Expr;
class Parse;

namespace mplc { namespace events {
    class IEventBase;

    //Условие выборки
    class EventsCondition {
    public:
        typedef OpcUa_StatusCode (*get_column_name_fn)(const std::string& field_name, std::ostringstream& out);
        ADD_PTR_TYPEDEF(EventsCondition);
        EventsCondition() {
            _parser = nullptr;
        }
        MPLC_EVENTS_API ~EventsCondition(); // Why it's needed to build properly wtf?

        //Разобрать по тестовому описанию
        MPLC_EVENTS_API OpcUa_StatusCode Load(const std::string& str);
        MPLC_EVENTS_API static ptr make() {
            return lib::make_shared<EventsCondition>();
        }
        static ptr make(const std::string& str) {
            lib::shared_ptr<EventsCondition> cond = lib::make_shared<EventsCondition>();
            if (OpcUa_IsBad(cond->Load(str))) {
                cond.reset();
            }
            return cond;
        }
        //Проверить удовлетворяет ли сообщение или архив сообщения данному условию
        MPLC_EVENTS_API OpcUa_StatusCode Test(const IEventBase* event_rec, bool& res);
        MPLC_EVENTS_API OpcUa_StatusCode ToSQL(std::ostringstream& out,
                                               std::map<std::string, std::string>& bind,
                                               get_column_name_fn column_name) const;

    private:
        //Вычислить узел синтаксического дерева pExpr для сообщения eventInstance и результат записать в res
        OpcUa_StatusCode ReadNodeValue(const IEventBase* event_rec, OpcUa_VariantHlp& res, Expr* pExpr);

        std::string _inputString;  //Исходный текст условия. Необходимо хранить, так как на элементы
                                   //строки могут быть ссылки из ситнаксического дерева
        Parse* _parser;  //Синтаксическоое дерево
    };
}}  // namespace mplc::events
