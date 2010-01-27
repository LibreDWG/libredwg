#define DWG_LOGLEVEL DWG_LOGLEVEL_INFO

#include "logging.h"




int main() {
  LOG(NONE, "Errei por %d!", 1) //never use this trick!
  LOG_ERROR("Errei por %d!", 2)
  LOG_INFO("Errei por %d!", 3)
  LOG_TRACE("Errei por %d!", 4)
  LOG_ALL("Errei por %d!", 5)
  return 0;
}
