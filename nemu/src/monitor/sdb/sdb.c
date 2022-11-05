#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sdb.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  nemu_state.state = NEMU_QUIT; 
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args);

static int cmd_info(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "Step one instruction exactly", cmd_si},
	{ "info", "Generic command for showing things about the program being debugged.", cmd_info}
  /* TODO: Add more commands */
};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_info(char *args) {
	char *arg = strtok(NULL, " ");
	int i;
	if (NULL == arg)  {
		for (i = 0; i < NR_CMD; ++i) {
			if (strcmp("info", cmd_table[i].name) == 0) {
				printf("info - %s\n", cmd_table[i].description);
				break;
			}
		}
	} else {
   	if (strcmp("r", arg) == 0) {
			isa_reg_display();	
		} else {
			bool success;
			word_t reg_value = isa_reg_str2val(arg, &success);
			if (success) {
				printf("%-5s  0x%032x\n", arg, reg_value);	
			} else {
				printf("register %s not exist\n", arg);	
			}
		}
	}
	return 0;
}
	

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static void str_to_uint64(char *str, uint64_t *n) {
	uint64_t result = 0;
	int len = strlen(str), i;
	for (i = 0; i < len; ++i) {
		if (str[i] < '0' || str[i] > '9') {
			return;
		}
		result = result*10 + (str[i] - '0');
	}
	*n = result;
}

static int cmd_si(char *args) {	
	printf("test cmd_si\n");
	printf("args:%s\n", args);
	uint64_t step = 1;
	char *arg = strtok(NULL, " ");
	if (NULL != arg) {
		str_to_uint64(arg, &step);
	}
	printf("step:%lu\n", step);
	cpu_exec(step);	
	return 0;		
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }
  for (char *str; (str = rl_gets()) != NULL; ) {
	char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
