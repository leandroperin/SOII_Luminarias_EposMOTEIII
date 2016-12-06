// EPOS ARMv7 CPU Mediator Implementation

#include <architecture/armv7/cpu.h>
#include <system.h>

__BEGIN_SYS

// Class attributes
unsigned int ARMv7::_cpu_clock;
unsigned int ARMv7::_bus_clock;

// Class methods
void ARMv7::Context::save() volatile
{
    ASM("       sub     sp, #4                  \n"
        "       push    {r12}                   \n"
        "       mov     r12, pc                 \n"
        "       str     r12, [sp,#4]            \n"
        "       pop     {r12}                   \n"
        "       push    {r0-r12, lr}            \n"
        "       sub     sp, #4                  \n"
        "       push    {r12}                   \n");
    mrs12();
    ASM("       str     r12, [sp,#4]            \n"
        "       pop     {r12}                   \n"
        "       str     sp, [%0]                \n" : : "r"(this));
}

void ARMv7::Context::load() const volatile
{
    System::_heap->free(reinterpret_cast<void *>(Memory_Map<Machine>::SYS_STACK), Traits<System>::STACK_SIZE);
    ASM("       mov     sp, %0                  \n"
        "       isb                             \n" // serialize the pipeline so that SP gets updated before the pop
        "       pop     {r12}                   \n" : : "r"(this));
    msr12();
    ASM("       pop     {r0-r12, lr}            \n"
        "       pop     {pc}                    \n");
}

void ARMv7::switch_context(Context * volatile * o, Context * volatile n)
{
    ASM("       sub     sp, #4                  \n"
        "       push    {r12}                   \n"
        "       adr     r12, .ret               \n"
        "       str     r12, [sp,#4]            \n"
        "       pop     {r12}                   \n");
    mrs12();
    ASM("       push    {r0-r12, lr}            \n"
        "       mrs     r12, xpsr               \n"
        "       push    {r12}                   \n"
        "       str     sp, [%0]                \n"
        "       mov     sp, %1                  \n"
        "       isb                             \n" // serialize the pipeline so that SP gets updated before the pop
        "       pop     {r12}                   \n" : : "r"(o), "r"(n));
    msr12();
    ASM("       pop     {r0-r12, lr}            \n"
        "       push    {r12}                   \n"
        "       ldr     r12, [sp, #4]           \n"
        "       mov     pc, r12                 \n" // popping directly into PC causes a Usage Fault???
        ".ret:  pop     {r12}                   \n"
        "       add     sp, #4                  \n"
        "       bx      lr                      \n");
}

__END_SYS
