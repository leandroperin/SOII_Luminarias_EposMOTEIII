// EPOS eMote3 Interrupt Controller Initialization

#include <cpu.h>
#include <ic.h>
#include <timer.h>
#include <usb.h>

__BEGIN_SYS

void Cortex_M_IC::init()
{
    db<Init, IC>(TRC) << "IC::init()" << endl;

    CPU::int_disable(); // will be reenabled at Thread::init()
    db<Init, IC>(TRC) << "IC::init:CCR = " << scs(CCR) << endl;
    scs(CCR) |= BASETHR; // BUG
    db<Init, IC>(TRC) << "IC::init:CCR = " << scs(CCR) << endl;

    disable(); // will be enabled on demand as handlers are registered

    // Set all interrupt handlers to int_not()
    for(Interrupt_Id i = 0; i < INTS; i++)
        _int_vector[i] = int_not;
    _int_vector[INT_HARD_FAULT] = hard_fault;

    for(Interrupt_Id i = 0; i < INTS; i++)
        _eoi_vector[i] = 0;
    _eoi_vector[irq2int(IRQ_GPT0A)] = Cortex_M_GPTM::eoi;
    _eoi_vector[irq2int(IRQ_GPT1A)] = Cortex_M_GPTM::eoi;
    _eoi_vector[irq2int(IRQ_GPT2A)] = Cortex_M_GPTM::eoi;
    _eoi_vector[irq2int(IRQ_GPT3A)] = Cortex_M_GPTM::eoi;
    _eoi_vector[irq2int(IRQ_USB)] = Cortex_M_USB::eoi;
    _eoi_vector[irq2int(IRQ_GPIOA)] = Cortex_M_GPIO::eoi;
    _eoi_vector[irq2int(IRQ_GPIOB)] = Cortex_M_GPIO::eoi;
    _eoi_vector[irq2int(IRQ_GPIOC)] = Cortex_M_GPIO::eoi;
    _eoi_vector[irq2int(IRQ_GPIOD)] = Cortex_M_GPIO::eoi;
}

__END_SYS
