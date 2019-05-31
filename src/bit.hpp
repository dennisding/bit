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
		return bit_type<type>::convert_from(this, index);
	}

	template <typename... types>
	std::tuple<types...> as_tuple()
	{
		using index = typename make_index<types...>::type;
		
		return make_tuple<types...>(index());
	}

	template <typename type>
	type read(const std::string &name)
	{
		return read(name.c_str());
	}

	template <typename type>
	type read(const char* name)
	{
		auto child = read(name);
		if (!child) {
			return type();
		}

		return child->as<type>();
	}

	template <typename ...types>
	std::tuple<types...> read_tuple(const std::string &name)
	{
		return read_tuple<types...>(name.c_str());
	}

	template <typename ...types>
	std::tuple<types...> read_tuple(const char* name)
	{
		auto child = read(name);
		if (!child) {
			return std::make_tuple<types...>(types()...);
		}

		return child->as_tuple<types...>();
	}

	template <typename type>
	void copy_to(type *vec, size_t size)
	{
		for (size_t index = 0; index < size; ++index) {
			vec[index] = this->as<type>(index);
		}
	}

	const std::string& name()
	{
		return name_;
	}

	// virtual methods
	virtual void ready()
	{
	}

	virtual std::shared_ptr<bit> read(const char *name)
	{
		return nullptr;
	}

	virtual std::shared_ptr<bit> read(size_t index)
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
	template <typename ...types, size_t ...indexs>
	std::tuple<types...> make_tuple(tuple_index<indexs...>&& index)
	{
		return std::make_tuple<types...>(this->as<types>(indexs)...);
	}

private:
	std::string name_;
};

using bit_ptr = std::shared_ptr<bit>;

// bit value
template <typename type, size_t fixed_size>
class bit_value : public bit
{
	using value_vector = std::vector<type>;
public:
	bit_value(const std::string& name) : bit(name), values_(fixed_size)
	{
	}

	virtual void set_value(const std::string& value, size_t index = 0)
	{
		assert(index < values_.size());
		values_[index] = bit_type<type>::convert_from((std::string&)value);
	}

	virtual int as_int(size_t index = 0)
	{
		assert(index < values_.size());
		return do_convert<int, type>(values_[index]);
	}

	template <typename return_type, typename postpone>
	return_type do_convert(type& value)
	{
		return bit_type<return_type>::convert_from(value);
	}

	virtual float as_float(size_t index = 0)
	{
		assert(index < values_.size());
		return do_convert<float, type>(values_[index]);
	}

	virtual std::string as_string(size_t index = 0)
	{
		assert(index < values_.size());
		return do_convert<std::string, type>(values_[index]);
	}

private:
	value_vector values_;
};

class bit_block : public bit
{
	using child_vector = std::vector<bit_ptr>;
public:
	bit_block(const std::string& name) : bit(name), list_(false), value_(nullptr)
	{
	}

	void add_child(bit_ptr &&child)
	{
		childs_.push_back(std::forward<bit_ptr>(child));
	}

	virtual bit_ptr read(const char *name)
	{
		for (auto& child : childs_) {
			if (std::strcmp(child->name().c_str(), name) == 0) {
				return child;
			}
		}

		return nullptr;
	}

	// virtual methodos
	virtual void ready()
	{

	}

	virtual void set_value(const std::string& value, size_t index = 0)
	{
		value_ = std::make_shared<bit_value<std::string, 1>>("_");
		value_->set_value(value);
	}

	virtual bit_ptr read(size_t index)
	{
		assert(index < childs_.size());

		return childs_[index];
	}

	virtual int as_int(size_t index = 0)
	{
		if (value_) {
			return value_->as_int();
		}

		return 0;
	}

	virtual float as_float(size_t index = 0)
	{
		if (value_) {
			return value_->as_float();
		}
		return 0.f;
	}

	virtual std::string as_string(size_t index = 0)
	{
		if (value_) {
			return value_->as_string();
		}
		return "";
	}

private:
	bool list_;
	bit_ptr value_;
	child_vector childs_;
};

#include "bit_types.hpp"

using bit_str = bit_value<std::string, 1>;
