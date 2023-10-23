# CSE 167 HW2

## Optimization I done:
1. bounding box for hw_2_1, hw_2_2 and hw_2_3 for faster rendering

## Normal Part
1. using the same way as write up instruct to run
2. pre-rendering image is in `output` folder

## Helper Function

- `bool is_inside_triangle(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p)`
- `Vector2 project(const Vector3 &p)` 
- `Vector2 toScreenSpace(const Vector2 &p, int width, int height, Real s)`
- `Vector3 barycentric_coordinates(const Vector2 &A, const Vector2 &B, const Vector2 &C, const Vector2 &P)`
- `void downsample(Image3& img, const Image3& superImg, int AA_FACTOR) `

More usage of the function see comments

## Extra Credit

