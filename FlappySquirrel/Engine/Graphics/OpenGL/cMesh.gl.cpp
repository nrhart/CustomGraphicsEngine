#include "Engine/Graphics/cMesh.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Graphics/VertexFormats.h>
#include "Includes.h"

namespace eae6320
{
    namespace Graphics
    {
        cResult cMesh::Initialize(
            const VertexFormats::sVertex_mesh* vertices,
            const uint16_t vertexCount,
            const uint16_t* indices,
            const uint16_t indexCount)
        {
            auto result = Results::Success;
            m_indexCount = indexCount;

            // --- Create VAO ---
            glGenVertexArrays(1, &m_vertexArrayId);
            glBindVertexArray(m_vertexArrayId);

            // --- Create VBO ---
            {
                glGenBuffers(1, &m_vertexBufferId);
                glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferId);
                glBufferData(GL_ARRAY_BUFFER,
                    sizeof(VertexFormats::sVertex_mesh) * vertexCount,
                    vertices,
                    GL_STATIC_DRAW);

                // Attribute 0 = POSITION (float3)
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                    sizeof(VertexFormats::sVertex_mesh),
                    reinterpret_cast<const GLvoid*>(offsetof(VertexFormats::sVertex_mesh, x)));
                glEnableVertexAttribArray(0);

                // Attribute 1 = COLOR (uint8_t4 normalized)
                glVertexAttribPointer(
                    1, 
                    4, 
                    GL_UNSIGNED_BYTE, 
                    GL_TRUE, 
                    sizeof(VertexFormats::sVertex_mesh),
                    reinterpret_cast<const GLvoid*>(offsetof(VertexFormats::sVertex_mesh, r)));
                glEnableVertexAttribArray(1);
            }

            // --- Create Index Buffer (EBO) ---
            {
                glGenBuffers(1, &m_indexBufferId);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferId);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    sizeof(uint16_t) * indexCount,
                    indices,
                    GL_STATIC_DRAW);
            }

            // Optional: unbind VAO (EBO binding is remembered by VAO)
            glBindVertexArray(0);

            EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
            return result;
        }

        void cMesh::Draw() const
        {
            EAE6320_ASSERT(m_vertexArrayId != 0);
            glBindVertexArray(m_vertexArrayId);

            constexpr GLenum mode = GL_TRIANGLES;
            const GLvoid* const offset = 0;
            glDrawElements(mode, m_indexCount, GL_UNSIGNED_SHORT, offset);

            EAE6320_ASSERT(glGetError() == GL_NO_ERROR);
        }

        void cMesh::CleanUp()
        {
            if (m_indexBufferId) { glDeleteBuffers(1, &m_indexBufferId);  m_indexBufferId = 0; }
            if (m_vertexBufferId) { glDeleteBuffers(1, &m_vertexBufferId); m_vertexBufferId = 0; }
            if (m_vertexArrayId) { glDeleteVertexArrays(1, &m_vertexArrayId); m_vertexArrayId = 0; }
            m_indexCount = 0;
        }
    }
}
