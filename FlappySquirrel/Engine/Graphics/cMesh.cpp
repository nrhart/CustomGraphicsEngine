#include "cMesh.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Graphics/VertexFormats.h>
#include <Engine/Platform/Platform.h>
#include <cstdint>
#include <cstring>
#include <new>

using namespace eae6320;
using namespace eae6320::Graphics;

namespace
{
	using V = VertexFormats::sVertex_mesh;
}

cResult cMesh::Load(cMesh*& o_mesh, const char* const i_pathToBuiltMesh)
{
	auto result = Results::Success;
	o_mesh = nullptr;

	// 1) Load the entire binary file
	Platform::sDataFromFile dataFromFile;
	if (!(result = Platform::LoadBinaryFile(i_pathToBuiltMesh, dataFromFile)))
		return result;

	EAE6320_ASSERT(dataFromFile.data && dataFromFile.size >= sizeof(uint16_t) * 2);

	std::uintptr_t current = reinterpret_cast<std::uintptr_t>(dataFromFile.data);
	const std::uintptr_t end = current + dataFromFile.size;

	// ---- 1) vertexCount
	uint16_t vertexCount = 0;
	std::memcpy(&vertexCount, reinterpret_cast<void*>(current), sizeof(vertexCount));
	current += sizeof(vertexCount);

	// ---- 2) indexCount
	uint16_t indexCount = 0;
	std::memcpy(&indexCount, reinterpret_cast<void*>(current), sizeof(indexCount));
	current += sizeof(indexCount);

	// ---- 3) vertex array
	const std::size_t vertexBytes = static_cast<std::size_t>(vertexCount) * sizeof(V);
	if (current + vertexBytes > end)
	{
		dataFromFile.Free();
		return Results::Failure;
	}
	const auto* const vertices = reinterpret_cast<const V*>(current);
	current += vertexBytes;

	// ---- 4) index array
	const std::size_t indexBytes = static_cast<std::size_t>(indexCount) * sizeof(uint16_t);
	if (current + indexBytes > end)
	{
		dataFromFile.Free();
		return Results::Failure;
	}
	const auto* const indices = reinterpret_cast<const uint16_t*>(current);
	current += indexBytes;

	EAE6320_ASSERT(current <= end);

	// ---- Create mesh
	cMesh* newMesh = new (std::nothrow) cMesh();
	if (!newMesh)
	{
		dataFromFile.Free();
		return Results::OutOfMemory;
	}

	result = newMesh->Initialize(vertices, vertexCount, indices, indexCount);
	if (result)
		o_mesh = newMesh;
	else
	{
		newMesh->DecrementReferenceCount();
		o_mesh = nullptr;
	}

	dataFromFile.Free();
	return result;
}

cResult cMesh::Load(
	cMesh*& o_mesh,
	const VertexFormats::sVertex_mesh* vertices, uint16_t vertexCount,
	const uint16_t* indices, uint16_t indexCount)
{
	auto result = Results::Success;
	cMesh* newMesh = new (std::nothrow) cMesh();
	if (!newMesh) return Results::OutOfMemory;

	result = newMesh->Initialize(vertices, vertexCount, indices, indexCount);
	if (result)
		o_mesh = newMesh;
	else
	{
		newMesh->DecrementReferenceCount();
		o_mesh = nullptr;
	}

	return result;
}

// ------------------------------------------------------------
// Destructor
// ------------------------------------------------------------
cMesh::~cMesh()
{
	EAE6320_ASSERT(m_referenceCount == 0);
	CleanUp();
}
