/* process.h */
#define P_NAME_LEN  16
struct proc_s
{
    struct stackframe_s p_reg;  /* process's registers saved in stack frame */

    reg_t p_ldt_sel;    /* selector in gdt with ldt base and limit */
    struct segdesc_s p_ldt[LDT_ITEMS];  /* local descriptor for code and data */

    u32_t pid;  /* process id */
    char p_name[P_NAME_LEN];    /* process name */
};

struct proc_s proc_cpu[NR_TASKS + NR_PROCS];

struct proc_s *next_proc;


#define BEG_PROC_ADDR   (&proc_cpu[0])
#define END_PROC_ADDR   (&proc_cpu[NR_TASKS + NR_PROCS - 1])
