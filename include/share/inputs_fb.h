#pragma once
// clang-format off
#include <boost/unordered_map.hpp>
struct FBInput
{
	//std::string type;
	OpcUa_VariantHlp val;
};
typedef boost::unordered_map<std::string, OpcUa_VariantHlp> lua_vars;
/* --------- Available Types -----------*/

struct MPLCSHARE_API ParamType {
    enum type {
        __None,        
        __BOOL,        __BYTE,        
        __REAL,        __LREAL,     __DOUBLE,
        __DATE,        __DT,        __TIME,         __TOD,
        __UINT,        __UDINT,     __ULINT,        __USINT,        
        __DINT,        __INT,       __LINT,         __SINT,
        __DWORD,       __LWORD,     __WORD,        __STRING,
        __VARIANT,     __BYTE_STRING,
    };
    AddBaseEnumFunctions(ParamType, type, __None)
    const char* GetLuaType();
    OpcUa_BuiltInTypeHlp GetUaType();
};
struct fb_input_meta {
	const char* name;
	const int type;
};

class MPLCSHARE_API InputsFB {
    lua_vars* inputs;
    void AddInput(const std::string &name, const OpcUa_BuiltInTypeHlp& type);
protected:
    void LoadInputs(const fb_input_meta *inputs);
    int ReadFrom(lua_State *L, FBData& data);
	int WriteTo(lua_State *L, FBData& data) const;
    OpcUa_VariantHlp* GetFBInput(const std::string& name);
    const OpcUa_VariantHlp* GetFBInput(const std::string& name) const;
public:
    InputsFB();
    virtual ~InputsFB();
    OpcUa_VariantHlp GetInput(const std::string &src) const;
    OpcUa_StatusCode GetByteString(const std::string& src, OpcUa_ByteString& val) const;

    std::string GetString(const std::string &src) const ;
	std::string GetCP1251String(const std::string &src) const;
	double      GetDouble(const std::string &src, double defValue = 0) const;
	bool        GetBool(const std::string &src, bool defValue = false) const;
	int         GetInt(const std::string &src, int defValue = 0) const;
    int64_t     GetInt64(const std::string &src) const;
    
	void SetValue(const std::string &dst, int v);
	void SetValue(const std::string &dst, const std::string &v);
	void SetValue(const std::string &dst, double v);
	void SetValue(const std::string &dst, bool v);
	void SetValue(const std::string &dst, const OpcUa_VariantHlp& v);
	void SetValue(const std::string &dst, uint64_t v);
    void SetValue(const std::string &dst, int64_t v);
    void SetValue(const std::string &dst, const char* v);
	void SetValue(const std::string &dst, const OpcUa_DateTime& v);
    void SetByteStringValue(const std::string &dst, const OpcUa_ByteString& v);
    void SetByteStringValue(const std::string &dst, const char* s, int size);
};

