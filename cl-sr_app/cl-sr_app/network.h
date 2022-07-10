#pragma once

#define WIN32_LEAN_AND_MEAN

#include <WinSock2.h>

namespace cl_sr
{
	class Network
	{
	public:
		static bool Init();
		static void Shutdown();
	};
}