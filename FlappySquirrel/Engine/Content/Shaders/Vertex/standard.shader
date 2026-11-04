#include <Shaders/shaders.inc>

#if defined( EAE6320_PLATFORM_D3D )
void main(
    // Inputs
    in const float3 i_vertexPosition_local : POSITION,
    in const float4 i_color : COLOR,

    // Outputs
    out float4 o_vertexPosition_projected : SV_POSITION,
    out float4 o_color : COLOR
)
{
    // Transform the vertex
    float4 vLocal  = float4(i_vertexPosition_local, 1.0);
    float4 vWorld  = mul(g_transform_localToWorld, vLocal);
    float4 vCamera = mul(g_transform_worldToCamera, vWorld);
    o_vertexPosition_projected = mul(g_transform_cameraToProjected, vCamera);

    // Pass vertex color to fragment shader
    o_color = float4(i_color);
}

#elif defined( EAE6320_PLATFORM_GL )
layout(location = 0) in vec3 i_vertexPosition_local;
layout(location = 1) in vec4 i_color;

out vec4 v_color;

void main()
{
    vec4 vLocal  = vec4(i_vertexPosition_local, 1.0);
    vec4 vWorld  = g_transform_localToWorld * vLocal;
    vec4 vCamera = g_transform_worldToCamera * vWorld;
    gl_Position  = g_transform_cameraToProjected * vCamera;

    v_color = vec4(i_color);
}
#endif
