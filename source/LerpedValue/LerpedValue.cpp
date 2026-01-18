#include "LerpedValue.h"


void LerpedValue_float::Set(const float& real) {
	this->real = real;
}
void LerpedValue_float::SetAll(const float& value) {
	this->real = value;
	this->lerped = value;
}


float& LerpedValue_float::GetLerped() {
	return lerped;
}
float& LerpedValue_float::GetReal() {
	return real;
}

void LerpedValue_float::Update(const float& time) {
	lerped = lerped + time * (real - lerped);
}

void LerpedValue_float3::Set(const XMVECTOR& real) {
	this->real = real;
}
void LerpedValue_float3::SetAll(const XMVECTOR& value) {
	this->real = value;
	this->lerped = value;
}

XMVECTOR& LerpedValue_float3::GetLerped() {
	return lerped;
}
XMVECTOR& LerpedValue_float3::GetReal() {
	return real;
}

void LerpedValue_float3::Update(const float& time ) {

	lerped = lerped + time * XMVectorSubtract(real, lerped);
}