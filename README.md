## 🟢 HW7: OpenGL Shader-Based Phong Shading
This assignment implements a shader-based rendering pipeline using modern OpenGL. It renders a single shaded sphere using per-pixel Phong shading and reproduces the output of HW6 (software rasterization), but now with GLSL shaders and GPU rasterization.

The primary goal of this assignment is to demonstrate a fundamental understanding of programmable graphics pipelines in OpenGL. A unit sphere is generated programmatically and then transformed, projected, and rendered with per-pixel Phong shading using custom GLSL shaders. All scene parameters, including material properties, light source, and camera configuration, are passed from the C++ application to the shaders as uniform variables.

## Features / Requirements Met

* **Full Shader Pipeline:** All rendering (transformations, rasterization, shading) is performed entirely on the GPU using custom vertex and fragment shaders.
* **No Fixed-Function Pipeline:** No deprecated OpenGL fixed-function features are used.
* **Vertex Shader (`Phong.vert`):** Calculates fragment position and normal in world space, and transforms vertices to clip space.
* **Fragment Shader (`Phong.frag`):** Implements per-pixel Phong shading (ambient, diffuse, specular components).
* **Uniform Variable Transfer:** All constant values (matrices, light position, camera position, material properties) are passed from the C++ application to the shaders via uniforms.
* **Sphere Generation:** Programmatically generates a sphere's vertices, normals, and indices.
* **Modeling Transform:** Transforms the unit sphere to a sphere centered at `(0, 0, -7)` with a radius of `2`.
* **Camera Transform:** Configured with an eye point at `(0, 0, 0)` and looking along the `(-w)` direction (i.e., along the negative Z-axis).
* **Perspective Projection:** Uses a custom frustum defined by `l = -0.1, r = 0.1, b = -0.1, t = 0.1, n = -0.1, f = -1000.0`.
* **Viewport Transform:** Set to `512x512` pixels.
* **Phong Shading Parameters:**
    * Material Ambient (`ka`): `(0, 1, 0)`
    * Material Diffuse (`kd`): `(0, 0.5, 0)`
    * Material Specular (`ks`): `(0.5, 0.5, 0.5)`
    * Shininess (`p`): `32`
    * Ambient Light Intensity (`Ia`): `0.2`
    * Point Light Source: At `(-4, 4, -3)` with white unit intensity and no falloff.
* **Gamma Correction:** Applied with $\gamma = 2.2$ in the fragment shader for sRGB output.


## 🛠️ Build & Run Instructions (Visual Studio 2022)
🔧 Prerequisites
 * Visual Studio 2022 Community Edition
 * C++ Desktop Development workload installed

▶️ Steps
1. Clone the repository with:

```bash
git clone https://github.com/yourusername/HW7_PhongShader.git
```
 * All required source files, shaders, and libraries are included.


2. Click HW7.sln from CG_HW7_202213827 folder.
Ensure that main_Phong_Shader.cpp is the startup file.

3. Build the project:
Build > Build Solution (or press Ctrl + Shift + B)

4. Run the project:
Debug > Start Without Debugging (or press Ctrl + F5)

## 🖼️ Expected Output

![HW7_output](https://github.com/user-attachments/assets/2229c87c-7d39-4bba-9afc-aa594825f1e8)

 * A green Phong-shaded 3D sphere

 * Light from (−4, 4, −3)

 * Matches HW6 result

 * Smooth, gamma-corrected shading

## 👨‍💻 Author
Tuguldur SID: 202213827 / 
Konkuk University — Department of Computer Science & Engineering / 
Computer Graphics (Spring 2025)
