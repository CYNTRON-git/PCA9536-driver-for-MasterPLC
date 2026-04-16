#pragma once

namespace mplc {

    class JsonInfo {
    protected:
        virtual OpcUa_StatusCode load(const Value& json) = 0;

    public:
        OpcUa_StatusCode Parse(std::string file_name) {
            const std::string path = get_project_dir_utf8() + file_name;
            char* file_cache = nullptr;
            int size = 0;
            int64_t err_code;
            OpcUa_ReturnErrorIfBad(read_file(path, &file_cache, size, err_code));
            Document document;
            OpcUa_StatusCode status = parse_json(file_cache, document);
            if(OpcUa_IsGood(status)) status = load(document);
            delete[] file_cache;
            return status;
        }
    };
}  // namespace mplc
