//
// Common include for all shaders
//

#if defined( EAE6320_PLATFORM_GL )
    #version 420
#endif

// Constant Buffer Declaration Macro
#if defined( EAE6320_PLATFORM_D3D )
    #define DeclareConstantBuffer( i_name, i_id ) cbuffer i_name : register( b##i_id )
#elif defined( EAE6320_PLATFORM_GL )
    #define DeclareConstantBuffer( i_name, i_id ) layout( std140, binding = i_id ) uniform i_name
#endif

// Type Aliases
#if defined( EAE6320_PLATFORM_D3D )
    #define float4x4 row_major float4x4
    #define float4   float4
    #define float2   float2
#elif defined( EAE6320_PLATFORM_GL )
    #define float4x4 mat4
    #define float4   vec4
    #define float2   vec2
#endif

// Matrix Multiply Helper
#if defined( EAE6320_PLATFORM_D3D )
    // D3D expects mul( matrix, vector )
    #define Multiply( i_matrix, i_vector ) mul( i_matrix, i_vector )
#elif defined( EAE6320_PLATFORM_GL )
    #define Multiply( i_matrix, i_vector ) ( (i_matrix) * (i_vector) )
#endif

// Frame Constant Buffer (b0 / binding = 0)
DeclareConstantBuffer( g_frame, 0 )
{
    float4x4 g_transform_worldToCamera;
    float4x4 g_transform_cameraToProjected;

    float g_elapsedSecondCount_systemTime;
    float g_elapsedSecondCount_simulationTime;
    float2 g_padding;
};

// Draw Call Constant Buffer (b2 / binding = 2)
DeclareConstantBuffer( g_drawCall, 2 )
{
    float4x4 g_transform_localToWorld;
};
