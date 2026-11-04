#include <Shaders/shaders.inc>

#if defined( EAE6320_PLATFORM_D3D )
void main(
    // Inputs
    in const float4 i_fragmentPosition : SV_POSITION,
    in const float4 i_color : COLOR,
    // Outputs
    out float4 o_color : SV_TARGET )
{
    // Pass through vertex color
    o_color = float4(i_color);
}

#elif defined( EAE6320_PLATFORM_GL )
in vec4 v_color;
out vec4 o_color;

void main()
{
    // Pass through vertex color
    o_color = vec4(v_color);
}
#endif
