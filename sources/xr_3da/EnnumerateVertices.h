#pragma once

struct SEnumVerticesCallback
{
	virtual void operator () (const fVector3& p) = 0;
};
