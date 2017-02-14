#pragma once
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QStringList>
#include <algorithm>
#include <array>
#include <cstdint>
namespace rest {
template <class T, std::size_t t>
uint32_t
nof_matching_keywords(QStringList& keys, const std::array<T, t>& to_match)
{
  if (static_cast<uint32_t>(keys.size()) < to_match.size())
    return static_cast<uint32_t>(keys.size());
  // counting number of keys present in keys from to_match
  return std::accumulate(
    to_match.begin(), to_match.end(), 0u,
    [&keys](uint32_t prev, const auto& to_match) -> uint32_t {
      if (keys.contains(to_match, Qt::CaseSensitivity::CaseInsensitive)) {
        prev += 1;
      }
      return prev;
    });
}

template <typename F, std::size_t t>
bool
specified_kws_are(QJsonObject json,
                  const std::array<const QString, t>& keywords, F pred)
{
  for (const auto& kv : keywords) {
    if (!pred(json, kv))
      return false;
  }
  return true;
}

template <std::size_t t>
bool
specified_kws_are_numbers(QJsonObject json,
                          const std::array<const QString, t>& keywords)
{
  return specified_kws_are(json, keywords,
                           [](QJsonObject json, const QString& str) {
                             return (json.take(str)).isDouble();
                           });
}
template <std::size_t t>
bool
specified_kws_are_strings(QJsonObject json,
                          const std::array<const QString, t>& keywords)
{
  return specified_kws_are(json, keywords,
                           [](QJsonObject json, const QString& str) {

                             return (json.take(str)).isString();
                           });
}
template <std::size_t t>
bool
specified_kws_are_arrays(QJsonObject json,
                         const std::array<const QString, t>& keywords)
{
  return specified_kws_are(json, keywords,
                           [](QJsonObject json, const QString& str) {
                             return (json.take(str)).isArray();
                           });
}
}
