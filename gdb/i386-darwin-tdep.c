/* Darwin support for GDB, the GNU debugger.
   Copyright 1997, 1998, 1999, 2000, 2001, 2002, 2005, 2008, 2009
   Free Software Foundation, Inc.

   Contributed by Apple Computer, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "defs.h"
#include "frame.h"
#include "inferior.h"
#include "gdbcore.h"
#include "target.h"
#include "floatformat.h"
#include "symtab.h"
#include "regcache.h"
#include "libbfd.h"
#include "objfiles.h"

#include "i387-tdep.h"
#include "i386-tdep.h"
#include "amd64-tdep.h"
#include "osabi.h"
#include "ui-out.h"
#include "symtab.h"
#include "frame.h"
#include "gdb_assert.h"
#include "i386-darwin-tdep.h"
#include "solib.h"
#include "solib-darwin.h"

/* Offsets into the struct i386_thread_state where we'll find the saved regs.
   From <mach/i386/thread_status.h> and i386-tdep.h.  */
int i386_darwin_thread_state_reg_offset[] =
{
   0 * 4,   /* EAX */
   2 * 4,   /* ECX */
   3 * 4,   /* EDX */
   1 * 4,   /* EBX */
   7 * 4,   /* ESP */
   6 * 4,   /* EBP */
   5 * 4,   /* ESI */
   4 * 4,   /* EDI */
  10 * 4,   /* EIP */
   9 * 4,   /* EFLAGS */
  11 * 4,   /* CS */
   8,       /* SS */
  12 * 4,   /* DS */
  13 * 4,   /* ES */
  14 * 4,   /* FS */
  15 * 4    /* GS */
};

const int i386_darwin_thread_state_num_regs = 
  ARRAY_SIZE (i386_darwin_thread_state_reg_offset);

/* Offsets into the struct x86_thread_state64 where we'll find the saved regs.
   From <mach/i386/thread_status.h> and amd64-tdep.h.  */
int amd64_darwin_thread_state_reg_offset[] =
{
  0 * 8,			/* %rax */
  1 * 8,			/* %rbx */
  2 * 8,			/* %rcx */
  3 * 8,			/* %rdx */
  5 * 8,			/* %rsi */
  4 * 8,			/* %rdi */
  6 * 8,			/* %rbp */
  7 * 8,			/* %rsp */
  8 * 8,			/* %r8 ... */
  9 * 8,
  10 * 8,
  11 * 8,
  12 * 8,
  13 * 8,
  14 * 8,
  15 * 8,			/* ... %r15 */
  16 * 8,			/* %rip */
  17 * 8,			/* %rflags */
  18 * 8,			/* %cs */
  -1,				/* %ss */
  -1,				/* %ds */
  -1,				/* %es */
  19 * 8,			/* %fs */
  20 * 8			/* %gs */
};

const int amd64_darwin_thread_state_num_regs = 
  ARRAY_SIZE (amd64_darwin_thread_state_reg_offset);

static void
i386_darwin_init_abi (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  /* We support the SSE registers.  */
  tdep->num_xmm_regs = I386_NUM_XREGS - 1;
  set_gdbarch_num_regs (gdbarch, I386_SSE_NUM_REGS);

  tdep->struct_return = reg_struct_return;

  tdep->sigcontext_addr = NULL;
  tdep->sc_reg_offset = i386_darwin_thread_state_reg_offset;
  tdep->sc_num_regs = 16;

  tdep->jb_pc_offset = 20;

  set_solib_ops (gdbarch, &darwin_so_ops);
}

static void
x86_darwin_init_abi_64 (struct gdbarch_info info, struct gdbarch *gdbarch)
{
  struct gdbarch_tdep *tdep = gdbarch_tdep (gdbarch);

  amd64_init_abi (info, gdbarch);

  tdep->struct_return = reg_struct_return;

  /* We don't do signals yet.  */
  tdep->sigcontext_addr = NULL;
  tdep->sc_reg_offset = amd64_darwin_thread_state_reg_offset;
  tdep->sc_num_regs = ARRAY_SIZE (amd64_darwin_thread_state_reg_offset);

  tdep->jb_pc_offset = 148;

  set_solib_ops (gdbarch, &darwin_so_ops);
}

static enum gdb_osabi
i386_mach_o_osabi_sniffer (bfd *abfd)
{
  if (!bfd_check_format (abfd, bfd_object))
    return GDB_OSABI_UNKNOWN;
  
  if (bfd_get_arch (abfd) == bfd_arch_i386)
    return GDB_OSABI_DARWIN;

  return GDB_OSABI_UNKNOWN;
}

void
_initialize_i386_darwin_tdep (void)
{
  gdbarch_register_osabi_sniffer (bfd_arch_unknown, bfd_target_mach_o_flavour,
                                  i386_mach_o_osabi_sniffer);

  gdbarch_register_osabi (bfd_arch_i386, bfd_mach_i386_i386,
			  GDB_OSABI_DARWIN, i386_darwin_init_abi);

  gdbarch_register_osabi (bfd_arch_i386, bfd_mach_x86_64,
                          GDB_OSABI_DARWIN, x86_darwin_init_abi_64);
}
