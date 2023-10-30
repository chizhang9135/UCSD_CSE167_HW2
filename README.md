# CSE 167 HW2

## Optimization I done:
1. bounding box for faster rendering
2. clipping as extra credit for 2_1 for allow even if one vertex is outside the screen is still render

## Normal Part
1. using the same way as write up instruct to run
2. pre-rendering image is in `output` folder
3. My own scene is in `output` folder named `hw_2_5.png`



## Extra Credit
### Part Completed:
1. 2-1 clipping (Done)
   1. I make a function to clip based on znear (check in hw2.cpp)
      ```c++
      /**
      * Clip a triangle against the near clipping plane
      * @param p0 The first vertex of the triangle
      * @param p1 The second vertex of the triangle
      * @param p2 The third vertex of the triangle
      * @param z_near The distance of the near clipping plane
      * @return The clipped triangle vertices
        */
        std::vector<Vector3> clipTriangle(const Vector3& p0, const Vector3& p1, const Vector3& p2, Real z_near)
         ```
   2. To run, using the command
      ```shell
      ./balboa -hw 2_1_bonus -s [scale] -p0 [x] [y] [z] -p1 [x] [y] [z] -p2 [x] [y] [z] -color [r] [g] [b] -znear [z_near]
      ```
   3. example output is under `output/bonus_2_1` folder: with the input
      ```shell
      # with clipping
      ./balboa -hw 2_1_bonus -s 1 -p0 -1 -1 -3 -p1 0 1 -3 -p2 1 -1 -5 -color 0.7 0.3 0.4 -znear -4
      # without clipping
      ./balboa -hw 2_1 -s 1 -p0 -1 -1 -3 -p1 0 1 -3 -p2 1 -1 -5 -color 0.7 0.3 0.4 -znear -4 
      ```
      1. `without_clipping.png`: without clipping
      2. `with_clipping.png`: with clipping


2. 2-2 (Done)
   1. Now, the 2_2 and 2_2_bonus will print how long it takes to render the image.
   2. after occluded, it is faster
   3. example output as acceleration:
   ```shell
   # chizhang@Chis-MacBook-Pro cmake-build-debug % ./balboa -hw 2_2_bonus -s 1 -znear 1e-6 -scene_id 1
   hw_2_2_bonus took 27 milliseconds.
   # chizhang@Chis-MacBook-Pro cmake-build-debug % ./balboa -hw 2_2 -s 1 -znear 1e-6 -scene_id 1
   hw_2_2 took 42 milliseconds.
   ```
   The output image is under `output/bonus_2_2` folder as `hw_2_2_bonus-id1.png`. We can see it is identical to `hw_2_2_1.png` under `output` but faster.


3. 2-4 animation (Done)
   1. Animation is in `output/animation` folder
      1. for cube and prime, I interpolate the 3D transform for the object
      2. for the teapot, I interpolate the 3D transform for the camera (lookat)
   2. The source json files are under directory `cube`, `prime`, `teapot`.
   3. To run, using the following command
      ```shell
      ./balboa -animation_hw_2 [directory with json files]
      ```

