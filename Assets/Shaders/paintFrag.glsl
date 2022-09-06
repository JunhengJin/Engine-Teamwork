#version 440 core
#extension GL_EXT_shader_image_load_store : enable

in Vertex
{
	in vec2 interp_UV;
	in vec3 vNormal;
	in vec3 paintDir;
	in vec4 posPaintSpace;
} IN;

// The previous state of the paint map.
uniform layout(binding = 1, r8ui) uimage2D previous_paint_map;

// The size of the paint map.
uniform int paint_map_size;

uniform sampler2D paintTex;

const uint max_ubyte = 255;

void main()
{
    ivec2 uv_pixels = ivec2(IN.interp_UV * paint_map_size);
    uv_pixels.x = clamp(uv_pixels.x, 0, paint_map_size - 1);
    uv_pixels.y = clamp(uv_pixels.y, 0, paint_map_size - 1);
    uint previous_paint_color = imageLoad(previous_paint_map, uv_pixels).r;


    vec3 projCoords = IN.posPaintSpace.xyz / IN.posPaintSpace.w;
    projCoords = (projCoords * 0.5) + 0.5;
    uint addedColor = uint(max_ubyte * gl_FragDepth);

    // Computes incidence angle between paint ball direction and face normal.
    float paintLevel = 1.0 - texture2D(paintTex, projCoords.xy).r;
    float incidence = dot(IN.paintDir, IN.vNormal);

    // If dot product < 0 then the face got hit by the paint.
    if (incidence < 0 && paintLevel > 0)
    {
        // Computes new paint color value.
        uint paint = min(previous_paint_color, addedColor);

        imageStore(previous_paint_map, uv_pixels, uvec4(paint));
		
		//imageStore(previous_paint_map, uv_pixels, uvec4(1));
    }
	
	imageStore(previous_paint_map, uv_pixels, uvec4(max_ubyte)); //paint whole fragment
}