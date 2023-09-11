#include "cbor.h"

const CHAR _G_prefix[] = "\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t\t";

struct MYC : public CBOR 
{
	int _M_Level;
	BOOLEAN _M_bNamed = FALSE;

	MYC(int Level = 0) : _M_Level(Level) {}

	PCSTR prefix()
	{
		if (_M_bNamed)
		{
			_M_bNamed = FALSE;
			return "";
		}
		return &_G_prefix[_countof(_G_prefix) - 1 - _M_Level];
	}

	virtual BOOL OnUint(ULONG64 i)
	{
		DbgPrint("%s%I64x\n", prefix(), i);

		return TRUE;
	}

	virtual BOOL OnInt(LONG64 i)
	{
		DbgPrint("%s-%I64x\n", prefix(), -i);
		return TRUE;
	}

	virtual BOOL OnBinary(PBYTE pb, ULONG cb)
	{
		DumpBytes(prefix(), pb, cb, CRYPT_STRING_HEXRAW|CRYPT_STRING_NOCRLF);
		return TRUE;
	}

	virtual BOOL OnString(PCSTR psz, ULONG len)
	{
		DbgPrint("%s\"%.*s\"\n", prefix(), len, psz);
		return TRUE;
	}

	virtual BOOL BeginArray(ULONG n)
	{
		DbgPrint("%s[ // [%x]\n", prefix(), n);
		_M_Level++;
		return TRUE;
	}

	virtual BOOL BeginMap(ULONG n)
	{
		DbgPrint("%s{ // [%x]\n", prefix(), n);
		_M_Level++;
		return TRUE;
	}

	virtual BOOL OnSpec(T7 v)
	{
		PCSTR pref = prefix();
		switch (v)
		{
		case t_false:
			DbgPrint("%sfalse\n", pref);
			break;
		case t_true:
			DbgPrint("%strue\n", pref);
			break;
		case t_null:
			DbgPrint("%snull\n", pref);
			break;
		case t_undefined:
			DbgPrint("%sundefined\n", pref);
			break;
		}
		return TRUE;
	}

	virtual BOOL EndArray()
	{
		_M_Level--;
		DbgPrint("%s]\n", prefix());
		return TRUE;
	}

	virtual BOOL EndMap()
	{
		_M_Level--;
		DbgPrint("%s}\n", prefix());
		return TRUE;
	}

	virtual BOOL GetItem(PCSTRING name, ULONG /*i*/, CBOR** pp)
	{
		if (name)
		{
			if (MAXUSHORT+(ULONG_PTR)name->Buffer < 2*MAXUSHORT)
			{
				DbgPrint("%s%d : ", prefix(), (ULONG)(ULONG_PTR)name->Buffer);
			}
			else
			{
				DbgPrint("%s\"%Z\" : ", prefix(), name);
			}

			_M_bNamed = TRUE;
		}

		*pp = this;
		return TRUE;
	}
};
