#pragma once

#include <string>

class static_id{
    public:
	static_id(){}
        static std::string latest_id;
};

std::string static_id::latest_id = "-1";
