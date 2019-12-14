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
#include "common/camera.h"
#include "common/rtw_stb_image.h"
#include "common/texture.h"
#include "box.h"
#include "bvh.h"
#include "constant_medium.h"
#include "hittable_list.h"
#include "material.h"
#include "moving_sphere.h"
#include "sphere.h"

#include <iostream>


vec3 ray_color(const ray& r, hittable *world, int depth) {
    hit_record rec;
    if (depth <= 0 || !world->hit(r, 0.001, infinity, rec))
        return vec3(0,0,0);

    ray scattered;
    vec3 attenuation;
    vec3 emitted = rec.mat_ptr->emitted(rec.u, rec.v, rec.p);
    if (!rec.mat_ptr->scatter(r, rec, attenuation, scattered))
        return emitted;

    return emitted + attenuation * ray_color(scattered, world, depth-1);
}

hittable *earth() {
    int nx, ny, nn;
    //unsigned char *tex_data = stbi_load("tiled.jpg", &nx, &ny, &nn, 0);
    unsigned char *tex_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0);
    material *mat = new lambertian(new image_texture(tex_data, nx, ny));
    return new sphere(vec3(0, 0, 0), 2, mat);
}

hittable *two_spheres() {
    texture *checker = new checker_texture(
        new constant_texture(vec3(0.2,0.3, 0.1)), new constant_texture(vec3(0.9, 0.9, 0.9)));

    hittable_list *objects = new hittable_list();

    objects->add(new sphere(vec3(0,-10, 0), 10, new lambertian(checker)));
    objects->add(new sphere(vec3(0, 10, 0), 10, new lambertian(checker)));

    return objects;
}

hittable *final() {
    int nb = 20;

    hittable_list *boxes1 = new hittable_list();
    material *ground = new lambertian(new constant_texture(vec3(0.48, 0.83, 0.53)));

    for (int i = 0; i < nb; i++) {
        for (int j = 0; j < nb; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1,101);
            auto z1 = z0 + w;
            boxes1->add(new box(vec3(x0,y0,z0), vec3(x1,y1,z1), ground));
        }
    }

    hittable_list *objects = new hittable_list();

    objects->add(new bvh_node(boxes1, 0, 1));

    material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));
    objects->add(new xz_rect(123, 423, 147, 412, 554, light));

    vec3 center(400, 400, 200);
    objects->add(new moving_sphere(
        center, center+vec3(30, 0, 0), 0, 1, 50,
        new lambertian(new constant_texture(vec3(0.7, 0.3, 0.1)))
    ));

    objects->add(new sphere(vec3(260, 150, 45), 50, new dielectric(1.5)));
    objects->add(new sphere(vec3(0, 150, 145), 50, new metal(vec3(0.8, 0.8, 0.9), 10.0)));

    hittable *boundary = new sphere(vec3(360, 150, 145), 70, new dielectric(1.5));
    objects->add(boundary);
    objects->add(new constant_medium(boundary, 0.2, new constant_texture(vec3(0.2, 0.4, 0.9))));
    boundary = new sphere(vec3(0, 0, 0), 5000, new dielectric(1.5));
    objects->add(new constant_medium(boundary, .0001, new constant_texture(vec3(1,1,1))));

    int nx, ny, nn;
    unsigned char *tex_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0);
    material *emat = new lambertian(new image_texture(tex_data, nx, ny));
    objects->add(new sphere(vec3(400,200, 400), 100, emat));
    texture *pertext = new noise_texture(0.1);
    objects->add(new sphere(vec3(220,280, 300), 80, new lambertian(pertext)));

    int ns = 1000;
    hittable_list *boxes2 = new hittable_list();
    material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
    for (int j = 0; j < ns; j++) {
        boxes2->add(new sphere(vec3::random(0,165), 10, white));
    }

    objects->add(new translate(
        new rotate_y(new bvh_node(boxes2, 0.0, 1.0), 15), vec3(-100,270,395)));

    return objects;
}

hittable *cornell_final() {
    hittable_list *objects = new hittable_list();

    texture *pertext = new noise_texture(0.1);

    int nx, ny, nn;
    unsigned char *tex_data = stbi_load("earthmap.jpg", &nx, &ny, &nn, 0);

    material *mat = new lambertian(new image_texture(tex_data, nx, ny));

    material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
    material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
    material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
    material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));

    objects->add(new flip_normals(new yz_rect(0, 555, 0, 555, 555, green)));
    objects->add(new yz_rect(0, 555, 0, 555, 0, red));
    objects->add(new xz_rect(123, 423, 147, 412, 554, light));
    objects->add(new flip_normals(new xz_rect(0, 555, 0, 555, 555, white)));
    objects->add(new xz_rect(0, 555, 0, 555, 0, white));
    objects->add(new flip_normals(new xy_rect(0, 555, 0, 555, 555, white)));

    hittable *boundary2 = new translate(
        new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), new dielectric(1.5)), -18),
        vec3(130,0,65)
    );
    objects->add(boundary2);
    objects->add(
        new constant_medium(boundary2, 0.2, new constant_texture(vec3(0.9, 0.9, 0.9))));

    return objects;
}

hittable *cornell_balls() {
    hittable_list *objects = new hittable_list();

    material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
    material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
    material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
    material *light = new diffuse_light(new constant_texture(vec3(5, 5, 5)));

    objects->add(new flip_normals(new yz_rect(0, 555, 0, 555, 555, green)));
    objects->add(new yz_rect(0, 555, 0, 555, 0, red));
    objects->add(new xz_rect(113, 443, 127, 432, 554, light));
    objects->add(new flip_normals(new xz_rect(0, 555, 0, 555, 555, white)));
    objects->add(new xz_rect(0, 555, 0, 555, 0, white));
    objects->add(new flip_normals(new xy_rect(0, 555, 0, 555, 555, white)));

    hittable *boundary = new sphere(vec3(160, 100, 145), 100, new dielectric(1.5));
    objects->add(boundary);

    objects->add(new constant_medium(boundary, 0.1, new constant_texture(vec3(1.0, 1.0, 1.0))));
    objects->add(new translate(
        new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white), 15),
        vec3(265,0,295)
    ));

    return objects;
}

hittable *cornell_smoke() {
    hittable_list *objects = new hittable_list();

    material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
    material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
    material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
    material *light = new diffuse_light(new constant_texture(vec3(7, 7, 7)));

    objects->add(new flip_normals(new yz_rect(0, 555, 0, 555, 555, green)));
    objects->add(new yz_rect(0, 555, 0, 555, 0, red));
    objects->add(new xz_rect(113, 443, 127, 432, 554, light));
    objects->add(new flip_normals(new xz_rect(0, 555, 0, 555, 555, white)));
    objects->add(new xz_rect(0, 555, 0, 555, 0, white));
    objects->add(new flip_normals(new xy_rect(0, 555, 0, 555, 555, white)));

    hittable *b1 = new translate(
        new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18),
        vec3(130,0,65)
    );

    hittable *b2 = new translate(
        new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white),  15),
        vec3(265,0,295)
    );

    objects->add(new constant_medium(b1, 0.01, new constant_texture(vec3(1.0, 1.0, 1.0))));
    objects->add(new constant_medium(b2, 0.01, new constant_texture(vec3(0.0, 0.0, 0.0))));

    return objects;
}

hittable *cornell_box() {
    hittable_list *objects = new hittable_list();

    material *red = new lambertian(new constant_texture(vec3(0.65, 0.05, 0.05)));
    material *white = new lambertian(new constant_texture(vec3(0.73, 0.73, 0.73)));
    material *green = new lambertian(new constant_texture(vec3(0.12, 0.45, 0.15)));
    material *light = new diffuse_light(new constant_texture(vec3(15, 15, 15)));

    objects->add(new flip_normals(new yz_rect(0, 555, 0, 555, 555, green)));
    objects->add(new yz_rect(0, 555, 0, 555, 0, red));
    objects->add(new xz_rect(213, 343, 227, 332, 554, light));
    objects->add(new flip_normals(new xz_rect(0, 555, 0, 555, 555, white)));
    objects->add(new xz_rect(0, 555, 0, 555, 0, white));
    objects->add(new flip_normals(new xy_rect(0, 555, 0, 555, 555, white)));

    objects->add(new translate(
        new rotate_y(new box(vec3(0, 0, 0), vec3(165, 165, 165), white), -18),
        vec3(130,0,65)
    ));

    objects->add(new translate(
        new rotate_y(new box(vec3(0, 0, 0), vec3(165, 330, 165), white),  15),
        vec3(265,0,295)
    ));

    return objects;
}

hittable *two_perlin_spheres() {
    hittable_list *objects = new hittable_list();

    texture *pertext = new noise_texture(4);
    objects->add(new sphere(vec3(0,-1000, 0), 1000, new lambertian(pertext)));
    objects->add(new sphere(vec3(0, 2, 0), 2, new lambertian(pertext)));

    return objects;
}

hittable *simple_light() {
    hittable_list *objects = new hittable_list();

    texture *pertext = new noise_texture(4);
    objects->add(new sphere(vec3(0,-1000, 0), 1000, new lambertian(pertext)));
    objects->add(new sphere(vec3(0,2,0), 2, new lambertian(pertext)));
    objects->add(
        new sphere(vec3(0,7,0), 2, new diffuse_light(new constant_texture(vec3(4,4,4)))));
    objects->add(
        new xy_rect(3, 5, 1, 3, -2, new diffuse_light(new constant_texture(vec3(4,4,4)))));

    return objects;
}

hittable *random_scene() {
    hittable_list *objects = new hittable_list();

    texture *checker = new checker_texture(
        new constant_texture(vec3(0.2, 0.3, 0.1)),
        new constant_texture(vec3(0.9, 0.9, 0.9))
    );

    objects->add(new sphere(vec3(0,-1000,0), 1000, new lambertian(checker)));

    for (int a = -10; a < 10; a++) {
        for (int b = -10; b < 10; b++) {
            auto choose_mat = random_double();
            vec3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());
            if ((center - vec3(4, .2, 0)).length() > 0.9) {
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = vec3::random() * vec3::random();
                    objects->add(new moving_sphere(
                        center, center + vec3(0, random_double(0,.5), 0), 0.0, 1.0, 0.2,
                        new lambertian(new constant_texture(albedo))
                    ));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = vec3::random(.5, 1);
                    auto fuzz = random_double(0, .5);
                    objects->add(new sphere(center, 0.2, new metal(albedo, fuzz)));
                } else {
                    // glass
                    objects->add(new sphere(center, 0.2, new dielectric(1.5)));
                }
            }
        }
    }

    objects->add(new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5)));
    objects->add(new sphere(
        vec3(-4, 1, 0), 1.0, new lambertian(new constant_texture(vec3(0.4, 0.2, 0.1)))));
    objects->add(new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0)));

    return new bvh_node(objects, 0.0, 1.0);
}

int main() {
    int nx = 600;
    int ny = 600;
    int num_samples = 100;
    int max_depth = 50;

    std::cout << "P3\n" << nx << ' ' << ny << "\n255\n";

    auto R = cos(pi/4);

    #define SCENE_ID 2

    #if SCENE_ID == 0
        hittable *world = random_scene();
    #elif SCENE_ID == 1
        hittable *world = two_spheres();
    #elif SCENE_ID == 2
        hittable *world = two_perlin_spheres();
    #elif SCENE_ID == 3
        hittable *world = earth();
    #elif SCENE_ID == 4
        hittable *world = simple_light();
    #elif SCENE_ID == 5
        hittable *world = cornell_box();
    #elif SCENE_ID == 6
        hittable *world = cornell_balls();
    #elif SCENE_ID == 7
        hittable *world = cornell_smoke();
    #elif SCENE_ID == 8
        hittable *world = cornell_final();
    #else
        hittable *world = final();
    #endif

    vec3 lookfrom(278, 278, -800);
    //vec3 lookfrom(478, 278, -600);
    vec3 lookat(278,278,0);
    //vec3 lookfrom(0, 0, 6);
    //vec3 lookat(0,0,0);
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;
    auto vfov = 40.0;

    camera cam(
        lookfrom, lookat, vec3(0,1,0), vfov, double(nx)/ny, aperture, dist_to_focus, 0.0, 1.0);

    for (int j = ny-1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < nx; ++i) {
            vec3 color;
            for (int s = 0; s < num_samples; ++s) {
                auto u = (i + random_double()) / nx;
                auto v = (j + random_double()) / ny;
                ray r = cam.get_ray(u, v);
                color += ray_color(r, world, max_depth);
            }
            color.write_color(std::cout, num_samples);
        }
    }

    std::cerr << "\nDone.\n";
}
