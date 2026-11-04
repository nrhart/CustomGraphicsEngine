// Tools/MeshBuilder/cMeshBuilder.h

#pragma once

#include <Tools/AssetBuildLibrary/iBuilder.h>

namespace eae6320
{
    namespace Assets
    {
        class cMeshBuilder final : public iBuilder
        {
        private:
            eae6320::cResult Build(const std::vector<std::string>& i_optionalArguments) override;

        };
    }
}
