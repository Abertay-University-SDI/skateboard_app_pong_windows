#pragma once

#include <Skateboard/Mathematics.h>


float4 colourHSVtoRGBA(float h, float s, float v)
{
	float i;
	while (h >= 360.0f)
		h -= 360.0f;
	h /= 60.f;
	i = (float)floor(h);
	float f = h - i;
	float p = v * (1 - s);
	float q = v * (1 - (s * f));
	float t = v * (1 - (s * (1 - f)));
	switch ((int)i)
	{
	case 0: return float4(v, t, p, 1.f);
	case 1: return float4(q, v, p, 1.f);
	case 2: return float4(p, v, t, 1.f);
	case 3: return float4(p, q, v, 1.f);
	case 4: return float4(t, p, v, 1.f);
	case 5: return float4(v, p, q, 1.f);
	}
	return float4();
}
