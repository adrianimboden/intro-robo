#pragma once

#ifndef __cplusplus
#error sorry, this header is c++ only
#endif

template <typename T, T* pObj>
class ScopedInitialize
{
public:
	template <typename... Args>
	ScopedInitialize(Args... args)
	{
		new (pObj)T();
	}

	~ScopedInitialize()
	{
		pObj->~T();
	}
};

#define SCOPED_INIT(obj) ScopedInitialize<decltype(obj), &obj> scopedInit##__COUNT__;
