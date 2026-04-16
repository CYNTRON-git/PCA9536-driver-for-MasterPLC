#ifndef __INS___COMM_H__
#define __INS___COMM_H__

#include "fb.h"

typedef struct
{
	RBOOLEAN m_bLeadEdge;
	RBOOLEAN m_bCurrentValue;
}LeadEdge;


typedef struct
{
	RBOOLEAN m_bBackEdge;
	RBOOLEAN m_bCurrentValue;
}BackEdge;

extern void initialize_leadedge(LeadEdge PTR le, RBOOLEAN initVal);
extern void putvalue_leadedge(LeadEdge PTR le, RBOOLEAN newVal);
extern RBOOLEAN getvalue_leadedge(LeadEdge PTR le);
extern void initialize_backedge(BackEdge PTR be);
extern void putvalue_backedge(BackEdge PTR be, RBOOLEAN newVal);
extern RBOOLEAN getvalue_backedge(BackEdge PTR be);

#endif //__INS___COMM_H__
