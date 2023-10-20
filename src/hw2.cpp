#include "hw2.h"
#include "hw2_scenes.h"

using namespace hw2;

bool is_inside_triangle(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p) {
    // Compute the edge vectors of the triangle
    Vector2 e01 = p1 - p0;
    Vector2 e12 = p2 - p1;
    Vector2 e20 = p0 - p2;

    // Rotate each edge vector by 90 degrees to obtain the normal vectors
    Vector2 n01(e01.y, -e01.x);
    Vector2 n12(e12.y, -e12.x);
    Vector2 n20(e20.y, -e20.x);

    // Compute dot products
    float d1 = dot(p - p0, n01);
    float d2 = dot(p - p1, n12);
    float d3 = dot(p - p2, n20);

    // Check if the point lies in the intersection of all positive or all negative half-planes
    return (d1 >= 0 && d2 >= 0 && d3 >= 0) || (d1 <= 0 && d2 <= 0 && d3 <= 0);
}


Image3 hw_2_1(const std::vector<std::string> &params) {
    // Homework 2.1: render a single 3D triangle

    Image3 img(640 /* width */, 480 /* height */);

    Vector3 p0{0, 0, -1};
    Vector3 p1{1, 0, -1};
    Vector3 p2{0, 1, -1};
    Real s = 1; // scaling factor of the view frustrum
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    Real z_near = 1e-6; // distance of the near clipping plane
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-p0") {
            p0.x = std::stof(params[++i]);
            p0.y = std::stof(params[++i]);
            p0.z = std::stof(params[++i]);
        } else if (params[i] == "-p1") {
            p1.x = std::stof(params[++i]);
            p1.y = std::stof(params[++i]);
            p1.z = std::stof(params[++i]);
        } else if (params[i] == "-p2") {
            p2.x = std::stof(params[++i]);
            p2.y = std::stof(params[++i]);
            p2.z = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        }
    }

    auto project = [](const Vector3 &p) -> Vector2 {
        return { -p.x / p.z, -p.y / p.z };
    };

    // Function to transform the projected point to screen space
    auto toScreenSpace = [&](const Vector2 &p) -> Vector2 {
        float w = img.width;
        float h = img.height;
        float aspect_ratio = w / h;

        return { w * (p.x + aspect_ratio * s) / (2 * aspect_ratio * s),
                 h * (1 - (p.y + s) / (2 * s)) };  // y-axis is flipped
    };

    // Project the 3D triangle vertices to 2D
    Vector2 p0_projected = project(p0);
    Vector2 p1_projected = project(p1);
    Vector2 p2_projected = project(p2);

    // Convert the projected vertices to screen space
    Vector2 p0_screen = toScreenSpace(p0_projected);
    Vector2 p1_screen = toScreenSpace(p1_projected);
    Vector2 p2_screen = toScreenSpace(p2_projected);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
                img(x, y) = Vector3{0.5, 0.5, 0.5};
                // Check if the pixel is inside the triangle
                if (is_inside_triangle(p0_screen, p1_screen, p2_screen, {x, y})) {
                    img(x, y) = color;
                }

        }
    }
    return img;
}

Image3 hw_2_2(const std::vector<std::string> &params) {
    // Homework 2.2: render a triangle mesh

    Image3 img(640 /* width */, 480 /* height */);

    Real s = 1; // scaling factor of the view frustrum
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }

    TriangleMesh mesh = meshes[scene_id];
    UNUSED(mesh); // silence warning, feel free to remove this

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}

Image3 hw_2_3(const std::vector<std::string> &params) {
    // Homework 2.3: render a triangle mesh with vertex colors

    Image3 img(640 /* width */, 480 /* height */);

    Real s = 1; // scaling factor of the view frustrum
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }

    TriangleMesh mesh = meshes[scene_id];
    UNUSED(mesh); // silence warning, feel free to remove this

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}

Image3 hw_2_4(const std::vector<std::string> &params) {
    // Homework 2.4: render a scene with transformation
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.camera.resolution.x,
               scene.camera.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{1, 1, 1};
        }
    }
    return img;
}

