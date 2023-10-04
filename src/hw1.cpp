#include "hw1.h"
#include "hw1_scenes.h"

using namespace hw1;
// TODO: office hours question: < or <= for determining whether a pixel is inside a shape
// TODO: office hours question: the gray on my end is not the same as the gray on the pdf
// TODO: office hours question: for hw1_1, do we allow to make Circle objects so that we can use the helper functions?

// Helper functions to check whether a pixel is inside a shape
bool is_inside_circle(const Vector2 &point, const Circle &circle) {
    Real distance = length(point - circle.center);
    return distance < circle.radius;
}

bool is_inside_rectangle(const Vector2 &point, const Rectangle &rectangle) {
    return (point.x >= rectangle.p_min.x) && (point.x <= rectangle.p_max.x) &&
           (point.y >= rectangle.p_min.y) && (point.y <= rectangle.p_max.y);
}

bool is_inside_triangle(const Vector2 &point, const Triangle &triangle) {
    // Compute the edge vectors of the triangle
    Vector2 e01 = triangle.p1 - triangle.p0;
    Vector2 e12 = triangle.p2 - triangle.p1;
    Vector2 e20 = triangle.p0 - triangle.p2;

    // Rotate each edge vector by 90 degrees to obtain the normal vectors
    Vector2 n01(e01.y, -e01.x);
    Vector2 n12(e12.y, -e12.x);
    Vector2 n20(e20.y, -e20.x);

    // Compute dot products
    float d1 = dot(point - triangle.p0, n01);
    float d2 = dot(point - triangle.p1, n12);
    float d3 = dot(point - triangle.p2, n20);

    // Check if the point lies in the intersection of all positive or all negative half-planes
    return (d1 >= 0 && d2 >= 0 && d3 >= 0) || (d1 <= 0 && d2 <= 0 && d3 <= 0);
}


Image3 hw_1_1(const std::vector<std::string> &params) {
    // Homework 1.1: render a circle at the specified
    // position, with the specified radius and color.

    Image3 img(640 /* width */, 480 /* height */);


    Vector2 center = Vector2{img.width / 2 + Real(0.5), img.height / 2 + Real(0.5)};
    Real radius = 100.0;
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-center") {
            Real x = std::stof(params[++i]);
            Real y = std::stof(params[++i]);
            center = Vector2{x, y};
        } else if (params[i] == "-radius") {
            radius = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        }
    }

    Vector3 background_color = {0.5, 0.5, 0.5};

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector2 pixel_center = Vector2{x + Real(0.5), y + Real(0.5)};

            Circle circle_info = {center, radius, color, 1.0, Matrix3x3()}; // dummy values for the missing Circle fields

            if (is_inside_circle(pixel_center, circle_info)) {
                img(x, y) = color;
            } else {
                img(x, y) = background_color;
            }
        }
    }
    return img;
}



Image3 hw_1_2(const std::vector<std::string> &params) {
    // Homework 1.2: render multiple circles
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    int scene_id = std::stoi(params[0]);
    const CircleScene &scene = hw1_2_scenes[scene_id];

    Image3 img(scene.resolution.x, scene.resolution.y);

    // fill the image with the background color
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background;
        }
    }

    // for bonus points, by box bounding, reduce unnecessary pixel calculations
    for (const Circle &circle : scene.objects) {
        Vector2 bounding_box_min = circle.center - Vector2{circle.radius, circle.radius};
        Vector2 bounding_box_max = circle.center + Vector2{circle.radius, circle.radius};

        int x_start = std::max(0, static_cast<int>(bounding_box_min.x));
        int x_end = std::min(img.width, static_cast<int>(bounding_box_max.x));
        int y_start = std::max(0, static_cast<int>(bounding_box_min.y));
        int y_end = std::min(img.height, static_cast<int>(bounding_box_max.y));

        for (int y = y_start; y <= y_end; y++) {
            for (int x = x_start; x <= x_end; x++) {
                Vector2 pixel_center = Vector2{x + Real(0.5), y + Real(0.5)};

                if (is_inside_circle(pixel_center, circle)) {
                    img(x, y) = circle.color;
                }
            }
        }
    }
    return img;
}



Image3 hw_1_3(const std::vector<std::string> &params) {
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector2 pixel_point(x + Real(0.5), y + Real(0.5));
            Vector3 pixel_color = scene.background;

            for (const auto& shape : scene.shapes) {
                if (auto *circle = std::get_if<Circle>(&shape)) {
                    if (is_inside_circle(pixel_point, *circle)) {
                        pixel_color = circle->color;
                    }
                } else if (auto *rectangle = std::get_if<Rectangle>(&shape)) {
                    if (is_inside_rectangle(pixel_point, *rectangle)) {
                        pixel_color = rectangle->color;
                    }
                } else if (auto *triangle = std::get_if<Triangle>(&shape)) {
                    if (is_inside_triangle(pixel_point, *triangle)) {
                        pixel_color = triangle->color;
                    }
                }
            }
            img(x, y) = pixel_color;
        }
    }

    return img;
}



Image3 hw_1_4(const std::vector<std::string> &params) {
    // Homework 1.4: render transformed shapes
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}



Image3 hw_1_5(const std::vector<std::string> &params) {
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    int samples = 4; // 4x4 sampling pattern
    Image3 img(scene.resolution.x * samples, scene.resolution.y * samples);  // Upsample the image by 4x

    for (int base_y = 0; base_y < scene.resolution.y; base_y++) {
        for (int base_x = 0; base_x < scene.resolution.x; base_x++) {

            for (int sub_y = 0; sub_y < samples; sub_y++) {
                for (int sub_x = 0; sub_x < samples; sub_x++) {

                    int x = base_x * samples + sub_x;
                    int y = base_y * samples + sub_y;

                    Vector2 pixel_point(base_x + (sub_x + Real(0.5)) / samples,
                                        base_y + (sub_y + Real(0.5)) / samples);

                    bool set_color = false;

                    for (const auto& shape : scene.shapes) {
                        if (auto *circle = std::get_if<Circle>(&shape)) {
                            if (is_inside_circle(pixel_point, *circle)) {
                                img(x, y) = circle->color;
                                set_color = true;
                                break;
                            }
                        } else if (auto *rectangle = std::get_if<Rectangle>(&shape)) {
                            if (is_inside_rectangle(pixel_point, *rectangle)) {
                                img(x, y) = rectangle->color;
                                set_color = true;
                                break;
                            }
                        } else if (auto *triangle = std::get_if<Triangle>(&shape)) {
                            if (is_inside_triangle(pixel_point, *triangle)) {
                                img(x, y) = triangle->color;
                                set_color = true;
                                break;
                            }
                        }
                    }

                    if (!set_color) {
                        img(x, y) = scene.background;
                    }
                }
            }
        }
    }
    return img;
}






Image3 hw_1_6(const std::vector<std::string> &params) {
    // Homework 1.6: alpha blending
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}
