return
{
    vertexCount = 4,
    indexCount  = 6,

    vertices =
    {
        -- bottom-left (red)
        { position = { x = -2.0, y = -0.5, z = 0.0 }, color = { r = 1.0, g = 0.0, b = 0.0, a = 1.0 } },

        -- bottom-right (green)
        { position = { x =  2.0, y = -0.5, z = 0.0 }, color = { r = 0.0, g = 1.0, b = 0.0, a = 1.0 } },

        -- top-right (blue)
        { position = { x =  2.0, y =  0.5, z = 0.0 }, color = { r = 0.0, g = 0.0, b = 1.0, a = 1.0 } },

        -- top-left (white)
        { position = { x = -2.0, y =  0.5, z = 0.0 }, color = { r = 1.0, g = 1.0, b = 1.0, a = 1.0 } },
    },

    indices =
    {
        0, 1, 2,   -- first triangle
        0, 2, 3    -- second triangle
    }
}
