#ifndef __PARSER_HPP
#define __PARSER_HPP

#include <iostream>
#include <vector>

struct cfg_option
{
    std::string name;
    std::string value;
};

namespace parser
{
    int parse_config_line(const char * line, std::vector<struct cfg_option>&list);
    bool is_bool(std::string&str);
}

#endif
