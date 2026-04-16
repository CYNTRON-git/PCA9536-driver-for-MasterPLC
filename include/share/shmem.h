#pragma once

const int NodeNameLen = 32;
const int NodeDataLen = 32;

const int SharedMemMemBlockSize = 2048;


// @todo Переделать под OpcUaTypes
enum ShMemType
{
    ShMemType_Struct,
    ShMemType_Int,
    ShMemType_String
};

struct NodeDsc
{
    NodeDsc(const char* name, ShMemType type)
        : Parent(0), Data{} {
        if (strlen(name) > NodeNameLen)
            throw;
        strcpy(Name, name);
        Type = type;
        Childs = nullptr;
        ChildsSize = 0;
        //Data = nullptr;
    }

    char Name[NodeNameLen];
    ShMemType Type;
    //NodeDsc* Parent;
    int Parent;
    int ChildsSize;
    NodeDsc** Childs;
    //void* Data;
    // @todo Пока статический размер. Потом, может быть, надо переделать на динамическое выделение
    char Data[NodeDataLen];
};


class BaseShMem
{
protected:
    unsigned int CurrentSize;
    unsigned int FreeSize;
    HANDLE hMapFile;
};


class ShMemModel : public BaseShMem
{

    int Counter;
    NodeDsc* _Begin;
    NodeDsc* _Nodes;
    typedef std::map<int, NodeDsc*> NodeDscMapType;
    NodeDscMapType NodeDscMap;

    void AddRoot(const char* name);

    public:
    MPLCSHARE_API ShMemModel(const char* name);

    MPLCSHARE_API int Add(int parent, const char* name, const ShMemType type = ShMemType_Struct);
    MPLCSHARE_API HANDLE Create(const char* name);
    MPLCSHARE_API void Update(int id, void* val);

};
