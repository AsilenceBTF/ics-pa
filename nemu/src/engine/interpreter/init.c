#include <cpu/cpu.h>

void sdb_mainloop();

void engine_start() {
#ifdef CONFIG_TARGET_AM
  printf("%d\n", nemu_state.state);
  cpu_exec(-1);
#else
  /* Receive commands from user. */
  sdb_mainloop();
#endif
}
