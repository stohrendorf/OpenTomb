#ifdef SKELETAL
#define VTX_INPUT_BONE_INDEX
#endif

#define VTX_INPUT_TEXCOORD

#include "vtx_input.glsl"
#include "transform_interface.glsl"
#include "camera_interface.glsl"

#include "geometry_pipeline_interface.glsl"

void main()
{
    gpi.texCoord = a_texCoord;
    gpi.texIndex = a_texIndex;
    gpi.color = a_color;

    #ifdef SKELETAL
    vec4 vtx = u_viewProjection * u_modelMatrix * u_bones[int(a_boneIndex)] * vec4(a_position, 1);
    #else
    vec4 vtx = u_viewProjection * u_modelMatrix * vec4(a_position, 1);
    #endif
    vtx.z = (vtx.z / vtx.w + 1/512.0) * vtx.w;// depth offset
    gl_Position = vtx;
}
