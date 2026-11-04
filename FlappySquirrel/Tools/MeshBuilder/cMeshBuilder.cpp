#include "cMeshBuilder.h"

#include <Engine/Graphics/VertexFormats.h>
#include <External/Lua/Includes.h>
#include <Tools/AssetBuildLibrary/Functions.h> 
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace eae6320;
using namespace eae6320::Assets;

namespace
{
	using V = eae6320::Graphics::VertexFormats::sVertex_mesh;

	static bool LoadVertices(lua_State* L,
		std::vector<V>& outVerts, const std::string& i_path)
	{
		if (!lua_istable(L, -1))
		{
			OutputErrorMessageWithFileInfo(i_path.c_str(), "'vertices' is not a table");
			return false;
		}

		const int count = static_cast<int>(luaL_len(L, -1));
		outVerts.resize(count);

		for (int i = 1; i <= count; ++i)
		{
			lua_pushinteger(L, i);
			lua_gettable(L, -2);
			if (!lua_istable(L, -1))
			{
				OutputErrorMessageWithFileInfo(i_path.c_str(), "Vertex entry not a table");
				lua_pop(L, 1);
				return false;
			}

			lua_getfield(L, -1, "position");
			if (!lua_istable(L, -1))
			{
				OutputErrorMessageWithFileInfo(i_path.c_str(), "Vertex missing 'position'");
				lua_pop(L, 2);
				return false;
			}

			lua_getfield(L, -1, "x");
			outVerts[i - 1].x = static_cast<float>(lua_tonumber(L, -1));
			lua_pop(L, 1);
			lua_getfield(L, -1, "y");
			outVerts[i - 1].y = static_cast<float>(lua_tonumber(L, -1));
			lua_pop(L, 1);
			lua_getfield(L, -1, "z");
			outVerts[i - 1].z = static_cast<float>(lua_tonumber(L, -1));
			lua_pop(L, 1);
			lua_pop(L, 1);

			lua_getfield(L, -1, "color");
			if (lua_istable(L, -1))
			{
				float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
				lua_getfield(L, -1, "r");
				if (lua_isnumber(L, -1)) r = static_cast<float>(lua_tonumber(L, -1));
				lua_pop(L, 1);
				lua_getfield(L, -1, "g");
				if (lua_isnumber(L, -1)) g = static_cast<float>(lua_tonumber(L, -1));
				lua_pop(L, 1);
				lua_getfield(L, -1, "b");
				if (lua_isnumber(L, -1)) b = static_cast<float>(lua_tonumber(L, -1));
				lua_pop(L, 1);
				lua_getfield(L, -1, "a");
				if (lua_isnumber(L, -1)) a = static_cast<float>(lua_tonumber(L, -1));
				lua_pop(L, 1);

				outVerts[i - 1].r = static_cast<uint8_t>(r * 255.0f + 0.5f);
				outVerts[i - 1].g = static_cast<uint8_t>(g * 255.0f + 0.5f);
				outVerts[i - 1].b = static_cast<uint8_t>(b * 255.0f + 0.5f);
				outVerts[i - 1].a = static_cast<uint8_t>(a * 255.0f + 0.5f);
			}
			else
			{
				outVerts[i - 1].r = outVerts[i - 1].g = outVerts[i - 1].b = outVerts[i - 1].a = 255;
			}
			lua_pop(L, 1);
			lua_pop(L, 1);
		}
		return true;
	}

	static bool LoadIndices(lua_State* L,
		std::vector<uint16_t>& outIdx, const std::string& i_path)
	{
		if (!lua_istable(L, -1))
		{
			OutputErrorMessageWithFileInfo(i_path.c_str(), "'indices' is not a table");
			return false;
		}

		const int count = static_cast<int>(luaL_len(L, -1));
		outIdx.resize(count);

		for (int i = 1; i <= count; ++i)
		{
			lua_pushinteger(L, i);
			lua_gettable(L, -2);
			outIdx[i - 1] = static_cast<uint16_t>(lua_tointeger(L, -1));
			lua_pop(L, 1);
		}
		return true;
	}
}

cResult cMeshBuilder::Build(const std::vector<std::string>&)
{
	cResult result = Results::Success;

	std::vector<Graphics::VertexFormats::sVertex_mesh> vertices;
	std::vector<uint16_t> indices;

	// ------------- Load Lua mesh -------------
	lua_State* L = luaL_newstate();
	if (!L)
	{
		OutputErrorMessageWithFileInfo(m_path_source, "Failed to create Lua state");
		return Results::Failure;
	}

	if (luaL_loadfile(L, m_path_source) != LUA_OK)
	{
		OutputErrorMessageWithFileInfo(m_path_source, "Failed to load Lua file");
		lua_close(L);
		return Results::Failure;
	}

	if (lua_pcall(L, 0, LUA_MULTRET, 0) != LUA_OK)
	{
		OutputErrorMessageWithFileInfo(m_path_source, "Failed to execute Lua file");
		lua_close(L);
		return Results::Failure;
	}

	if (lua_gettop(L) != 1 || !lua_istable(L, -1))
	{
		OutputErrorMessageWithFileInfo(m_path_source, "Lua file did not return a table");
		lua_close(L);
		return Results::Failure;
	}

	lua_getfield(L, -1, "vertices");
	if (!LoadVertices(L, vertices, m_path_source))
	{
		lua_close(L);
		return Results::Failure;
	}
	lua_pop(L, 1);

	lua_getfield(L, -1, "indices");
	if (!LoadIndices(L, indices, m_path_source))
	{
		lua_close(L);
		return Results::Failure;
	}
	lua_pop(L, 1);

	lua_pop(L, 1);
	lua_close(L);

	// ------------- Write Binary mesh -------------
	std::ofstream fout(m_path_target, std::ios::binary);
	if (!fout.is_open())
	{
		OutputErrorMessageWithFileInfo(m_path_target, "Failed to open output binary file for writing");
		return Results::Failure;
	}

	const uint16_t vertexCount = static_cast<uint16_t>(vertices.size());
	const uint16_t indexCount = static_cast<uint16_t>(indices.size());

	// 1) Write vertex count
	fout.write(reinterpret_cast<const char*>(&vertexCount), sizeof(uint16_t));

	// 2) Write index count
	fout.write(reinterpret_cast<const char*>(&indexCount), sizeof(uint16_t));

	// 3) Write vertex array
	fout.write(reinterpret_cast<const char*>(vertices.data()),
		sizeof(Graphics::VertexFormats::sVertex_mesh) * vertexCount);

	// 4) Write index array
	fout.write(reinterpret_cast<const char*>(indices.data()), sizeof(uint16_t) * indexCount);

	fout.close();

#ifdef _DEBUG
	std::cout << "[MeshBuilder] Built mesh: " << m_path_target
		<< " | Vertices: " << vertexCount
		<< " | Indices: " << indexCount << std::endl;
#endif

	return result;
}
