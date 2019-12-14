#ifndef BOX_H
#define BOX_H
//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "common/rtweekend.h"
#include "aarect.h"
#include "hittable_list.h"


class box: public hittable  {
    public:
        box() {}

        box(const vec3& p0, const vec3& p1, material *ptr);

        virtual bool hit(const ray& r, double t0, double t1, hit_record& rec) const;

        virtual bool bounding_box(double t0, double t1, aabb& output_box) const {
            output_box = aabb(pmin, pmax);
            return true;
        }

        vec3 pmin, pmax;
        hittable *list_ptr;
};

box::box(const vec3& p0, const vec3& p1, material *ptr) {
    pmin = p0;
    pmax = p1;
    hittable_list *sides = new hittable_list();
    sides->add(new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), ptr));
    sides->add(new flip_normals(new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), ptr)));
    sides->add(new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), ptr));
    sides->add(new flip_normals(new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), ptr)));
    sides->add(new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), ptr));
    sides->add(new flip_normals(new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), ptr)));

    list_ptr = sides;
}

bool box::hit(const ray& r, double t0, double t1, hit_record& rec) const {
    return list_ptr->hit(r, t0, t1, rec);
}

#endif
