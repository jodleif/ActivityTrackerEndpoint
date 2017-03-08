#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace excelexport {
using dataformat = std::vector<std::vector<std::string>>;

std::vector<std::uint8_t> serialized_export(const dataformat& vec);
}
