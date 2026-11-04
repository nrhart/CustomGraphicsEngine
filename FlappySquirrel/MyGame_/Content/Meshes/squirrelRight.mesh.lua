return
{
    vertexCount = 60,
    indexCount = 90,

    vertices =
    {
        -- Body (medium brown)
        { position = { x =  0.15, y = -0.05, z = 0.0 }, color = { r = 0.55, g = 0.27, b = 0.07, a = 1.0 } },
        { position = { x = -0.10, y = -0.05, z = 0.0 }, color = { r = 0.55, g = 0.27, b = 0.07, a = 1.0 } },
        { position = { x = -0.10, y = -0.30, z = 0.0 }, color = { r = 0.55, g = 0.27, b = 0.07, a = 1.0 } },
        { position = { x =  0.15, y = -0.30, z = 0.0 }, color = { r = 0.55, g = 0.27, b = 0.07, a = 1.0 } },

        -- Face (lighter brown)
        { position = { x = -0.10, y =  0.00,  z = 0.0 }, color = { r = 0.7, g = 0.45, b = 0.2, a = 1.0 } },
        { position = { x = -0.25, y =  0.00,  z = 0.0 }, color = { r = 0.7, g = 0.45, b = 0.2, a = 1.0 } },
        { position = { x = -0.25, y = -0.20, z = 0.0 }, color = { r = 0.7, g = 0.45, b = 0.2, a = 1.0 } },
        { position = { x = -0.10, y = -0.20, z = 0.0 }, color = { r = 0.7, g = 0.45, b = 0.2, a = 1.0 } },

        -- Ear (gray)
        { position = { x = -0.05, y =  0.00, z = 0.0 }, color = { r = 0.6, g = 0.6, b = 0.6, a = 1.0 } },
        { position = { x = -0.25, y =  0.00, z = 0.0 }, color = { r = 0.6, g = 0.6, b = 0.6, a = 1.0 } },
        { position = { x = -0.25, y =  0.15, z = 0.0 }, color = { r = 0.6, g = 0.6, b = 0.6, a = 1.0 } },
        { position = { x = -0.05, y =  0.15, z = 0.0 }, color = { r = 0.6, g = 0.6, b = 0.6, a = 1.0 } },

        -- Tail base (orange brown)
        { position = { x =  0.00, y =  0.10, z = 0.0 }, color = { r = 0.8, g = 0.4, b = 0.1, a = 1.0 } },
        { position = { x = -0.20, y =  0.10, z = 0.0 }, color = { r = 0.8, g = 0.4, b = 0.1, a = 1.0 } },
        { position = { x = -0.20, y =  0.30, z = 0.0 }, color = { r = 0.8, g = 0.4, b = 0.1, a = 1.0 } },
        { position = { x =  0.00, y =  0.30, z = 0.0 }, color = { r = 0.8, g = 0.4, b = 0.1, a = 1.0 } },

        -- Tail tip (reddish)
        { position = { x = -0.05, y =  0.25, z = 0.0 }, color = { r = 0.9, g = 0.25, b = 0.1, a = 1.0 } },
        { position = { x = -0.45, y =  0.25, z = 0.0 }, color = { r = 0.9, g = 0.25, b = 0.1, a = 1.0 } },
        { position = { x = -0.45, y =  0.40, z = 0.0 }, color = { r = 0.9, g = 0.25, b = 0.1, a = 1.0 } },
        { position = { x = -0.05, y =  0.40, z = 0.0 }, color = { r = 0.9, g = 0.25, b = 0.1, a = 1.0 } },

        -- Upper tail detail (darker)
        { position = { x = -0.15, y =  0.20, z = 0.0 }, color = { r = 0.5, g = 0.2, b = 0.05, a = 1.0 } },
        { position = { x = -0.40, y =  0.20, z = 0.0 }, color = { r = 0.5, g = 0.2, b = 0.05, a = 1.0 } },
        { position = { x = -0.40, y =  0.45, z = 0.0 }, color = { r = 0.5, g = 0.2, b = 0.05, a = 1.0 } },
        { position = { x = -0.15, y =  0.45, z = 0.0 }, color = { r = 0.5, g = 0.2, b = 0.05, a = 1.0 } },

        -- Arm (light brown)
        { position = { x = -0.10, y = -0.25, z = 0.0 }, color = { r = 0.8, g = 0.5, b = 0.2, a = 1.0 } },
        { position = { x = -0.10, y = -0.10, z = 0.0 }, color = { r = 0.8, g = 0.5, b = 0.2, a = 1.0 } },
        { position = { x = -0.20, y = -0.10, z = 0.0 }, color = { r = 0.8, g = 0.5, b = 0.2, a = 1.0 } },
        { position = { x = -0.20, y = -0.25, z = 0.0 }, color = { r = 0.8, g = 0.5, b = 0.2, a = 1.0 } },

        -- Belly (tan)
        { position = { x =  0.25, y = -0.15, z = 0.0 }, color = { r = 0.9, g = 0.75, b = 0.5, a = 1.0 } },
        { position = { x =  0.00, y = -0.15, z = 0.0 }, color = { r = 0.9, g = 0.75, b = 0.5, a = 1.0 } },
        { position = { x =  0.00, y =  0.05, z = 0.0 }, color = { r = 0.9, g = 0.75, b = 0.5, a = 1.0 } },
        { position = { x =  0.25, y =  0.05, z = 0.0 }, color = { r = 0.9, g = 0.75, b = 0.5, a = 1.0 } },

        -- Shoulder
        { position = { x =  0.30, y =  0.00, z = 0.0 }, color = { r = 0.6, g = 0.35, b = 0.15, a = 1.0 } },
        { position = { x =  0.10, y =  0.00, z = 0.0 }, color = { r = 0.6, g = 0.35, b = 0.15, a = 1.0 } },
        { position = { x =  0.10, y =  0.20, z = 0.0 }, color = { r = 0.6, g = 0.35, b = 0.15, a = 1.0 } },
        { position = { x =  0.30, y =  0.20, z = 0.0 }, color = { r = 0.6, g = 0.35, b = 0.15, a = 1.0 } },

        -- Tail base segment (dark brown)
        { position = { x =  0.35, y =  0.00, z = 0.0 }, color = { r = 0.4, g = 0.2, b = 0.1, a = 1.0 } },
        { position = { x =  0.30, y =  0.00, z = 0.0 }, color = { r = 0.4, g = 0.2, b = 0.1, a = 1.0 } },
        { position = { x =  0.30, y =  0.10, z = 0.0 }, color = { r = 0.4, g = 0.2, b = 0.1, a = 1.0 } },
        { position = { x =  0.35, y =  0.10, z = 0.0 }, color = { r = 0.4, g = 0.2, b = 0.1, a = 1.0 } },

        -- Nose area
        { position = { x = -0.25, y = -0.15, z = 0.0 }, color = { r = 0.3, g = 0.15, b = 0.1, a = 1.0 } },
        { position = { x = -0.30, y = -0.15, z = 0.0 }, color = { r = 0.3, g = 0.15, b = 0.1, a = 1.0 } },
        { position = { x = -0.30, y =  0.00, z = 0.0 }, color = { r = 0.3, g = 0.15, b = 0.1, a = 1.0 } },
        { position = { x = -0.25, y =  0.00, z = 0.0 }, color = { r = 0.3, g = 0.15, b = 0.1, a = 1.0 } },

        -- Feet
        { position = { x =  0.25, y =  0.20, z = 0.0 }, color = { r = 0.5, g = 0.25, b = 0.1, a = 1.0 } },
        { position = { x =  0.20, y =  0.20, z = 0.0 }, color = { r = 0.5, g = 0.25, b = 0.1, a = 1.0 } },
        { position = { x =  0.20, y =  0.30, z = 0.0 }, color = { r = 0.5, g = 0.25, b = 0.1, a = 1.0 } },
        { position = { x =  0.25, y =  0.30, z = 0.0 }, color = { r = 0.5, g = 0.25, b = 0.1, a = 1.0 } },

        { position = { x =  0.15, y =  0.20, z = 0.0 }, color = { r = 0.5, g = 0.25, b = 0.1, a = 1.0 } },
        { position = { x =  0.10, y =  0.20, z = 0.0 }, color = { r = 0.5, g = 0.25, b = 0.1, a = 1.0 } },
        { position = { x =  0.10, y =  0.30, z = 0.0 }, color = { r = 0.5, g = 0.25, b = 0.1, a = 1.0 } },
        { position = { x =  0.15, y =  0.30, z = 0.0 }, color = { r = 0.5, g = 0.25, b = 0.1, a = 1.0 } },

        -- Legs
        { position = { x =  0.20, y = -0.35, z = 0.0 }, color = { r = 0.6, g = 0.3, b = 0.15, a = 1.0 } },
        { position = { x =  0.00, y = -0.35, z = 0.0 }, color = { r = 0.6, g = 0.3, b = 0.15, a = 1.0 } },
        { position = { x =  0.00, y = -0.30, z = 0.0 }, color = { r = 0.6, g = 0.3, b = 0.15, a = 1.0 } },
        { position = { x =  0.20, y = -0.30, z = 0.0 }, color = { r = 0.6, g = 0.3, b = 0.15, a = 1.0 } },

        -- Tail bottom tip
        { position = { x =  0.35, y = -0.10, z = 0.0 }, color = { r = 0.4, g = 0.2, b = 0.1, a = 1.0 } },
        { position = { x =  0.25, y = -0.10, z = 0.0 }, color = { r = 0.4, g = 0.2, b = 0.1, a = 1.0 } },
        { position = { x =  0.25, y = -0.05, z = 0.0 }, color = { r = 0.4, g = 0.2, b = 0.1, a = 1.0 } },
        { position = { x =  0.35, y = -0.05, z = 0.0 }, color = { r = 0.4, g = 0.2, b = 0.1, a = 1.0 } },
    },

    indices =
    {
        0, 2, 1, 0, 3, 2,
        4, 6, 5, 4, 7, 6,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23,
        24, 26, 25, 24, 27, 26,
        28, 29, 30, 28, 30, 31,
        32, 33, 34, 32, 34, 35,
        36, 37, 38, 36, 38, 39,
        40, 41, 42, 40, 42, 43,
        44, 45, 46, 44, 46, 47,
        48, 49, 50, 48, 50, 51,
        52, 53, 54, 52, 54, 55,
        56, 57, 58, 56, 58, 59
    }
}
