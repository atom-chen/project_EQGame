#include "EQCommPCH.h"

#include "luabind/luabind/luabind.hpp"
#include <luabind/lua_include.hpp>

#include "LuaManager.h"

#include "ConfigOption.h"
#include "EQOgreSys.h"

using namespace luabind;

bool CLuaManager::RegisterConfigOptionClass()
{
	module( EQ_GLOBA_CLASS )
		[
			luabind::def("getEQOgreSys",&EQOgreSys::getSingleton)
			,

			class_<ConfigOption>("ConfigOption")
			.def("setFullScreen",&ConfigOption::setFullScreen)
			.def("changeResolution",&ConfigOption::changeResolution)
			.def("changeAntiAliasing",&ConfigOption::changeAntiAliasing)
			.def("changeShadow",&ConfigOption::changeShadow)
			.def("changeParticleQuality",&ConfigOption::changeParticleQuality)
			.def("insertAAOption",&ConfigOption::insertAAOption)
			.def("isFullScreen",&ConfigOption::isFullScreen)
			.def("getWidth",&ConfigOption::getWidth)
			.def("getHeight",&ConfigOption::getHeight)
			.def("getAA",&ConfigOption::getAA)
			.def("getShadow",&ConfigOption::getShadow)
			.def("getParticleQuality",&ConfigOption::getParticleQuality)
			.def("save",&ConfigOption::save)
			,

			class_<EQOgreSys>("EQOgreSys")
			.def("getConfigOption",&EQOgreSys::getConfigOption)
		];

	return true;
}