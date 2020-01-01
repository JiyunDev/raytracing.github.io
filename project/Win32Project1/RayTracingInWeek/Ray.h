#pragma once

#include "Vector.h"

class ray 
{
public:
	ray() {}
	ray(const vec3& org, const vec3 dir) { origin = org; direction = dir; };

	vec3 get_origin() const { return origin; }
	vec3 get_direction() const { return direction; }
	vec3 point_at(float t) const { return origin + t*direction; }

private:
	vec3 origin;
	vec3 direction;
};