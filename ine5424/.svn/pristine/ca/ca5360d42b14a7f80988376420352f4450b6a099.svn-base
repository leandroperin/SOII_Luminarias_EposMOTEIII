// EPOS Cortex_M Timer Mediator Declarations

#ifndef __cortex_m_timer_h
#define __cortex_m_timer_h

#include <cpu.h>
#include <ic.h>
#include <rtc.h>
#include <timer.h>
#include <machine.h>
#include __MODEL_H

__BEGIN_SYS

class Cortex_M_Sys_Tick: public Cortex_M_Model
{
private:
    typedef TSC::Hertz Hertz;

public:
    typedef CPU::Reg32 Count;
    static const unsigned int CLOCK = Traits<CPU>::CLOCK;

protected:
    Cortex_M_Sys_Tick() {}

public:
    static void enable() {
        scs(STCTRL) |= ENABLE;
    }
    static void disable() {
        scs(STCTRL) &= ~ENABLE;
    }

    static Hertz clock() { return CLOCK; }

    static void init(unsigned int f) {
        scs(STCTRL) = 0;
        scs(STCURRENT) = 0;
        scs(STRELOAD) = CLOCK / f;
        scs(STCTRL) = CLKSRC | INTEN;
    }
};

class Cortex_M_GPTM: public Cortex_M_Model
{
protected:
    const static unsigned int CLOCK = Traits<CPU>::CLOCK;

    typedef CPU::Reg32 Count;

protected:
    Cortex_M_GPTM(int channel, const Count & count, bool interrupt = true, bool periodic = true)
    : _channel(channel), _base(reinterpret_cast<Reg32 *>(TIMER0_BASE + 0x1000 * channel)) {
        disable();
        power(FULL);
        reg(GPTMCFG) = 0; // 32-bit timer
        reg(GPTMTAMR) = periodic ? 2 : 1; // 2 -> Periodic, 1 -> One-shot
        reg(GPTMTAILR) = count;
        reg(GPTMIMR) = interrupt * TATO_INT; // Enable or disable time-out interrupt
        enable();
    }

public:
    ~Cortex_M_GPTM() { disable(); power(OFF); }

    unsigned int clock() const { return CLOCK; }
    bool running() { return !(reg(GPTMRIS) & TATO_INT); }

    Count read() { return reg(GPTMTAR); }

    void enable() { reg(GPTMICR) = TATO_INT; reg(GPTMCTL) |= TAEN; }
    void disable() { reg(GPTMCTL) &= ~TAEN; }

    void power(const Power_Mode & mode) { Cortex_M_Model::timer_power(_channel, mode); }

    static void eoi(const IC::Interrupt_Id & int_id) {
        if(int_id == IC::irq2int(IC::IRQ_GPT0A))
            reg(reinterpret_cast<Reg32 *>(TIMER0_BASE), GPTMICR) = -1;
        else if(int_id == IC::irq2int(IC::IRQ_GPT1A))
            reg(reinterpret_cast<Reg32 *>(TIMER1_BASE), GPTMICR) = -1;
        else if(int_id == IC::irq2int(IC::IRQ_GPT2A))
            reg(reinterpret_cast<Reg32 *>(TIMER2_BASE), GPTMICR) = -1;
        else if(int_id == IC::irq2int(IC::IRQ_GPT3A))
            reg(reinterpret_cast<Reg32 *>(TIMER3_BASE), GPTMICR) = -1;
    }
private:
    volatile Reg32 & reg(unsigned int o) { return _base[o / sizeof(Reg32)]; }
    static volatile Reg32 & reg(Reg32 * base, unsigned int o) { return base[o / sizeof(Reg32)]; }

protected:
    int _channel;
private:
    Reg32 * _base;
};

class Cortex_M_PWM_Timer: public Cortex_M_Model
{
protected:
    const static unsigned int CLOCK = Traits<CPU>::CLOCK;

    typedef CPU::Reg32 Count;

protected:
    Cortex_M_PWM_Timer(int channel, const Count & reload, const Count & match, bool invert = false)
    : _channel(channel), _base(reinterpret_cast<Reg32 *>(TIMER0_BASE + 0x1000 * channel)) {
        assert(_channel < TIMERS);
        disable();
        power(FULL);
        reg(GPTMCFG) = 4; // 4 -> 16-bit, only possible value for PWM
        reg(GPTMTAMR) = TCMR | TAMS | 2; // 2 -> Periodic, 1 -> One-shot
        reg(GPTMTAPR) = reload >> 16;
        reg(GPTMTAILR) = reload;
        reg(GPTMTAPMR) = match >> 16;
        reg(GPTMTAMATCHR) = match;
        if(invert)
            reg(GPTMCTL) |= TBPWML;
        else
            reg(GPTMCTL) &= ~TBPWML;
        enable();
    }

public:
    ~Cortex_M_PWM_Timer() { disable(); power(OFF); }

    unsigned int clock() const { return CLOCK; }

    void enable() { reg(GPTMCTL) |= TAEN; }
    void disable() { reg(GPTMCTL) &= ~TAEN; }

    void power(const Power_Mode & mode) { Cortex_M_Model::timer_power(_channel, mode); }

private:
    volatile Reg32 & reg(unsigned int o) { return _base[o / sizeof(Reg32)]; }

private:
    int _channel;
    Reg32 * _base;
};

// Tick timer used by the system
class Cortex_M_Timer: private Timer_Common
{
    friend class Cortex_M;
    friend class Init_System;

protected:
    static const unsigned int CHANNELS = 2;
    static const unsigned int FREQUENCY = Traits<Cortex_M_Timer>::FREQUENCY;

    typedef Cortex_M_Sys_Tick Engine;
    typedef Engine::Count Count;
    typedef IC::Interrupt_Id Interrupt_Id;

public:
    using Timer_Common::Hertz;
    using Timer_Common::Tick;
    using Timer_Common::Handler;
    using Timer_Common::Channel;

    // Channels
    enum {
        SCHEDULER,
        ALARM,
        USER
    };

protected:
    Cortex_M_Timer(const Hertz & frequency, const Handler & handler, const Channel & channel, bool retrigger = true)
    : _channel(channel), _initial(FREQUENCY / frequency), _retrigger(retrigger), _handler(handler) {
        db<Timer>(TRC) << "Timer(f=" << frequency << ",h=" << reinterpret_cast<void*>(handler) << ",ch=" << channel << ") => {count=" << _initial << "}" << endl;

        if(_initial && (channel < CHANNELS) && !_channels[channel])
            _channels[channel] = this;
        else
            db<Timer>(WRN) << "Timer not installed!"<< endl;

        for(unsigned int i = 0; i < Traits<Machine>::CPUS; i++)
            _current[i] = _initial;
    }

public:
    ~Cortex_M_Timer() {
        db<Timer>(TRC) << "~Timer(f=" << frequency() << ",h=" << reinterpret_cast<void*>(_handler)
                       << ",ch=" << _channel << ") => {count=" << _initial << "}" << endl;

        _channels[_channel] = 0;
    }

    Hertz frequency() const { return (FREQUENCY / _initial); }
    void frequency(const Hertz & f) { _initial = FREQUENCY / f; reset(); }

    Tick read() { return _current[Machine::cpu_id()]; }

    int reset() {
        db<Timer>(TRC) << "Timer::reset() => {f=" << frequency()
                       << ",h=" << reinterpret_cast<void*>(_handler)
                       << ",count=" << _current[Machine::cpu_id()] << "}" << endl;

        int percentage = _current[Machine::cpu_id()] * 100 / _initial;
        _current[Machine::cpu_id()] = _initial;

        return percentage;
    }

    void handler(const Handler & handler) { _handler = handler; }

    static void enable() { Engine::enable(); }
    static void disable() { Engine::disable(); }

private:
    static Hertz count2freq(const Count & c) { return c ? Engine::clock() / c : 0; }
    static Count freq2count(const Hertz & f) { return f ? Engine::clock() / f : 0;}

    static void int_handler(const Interrupt_Id & i);

    static void init();

private:
    unsigned int _channel;
    Count _initial;
    bool _retrigger;
    volatile Count _current[Traits<Machine>::CPUS];
    Handler _handler;

    static Cortex_M_Timer * _channels[CHANNELS];
};

// Timer used by Thread::Scheduler
class Scheduler_Timer: public Cortex_M_Timer
{
private:
    typedef RTC::Microsecond Microsecond;

public:
    Scheduler_Timer(const Microsecond & quantum, const Handler & handler): Cortex_M_Timer(1000000 / quantum, handler, SCHEDULER) {}
};

// Timer used by Alarm
class Alarm_Timer: public Cortex_M_Timer
{
public:
    static const unsigned int FREQUENCY = Timer::FREQUENCY;

public:
    Alarm_Timer(const Handler & handler): Cortex_M_Timer(FREQUENCY, handler, ALARM) {}
};


// User timer
class User_Timer: private Timer_Common, private Cortex_M_GPTM
{
public:
    using Timer_Common::Microsecond;
    using Timer_Common::Handler;
    using Timer_Common::Channel;

public:
    User_Timer(const Handler & handler, const Channel & channel, const Microsecond & time, bool periodic = false)
    : Cortex_M_GPTM(channel, us2count(time), handler, periodic), _handler(handler) {
        if(_handler) {
            IC::IRQ id = _channel == 0 ? IC::IRQ_GPT0A : _channel == 1 ? IC::IRQ_GPT1A :
                         _channel == 2 ? IC::IRQ_GPT2A : IC::IRQ_GPT3A;
            IC::int_vector(IC::irq2int(id), _handler);
            IC::enable(id);
        }
    }
    ~User_Timer() {
        if(_handler) {
            IC::IRQ id = _channel == 0 ? IC::IRQ_GPT0A : _channel == 1 ? IC::IRQ_GPT1A :
                         _channel == 2 ? IC::IRQ_GPT2A : IC::IRQ_GPT3A;
            IC::disable(id);
        }
    }

    using Cortex_M_GPTM::running;

    Microsecond read() { return count2us(Cortex_M_GPTM::read()); }

    using Cortex_M_GPTM::enable;
    void disable() {
        Cortex_M_GPTM::disable();
        if(_handler) {
            IC::IRQ id = _channel == 0 ? IC::IRQ_GPT0A : _channel == 1 ? IC::IRQ_GPT1A :
                         _channel == 2 ? IC::IRQ_GPT2A : IC::IRQ_GPT3A;
            IC::disable(id);
        }
    }
    using Cortex_M_GPTM::power;

private:
    static Reg32 us2count(const Microsecond & us) { return us * (CLOCK / 1000000); }
    static Microsecond count2us(Reg32 count) { return count / (CLOCK / 1000000); }

private:
    Handler _handler;
};

// PWM Mediator
class PWM: private Timer_Common, public Cortex_M_PWM_Timer
{
public:
    using Timer_Common::Hertz;
    using Timer_Common::Channel;

public:
    PWM(const Channel & channel, const Hertz & frequency, unsigned char duty_cycle_percent, char gpio_port = 'A', unsigned int gpio_pin = 0)
    : Cortex_M_PWM_Timer(channel, freq2reload(frequency), percent2match(duty_cycle_percent, freq2reload(frequency))) {
        Cortex_M_Model::pwm_config(channel, gpio_port, gpio_pin);
    }

    ~PWM() {}

    using Cortex_M_PWM_Timer::enable;
    using Cortex_M_PWM_Timer::disable;
    using Cortex_M_PWM_Timer::power;

private:
    static Count freq2reload(const Hertz & freq) { return CLOCK / freq; }
    static Count percent2match(unsigned char duty_cycle_percent, const Count & period) { 
        return period - ((period * duty_cycle_percent) / 100);
    }
};

__END_SYS

#endif
