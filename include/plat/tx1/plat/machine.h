/*
 * Copyright 2017, Data61
 * Commonwealth Scientific and Industrial Research Organisation (CSIRO)
 * ABN 41 687 119 230.
 *
 * This software may be distributed and modified according to the terms of
 * the GNU General Public License version 2. Note that NO WARRANTY is provided.
 * See "LICENSE_GPLv2.txt" for details.
 *
 * @TAG(DATA61_GPL)
 */

#ifndef __PLAT_MACHINE_H
#define __PLAT_MACHINE_H

enum IRQConstants {
    maxIRQ                          = 224
} platform_interrupt_t;

#include <arch/machine/gic_pl390.h>

#endif  /* ! __PLAT_MACHINE_H */
