#pragma once

#include <string>
#include <vector>
#include <map>

namespace SRT
{
	static constexpr struct Version
	{
		int major = 1;
		int minor = 0;
		int patch = 1;
	} version_;

	constexpr double pi_ = 3.14159265358979323846264338328;
	constexpr double tau_ = 6.283185307179586476925286766559;

	typedef std::map<std::string, std::string> strmap;

	struct vec3i
	{
		int x;
		int y;
		int z;
	};

	struct vec3d
	{
		double x;
		double y;
		double z;
	};
}