#include "hw2.h"
#include "hw2_scenes.h"

using namespace hw2;

/**
 * This function checks if a point is inside a triangle.
 * @param p0 point 0 of the triangle
 * @param p1 point 1 of the triangle
 * @param p2 point 2 of the triangle
 * @param p point to be checked
 * @return true if the point is inside the triangle, false otherwise
 */
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

/**
 * This function projects a 3D point to 2D.
 * @param p 3D point to be projected
 * @return 2D point after projection
 */
Vector2 project(const Vector3 &p) {
    return { -p.x / p.z, -p.y / p.z };
}

/**
 * This function converts a point from normalized device coordinates to screen space.
 * @param p The point in normalized device coordinates
 * @param width The width of the screen
 * @param height The height of the screen
 * @param s The scaling factor of the view frustum
 * @return The point in screen space
 */
Vector2 toScreenSpace(const Vector2 &p, int width, int height, Real s) {
    float aspect_ratio = static_cast<float>(width) / height;

    return { width * (p.x + aspect_ratio * s) / (2 * aspect_ratio * s),
             height * (1 - (p.y + s) / (2 * s)) };  // y-axis is flipped
}

Vector3 barycentric_coordinates(const Vector2 &A, const Vector2 &B, const Vector2 &C, const Vector2 &P) {
    Real denom = (B.y - C.y) * (A.x - C.x) + (C.x - B.x) * (A.y - C.y);
    if (std::abs(denom) < 1e-6) {
        // Triangle is degenerate, return invalid barycentric coordinates
        return Vector3{-1, -1, -1};
    }

    Real b0 = ((B.y - C.y) * (P.x - C.x) + (C.x - B.x) * (P.y - C.y)) / denom;
    Real b1 = ((C.y - A.y) * (P.x - C.x) + (A.x - C.x) * (P.y - C.y)) / denom;
    Real b2 = 1.0 - b0 - b1;

    return Vector3{b0, b1, b2};
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


    // Project the 3D triangle vertices to 2D
    Vector2 p0_projected = project(p0);
    Vector2 p1_projected = project(p1);
    Vector2 p2_projected = project(p2);

    // Convert the projected vertices to screen space
    Vector2 p0_screen = toScreenSpace(p0_projected, img.width, img.height, s);
    Vector2 p1_screen = toScreenSpace(p1_projected, img.width, img.height, s);
    Vector2 p2_screen = toScreenSpace(p2_projected, img.width, img.height, s);

    const int samples = 4;

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector3 accumulated_color{0, 0, 0}; // Accumulator for the samples

            for (int i = 0; i < samples; i++) {
                for (int j = 0; j < samples; j++) {
                    // Calculate the sample's position
                    float sample_x = x + float(i + 0.5) / samples;
                    float sample_y = y + float(j + 0.5) / samples;

                    // Check if this sample is inside the triangle
                    if (is_inside_triangle(p0_screen, p1_screen, p2_screen, {sample_x, sample_y})) {
                        accumulated_color += color;
                    } else {
                        accumulated_color += Vector3{0.5, 0.5, 0.5}; // Default color
                    }
                }
            }

            // Average the samples and assign to pixel
            img(x, y) = accumulated_color / Real(samples * samples);
        }
    }
    return img;
}

Image3 hw_2_2(const std::vector<std::string> &params) {
    // Homework 2.2: render a triangle mesh
    const int AA_FACTOR = 4;
    const int SUPER_WIDTH = 640 * AA_FACTOR;
    const int SUPER_HEIGHT = 480 * AA_FACTOR;

    Image3 superImg(SUPER_WIDTH, SUPER_HEIGHT);

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

    for (int y = 0; y < SUPER_HEIGHT; y++) {
        for (int x = 0; x < SUPER_WIDTH; x++) {
            superImg(x, y) = Vector3{0.5, 0.5, 0.5};
        }
    }

    // Render each triangle
    for (const auto &face : mesh.faces) {
        Vector3 v0 = mesh.vertices[face[0]];
        Vector3 v1 = mesh.vertices[face[1]];
        Vector3 v2 = mesh.vertices[face[2]];

        Vector2 projected_v0 = toScreenSpace(project(v0), SUPER_WIDTH, SUPER_HEIGHT, s);
        Vector2 projected_v1 = toScreenSpace(project(v1), SUPER_WIDTH, SUPER_HEIGHT, s);
        Vector2 projected_v2 = toScreenSpace(project(v2), SUPER_WIDTH, SUPER_HEIGHT, s);

        int x_min = std::min({projected_v0.x, projected_v1.x, projected_v2.x});
        int x_max = std::max({projected_v0.x, projected_v1.x, projected_v2.x});
        int y_min = std::min({projected_v0.y, projected_v1.y, projected_v2.y});
        int y_max = std::max({projected_v0.y, projected_v1.y, projected_v2.y});

        // Clip against screen bounds
        x_min = std::max(0, x_min);
        y_min = std::max(0, y_min);
        x_max = std::min(SUPER_WIDTH - 1, x_max);
        y_max = std::min(SUPER_HEIGHT - 1, y_max);

        for (int y = y_min; y <= y_max; y++) {
            for (int x = x_min; x <= x_max; x++) {
                if (is_inside_triangle(projected_v0, projected_v1, projected_v2, Vector2(x + 0.5, y + 0.5))) {
                    superImg(x, y) = mesh.face_colors[&face - &mesh.faces[0]];
                }
            }
        }
    }

// Downsampling
    for (int y = 0; y < 480; y++) {
        for (int x = 0; x < 640; x++) {
            Vector3 sumColor = Vector3{0, 0, 0};
            for (int dy = 0; dy < AA_FACTOR; dy++) {
                for (int dx = 0; dx < AA_FACTOR; dx++) {
                    sumColor += superImg(x * AA_FACTOR + dx, y * AA_FACTOR + dy);
                }
            }
            img(x, y) = sumColor / Real(AA_FACTOR * AA_FACTOR);
        }
    }


    return img;
}

Image3 hw_2_3(const std::vector<std::string> &params) {
    // Homework 2.3: render a triangle mesh with interpolated vertex colors and super-sampling

    const int AA_FACTOR = 4;
    const int SUPER_WIDTH = 640 * AA_FACTOR;
    const int SUPER_HEIGHT = 480 * AA_FACTOR;

    Image3 superImg(SUPER_WIDTH, SUPER_HEIGHT);
    Image3 img(640, 480);

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

    for (int y = 0; y < SUPER_HEIGHT; y++) {
        for (int x = 0; x < SUPER_WIDTH; x++) {
            superImg(x, y) = Vector3{0.5, 0.5, 0.5};
        }
    }

    // Render each triangle with interpolated vertex colors
    for (const auto &face : mesh.faces) {
        Vector3 v0 = mesh.vertices[face[0]];
        Vector3 v1 = mesh.vertices[face[1]];
        Vector3 v2 = mesh.vertices[face[2]];

        Vector3 color0 = mesh.vertex_colors[face[0]];
        Vector3 color1 = mesh.vertex_colors[face[1]];
        Vector3 color2 = mesh.vertex_colors[face[2]];

        Vector2 projected_v0 = toScreenSpace(project(v0), SUPER_WIDTH, SUPER_HEIGHT, s);
        Vector2 projected_v1 = toScreenSpace(project(v1), SUPER_WIDTH, SUPER_HEIGHT, s);
        Vector2 projected_v2 = toScreenSpace(project(v2), SUPER_WIDTH, SUPER_HEIGHT, s);

        int x_min = std::min({projected_v0.x, projected_v1.x, projected_v2.x});
        int x_max = std::max({projected_v0.x, projected_v1.x, projected_v2.x});
        int y_min = std::min({projected_v0.y, projected_v1.y, projected_v2.y});
        int y_max = std::max({projected_v0.y, projected_v1.y, projected_v2.y});

        // Clip against screen bounds
        x_min = std::max(0, x_min);
        y_min = std::max(0, y_min);
        x_max = std::min(SUPER_WIDTH - 1, x_max);
        y_max = std::min(SUPER_HEIGHT - 1, y_max);

        for (int y = y_min; y <= y_max; y++) {
            for (int x = x_min; x <= x_max; x++) {
                if (is_inside_triangle(projected_v0, projected_v1, projected_v2, Vector2(x + 0.5, y + 0.5))) {
                    Vector3 barycentric = barycentric_coordinates(projected_v0, projected_v1, projected_v2, Vector2(x + 0.5, y + 0.5));
                    Vector3 interpolated_color = barycentric.x * color0 + barycentric.y * color1 + barycentric.z * color2;
                    superImg(x, y) = interpolated_color;
                }
            }
        }
    }

    // Downsampling
    for (int y = 0; y < 480; y++) {
        for (int x = 0; x < 640; x++) {
            Vector3 sumColor = Vector3{0, 0, 0};
            for (int dy = 0; dy < AA_FACTOR; dy++) {
                for (int dx = 0; dx < AA_FACTOR; dx++) {
                    sumColor += superImg(x * AA_FACTOR + dx, y * AA_FACTOR + dy);
                }
            }
            img(x, y) = sumColor / Real(AA_FACTOR * AA_FACTOR);
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

