GLEngine
======
GLEngine is a C++ OpenGL graphics engine which aimed to be a skill, knowledge and possibly graphical showcase. It will support many graphic features, from something as basic as textures and shaders to Deferred Rendering, PBR and such.


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
    * **TODO :** Logging
    * **TODO :** CPU/GPU profiling
    * **TODO :** GUI using AntTweakBar first, then Qt5 (which imply a whole project revamping)
    * **TODO :** G-Buffer visualization & export for debugging purpose

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
