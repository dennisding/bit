#include "bit.hpp"

#include <iostream>

static bit_ptr create()
{
	auto root = std::make_shared<bit_block>("root");

	root->set_value("12342333model_name");

	// create name
	auto name = std::make_shared<bit_str>("name");
	name->set_value("1234");

	// create pos
	auto pos = std::make_shared<bit_value<float, 3>>("pos");
	pos->set_value("1.1f");
	pos->set_value("2.2f", 1);
	pos->set_value("3.3", 2);

	// add child to root
	root->add_child(std::move(name));
	root->add_child(std::move(pos));

	return root;
}

struct vector
{
	float x_;
	float y_;
	float z_;
};

template <>
struct bit_type<vector>
{
	static vector convert_from(bit* b, size_t index)
	{
		vector v;
		b->copy_to(&v.x_, 3);

		return v;
	}
};

int main(int argc, const char** argv)
{
	// model = model_name
	//		name{str} = hello_world
	//		pos{float, float, float} = 1.f, 2.f, 3.f
	//

	auto root = create();

	auto iv = root->as_int();

	auto rt = root->as_tuple<int, float>();

	float name = root->read<float>("name");

	auto pos_value = root->read_tuple<float, float, float>("pos");

	auto pos_block = root->read("pos");

	auto pos = pos_block->as_tuple<float, float, float>();

	auto new_pos = root->read<vector>("pos");
}