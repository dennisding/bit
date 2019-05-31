#pragma once

#include <string>

// predefine
template <typename type>
struct bit_type;

class bit;

template <>
struct bit_type<int>
{
	template <typename type>
	static int convert_from(type& v)
	{
		return (int)v;
	}

	static int convert_from(std::string& value)
	{
		return std::stoi(value);
	}

	static int convert_from(bit* b, size_t index)
	{
		return b->as_int(index);
	}
};

template <>
struct bit_type<float>
{
	template <typename type>
	static float convert_from(type&& v)
	{
		return (float)v;
	}

	static float convert_from(std::string& value)
	{
		return std::stof(value);
	}

	static float convert_from(bit* b, size_t index)
	{
		return b->as_float(index);
	}
};

template <>
struct bit_type<std::string>
{
	template <typename type>
	static std::string convert_from(type &&value)
	{
		return std::to_string(value);
	}

	static std::string convert_from(std::string& value)
	{
		return value;
	}

	static std::string convert_from(bit* b, size_t index)
	{
		return b->as_string();
	}
};