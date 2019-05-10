#pragma once

#include <tuple>
#include <memory>
#include <string>
#include <vector>
#include <cassert>

template <typename type>
struct bit_type
{
};

class bit
{
	template <size_t ...indexs>
	struct tuple_index
	{
	};

	template <typename head, typename tails>
	struct cat_tuple_index {};

	template <size_t ...head_index, size_t ...tail_index>
	struct cat_tuple_index<tuple_index<head_index...>, tuple_index<tail_index...>>
	{
		using type = tuple_index<head_index..., tail_index...>;
	};

	template <typename ...types>
	struct make_index
	{
	};

	template <typename head>
	struct make_index<head>
	{
		using type = tuple_index<0>;
	};

	template <typename head, typename ...tails>
	struct make_index<head, tails...>
	{
		using head_index = tuple_index<sizeof...(tails)>;
		using tail_index = typename make_index<tails...>::type;
		using type = typename cat_tuple_index<tail_index, head_index>::type;
	};

public:
	bit(const std::string& name) : name_(name)
	{
	}

	virtual ~bit() {}

	template <typename type>
	type as(size_t index = 0)
	{
		return bit_type<type>::convert_from(this);
	}

	template <typename ...types>
	std::tuple<types...> as_tuple()
	{
		using index = typename make_index<sizeof...(types)>::type;
		
		return as_tuple<types...>(index());
	}

	template <typename ...types, size_t ...indexs>
	std::tuple<types...> as_tuple(tuple_index<indexs...>& index)
	{
//		return std::make_tuple<types...>(bit_type<types>::convert_from(this, indexs)...);
		return std::make_tuple<types...>(as<types>(indexs)...);
	}

	template <typename type>
	type read(const std::string &name)
	{
		auto child = read(name);
		if (!child) {
			return type();
		}

		return child->as<type>();
	}

	const std::string& name()
	{
		return name_;
	}

	// virtual methods
	virtual std::shared_ptr<bit> read(const std::string& name)
	{
		return nullptr;
	}

	virtual void set_value(const std::string& name, size_t index = 0)
	{
	}

	virtual int as_int(size_t index = 0)
	{
		return 0;
	}

	virtual float as_float(size_t index = 0)
	{
		return 0.f;
	}

	virtual std::string as_string(size_t index = 0)
	{
		return "";
	}

private:
	std::string name_;
};

using bit_ptr = std::shared_ptr<bit>;

class bit_block : public bit
{
	using child_vector = std::vector<bit_ptr>;
public:
	bit_block(const std::string& name) : bit(name), value_(nullptr)
	{

	}

	void add_child(bit_ptr &&child)
	{
		childs_.push_back(std::forward<bit_ptr>(child));
	}

	virtual bit_ptr read(const std::string& name)
	{
		for (auto& child : childs_) {
			if (child->name() == name) {
				return child;
			}
		}

		return nullptr;
	}

private:
	bit_ptr value_;
	child_vector childs_;
};

template <typename type, size_t fixed_size>
class bit_value : public bit
{
	using value_vector = std::vector<type>;
public:
	bit_value(const std::string& name) : bit(name), values_(fixed_size)
	{
	}

	virtual void set_value(std::string&& value, size_t index = 0)
	{
		assert(index < fixed_size);
		values_[index] = bit_type<type>::convert_from(std::forward<std::string>(value));
	}

	virtual int as_int(size_t index = 0)
	{
		assert(index < fixed_size);
		return do_convert<int, type>(values_[index]);
//		return bit_type<type>::convert_from(values_[index]);
	}

	template <typename return_type, typename postpone>
	return_type do_convert(type& value)
	{
		return bit_type<return_type>::convert_from(value);
	}

	virtual float as_float(size_t index = 0)
	{
		return do_convert<float, type>(values_[index]);
	}

	virtual std::string as_string(size_t index = 0)
	{
		return do_convert<std::string, type>(values_[index]);
	}

private:
	value_vector values_;
};

#include "bit_types.hpp"

using bit_str = bit_value<std::string, 1>;
