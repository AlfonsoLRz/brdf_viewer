# BRDF Viewer
Visualization of custom BRDFs defined through a GLSL pre-defined syntaxis, with the following form:
```
# variables go here...
# only floats and boolean supported right now.

::begin parameters
float name [min val] [max val] [default val]
bool name default val
::end parameters

::begin reflectanceShader

vec3 BRDF(vec3 L, vec3 N, vec3 V, vec3 X, vec3 Y)
{
    ... 
    return N * length;
}

::end reflectanceShader

::begin renderingShader

vec3 BRDF( vec3 kad, vec3 ks, float shadowDiffuse, float shadowSpecular, vec3 L, vec3 N, vec3 V, vec3 X, vec3 Y )
{
    ...
    return shadowDiffuse * color * brdfFactor;
}

::end renderingShader
```

By default, this work integrates six different shaders for typical BRDFs, including Phong, Lambertian, Cook-Torrance, Minnaert, Ward (anisotropic) and Oren-Nayar. The integration and reload of new shaders included by users are still pending. Currently, the provided shaders can be redone to build a custom BRDF function.

They can be either visualized as an implicit function mapped into a unit sphere, or by rendering a dragon mesh coloured with the implemented BRDF. Both kinds of visualization are following depicted:

![alt text](md_assets/brdf.png "BRDF Rendering")
