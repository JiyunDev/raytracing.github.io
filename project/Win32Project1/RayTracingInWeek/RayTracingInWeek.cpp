// Win32Project1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Vector.h"
#include "Ray.h"
#include <iostream>

class hitable;

vec3 lerp(vec3 v1, vec3 v2, float t)
{
	return (1.0f - t)*v1 + t*v2;
}

vec3 color(const ray& r)//, hitable *world, int depth)
{
	vec3 unit_dir = unit_vector(r.get_direction());
	//unit vector element is in range [-1, 1]
	float t = 0.5f*(unit_dir.y() + 1.0f);
	return lerp(vec3(1.0f, 1.0f, 1.0f), vec3(0.5f, 0.7f, 1.0f), t);
}

int main() {
	//fix the screen size for now
	int nx = 200;
	int ny = 100;
	std::cout << "P3\n" << nx << " " << ny << "\n255\n";

	//define screenspace

	vec3 lower_left_corner(-2.0, -1.0, -1.0);
	vec3 horizontal(4.0, 0.0, 0.0);
	vec3 vertical(0.0, 2.0, 0.0);
	vec3 origin(0.0, 0.0, 0.0);

	for (int j = ny - 1; j >= 0; j--) {
		for (int i = 0; i < nx; i++) {
			float u = float(i) / float(nx);
			float v = float(j) / float(ny);

			ray r(origin, lower_left_corner + u*horizontal + v*vertical);
			vec3 col = color(r);
			
			//scale back to color [0, 255]
			int ir = int(255.99*col[0]);
			int ig = int(255.99*col[1]);
			int ib = int(255.99*col[2]);

			std::cout << ir << " " << ig << " " << ib <<"\n";
		}
	}
}