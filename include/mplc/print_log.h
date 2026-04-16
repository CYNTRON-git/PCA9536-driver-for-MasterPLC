#pragma once
#include <string>
#include <opcua_platformdefs.h>
#include <opcua_trace.h>
#include <opcua/opcua_thread.h>

/*
Примеры использования макросов

BEGIN_PRINT(logLevels::INF) 
for(int = 0;i<5; i++)
{
 какой-то код
 ADD_PRINT("cnt %d ",i);
}
END_PRINT

Вывод:
73820434: |41864| [INF] cnt 0 cnt 1 cnt 2 cnt 3 cnt 4

Если объем кода не слишком большой можно разместить его в 
PRINT_BLK(logLevels::INF, for(int = 0;i<5; i++) {
 какой-то код
 ADD_PRINT("cnt %d ",i);
});

результат будет тот же самый
*/

#define PRINT_BLK(lvl, body)                                                                                           \
    do {                                                                                                               \
        logMsgPrint __logMsgPrint(lvl);                                                                                \
        {body} __logMsgPrint.print();                                                                                  \
    } while (0)

// Инициализация строки вывода в лог. 
// Создается объект содержащий строку и в ней формируется начальный перефикс с задаанным уровнем логииирования lvl.
#define BEGIN_PRINT(lvl)                                                                                               \
    {                                                                                                                  \
        logMsgPrint __logMsgPrint(lvl);

// макрос добавление текса в строку вывода в лог. 
#define ADD_PRINT(...)                                                                                                 \
    do {                                                                                                               \
        __logMsgPrint.add(__VA_ARGS__);                                                                                \
    } while (0)

// Собственно макрос вывода в лог. Вызывает деструктор в котором и выполняется вывод сторки в лог
#define END_PRINT                                                                                                      \
    }


typedef enum {
    NINT = -1,
    INF = OPCUA_TRACE_LEVEL_INFO,     // "INF"
    TRA = OPCUA_TRACE_LEVEL_DEBUG,    // "TRA"
    WAR = OPCUA_TRACE_LEVEL_WARNING,  // "WAR"
    SYS = OPCUA_TRACE_LEVEL_SYSTEM,   // "SYS"
    ERR = OPCUA_TRACE_LEVEL_ERROR,    // "ERR";
    UNK = OPCUA_TRACE_LEVEL_CONTENT  // "UNK"
} logLevels;

class logMsgPrint {
    logLevels setLevel;
    std::string pr_str;
    char _tmp_str[4096];

    /// Формирует префикс строки вывода в лог
    void makePrefix() {
        char firstMessage[50];
        switch (setLevel) {
        case INF:
            sprintf(firstMessage, "%u: |%u| [INF] ", RGetTime_ms(), OpcUa_Thread_GetCurrentThreadId());
            break;
        case TRA:
            sprintf(firstMessage, "%u: |%u| [TRA] ", RGetTime_ms(), OpcUa_Thread_GetCurrentThreadId());
            break;
        case WAR:
            sprintf(firstMessage, "%u: |%u| [WAR] ", RGetTime_ms(), OpcUa_Thread_GetCurrentThreadId());
            break;
        case SYS:
            sprintf(firstMessage, "%u: |%u| [SYS] ", RGetTime_ms(), OpcUa_Thread_GetCurrentThreadId());
            break;
        case ERR:
            sprintf(firstMessage, "%u: |%u| [ERR] ", RGetTime_ms(), OpcUa_Thread_GetCurrentThreadId());
            break;
        case UNK:
            sprintf(firstMessage, "%u: |%u| [UNK] ", RGetTime_ms(), OpcUa_Thread_GetCurrentThreadId());
            break;
        default:
            return;
        }
        pr_str = firstMessage;
    }

    // Выводит строку в лог и сбрасывает в неинициализированное состояние
    int print() {
        if ((OpcUa_Trace_GetTraceLevel() & setLevel) == 0)
            return 0;

        PRINTLN(pr_str.c_str());
        setLevel = NINT;  // Сбрасываем состояние.
        return 0;
    };

public:
    logMsgPrint(logLevels lvl) {
        setLevel = lvl;
        makePrefix();
    };

    ~logMsgPrint() {
        print();
    };

    /// Добавляет текст в строку вывода в лог
    void add(_Printf_format_string_ const char* fmt, ...) {
        if (setLevel == NINT) { 
            makePrefix();
            setLevel = UNK;  // Для возможности вывода в той же области видимости после уже вызванного print. Может и не надо.
        }

        va_list ap;
        va_start(ap, fmt);
        int ret = vsnprintf(_tmp_str, sizeof(_tmp_str), fmt, ap);
        va_end(ap);
        if (ret > 0)
            pr_str.append(_tmp_str);
    };
};
