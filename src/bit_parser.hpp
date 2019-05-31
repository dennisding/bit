#pragma once

#include "bit.hpp"

#include <vector>

class bit_parser
{
	using bit_vector = std::vector<bit_ptr>;
public:
	bit_parser(const char* buffer, size_t size) 
		: buffer_(buffer), 
		size_(size),
		index_(0)
	{
	}

	bit_ptr parse()
	{
		root_ = std::make_shared<bit_block>("root");

		skip_blank();
		// parse childs
		parse_childs();

		return root_;
	}

private:
	void parse_childs()
	{
		std::string name = parse_str();
		if (name.empty()) {
			return;
		}
	}

	std::string parse_str()
	{
		if (is_finish()) {
			return "";
		}

		char c = buffer_[index_];
		if (c == '\'' ||c == '\"') {
			++index_;
			return scan_str(c);
		}
		else {
			return scan_str(0);
		}
	}

	std::string scan_id_str()
	{
		size_t begin = index_;
		size_t end = index_;

		++index_;
		while (!is_finish()) {
			char c = buffer_[index_];
			++index_;
			if (c == '\r' || c == '\n' || c == '{' || c == '>') { // string end
				break;
			}
		}
	}

	std::string scan_str()
	{

	}

	std::string scan_str(char ch)
	{
		size_t begin = index_;
		size_t end = index_;

		++index_;
		while (!is_finish()) {
			char c = buffer_[index_];
			if (c == ch) { // end of string
				++index_;
				break;
			}
			else { // has other char
				++index_;
				end = index_;
			}
		}

		return std::string(buffer_, begin, end);
	}

	void skip_blank()
	{
		while (!is_finish() && is_blank(buffer_[index_])) ++index_;
	}

	bool is_blank(char c)
	{
		return c == ' ' || c == '\r' || c == '\n' || c == '\t';
	}

	bool is_finish()
	{
		return index_ >= size_;
	}

private:
	const char* buffer_;
	size_t size_;

	// tmp variable
	size_t index_;
	bit_ptr root_;
	bit_vector stack_;
};

bit_ptr parse_bit(const char* buffer, size_t size)
{
	bit_parser parser(buffer, size);
	return parser.parse();
}
