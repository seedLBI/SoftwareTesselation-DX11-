#ifndef LERPED_VALUE_H
#define LERPED_VALUE_H

#include <DirectXMath.h>
using namespace DirectX;


struct LerpedValue_float {
private:
	float real;
	float lerped;

public:
	void SetAll(const float& value);
	void Set(const float& real);
	float& GetLerped();
	float& GetReal();

	void Update(const float& time );
};

struct LerpedValue_float3 {
private:
	XMVECTOR real;
	XMVECTOR lerped;

public:
	void SetAll(const XMVECTOR& value);
	void Set(const XMVECTOR& real);
	XMVECTOR& GetLerped();
	XMVECTOR& GetReal();

	void Update(const float& time);
};




#endif