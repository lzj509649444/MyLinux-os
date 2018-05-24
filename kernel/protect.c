/* protect.c */

#include "kernel.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "i8259a.h"
#include "protect.h"

#define INT_VECTROE_SYS	0x90

struct desctableptr_s {
  char limit[sizeof(u16_t)];
  char base[sizeof(u32_t)];		/* really u24_t + pad for 286 */
};

struct gatedesc_s {
  u16_t offset_low;
  u16_t selector;
  u8_t pad;			/* |000|XXXXX| ig & trpg, |XXXXXXXX| task g */
  u8_t p_dpl_type;		/* |P|DL|0|TYPE| */
  u16_t offset_high;
};

struct gate_table_s{
    void (*gate)();
    u8_t vec_nr;
    u8_t privilege;
};

/* task state segment */
struct tss_s {
  reg_t backlink;
  reg_t esp0;                    /* stack pointer to use during interrupt */
  reg_t ss0;                    /*   "   segment  "  "    "        "     */
  reg_t esp1;
  reg_t ss1;
  reg_t esp2;
  reg_t ss2;
  reg_t cr3;
  reg_t eip;
  reg_t flags;
  reg_t eax;
  reg_t ecx;
  reg_t edx;
  reg_t ebx;
  reg_t esp;
  reg_t ebp;
  reg_t esi;
  reg_t edi;
  reg_t es;
  reg_t cs;
  reg_t ss;
  reg_t ds;
  reg_t fs;
  reg_t gs;
  reg_t ldt;
  u16_t trap;
  u16_t iobase;
/* u8_t iomap[0]; */
};


PRIVATE struct gatedesc_s idt[IDT_ITEMS];	/* zero-init so none present */
PUBLIC struct tss_s tss;    /* zero init */


PRIVATE void init_proctable();
PRIVATE void init_idt_desc(unsigned vec_nr, u32_t offset, unsigned dpl_type);
PUBLIC void init_dataseg(register struct segdesc_s *segdp, u32_t base, u32_t size, int privilege);
PUBLIC u32_t seg2phys(u16_t seg);
PRIVATE void sdesc(register struct segdesc_s *segdp, u32_t base, u32_t size);
PRIVATE void resetGDT();

void divide_error();
void single_step_exception();
void nmi();
void breakpoint_exception();
void overflow();
void bounds_check();
void inval_opcode();
void copr_not_available();
void double_fault();
void copr_seg_overrun();
void inval_tss();
void segment_not_present();
void stack_exception();
void general_protection();
void page_fault();
void copr_error();
void hwint00();
void hwint01();
void hwint02();
void hwint03();
void hwint04();
void hwint05();
void hwint06();
void hwint07();
void hwint08();
void hwint09();
void hwint10();
void hwint11();
void hwint12();
void hwint13();
void hwint14();
void hwint15();
void sys_call();

PUBLIC void prot_init()
{
    struct gate_table_s *gtp;
    struct desctableptr_s *dtp;
    register struct proc_s *rp;

    static struct gate_table_s gate_table[] = { 
		{ divide_error, DIVIDE_VECTOR, INTR_PRIVILEGE },
		{ single_step_exception, DEBUG_VECTOR, INTR_PRIVILEGE },
		{ nmi, NMI_VECTOR, INTR_PRIVILEGE },
		{ breakpoint_exception, BREAKPOINT_VECTOR, USER_PRIVILEGE },
		{ overflow, OVERFLOW_VECTOR, USER_PRIVILEGE },
		{ bounds_check, BOUNDS_VECTOR, INTR_PRIVILEGE },
		{ inval_opcode, INVAL_OP_VECTOR, INTR_PRIVILEGE },
		{ copr_not_available, COPROC_NOT_VECTOR, INTR_PRIVILEGE },
		{ double_fault, DOUBLE_FAULT_VECTOR, INTR_PRIVILEGE },
		{ copr_seg_overrun, COPROC_SEG_VECTOR, INTR_PRIVILEGE },
		{ inval_tss, INVAL_TSS_VECTOR, INTR_PRIVILEGE },
		{ segment_not_present, SEG_NOT_VECTOR, INTR_PRIVILEGE },
		{ stack_exception, STACK_FAULT_VECTOR, INTR_PRIVILEGE },
		{ general_protection, PROTECTION_VECTOR, INTR_PRIVILEGE },
		{ page_fault, PAGE_FAULT_VECTOR, INTR_PRIVILEGE },
	 	{ copr_error, COPROC_ERR_VECTOR, INTR_PRIVILEGE },
        { hwint00, VECTOR( 0), INTR_PRIVILEGE },
		{ hwint01, VECTOR( 1), INTR_PRIVILEGE },
    	{ hwint02, VECTOR( 2), INTR_PRIVILEGE },
    	{ hwint03, VECTOR( 3), INTR_PRIVILEGE },
    	{ hwint04, VECTOR( 4), INTR_PRIVILEGE },
    	{ hwint05, VECTOR( 5), INTR_PRIVILEGE },
		{ hwint06, VECTOR( 6), INTR_PRIVILEGE },
		{ hwint07, VECTOR( 7), INTR_PRIVILEGE },
    	{ hwint08, VECTOR( 8), INTR_PRIVILEGE },
    	{ hwint09, VECTOR( 9), INTR_PRIVILEGE },
    	{ hwint10, VECTOR(10), INTR_PRIVILEGE },
    	{ hwint11, VECTOR(11), INTR_PRIVILEGE },
    	{ hwint12, VECTOR(12), INTR_PRIVILEGE },
    	{ hwint13, VECTOR(13), INTR_PRIVILEGE },
    	{ hwint14, VECTOR(14), INTR_PRIVILEGE },
    	{ hwint15, VECTOR(15), INTR_PRIVILEGE },
        { sys_call, INT_VECTROE_SYS, USER_PRIVILEGE}
        
    };
    
    resetGDT();

    /* Build gdt and idt pointers in GDT */
    
    /* Build descriptors for interrupt gates in IDT */
    for(gtp = &gate_table[0]; gtp < &gate_table[ sizeof(gate_table)/ sizeof(gate_table[0]) ];gtp ++)
    {
        init_idt_desc(gtp->vec_nr, (u32_t)gtp->gate, 
               PRESENT | INT_GATE_TYPE | (gtp->privilege << DPL_SHIFT) );
    }


    /* set new gdt base address and limitation */
    dtp= (struct desctableptr_s *) &gdt[GDT_INDEX];
    * (u16_t *) dtp->limit = (sizeof gdt) - 1;
    * (u32_t *) dtp->base =(u32_t)gdt;

    /*set new idt base address and limitation */
    dtp= (struct desctableptr_s *) &gdt[IDT_INDEX];
    * (u16_t *) dtp->limit = (sizeof idt) - 1;
    * (u32_t *) dtp->base = (u32_t)idt;
    
    /* Build main TSS in GDT table */
    memset((char *)&tss, 0, sizeof(tss));
    tss.ss0 = SS_SELECTOR;
    init_dataseg(&gdt[TSS_INDEX],vir2phys(seg2phys(DS_SELECTOR),&tss),sizeof(tss),INTR_PRIVILEGE);
    gdt[TSS_INDEX].access = PRESENT | (INTR_PRIVILEGE << DPL_SHIFT) | TSS_TYPE;
    tss.iobase = sizeof(tss);

    /* build local descriptor in GDT for LDT's in process table.
     * The LDT is are allocated at compile time in the process table,and
     * initialized whenever a process's map is initialized or changed */
     int ldt_index = 0;
    for(rp = BEG_PROC_ADDR,ldt_index = FIRST_LDT_INDEX; rp <= END_PROC_ADDR; ++rp, ldt_index++)
    {

        init_dataseg(&gdt[ldt_index],vir2phys(seg2phys(DS_SELECTOR),rp->p_ldt),sizeof(rp->p_ldt),INTR_PRIVILEGE);
        gdt[ldt_index].access = PRESENT | LDT_TYPE;
        rp->p_ldt_sel = ldt_index * DESC_SIZE;
    }
    
    init_proctable();

}

PRIVATE void init_proctable()
{
    /* initialize process  */
    struct proc_s *procp = proc_cpu;
    struct task_t *ptask_t = task_table;

    int privilege = 0;
    int eflags = 0x1202;
	

    for(int i = 0 ; i < NR_TASKS + NR_PROCS; ++i)
    {
        privilege = i< NR_TASKS ? TASK_PRIVILEGE: USER_PRIVILEGE;

        /* set IOPL and IF , user process have no permission to visit I/O */
        eflags = i < NR_TASKS ? 0x1202 : 0x202;

        /* set process code segment */
        memcpy((char *)&procp->p_ldt[CS_LDT_INDEX], (char *)&gdt[CS_INDEX],DESC_SIZE);
        procp->p_ldt[CS_LDT_INDEX].access = (PRESENT | SEGMENT | EXECUTABLE | READABLE) | (privilege<<DPL_SHIFT);
    
        /* set process data segment */
        memcpy((char *)&procp->p_ldt[DS_LDT_INDEX], (char *)&gdt[DS_INDEX],DESC_SIZE);
        procp->p_ldt[DS_LDT_INDEX].access = (PRESENT | SEGMENT | WRITEABLE) | (privilege<<DPL_SHIFT);

        /* set process video segment */
        memcpy((char *)&procp->p_ldt[GS_LDT_INDEX],(char *)&gdt[GS_INDEX],DESC_SIZE);
        procp->p_ldt[GS_LDT_INDEX].access = (PRESENT | SEGMENT | WRITEABLE) | (privilege<<DPL_SHIFT);
    
        procp->p_reg.cs = (CS_LDT_INDEX * DESC_SIZE) | TI | privilege;
        procp->p_reg.ds = (DS_LDT_INDEX * DESC_SIZE) | TI | privilege;
        procp->p_reg.es = procp->p_reg.ds;
        procp->p_reg.fs = procp->p_reg.ds;
        procp->p_reg.ss = procp->p_reg.ds;
        procp->p_reg.gs = (GS_LDT_INDEX * DESC_SIZE) | TI | privilege;

        procp->p_reg.eip = (u32_t)(ptask_t->funcation);
        procp->p_reg.esp = (u32_t)(ptask_t->stack); /* local stack for process */
        procp->p_reg.eflags = eflags;   /* IF=1, IOPL=1, bit 2 is always 1 */

        procp->pid = i;

        strcpy(procp->p_name,ptask_t->name);

        ++procp;
        ++ptask_t;
    }

    next_proc = BEG_PROC_ADDR;

}

/*===========================================================================*
 *				init_dataseg				     *
 *===========================================================================*/
PUBLIC void init_dataseg(register struct segdesc_s *segdp, u32_t base, u32_t size, int privilege)
{
    /* build descriptor for a data segment */
    sdesc(segdp, base, size);
    segdp->access = (privilege << DPL_SHIFT) | (PRESENT | SEGMENT | WRITEABLE);
}


/*===========================================================================*
 *				sdesc					     *
 *===========================================================================*/
PRIVATE void sdesc(register struct segdesc_s *segdp, u32_t base, u32_t size)
{
    segdp->base_low = base;
    segdp->base_middle = base >> BASE_MIDDLE_SHIFT;
    segdp->base_high = base >> BASE_HIGH_SHIFT;

    --size;
    segdp->limit_low = size;
    segdp->granularity = (size >> GRANULARITY_SHIFT) | DEFAULT; 
}

/*===========================================================================*
 *				seg2phys					     *
 *===========================================================================*/
PUBLIC u32_t seg2phys(u16_t seg)
{
    register struct segdesc_s *segdp;
    u32_t base;
    segdp = &gdt[seg>>3];
    base = ((u32_t)segdp->base_low<<0) | ((u32_t)segdp->base_middle<<16) | ((u32_t)segdp->base_high<<24);

    return base;
    
}

/*===========================================================================*
 *				init_idt_desc				     *
 *===========================================================================*/
PRIVATE void init_idt_desc(unsigned vec_nr, u32_t offset, unsigned dpl_type)
{
    /* Build descriptor for an interrupt gate */

    register struct gatedesc_s *idtp;

    idtp = &idt[vec_nr];
    idtp->offset_low = offset;
    idtp->selector = CS_SELECTOR;
    idtp->p_dpl_type = dpl_type;
    idtp->offset_high = offset >> OFFSET_HIGH_SHIFT;

}



PRIVATE void resetGDT()
{

    struct desctableptr_s *dtp;
    /* get GDT descriptor address */
    dtp= (struct desctableptr_s *) &gdt[GDT_INDEX];
    /* copy the old GDT to kernel GDT space */
    memcpy((char *)gdt,(char *)(*(u32_t *)dtp->base),*(u16_t *)dtp->limit);


/****************************************************************************
    struct segdesc_s *gdtp = (struct segdesc_s *)(&gdt[GDT_INDEX]);
    u16_t limit_low = gdtp->limit_low;
    u8_t  limit_high = gdtp->granularity & 0x0F;
    u16_t base_low  = gdtp->base_low;
    u8_t  base_middle = gdtp->base_middle;
    u8_t  base_high = gdtp->base_high;

    u32_t limit = (limit_high<<OFFSET_HIGH_SHIFT) | limit_low;
    u32_t base  = (base_high<<BASE_HIGH_SHIFT) | (base_middle<<BASE_MIDDLE_SHIFT) |(base_low);
    memcpy((char *)&gdt,(char *)base,30);
*************************************************************************************************/
    /* copy the old GDT to kernel GDT space */
    

}

