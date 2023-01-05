#include "sl_ot_init.h"

void sl_ot_init(void)
{
  sl_ot_create_instance();
  sl_ot_ncp_init();
}