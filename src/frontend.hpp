#pragma once
#include "cpu.hpp"
#include <sstream>
int assembly_cli(int argc, char** argv);
bool assembly_command(const std::string&, std::istringstream&, Memory&, CPU&);
