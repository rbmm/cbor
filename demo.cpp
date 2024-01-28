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

//////////////////////////////////////////////////////////////////////////

void cbt()
{
	ULONG s = 0;
	LONG cb = 0;
	PBYTE buf = 0;

	UCHAR transactionId[] = {0x11,0x22,0x33};
	UCHAR ticket[] = {0x44,0x55,0x66};
	UCHAR cancellationId[] = {0x77,0x88,0x99};

	while (!encode_bin(encode_string(
		encode_int(encode_string(
		encode_enum(encode_string(
		encode_int(encode_string(
		encode_int(encode_string(
		encode_int(encode_string(
		encode_map(encode_string(
		encode_string(encode_string(
		encode_bin(encode_string(
		encode_bin(encode_string(
		encode_int(encode_string(
		encode_int(encode_string(
		encode_int(encode_string(
		encode_map(buf, &cb, 7), 
		&cb, "command"), &cb, -3), 
		&cb, "flags"), &cb, 0x400000),
		&cb, "timeout"), &cb, 0x240c8400),
		&cb, "transactionId"), &cb, sizeof(transactionId), transactionId),
		&cb, "ticket"), &cb, sizeof(ticket), ticket),
		&cb, "request"), &cb, "some string"),
		&cb, "webAuthNPara"),
		&cb, 6),
		&cb, "wnd"), &cb, 0x6088a),
		&cb, "userVerification"), &cb, 1),
		&cb, "attestationPreference"), &cb, 0),
		&cb, "requireResident"), &cb, CBOR::t_true),
		&cb, "attachment"), &cb, 0),
		&cb, "cancellationId"), &cb, sizeof(cancellationId), cancellationId) && !s)
	{
		buf = (PBYTE)alloca(s = cb = -cb);
	}

	MYC cbr;
	if (cbr.decode(buf, s, &s))
	{
		__nop();
	}
}

/************************************************************************/
/* 

{ // [7]
	"command" : -3
	"flags" : 400000
	"timeout" : 240c8400
	"transactionId" : 112233
	"ticket" : 445566
	"request" : "some string"
	"webAuthNPara" : { // [6]
		"wnd" : 6088a
		"userVerification" : 1
		"attestationPreference" : 0
		"requireResident" : true
		"attachment" : 0
		"cancellationId" : 778899
	}
}

*/
/************************************************************************/