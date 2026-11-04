--[[
    This file lists every asset that must be built by the AssetBuildSystem
]]

return
{
    -- Meshes
    meshes =
    {
        { path = "Meshes/square.mesh.lua" },
        { path = "Meshes/rectangle.mesh.lua" },
        { path = "Meshes/tree.mesh.lua" },
        { path = "Meshes/squirrelRight.mesh.lua" },
        { path = "Meshes/squirrelLeft.mesh.lua" },
        { path = "Meshes/plane.mayamesh" },
        { path = "Meshes/sphere.mayamesh" },
        { path = "Meshes/duck.mayamesh" },
    },

    -- Shaders
    shaders =
    {
        { path = "Shaders/Vertex/standard.shader", arguments = { "vertex" } },
        { path = "Shaders/Fragment/standard.shader", arguments = { "fragment" } },

        { path = "Shaders/Vertex/vertexInputLayout_mesh.shader", arguments = { "vertex" } },
        { path = "Shaders/Fragment/animatedColor.shader", arguments = { "fragment" } },
    },

    -- Audio
    audio =
    {
        { path = "Audio/jump.wav" },
        { path = "Audio/bgmusic.wav" },
        { path = "Audio/triple.wav" },
        { path = "Audio/die.wav" },
        { path = "Audio/flap.wav" },
        { path = "Audio/point.wav" },
        { path = "Audio/hit.wav" },
    },

}
