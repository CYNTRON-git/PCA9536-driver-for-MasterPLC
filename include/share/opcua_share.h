#ifndef _OPCUA_SHARE_ 
#define _OPCUA_SHARE_ 
#define OPC_TIMEOUT 20000
#endif

/* Дефайн введен по причине:
Наблюдается неправильное использование функции OpcUa_String_StrLen -
if (Value.ArrayType == OpcUa_VariantArrayType_Scalar)
{
	OpcUa_String_Initialize(&(m_Value.Value.String));
	OpcUa_Int32 iLen = OpcUa_String_StrLen(&(Value.Value.String));
	if (iLen>0)
		OpcUa_String_StrnCpy(&(m_Value.Value.String), &(Value.Value.String), iLen);
}
else
Тут должно быть OpcUa_String_StrSize
Надо просмотреть все места использования OpcUa_String_StrLen.А то с русскими будут проблемы.Подозреваю там надо везде менять на  OpcUa_String_StrSize.
*/
#define InSAT_OpcUa_String_StrnCpy(a,b) OpcUa_String_StrnCpy(a, b, OpcUa_String_StrSize(b))
