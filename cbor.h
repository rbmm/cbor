#pragma once

struct CBOR 
{
	enum T7 { t_false, t_true, t_null, t_undefined };

	enum T : UCHAR {
		t_uint, t_int, t_bin, t_string, t_array, t_map, t_enum
	};

	PBYTE encode(_Out_writes_(cb) PBYTE /*pb*/, _In_ ULONG /*cb*/, _Out_ ULONG * /*pcb*/)
	{
		return 0;
	}

	PBYTE decode(_In_reads_bytes_(cb) PBYTE pb, _In_ ULONG cb, _Out_ ULONG *pcb);

	virtual BOOL OnUint(ULONG64 /*i*/)
	{
		return TRUE;
	}

	virtual BOOL OnInt(LONG64 /*i*/)
	{
		return TRUE;
	}

	virtual BOOL OnBinary(PBYTE /*pb*/, ULONG /*cb*/)
	{
		return TRUE;
	}

	virtual BOOL OnString(PCSTR /*psz*/, ULONG /*len*/)
	{
		return TRUE;
	}

	virtual BOOL BeginArray(ULONG /*n*/)
	{
		return TRUE;
	}

	virtual BOOL BeginMap(ULONG /*n*/)
	{
		return TRUE;
	}

	virtual BOOL GetItem(PCSTRING /*name*/, ULONG /*i*/, CBOR** pp)
	{
		*pp = this;
		return TRUE;
	}

	virtual BOOL OnSpec(T7 /*v*/)
	{
		return TRUE;
	}

	virtual BOOL EndArray()
	{
		return TRUE;
	}

	virtual BOOL EndMap()
	{
		return TRUE;
	}
};