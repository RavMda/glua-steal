/* Copyright (C) 2019 Lewis Clark

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>. */


#include "lualoader.hpp"

static int include(lua_State*) {
	glt::ssdk::ILuaInterface* lua = glt::ssdk::g_clientluainterface;
	const char* filename = lua->CheckString(1);
	const char* fake_filename = lua->IsType(2, GarrysMod::Lua::Type::STRING) ? lua->GetString(2) : filename;

	try {
		glt::lua::RunLua(lua, fake_filename, glt::lua::GetLuaFileContents(filename));
	}
	catch (const std::exception& ex) {
		glt::GetLogger()->warn("Failed to include {}\t{}", filename, ex.what());
	}

	return 0;
}

void glt::lua::RunLua(ssdk::ILuaInterface* lua, const std::string& identifier, const std::string& code,
	const std::string& gfilename, const std::string& gcode) {

	if (luaL_loadbuffer(lua->GetLuaState(), code.c_str(), code.length(), identifier.c_str())) {
		const char* errstr = lua->GetString(-1);
		lua->Pop(1);
		throw std::runtime_error(fmt::format("syntax error '{}'", errstr));
	}

	CreateEnvironment(lua, gfilename, gcode);

	if (lua->PCall(0, 1, 0)) {
		const char* errstr = lua->GetString(-1);
		lua->Pop(1);
		throw std::runtime_error(fmt::format("execution error '{}'", errstr));
	}
}

std::pair<bool, bool> glt::lua::LoadLua(ssdk::ILuaInterface* lua, const std::string& filename, const std::string& code) {
	try {
		RunLua(lua, "gluasteal.lua", GetLuaFileContents(), filename, code);
	}
	catch (const std::filesystem::filesystem_error&) { // gluasteal.lua doesn't exist, supress.
		return std::make_pair(true, true);
	}

	bool shouldloadfile = true;
	bool shouldsavefile = true;

	if (lua->IsType(-1, GarrysMod::Lua::Type::BOOL)) {
		shouldloadfile = lua->GetBool(-1);
	}

	if (lua->IsType(0, GarrysMod::Lua::Type::BOOL)) {
		shouldsavefile = lua->GetBool(0);
	}

	lua->Pop(1);

	return std::make_pair(shouldloadfile, shouldsavefile);
}

std::string glt::lua::GetLuaFileContents(const std::string& path) {
	return file::ReadFile(path);
}

void glt::lua::CreateEnvironment(ssdk::ILuaInterface* lua, const std::string& filename, const std::string& code) {
	lua->CreateTable(); // env

	lua->CreateTable(); // env.gluasteal
		lua->PushString(filename.c_str(), filename.length());
		lua->SetField(-2, "SCRIPT");

		lua->PushString(code.c_str(), code.length());
		lua->SetField(-2, "SOURCE");

		lua->PushNumber(GLUASTEAL_VERSION);
		lua->SetField(-2, "VERSION");

		lua->PushCFunction(include);
		lua->SetField(-2, "include");
	lua->SetField(-2, "gluasteal");

	lua->CreateTable(); // env metatable
		lua->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
		lua->SetField(-2, "__index");
	lua->SetMetaTable(-2);

	lua_setfenv(lua->GetLuaState(), -2);
}

void print(const char* str)
{
	glt::ssdk::ILuaInterface* menu = glt::ssdk::g_menuluainterface;

	menu->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	menu->GetField(-1, "print");
	menu->PushString(str);
	menu->Call(1, 0);
	menu->Pop();
}

static int RunOnClient(lua_State*) {
	glt::ssdk::ILuaInterface* lua = glt::ssdk::g_menuluainterface;
	glt::ssdk::ILuaInterface* client = glt::ssdk::g_clientluainterface;

	if (!lua || !client) {
		return 0;
	}

	std::string code = std::string(lua->CheckString(1));

	try {
		glt::lua::RunLua(client, "lua/includes/init.lua", code);
		client->Pop(1);
	}
	catch (const std::exception& ex){
		print(ex.what());
	}

	return 0;
}

void glt::lua::MenuInit(ssdk::ILuaInterface* menu) {
	menu->PushSpecial(GarrysMod::Lua::SPECIAL_GLOB);
	menu->PushCFunction(RunOnClient);
	menu->SetField(-2, "RunOnClient");
	menu->Pop();
}
