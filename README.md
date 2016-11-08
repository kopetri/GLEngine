GLEngine
======
GLEngine is a C++ OpenGL graphics engine which aimed to be a skill, knowledge and possibly graphical showcase. It will support many graphic features, from something as basic as textures and shaders to Deferred Rendering, PBR and such.

Screenshot
------

![](http://i.imgur.com/BlgSNea.png)


Features
------

* Camera :
    * Movements
    * Zoom in/out
    * **TODO :** Support of physical camera (exposure, distortion...)

* Shaders :
    * Init/loading/binding from anywhere
    * **TODO :** Deferred rendering compliant
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
    * **TODO :** Blinn-Phong
    * **TODO :** (Tiled) Deferred rendering
    * **TODO :** Shadow-mapping

* Environment Mapping :
    * 6-faced cubemap generation
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
    * **TODO :** Logging
    * **TODO :** CPU profiling
    * **TODO :** GUI using Qt5 (which imply a whole project revamping)
    * **TODO :** G-Buffer visualization & export for debugging purpose

How to use
------
GLEngine was written using QtCreator as the IDE, CMake 3.0+ as the building tool, OpenGL 4.0+ as the Graphics API and a C++11 compiler in mind.
Download the source, open the CMakeList.txt file with QtCreator, build everything, and everything should be ready to use.

In GLEngine, hold the right mouse button to use the camera and its features. You can also toggle between solid and wireframe rendering using the F11/F12 buttons. 

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

