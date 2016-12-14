GLEngine
======
GLEngine is a C++ OpenGL graphics engine which aimed to be a skill, knowledge and possibly graphical showcase. It will support many graphic features, from something as basic as textures and shaders to Deferred Rendering, PBR and such.

Screenshot
------

![](http://i.imgur.com/hpewSi9.png)
![](http://i.imgur.com/6frnrfz.png)


Features
------

* Camera :
    * Movements
    * Zoom in/out
    * **TODO :** Support of physically based camera (exposure, distortion...)

* Textures :
    * Init/loading/binding from anywhere
	* Anisotropic Filtering
	* **TODO :** HDR
	* **TODO :** Cubemap

* Material :
	* PBR material pipeline :
		* Albedo
		* **TODO :** Normal
		* Roughness
		* Metalness
		* AO
	* **TODO :** Informations seamlessly sent to the G-Buffer

* Models/Meshes :
    * External models loading (.obj)
    * Textures support
    * **TODO :** PBR material pipeline compliant
    * **TODO :** Revamp & debug to support kinda buggy models

* Basic Shapes :
    * Simple shape generation (triangle, plane, cube)
    * Predefined array based
    * **TODO :** Shape topology computation (sphere...)

* Shaders :
    * Init/loading/binding from anywhere
    * G-Buffer support
    * PBR material pipeline compliant
	* **TODO :** UBOs
    
* Resources Manager :
    * **TODO :** Resources (textures, shaders, models, materials...) centralization

* Lights :
    * Point light
    * Directional light
    * **TODO :** Spot light

* Lighting :
    * Lambert
    * Phong
    * Cook-Torrance BRDF
    * Deferred Rendering
    * **TODO :** Blinn-Phong
    * **TODO :** Shadow-mapping (PCF/Variance)
    * **TODO :** Tiled Deferred Rendering (Compute shaders ?)

* PBR Pipeline :
    * BRDF :
        * Cook-Torrance model
        * Diffuse : Lambertian/Disney
        * Fresnel term : Schlick
        * Microfacet distribution : GGX
        * Geometry attenuation : GGX-Smith
    * Material pipeline using a roughness/metalness workflow
    * **TODO :** Image-Based Lighting

* Post-processing :
	* SSAO
	* **TODO :** Motion Blur (camera/per-fragment)
	* **TODO :** PPAA (FXAA...)
	* **TODO :** Bloom
	* **TODO :** Depth of Field
	* **TODO :** Screen-Space Reflections
	* **TODO :** Lens Flare
	* **TODO :** Eye Adaptation

* Environment Mapping :
    * 6-faced cubemap generation
    * **TODO :** Deferred Rendering compliant
    * **TODO :** Equirectangular HDR maps
 
* Utility :
    * GUI using ImGui
    * Basic/naive GPU profiling
    * G-Buffer visualization for debugging purpose
	* Borderless Fullscreen
    * **TODO :** Logging
    * **TODO :** CPU profiling
    * **TODO :** G-Buffer export as .png
    * **TODO :** GUI using Qt5 (which imply a whole project revamping)

How to use
------
GLEngine was written using Linux, QtCreator as the IDE, CMake 3.0+ as the building tool, OpenGL 4.0+ as the Graphics API and a C++11 compiler in mind.

Download the source, open the CMakeList.txt file with QtCreator, build the project, and everything should be ready to use.

* In GLEngine :
    * Hold the right mouse button to use the camera and its features
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

