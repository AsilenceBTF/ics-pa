#include <cpu/cpu.h>

void sdb_mainloop();

void engine_start() {
#ifdef CONFIG_TARGET_AM
    cpu_exec(-1);
#else
  /* Receive commands from user. */
  printf("leitanglong%d\n", nemu_state.state);
  sdb_mainloop();
#endif
}
