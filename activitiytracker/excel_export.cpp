#include "excel_export.h"
#include <iostream>
#include <xlnt/xlnt.hpp>

namespace {
static constexpr std::size_t defalloc{ 15'000 }; // ± empty excel file size
xlnt::workbook
make_workbook(std::string book_name)
{
  xlnt::workbook wb;
  auto sheet = wb.active_sheet();
  sheet.title(book_name);
  return wb;
}
}
std::vector<std::uint8_t>
excelexport::serialized_export(const excelexport::dataformat& data)
{
  auto wb = make_workbook("Test");
  auto sheet = wb.active_sheet();
  xlnt::row_properties rp;
  // add_data(std::ref(sheet), data);
  std::for_each(data.begin(), data.end(),
                [&sheet](const auto& row) { sheet.append(row); });
  std::vector<uint8_t> serialized;
  serialized.reserve(defalloc);
  wb.save(std::ref(serialized));
  return serialized;
}
