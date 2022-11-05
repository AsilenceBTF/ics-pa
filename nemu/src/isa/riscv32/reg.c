#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
	int i = 0; 
	int reg_count = sizeof(regs) / sizeof(regs[0]); 
	for (i = 0; i < reg_count; ++i) {
		bool success = false;
		word_t reg_value = isa_reg_str2val(regs[i], &success);
		if (!success) {
			reg_value = 0x8badf00d;
		}
		printf("%-5s  0x%032x\n", regs[i], reg_value);	
	}
}

word_t isa_reg_str2val(const char *s, bool *success) {
	int reg_count = sizeof(regs) / sizeof(regs[0]);
	int i;
	for (i = 0; i < reg_count; ++i){
		if (strcmp(s, regs[i]) == 0) {
			*success = true;		
			return cpu.gpr[i]._32;	
		}
	}
	*success = false;
	return 0;
}
