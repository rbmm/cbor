#include "stdafx.h"

#include "cbor.h"

struct CGetStr : public CBOR, CSTRING
{
	virtual BOOL OnUint(ULONG64 i)
	{
		if (i < MAXUSHORT)
		{
			Buffer = (PSTR)(ULONG_PTR)i;
			Length = 0;
			MaximumLength = 0;
			return TRUE;
		}
		return FALSE;
	}

	virtual BOOL OnInt(LONG64 i)
	{
		if (-i < MAXUSHORT)
		{
			Buffer = (PSTR)(ULONG_PTR)i;
			Length = 0;
			MaximumLength = 0;
			return TRUE;
		}
		return FALSE;
	}

	virtual BOOL OnString(PCSTR psz, ULONG len)
	{
		if (len < MAXUSHORT)
		{
			Buffer = psz;
			Length = (USHORT)len;
			MaximumLength = (USHORT)len;
			return TRUE;
		}
		return FALSE;
	}

	virtual BOOL OnBinary(PBYTE /*pb*/, ULONG /*cb*/)
	{
		return FALSE;
	}

	virtual BOOL BeginArray(ULONG /*n*/)
	{
		return FALSE;
	}

	virtual BOOL BeginMap(ULONG /*n*/)
	{
		return FALSE;
	}

	virtual BOOL GetItem(PCSTRING /*name*/, ULONG /*i*/, CBOR** pp)
	{
		*pp = this;
		return FALSE;
	}

	virtual BOOL OnSpec(T7 /*v*/)
	{
		return FALSE;
	}

	virtual BOOL EndArray()
	{
		return FALSE;
	}

	virtual BOOL EndMap()
	{
		return FALSE;
	}
};

union CBT 
{
	UCHAR value;
	struct {
		UCHAR len : 5;
		UCHAR type : 3;
	};
};

C_ASSERT(sizeof(CBT)==1);

PBYTE CBOR::decode(_In_reads_bytes_(cb) PBYTE pb, _In_ ULONG cb, _Out_ ULONG *pcb)
{
	union {
		ULONG64 v = 0;
		ULONG len;
		UCHAR bb[8];
	};

	if (!cb--)
	{
		return 0;
	}

	CBT cbt { *pb++ };

	if (23 < (len = cbt.len))
	{
		ULONG n;
		if (len > 27 || cb < (n = (1 << (len - 24))))
		{
			return 0;
		}

		cb -= n;

		PUCHAR q = bb + n;
		do 
		{
			*--q = *pb++;
		} while (--n);
	}

	*pcb = cb;

	CBOR* pm;

	switch (cbt.type)
	{
	case 0:
		return OnUint(v) ? pb : 0;
	case 1:
		return OnInt(-1 - v) ? pb : 0;

	case 2:
	case 3:
		if (cb < v)
		{
			return 0;
		}

		*pcb = cb - len;
		return (cbt.type == 2 ? OnBinary(pb, len) : OnString((PCSTR)pb, len)) ? pb + len : 0;

	case 5:
		if (v > 0x10000 || !BeginMap(len))
		{
			return 0;
		}

		if (v)
		{
			ULONG i = 0;
			CGetStr gstr;
			do 
			{
				if (!(pb = gstr.decode(pb, cb, &cb)) || 
					!GetItem(&gstr, i++, &pm) ||
					!(pb = pm->decode(pb, cb, &cb)))
				{
					return 0;
				}
			} while (--len);
		}
		*pcb = cb;

		return EndMap() ? pb : 0;

	case 4:

		if (v > 0x10000)
		{
			return 0;
		}

		if (!BeginArray(len))
		{
			return 0;
		}

		if (v)
		{
			ULONG i = 0;
			do 
			{
				if (!GetItem(0, i++, &pm) ||
					!(pb = pm->decode(pb, cb, &cb)))
				{
					return 0;
				}
			} while (--len);
		}
		*pcb = cb;

		return EndArray() ? pb : 0;

	case 6:
		return 0;
	case 7:
		switch (v - 20)
		{
		case 0:
			if (!OnSpec(t_false))
			{
				return 0;
			}
			break;
		case 1:
			if (!OnSpec(t_true))
			{
				return 0;
			}
			break;
		case 2:
			if (!OnSpec(t_null))
			{
				return 0;
			}
			break;
		case 3:
			if (!OnSpec(t_undefined))
			{
				return 0;
			}
			break;
		default:
			return 0;
		}
		return pb;
	}

	return 0;
}
