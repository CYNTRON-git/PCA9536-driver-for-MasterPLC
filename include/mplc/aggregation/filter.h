#pragma once
#include "enums.h"
#include "quality.h"

namespace mplc { namespace aggregation {

    class Filter final {
        uint32_t quality : 8;
        uint32_t ignore :  4;
        uint32_t options : 8;

    public:
        /*
         * Order: 0 - Good, 1 - Stop, 2 - Bad, 3 - Uncertain 
         * Bit filter mask
         *
         *           Options bits             Ignore bits       Convertion bits
         * [19 18 | 17 16 | 15 14 | 13 12] [11 | 10 | 9 | 8] [7 6 | 5 4 | 3 2 | 1 0]
         *    3       2       1       0      3    2   1   0    3     2     1     0
         *
         * Default:
         * [19 18 | 17 16 | 15 14 | 13 12] [11 | 10 | 9 | 8] [7 6 | 5 4 | 3 2 | 1 0]
         *  1  1    1  1    1  0    1  1    1    1    0   0   1 1   1 1   0 1   0 0
         *
         * Convertion bits:
         * 8 бит конвертации. Разделяется на 4 пары, каждая пара содержит номер типа,
         * в который будет выполнено преобразование
         * Пример:
         *      Отсутсиве конвертации - каждый тип останется самим собой
         *      [7 6 | 5 4 | 3 2 | 1 0]
         *       1 1   1 0   0 1   0 0 
         *      Биты 6 и 7 задают тип, в который преобразуется Uncertain. В данном случае
         *      в 6 и 7 бите хранится число 3 - номер самого Uncertain, следовательно
         *      Uncertain преобразуется в Uncertain.
         *      Аналогично для битов 5, 4 и типа Bad, битов 3, 2 и типа Stop, битов 1, 0 и типа Good
         *
         * По умолчанию:
         *      [7 6 | 5 4 | 3 2 | 1 0]
         *       1 1   1 1   0 1   0 0
         *      Каждый тип останется самим собой, за исключением Bad, что преобразуется в Uncertain
         *
         * Ignore bits:
         * 4 бита игнорирования значений с определённым типом.
         * Если установлены в 1 - игнорируются
         * Каждый бит из этих 4 задаёт отдельно поведение в случае обнаружения значения
         * с необходимым типом. Бит 11 для Uncertain, бит 10 для Bad, бит 9 для Stop,
         * бит 8 для Good 
         * По умолчанию:
         *      [11 10 9 8]
         *       1  1  0 0
         *      Игнорируются значения с типом Uncertain и Bad, Stop и Good остаются
         *
         * Options bits:
         * 8 бит, что разделяется на 4 пары, каждая пара указывает, с какой стороны
         * от значения с определённым типом берутся значения в случае интегрирования
         * Пример:
         *      [19 18 | 17 16 | 15 14 | 13 12]
         *       1  1    1  0    0  1    0  0
         *      Для значений типа Uncertain будут браться значения с обеих сторон
         *      Для значений типа Bad будут браться значения только слева
         *      Для значений типа Stop будут браться значения только справа
         *      Для значений типа Good не будут браться значения
         *
         * По умолчанию:
         *      [19 18 | 17 16 | 15 14 | 13 12]
         *       1  1    1  1    1  0    1  1
         *      Для значений типа Uncertain, Bad, Good будут браться значения с обеих сторон
         *      Для значений типа Stop будут браться значения только слева
         */

        enum QualityConv {
            qGoodNoConv         = 0b00000000,
            qGoodToStop         = 0b00000001,
            qGoodToBad          = 0b00000010,
            qGoodToUncertain    = 0b00000011,

            qStopToGood         = 0b00000000,
            qStopNoConv         = 0b00000100,
            qStopToBad          = 0b00001000,
            qStopToUncertain    = 0b00001100,

            qBadToGood          = 0b00000000,
            qBadToStop          = 0b00010000,
            qBadNoConv          = 0b00100000,
            qBadToUncertain     = 0b00110000,

            qUncertainToGood    = 0b00000000,
            qUncertainToStop    = 0b01000000,
            qUncertainToBad     = 0b10000000,
            qUncertainNoConv    = 0b11000000
        };

        enum Ignore {
            iGood               = 0b0001 << 8,
            iStop               = 0b0010 << 8,
            iBad                = 0b0100 << 8,
            iUncertain          = 0b1000 << 8
        };

        enum Options {
            oGoodNothing        = 0b00000000 << 12,
            oGoodRight          = 0b00000001 << 12,
            oGoodLeft           = 0b00000010 << 12,
            oGoodBoth           = 0b00000011 << 12,

            oStopNothing        = 0b00000000 << 12,
            oStopRight          = 0b00000100 << 12,
            oStopLeft           = 0b00001000 << 12,
            oStopBoth           = 0b00001100 << 12,

            oBadNothing         = 0b00000000 << 12,
            oBadRight           = 0b00010000 << 12,
            oBadLeft            = 0b00100000 << 12,
            oBadBoth            = 0b00110000 << 12,

            oUncertainNothing   = 0b00000000 << 12,
            oUncertainRight     = 0b01000000 << 12,
            oUncertainLeft      = 0b10000000 << 12,
            oUncertainBoth      = 0b11000000 << 12,
        };

        MPLC_AGGREAGATION_API Filter();
        MPLC_AGGREAGATION_API void init(uint32_t qualityFilter);
        MPLC_AGGREAGATION_API Quality::Type resultQuality(const Quality &t) const;
        MPLC_AGGREAGATION_API bool isIgnore(const Quality &t) const;
        MPLC_AGGREAGATION_API bool isPeek(const Quality &t) const;
        MPLC_AGGREAGATION_API int getOptions(const Quality &t) const;
    };
}}
