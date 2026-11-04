return
{
    vertexCount = 4,
    indexCount  = 6,

    vertices =
    {
        -- bottom-left
        { position = { x = -0.5, y = -5.0, z = 0.0 }, color = { r = 0.12, g = 0.55, b = 0.16, a = 1.0 }
 },

        -- bottom-right
        { position = { x =  0.5, y = -5.0, z = 0.0 }, color = { r = 0.12, g = 0.55, b = 0.16, a = 1.0 }
 },

        -- top-right
        { position = { x =  0.5, y =  5.0, z = 0.0 }, color = { r = 0.12, g = 0.55, b = 0.16, a = 1.0 }
 },

        -- top-left
        { position = { x = -0.5, y =  5.0, z = 0.0 }, color = { r = 0.12, g = 0.55, b = 0.16, a = 1.0 }
 },
    },

    indices =
    {
        0, 1, 2,   -- first triangle
        0, 2, 3    -- second triangle
    }
}
