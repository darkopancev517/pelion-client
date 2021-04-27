/*
 * Copyright (c) 2020, Vertexcom Technologies, Inc.
 * All rights reserved.
 *
 * NOTICE: All information contained herein is, and remains
 * the property of Vertexcom Technologies, Inc. and its suppliers,
 * if any. The intellectual and technical concepts contained
 * herein are proprietary to Vertexcom Technologies, Inc.
 * and may be covered by U.S. and Foreign Patents, patents in process,
 * and protected by trade secret or copyright law.
 * Dissemination of this information or reproduction of this material
 * is strictly forbidden unless prior written permission is obtained
 * from Vertexcom Technologies, Inc.
 *
 * Authors: Darko Pancev <darko.pancev@vertexcom.com>
 */

#include <stdio.h>

#include <vcsoc.h>

#include <vcrtos/cpu.h>
#include <vcrtos/thread.h>

extern uint32_t _sstack;
extern uint32_t _estack;

char *thread_arch_stack_init(thread_handler_func_t func, void *arg, void *stack_start, int stack_size)
{
    uint32_t *stk;
    stk = (uint32_t *)((uintptr_t)stack_start + stack_size);

    /* adjust to 32 bit boundary by clearing the last two bits in the address */
    stk = (uint32_t *)(((uint32_t)stk) & ~((uint32_t)0x3));

    /* stack start marker */
    stk--;
    *stk = STACK_MARKER;

    /* make sure the stack is double word aligned (8 bytes) */
    /* This is required in order to conform with Procedure Call Standard for the
     * ARM® Architecture (AAPCS) */
    /* http://infocenter.arm.com/help/topic/com.arm.doc.ihi0042e/IHI0042E_aapcs.pdf */
    if (((uint32_t) stk & 0x7) != 0) {
        /* add a single word padding */
        --stk;
        *stk = ~((uint32_t)STACK_MARKER);
    }

    /* ****************************** */
    /* Automatically popped registers */
    /* ****************************** */

    /* The following eight stacked registers are popped by the hardware upon
     * return from exception. (bx instruction in select_and_restore_context) */

    /* xPSR - initial status register */
    stk--;
    *stk = (uint32_t)INITIAL_XPSR;
    /* pc - initial program counter value := thread entry function */
    stk--;
    *stk = (uint32_t)func;
    /* lr - contains the return address when the thread exits */
    stk--;
    *stk = (uint32_t)thread_exit;
    /* r12 */
    stk--;
    *stk = 0;
    /* r3 - r1 */
    for (int i = 3; i >= 1; i--) {
        stk--;
        *stk = i;
    }
    /* r0 - contains the thread function parameter */
    stk--;
    *stk = (uint32_t)arg;

    /* ************************* */
    /* Manually popped registers */
    /* ************************* */

    /* The following registers are not handled by hardware in return from
     * exception, but manually by select_and_restore_context.
     * For the Cortex-M0(plus) we write registers R11-R4 in two groups to allow
     * for more efficient context save/restore code.
     * For the Cortex-M3 and Cortex-M4 we write them continuously onto the stack
     * as they can be read/written continuously by stack instructions. */

    /* exception return code  - return to task-mode process stack pointer */
    stk--;
    *stk = (uint32_t)EXCEPT_RET_TASK_MODE;

    /* r11 - r4 */
    for (int i = 11; i >= 4; i--) {
        stk--;
        *stk = i;
    }

    /* The returned stack pointer will be aligned on a 32 bit boundary not on a
     * 64 bit boundary because of the odd number of registers above (8+9).
     * This is not a problem since the initial stack pointer upon process entry
     * _will_ be 64 bit aligned (because of the cleared bit 9 in the stacked
     * xPSR and aligned stacking of the hardware-handled registers). */

    return (char*)stk;
}

void thread_arch_stack_print(void)
{
    int count = 0;
    thread_t *current_thread = thread_current();
    uint32_t *sp = (uint32_t *)current_thread->stack_pointer;

    printf("printing the current stack of thread %" PRIkernel_pid "\n", current_thread->pid);
    printf("  address:      data:\n");

    do {
        printf("  0x%08x:   0x%08x\n", (unsigned int)sp, (unsigned int)*sp);
        sp++;
        count++;
    } while (*sp != STACK_MARKER);

    printf("current stack size: %i byte\n", count);
}

int thread_arch_isr_stack_usage(void)
{
    uint32_t *ptr = &_sstack;
    while (((*ptr) == STACK_CANARY_WORD) && (ptr < &_estack))
    {
        ++ptr;
    }
    ptrdiff_t num_used_words = &_estack - ptr;
    return num_used_words * sizeof(*ptr);
}

void *thread_arch_isr_stack_pointer(void)
{
    void *msp = (void *)__get_MSP();
    return msp;
}

void *thread_arch_isr_stack_start(void)
{
    return (void *)&_sstack;
}

__attribute__((naked)) __attribute__((used)) void isr_pendsv(void)
{
    __asm__ volatile (
    /* PendSV handler entry point */
    /* save context by pushing unsaved registers to the stack */
    /* {r0-r3,r12,LR,PC,xPSR,s0-s15,FPSCR} are saved automatically on exception entry */
    ".thumb_func                      \n"
    ".syntax unified                  \n"
    "ldr    r1, =sched_active_thread  \n" /* r1 = &sched_active_thread  */
    "ldr    r12, [r1]                 \n" /* r12 = sched_active_thread   */
    "push   {lr}                      \n" /* push exception return code */

    "cpsid  i                         \n" /* Disable IRQs during sched_run */
    "bl     thread_scheduler_run      \n" /* perform scheduling */
    "cpsie  i                         \n" /* Re-enable interrupts */

    "cmp    r0, r12                   \n" /* if r0 == 0: (no switch required) */
    "it     eq                        \n"
    "popeq  {pc}                      \n" /* Pop exception to pc to return */

    "pop    {lr}                      \n" /* Pop exception from the exception stack */

    "mov    r1,r12                    \n" /* r1 = sched_active_thread */
    "cbz    r1, restore_context       \n" /* goto restore_context if r1 == 0 */

    "mrs    r2, psp                   \n" /* get stack pointer from user mode */

    "stmdb  r2!,{r4-r11,lr}           \n" /* save regs, including lr */
    "str    r2, [r1]                  \n" /* write r0 to thread->sp */

    /* current thread context is now saved */
    "restore_context:                 \n" /* Label to skip thread state saving */

    "ldr    r0, [r0]                  \n" /* load tcb->sp to register 1 */
    "ldmia  r0!, {r4-r11,lr}          \n" /* restore other registers, including lr */

    "msr    psp, r0                   \n" /* restore user mode SP to PSP reg */
    "bx     lr                        \n" /* load exception return value to PC,
                                           * causes end of exception*/

    /* return from exception mode to application mode */
    /* {r0-r3,r12,LR,PC,xPSR,s0-s15,FPSCR} are restored automatically on exception return */
     ".ltorg                           \n" /* literal pool needed to access
                                            * sched_active_thread */
     :
     :
     :
    );
}

void __attribute__((used)) isr_svc(void)
{
    SCB->ICSR = SCB_ICSR_PENDSVSET_Msk;
}

void thread_arch_yield_higher(void)
{
    cpu_trigger_pendsv_interrupt();
}
