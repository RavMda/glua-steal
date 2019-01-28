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


#ifndef INIT_H
#define INIT_H

#include <cinttypes>
#include <thread>

#include "logger.h"
#include "file/file.h"
#include "library/library.h"

#include "lua/luaexports.h"

#include "gamesdk/IVEngineClient.h"
#include "gamesdk/ILuaShared.h"
#include "gamesdk/ILuaInterface.h"

#include "hook/luashared.h"
#include "hook/luainterface.h"

namespace glt {
	std::uintptr_t* Init(std::uintptr_t*);
}

#endif