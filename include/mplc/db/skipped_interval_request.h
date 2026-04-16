#pragma once

#include "data_backup_response_item.h"

namespace mplc::archive {
    struct StoreLayer;

    // Типы для временного хранения параметров запросов на изменение интервалов после успешной записи данных в базу.
    struct DeleteRequestData {
        int64_t t_start;
        int64_t archive_item_id;
        int layer;
    };
    struct UpdateRequestData {
        int64_t new_t_start;
        int64_t t_start;
        int64_t archive_item_id;
        int layer;
    };

    // Запрос к базе от клиента резервирования данных на запись данных и последующее обновление соответствующих
    // интервалов.
    struct SkippedIntervalRequest {
        ADD_MAKE_PTR(SkippedIntervalRequest)
        // Данные полученные от мастера (с указанием item-а и layer-а).
        std::vector<data::ResponseItem> response_items;

        // callback подтверждающий успешную запись для обновления кэша и запроса следующего интервала для архива.
        lib::function<void(bool)> on_success_write;

        // Раздёленные по слоям данные для временного хранения перед записью(В том числе для случая если запись не
        // удалась и ожидается переподключение).
        std::vector<std::pair<StoreLayer*, size_t>> store_layers;
        PinVector data;

        // Параметры запросов на изменение интервалов временна хранящиеся от момента разбора ответа мастера до момента
        // подтверждения успешной даписи данных.
        std::vector<DeleteRequestData> delete_requests;
        std::vector<UpdateRequestData> update_requests;
    };

}  // namespace mplc::archive
