#pragma once
#include <share/mplcshare.h>
#include <rapidjson/filewritestream.h>

struct Right {
    enum Type { OpenWindow, Control, ChangeUsers, ChangeSelfPassword, BlockUser, Login, ReadUsers };
    static Type from_string(const std::string& str) {
        static std::map<std::string, Type> map;
        if (map.empty()) {
            map["OpenWindow"] = OpenWindow;
            map["Control"] = Control;
            map["ChangeUsers"] = ChangeUsers;
            map["ChangeSelfPassword"] = ChangeSelfPassword;
            map["BlockUsers"] = BlockUser;
            map["Login"] = Login;
            map["ReadUsers"] = ReadUsers;
        }
        return map[str];
    }

    enum Access { Enable = 0x1, Disable = 0x2, Confirm = 0x4, Log = 0x8 };
    int access;
    void SetAccess(const Value& node) {
        access = 0;
        if (GetSafeBoolValue(node, "Enable")) {
            access |= Right::Enable;
        }
        if (GetSafeBoolValue(node, "Disable")) {
            access |= Right::Disable;
        }
        if (GetSafeBoolValue(node, "Confirmation")) {
            access |= Right::Confirm;
        }
        if (GetSafeBoolValue(node, "WriteLog")) {
            access |= Right::Log;
        }
    }
};
typedef JsonTextStream<rapidjson::FileWriteStream>::type JsonFileWriter;

class ControlRight {
public:
    enum Type { ObjectRight, WindowRight, ElementRight, UnknownType } type;
    int64_t object_id;
    std::string window_name;
    std::string element_name;
    std::string right_name;
    int32_t right_flags;
    ControlRight(): type(ObjectRight), object_id(0), right_flags(0) {}

    void SetType() {
        if (window_name.empty() && element_name.empty())
            type = ObjectRight;
        else if (!window_name.empty()) {
            if (element_name.empty())
                type = WindowRight;
            else
                type = ElementRight;
        } else {
            type = UnknownType;
        }
    }
    ControlRight(const Value& node) {
        object_id = GetSafeInt64Value(node, "object_id");
        window_name = GetSafeStringValue(node, "window_name");
        element_name = GetSafeStringValue(node, "element_name");
        right_name = GetSafeStringValue(node, "right_name");
        right_flags = GetSafeIntValue(node, "right_flags");
        SetType();
    };

    template<class T>
    void to_json(T& writer) const {
        writer.StartObject();
        writer.Key("object_id");
        writer.Int64(object_id);
        writer.Key("element_name");
        writer.String(element_name);
        writer.Key("window_name");
        writer.String(window_name);
        writer.Key("right_name");
        writer.String(right_name);
        writer.Key("right_flags");
        writer.Int(right_flags);
        writer.EndObject();
    }

    ControlRight(int64_t _object_id,
                 const std::string& _window_name,
                 const std::string& _element_name,
                 const std::string& _right_name,
                 int32_t _right_flags)
        : object_id(_object_id), window_name(_window_name), element_name(_element_name), right_name(_right_name),
          right_flags(_right_flags) {
        SetType();
    };
    friend bool operator==(const ControlRight& left, const ControlRight& right) {  // for FB
        bool result = (left.object_id == right.object_id && left.window_name == right.window_name &&
                       left.element_name == right.element_name && left.right_name == right.right_name);
        return result;
    }
};
