#define VTX_INPUT_TEXCOORD
#define VTX_INPUT_COLOR_QUAD

#include "vtx_input.glsl"
#include "ui_pipeline_interface.glsl"

uniform vec2 u_screenSize;

float toLinear(in float srgb)
{
    return srgb <= 0.04045
    ? srgb / 12.92
    : pow((srgb + 0.055) / 1.055, 2.4);
}

vec4 toLinear(in vec4 srgb)
{
    return vec4(
    toLinear(srgb.r),
    toLinear(srgb.g),
    toLinear(srgb.b),
    srgb.a
    );
}

void main()
{
    vec2 p = (a_position.xy / u_screenSize) * 2 - 1;
    gl_Position = vec4(p.x, -p.y, 0, 1);
    upi.texCoord = a_texCoord;
    upi.texIndex = a_texIndex;
    upi.topLeft = toLinear(a_colorTopLeft);
    upi.topRight = toLinear(a_colorTopRight);
    upi.bottomLeft = toLinear(a_colorBottomLeft);
    upi.bottomRight = toLinear(a_colorBottomRight);
}
