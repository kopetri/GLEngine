GLEngine
======
GLEngine is a C++ OpenGL graphics engine which aimed to be a skill, knowledge and possibly graphical showcase. It will support many graphic features, from something as basic as textures and shaders to Deferred Rendering, PBR and such.

Screenshot
------

![](http://i.imgur.com/GNWNW98.png)
![](http://i.imgur.com/0KuTFTz.png)


Features
------

* Camera :
    * Movements
    * Zoom in/out
    * **TODO :** Support of physical camera (exposure, distortion...)

* Shaders :
    * Init/loading/binding from anywhere
    * SSAO
    * **TODO :** Deferred Rendering compliant
    * **TODO :** PBR material pipeline compliant
    * **TODO :** Post-processing (FXAA, DoF, motion blur, bloom, SSR, SSS...)

* Textures :
    * **TODO :** Init/loading/binding from anywhere
    * **TODO :** PBR material pipeline compliant (albedo, normal, roughness, metalness, AO)
    
* Models/Meshes :
    * External models loading (.obj)
    * Textures support
    * **TODO :** Revamp & debug to support kinda buggy models
    * **TODO :** PBR material pipeline compliant
    
* Lights :
    * Point light
    * **TODO :** Directional light
    * **TODO :** Spot light
    
* Resources Manager :
    * **TODO :** Resources (textures, shaders, models, materials...) centralization
    
* Basic Shapes :
    * Simple shape generation (triangle, plane, cube)
    * Predefined array based
    * **TODO :** Shape topology computation
    
* Lighting :
    * Lambert
    * Phong
    * Cook-Torrance BRDF
    * Deferred Rendering
    * **TODO :** Blinn-Phong
    * **TODO :** Tiled Deferred Rendering
    * **TODO :** Shadow-mapping

* Environment Mapping :
    * 6-faced cubemap generation
    * **TODO :** Deferred Rendering compliant
    * **TODO :** Equirectangular HDR maps

* PBR Pipeline :
    * BRDF :
        * Cook-Torrance model
        * Diffuse : Lambertian/Disney
        * Fresnel term : Schlick
        * Microfacet distribution : GGX
        * Geometry attenuation : GGX-Smith
    * **TODO :** Image-Based Lighting
    * **TODO :** Proper material pipeline using a roughness/metalness workflow
    
* Utility :
    * GUI using ImGui
    * Basic/naive GPU profiling
    * G-Buffer visualization for debugging purpose
    * **TODO :** Logging
    * **TODO :** CPU profiling
    * **TODO :** GUI using Qt5 (which imply a whole project revamping)
    * **TODO :** G-Buffer export as .png

How to use
------
GLEngine was written using Linux, QtCreator as the IDE, CMake 3.0+ as the building tool, OpenGL 4.0+ as the Graphics API and a C++11 compiler in mind.

Download the source, open the CMakeList.txt file with QtCreator, build the project, and everything should be ready to use.

* In GLEngine :
    * Hold the right mouse button to use the camera and its features
    * Toggle between solid and wireframe rendering using the F11/F12 buttons
    * Toggle between the different buffers using the 1-5 buttons

Dependencies (included)
------
- Window & Input system : GLFW
- OpenGL Function Loader : GLAD
- OpenGL Mathematic Functions : GLM
- Image Loading : stb
- Mesh Loading : Assimp

Credits
------
- Joey de Vries (LearnOpenGL)
- Kevin Fung (Glitter)

