#include "bit.hpp"

#include <iostream>

bit_ptr create()
{
	auto root = std::make_shared<bit_block>("root");

	root->set_value("model_name");

	// create name
	auto name = std::make_shared<bit_str>("name");
	name->set_value("1234hello_world");

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

std::string test(std::string &&value)
{
	return std::forward<std::string>(value);
}

int main(int argc, const char** argv)
{
	// model = model_name
	//		name{str} = hello_world
	//		pos{float, float, float} = 1.f, 2.f, 3.f
	//

	auto root = create();

	float name = root->read<float>("name");

	auto result = test("hello");

	std::cout << result << std::endl;

	//auto pos_block = root->read("pos");

	//auto pos = pos_block->as_tuple<float, float, float>();
}