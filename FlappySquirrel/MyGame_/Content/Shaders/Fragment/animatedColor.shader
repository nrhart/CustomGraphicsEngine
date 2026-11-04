#include <Shaders/shaders.inc>

#if defined( EAE6320_PLATFORM_D3D )

void main(
    // Inputs
    in const float4 i_fragmentPosition : SV_POSITION,
    in const float4 i_color : COLOR,
    // Outputs
    out float4 o_color : SV_TARGET )
{
    float speed = 3.0f;

    // Animated rainbow pattern
    float4 animatedColor;
    animatedColor.r = (sin(g_elapsedSecondCount_simulationTime * speed) * 0.4f) + 0.6f;
    animatedColor.g = (cos(g_elapsedSecondCount_simulationTime * speed) * 0.4f) + 0.6f;
    animatedColor.b = (sin(g_elapsedSecondCount_simulationTime * speed * 0.5f) * 0.4f) + 0.6f;
    animatedColor.a = 1.0f;

    // Cast vertex color explicitly to float4
    float4 vertexColor = float4(i_color.rgb, i_color.a);

    // Multiply each component explicitly
    o_color.r = Multiply(animatedColor.r, vertexColor.r);
    o_color.g = Multiply(animatedColor.g, vertexColor.g);
    o_color.b = Multiply(animatedColor.b, vertexColor.b);
    o_color.a = Multiply(animatedColor.a, vertexColor.a);
}

#elif defined( EAE6320_PLATFORM_GL )

// Match vertex shader output name
in vec4 v_color;
out vec4 o_color;

void main()
{
    float speed = 3.0f;

    vec4 animatedColor;
    animatedColor.r = (sin(g_elapsedSecondCount_simulationTime * speed) * 0.4f) + 0.6f;
    animatedColor.g = (cos(g_elapsedSecondCount_simulationTime * speed) * 0.4f) + 0.6f;
    animatedColor.b = (sin(g_elapsedSecondCount_simulationTime * speed * 0.5f) * 0.4f) + 0.6f;
    animatedColor.a = 1.0f;

    // Cast explicitly
    vec4 vertexColor = vec4(v_color.rgb, v_color.a);

    // Multiply each component explicitly
    o_color.r = Multiply(animatedColor.r, vertexColor.r);
    o_color.g = Multiply(animatedColor.g, vertexColor.g);
    o_color.b = Multiply(animatedColor.b, vertexColor.b);
    o_color.a = Multiply(animatedColor.a, vertexColor.a);
}
#endif
