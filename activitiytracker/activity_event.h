#ifndef ACTIVITY_EVENT_H
#define ACTIVITY_EVENT_H
#include <cstdint>
namespace event {
struct activity_event
{
  int64_t timestamp;
  int32_t activity;
};
}

#endif // ACTIVITY_EVENT_H
