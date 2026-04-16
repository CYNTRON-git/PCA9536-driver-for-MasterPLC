#ifndef STStandartFunctions_h
#define STStandartFunctions_h
#include "mplc_stdint.h"
// ST Libs
#undef LIMIT
#undef IN
#undef DELETE

extern int32_t TRUNC(double d);

extern int32_t REAL_TO_INT(double d);

extern int32_t WORD_BCD_TO_INT(int32_t word);

extern int32_t INT_TO_BCD_WORD(int32_t l);

extern int32_t ADD_PARAMS_ANY_INT(int l, int32_t* refs[], int32_t values[], int order[]);

extern double ADD_PARAMS_ANY_REAL(int l, int32_t* refs[], double values[], int order[]);

extern int32_t SUB(int32_t in1, int32_t in2);

extern double SUB_ANY_REAL(double in1, double in2);

extern int32_t DIV(int32_t l1, int32_t l2);

extern int32_t MOD(int32_t l1, int32_t l2);

extern uint64_t SHL(uint64_t in, int n);

extern uint64_t SHR(uint64_t in, int n);

//#ifdef INT64_SUPPORT
extern uint64_t ROR_LWORD(uint64_t in, int n);

extern uint64_t ROL_LWORD(uint64_t in, int n);
//#endif

extern uint32_t ROR_DWORD(uint32_t in, int n);

extern uint32_t ROL_DWORD(uint32_t in, int n);

extern uint16_t ROR_WORD(uint16_t in, int n);

extern uint16_t ROL_WORD(uint16_t in, int n);

extern uint8_t ROR_BYTE(uint8_t in, int n);

extern uint8_t ROL_BYTE(uint8_t in, int n);

extern uint32_t OR_PARAMS_LWORD(int l, uint32_t* refs[], uint32_t values[], int order[]);

extern uint8_t OR_PARAMS_BOOL(int l, uint8_t* refs[], uint8_t values[], int order[]);

extern uint8_t AND_PARAMS_BOOL(int l, uint8_t* refs[], uint8_t values[], int order[]);

extern uint32_t AND_PARAMS_LWORD(int l, uint16_t* refs[], uint32_t values[], int order[]);

extern uint32_t XOR_PARAMS_LWORD(int l, uint32_t* refs[], uint32_t values[], int order[]);

extern uint8_t XOR_PARAMS_BOOL(int l, uint8_t* refs[], uint8_t values[], int order[]);

extern uint32_t NOT_LWORD(uint32_t l);

extern uint8_t NOT_BOOL(uint8_t b);

extern int32_t MAX(int l, int32_t* refs[], int32_t values[], int order[]);

extern char* MAX_ANY_STRING(int l, char** refs[], char* values[], int order[]);

extern int32_t MIN(int l, int32_t* refs[], int32_t values[], int order[]);

extern char* MIN_ANY_STRING(int l, char** refs[], char* values[], int order[]);

extern int32_t LIMIT(int32_t mn, int32_t in, int32_t mx);

extern char* LIMIT_ANY_STRING(char* mn, char* in, char* mx);

extern uint8_t GT_ANY_STRING(int l, char** refs[], char* values[], int order[]);

extern uint8_t GE_ANY_STRING(int l, char** refs[], char* values[], int order[]);

extern uint8_t EQ_ANY_STRING(int l, char** refs[], char* values[], int order[]);

extern uint8_t LE_ANY_STRING(int l, char** refs[], char* values[], int order[]);

extern uint8_t LT_ANY_STRING(int l, char** refs[], char* values[], int order[]);

extern uint8_t NE_ANY_STRING(int l, char** refs[], char* values[], int order[]);

extern uint8_t GT(int l, int32_t* refs[], int32_t values[], int order[]);

extern uint8_t GE(int l, int32_t* refs[], int32_t values[], int order[]);

extern uint8_t EQ(int l, int32_t* refs[], int32_t values[], int order[]);

extern uint8_t LE(int l, int32_t* refs[], int32_t values[], int order[]);

extern uint8_t LT(int l, int32_t* refs[], int32_t values[], int order[]);

extern uint8_t NE(int l, int32_t* refs[], int32_t values[], int order[]);

extern int32_t LEN(const char* s, size_t len);

//extern char* LEFT(char* s, int32_t l);

//extern char* RIGHT(char* in, int32_t l);

extern const char* MID(const char* str, int32_t len, int32_t u8_substr_pos, int32_t u8_substr_len, size_t* out_len);

//extern char* CONCAT(int l, char** refs[], char* values[], int order[]);

//extern char* INSERT(char* in1, char* in2, int32_t p);
//
//extern char* DELETE(char* in, int32_t l, int32_t p);

//extern char* REPLACE(char* in1, char* in2, int32_t l, int32_t p);

extern FILETIME MUL_TIME_ANY_NUM(FILETIME in1, double in2);

extern FILETIME DIV_TIME_ANY_NUM(FILETIME in1, double in2);

extern char* MUX_ANY_STRING(int K, int l, char** refs[], char* values[], int order[]);

extern int32_t MUX_ANY_INT(int K, int l, int32_t* refs[], int32_t values[], int order[]);

extern double MUX_ANY_REAL(int K, int l, double* refs[], double values[], int order[]);

extern uint32_t MUX_ANY_BIT(int K, int l, uint32_t* refs[], uint32_t values[], int order[]);

extern uint64_t SetBit(uint64_t in, int n, char v);

extern char GetBit(uint64_t in, int n);

#endif
