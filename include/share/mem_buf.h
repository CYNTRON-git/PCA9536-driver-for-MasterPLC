#pragma once

const int BinaryStringallocSize = 100;

class WeakMemBuf {

protected:
    OpcUa_Byte* _Buf;

    void _Clear() {
        CurrentLength = AllocatedLength = 0;
        _Buf = CurrentBufPtr = nullptr;
    }

    public:
    OpcUa_Byte* CurrentBufPtr;
    OpcUa_Int32 CurrentLength;
    OpcUa_Int32 AllocatedLength;

    WeakMemBuf() {
        _Clear();
    }


    void Clear() {
        if (_Buf) {
            free(_Buf);
            _Clear();
        }
    }

    void ReallocIfNeed(OpcUa_Int32 needSize) {
        OpcUa_Int32 needLen = CurrentLength + needSize;
        if (needLen > AllocatedLength) {
            OpcUa_Int32 newAllocatedSize;
            if (needLen < AllocatedLength + BinaryStringallocSize)
                newAllocatedSize = AllocatedLength + BinaryStringallocSize;
            else
                newAllocatedSize = needLen;
            OpcUa_Byte* new_Buf;
            if ((new_Buf = (OpcUa_Byte*)realloc(_Buf, newAllocatedSize)) != nullptr) {
                _Buf = new_Buf;
                CurrentBufPtr = (_Buf + CurrentLength);
                memset(CurrentBufPtr, 0, newAllocatedSize - AllocatedLength);
                AllocatedLength = newAllocatedSize;
            }
        }
    }

    void AddOpcUa_String(const char* str) {
        OpcUa_Int32 str_len;
        int curFieldSize = sizeof(str_len);
        ReallocIfNeed(curFieldSize);
        if (str != nullptr) {
            str_len = strlen(str);
            PUT_UNALIGNED_DWORD(CurrentBufPtr, str_len);
            CurrentLength += curFieldSize;
            CurrentBufPtr += curFieldSize;
            ReallocIfNeed(str_len);
            memcpy((CurrentBufPtr), str, str_len);
            CurrentLength += str_len;
            CurrentBufPtr += str_len;
            curFieldSize += str_len;
        } else {
            str_len = -1;
            PUT_UNALIGNED_DWORD(CurrentBufPtr, str_len);
            CurrentLength += curFieldSize;
            CurrentBufPtr += curFieldSize;
        }
    }

    OpcUa_Byte* begin() {
        return _Buf;
    }
};


class MemBuf : public WeakMemBuf {

public:
    ~MemBuf() {
        Clear();
    }
};
