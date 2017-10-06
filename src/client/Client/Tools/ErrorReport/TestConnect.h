#pragma once

class TestConnect
{
public:
	// ip可以是域名
	static bool Run(const char * ip, int port, int waitSeconds);
};
