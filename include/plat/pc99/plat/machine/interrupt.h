/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(GD_GPL)
 */

#ifndef __PLAT_MACHINE_INTERRUPT_H
#define __PLAT_MACHINE_INTERRUPT_H

#include <config.h>
#include <types.h>
#include <util.h>

#include <arch/object/structures.h>
#include <arch/model/statedata.h>
#include <arch/kernel/apic.h>
#include <plat/machine/acpi.h>
#include <plat/machine/ioapic.h>
#include <plat/machine/pic.h>
#include <plat/machine/intel-vtd.h>

/* Handle a platform-reserved IRQ. */
static inline void
handleReservedIRQ(irq_t irq)
{
    if (config_set(CONFIG_IOMMU) && irq == irq_iommu) {
        vtd_handle_fault();
        return;
    }
}

/* Get the IRQ number currently working on. */
static inline irq_t
getActiveIRQ(void)
{
    if (ARCH_NODE_STATE(x86KScurInterrupt) == int_invalid) {
        return irqInvalid;
    }
    return ARCH_NODE_STATE(x86KScurInterrupt) - IRQ_INT_OFFSET;
}

/* Checks for pending IRQ */
static inline bool_t
isIRQPending(void)
{
    if (apic_is_interrupt_pending()) {
        return true;
    }

    if (config_set(CONFIG_IRQ_PIC) && pic_is_irq_pending()) {
        return true;
    }

    return false;
}

static inline void
ackInterrupt(irq_t irq)
{
    if (config_set(CONFIG_IRQ_PIC) && irq <= irq_isa_max) {
        pic_ack_active_irq();
    } else {
        apic_ack_active_interrupt();
    }
}

static inline void
handleSpuriousIRQ(void)
{
    /* do nothing */
}

static void inline
updateIRQState(word_t irq, x86_irq_state_t state)
{
    assert(irq >= 0 && irq <= maxIRQ);
    x86KSIRQState[irq] = state;
}

static inline void
maskInterrupt(bool_t disable, irq_t irq)
{
    if (irq >= irq_isa_min && irq <= irq_isa_max) {
        if (config_set(CONFIG_IRQ_PIC)) {
            pic_mask_irq(disable, irq);
        } else {
            /* We shouldn't receive interrupts on the PIC range
             * if not using the PIC, but soldier on anyway */
        }
    } else if (irq >= irq_user_min && irq <= irq_user_max) {
        x86_irq_state_t state = x86KSIRQState[irq];
        switch (x86_irq_state_get_irqType(state)) {
        case x86_irq_state_irq_ioapic: {
            uint32_t ioapic = x86_irq_state_irq_ioapic_get_id(state);
            uint32_t pin = x86_irq_state_irq_ioapic_get_pin(state);
            ioapic_mask(disable, ioapic, pin);
            state =  x86_irq_state_irq_ioapic_set_masked(state, disable);
            updateIRQState(irq, state);
        }
        break;
        case x86_irq_state_irq_msi:
            /* currently MSI interrupts can not be disabled */
            break;
        case x86_irq_state_irq_free:
            /* A spurious interrupt, and the resulting mask here,
             * could be from a user ripping out a vector before
             * the interrupt reached the kernel. Silently ignore */
            break;
        }
    } else {
        /* masking some other kind of interrupt source, this probably
         * shouldn't happen, but soldier on */
    }
}

#endif
