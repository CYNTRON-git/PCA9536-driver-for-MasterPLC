#ifndef _VM_TYPE_H_
#define _VM_TYPE_H_


//#define ELEMENTARY_METADATA_STRING "ElementaryMetadata"
//#define ARRAY_METADATA_STRING "ArrayMetadata"
//#define STRUCT_METADATA_STRING "StuctMetadata"
//
//const char LuaInternalStringTypeName[] = "string";
//const char LuaArrayTypeDefenitionKeyword[] = "oftype";
//
//typedef enum LuaTypes {
//    UnknownType = 0,
//    ElementaryType = 1,
//    ArrayType = 2,
//    StructType = 3,
//    FBType = 4,
//    ComplexType = 5
//} LuaTypes;
//
//class LuaType;
//
//class LuaVar {
//    const LuaType* _Type;
//    std::string _Name;
//    unsigned int _ID;
//
//public:
//    const char* Name(void) const { return _Name.c_str(); }
//
//    inline int Size() const;
//
//    unsigned int ID(void) const { return _ID; }
//    LuaVar(): _Type(nullptr), _ID(0) {}
//    LuaVar(const unsigned int ID, const std::string Name, const LuaType* Type) {
//        _ID = ID;
//        _Type = Type;
//        _Name = Name;
//    }
//    const LuaType* Type(void) const { return _Type; }
//};
//
//typedef std::map<unsigned int, LuaVar> LVMap;
//typedef std::map<std::string, LuaVar> LNVMap;
//
//class LuaType {
//    std::string _Name;
//    std::string _InternalName;
//    unsigned int _hash;
//    LuaTypes _Type;
//    int _Size;
//    std::map<std::string, LuaVar> _Fields;
//
//public:
//    /*	LuaType(void);
//        ~LuaType(void);*/
//    LuaType(std::string Name,
//            const std::string InternalName,
//            unsigned int Hash,
//            LuaTypes Type,
//            int size) {
//        _InternalName = InternalName;
//        _Name = Name;
//        _hash = Hash;
//        _Type = Type;
//        _Size = size;
//    }
//
//    int NumFields() const { return _Fields.size(); }
//
//    const LuaVar* GetField(const char* name) const {
//        std::map<std::string, LuaVar>::const_iterator it = _Fields.find(name);
//        return &it->second;
//    }
//
//    void AddField(const LuaVar field) {
//        std::map<std::string, LuaVar>::iterator it = _Fields.find(field.Name());
//        if(it != _Fields.end()) {
//            _Size -= it->second.Size();
//        } else {
//            if(_Fields.size() == 0)  // В силу каких то причин был создан простой тип
//            {
//                _Size = 0;
//            }
//        }
//        _Fields[field.Name()] = field;
//        _Size += field.Size();
//    }
//
//    const LuaTypes Type(void) const { return _Type; }
//
//    const std::string& InternalName(void) const { return _InternalName; }
//
//    const char* InternalNameChars(void) const { return _InternalName.c_str(); }
//
//    const std::string& Name() const { return _Name; }
//
//    inline int Size() const;
//
//    void Load() {}
//};
//
//inline int LuaType::Size() const { return _Size; }
//
//class LuaElementaryType : public LuaType {
//public:
//    LuaElementaryType(const std::string Name,
//                      const std::string InternalName,
//                      const unsigned int Hash,
//                      LuaTypes Type,
//                      int typeSize)
//        : LuaType(Name, InternalName, Hash, Type, typeSize) {}
//};
//
//typedef std::map<std::string, LuaType> LTMap;
//
///*class LuaElementaryMetadataType : public LuaType
//{
//public:
//    LuaElementaryMetadataType(void)
//    {
//
//    }
//};*/
//
//inline int LuaVar::Size() const { return _Type->Size(); }
//
//class LuaVarArray : public LuaVar {
//public:
//    LuaVarArray() {}
//};
//
//class LuaInfo {
//    void InitLuaTypes(void) {
//        LuaType LType(ELEMENTARY_METADATA_STRING,
//                      std::string(LuaInternalStringTypeName),
//                      0,
//                      ElementaryType,
//                      0);
//        LuaTypesMap.insert(std::pair<std::string, LuaType>(ELEMENTARY_METADATA_STRING, LType));
//
//        LuaTypesMap.insert(std::pair<std::string, LuaType>("", LuaType("", "", 0, UnknownType, 0)));
//    }
//
//public:
//    LTMap LuaTypesMap;
//    /// Описание пересенных (map через ID)
//    LVMap LuaVarsMap;
//    /// Описание пересенных (map через имя)
//    LNVMap LuaNameVarsMap;
//
//    std::map<std::string, LuaElementaryType> LuaETypeMap;
//
//    LuaInfo() {
//        // LuaETypeMap["ElementaryMetadata"]=
//        InitLuaTypes();
//    }
//
//    void Clear() {
//        LuaTypesMap.clear();
//        LuaVarsMap.clear();
//        LuaNameVarsMap.clear();
//        InitLuaTypes();
//    }
//};
//
//// Ппж надо будет объденить с LuaInfo
//class VMVariableInfo {
//    std::string _Name;
//    std::string _Type;
//    unsigned int _ID;
//    unsigned int _ParentID;
//    unsigned int _TaskIdx;
//    unsigned int _Access;
//    bool _IsArchived;
//
//public:
//    const char* Name(void) const { return _Name.c_str(); }
//
//    const char* Type(void) const { return _Type.c_str(); }
//
//    const unsigned int ID(void) const { return _ID; }
//    const unsigned int ParentID(void) const { return _ParentID; }
//
//    const unsigned int TaskIdx(void) const { return _TaskIdx; }
//
//    const bool IsArchived(void) const { return _IsArchived; }
//
//    const unsigned int Access(void) const { return _Access; }
//
//    VMVariableInfo(const unsigned int ID,
//                   const std::string Name,
//                   const std::string Type,
//                   const unsigned int ParentID,
//                   const unsigned int taskIdx,
//                   bool isArchived,
//                   unsigned int Access) {
//        _ID = ID;
//        _Name = Name;
//        _Type = Type;
//        _ParentID = ParentID;
//        _TaskIdx = taskIdx;
//        _IsArchived = isArchived;
//        _Access = Access;
//    }
//};

#endif  // _LUA_TYPE_H_
