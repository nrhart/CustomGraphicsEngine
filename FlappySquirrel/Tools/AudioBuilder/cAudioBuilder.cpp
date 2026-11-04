#include "cAudioBuilder.h"
#include <Tools/AssetBuildLibrary/Functions.h>
#include <Engine/Logging/Logging.h>
#include <fstream>
#include <vector>
#include <string>

namespace eae6320
{
    namespace Assets
    {
        // Simple header to identify your custom binary
        struct sAudioHeader
        {
            uint32_t magic = 'AUD0';    // Four-character file identifier
            uint32_t dataSize = 0;      // Bytes of raw audio
            uint8_t  loop = 0;          // 1 = loop, 0 = no loop
            float    volume = 1.0f;     // Default volume
        };
        cResult cAudioBuilder::Build(const std::vector<std::string>&)
        {
            cResult result = Results::Success;

            // --- Read WAV source file ---
            std::ifstream fin(m_path_source, std::ios::binary | std::ios::ate);
            if (!fin.is_open())
            {
                OutputErrorMessageWithFileInfo(m_path_source, "Failed to open input WAV file");
                return Results::Failure;
            }

            const std::streamsize fileSize = fin.tellg();
            fin.seekg(0, std::ios::beg);

            std::vector<char> audioData(static_cast<size_t>(fileSize));
            if (!fin.read(audioData.data(), fileSize))
            {
                OutputErrorMessageWithFileInfo(m_path_source, "Failed to read WAV file data");
                fin.close();
                return Results::Failure;
            }
            fin.close();

            // --- Write Binary .audio file ---
            std::ofstream fout(m_path_target, std::ios::binary);
            if (!fout.is_open())
            {
                OutputErrorMessageWithFileInfo(m_path_target, "Failed to open output file for writing");
                return Results::Failure;
            }

            sAudioHeader header;
            header.dataSize = static_cast<uint32_t>(fileSize);

            // 1) Write header
            fout.write(reinterpret_cast<const char*>(&header), sizeof(sAudioHeader));

            // 2) Write raw WAV bytes
            fout.write(audioData.data(), fileSize);

            fout.close();

#ifdef _DEBUG
            Logging::OutputMessage("[AudioBuilder] Built audio: %s -> %s | Size: %u bytes",
                m_path_source, m_path_target, header.dataSize);
#endif

            return result;
        }
    }
}
