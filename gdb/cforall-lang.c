/* C language support routines for GDB, the GNU debugger.

   Copyright (C) 1992-2018 Free Software Foundation, Inc.

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
#include "block.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "expression.h"
#include "parser-defs.h"
#include "language.h"
#include "varobj.h"
#include "c-lang.h"
#include "c-support.h"
#include "valprint.h"
#include "macroscope.h"
#include "charset.h"
#include "demangle.h"
#include "cp-abi.h"
#include "cp-support.h"
#include "gdb_obstack.h"
#include <ctype.h>
#include "gdbcore.h"

#define ADVANCE					\
  do {						\
    ++p;					\
    if (p == limit)				\
      error (_("Malformed escape sequence"));	\
  } while (0)


enum c_primitive_types {
  c_primitive_type_int,
  c_primitive_type_long,
  c_primitive_type_short,
  c_primitive_type_char,
  c_primitive_type_float,
  c_primitive_type_double,
  c_primitive_type_void,
  c_primitive_type_long_long,
  c_primitive_type_signed_char,
  c_primitive_type_unsigned_char,
  c_primitive_type_unsigned_short,
  c_primitive_type_unsigned_int,
  c_primitive_type_unsigned_long,
  c_primitive_type_unsigned_long_long,
  c_primitive_type_long_double,
  c_primitive_type_complex,
  c_primitive_type_double_complex,
  c_primitive_type_decfloat,
  c_primitive_type_decdouble,
  c_primitive_type_declong,
  nr_c_primitive_types
};

static const char *cforall_extensions[] =
{
  ".cfa", NULL
};

static int
cforall_sniff_from_mangled_name (const char *mangled, char **demangled)
{
    *demangled = gdb_demangle(mangled, 0 | DMGL_CFORALL);
    return *demangled != NULL;
}

/* Impforallementation of la_lookup_symbol_nonlocal for CFA.  */
/*
static struct block_symbol
cforall_lookup_symbol_nonlocal (const struct language_defn *langdef,
			     const char *name,
			     const struct block *block,
			     const domain_enum domain)
{
    printf("HELLO from cforall_lookup_symbol_nonlocal: %s\n", name);
  struct block_symbol result = {NULL, NULL};
  result = lookup_symbol_in_static_block (name, block, domain);
  if (result.symbol != NULL)
      return result;

  if (domain == VAR_DOMAIN)
  {
      struct gdbarch *gdbarch;

      if (block == NULL)
          gdbarch = target_gdbarch();
      else
          gdbarch = block_gdbarch(block);
      result.symbol = language_lookup_primitive_type_as_symbol (langdef, gdbarch, name);
      result.block = NULL;
      if (result.symbol != NULL)
          return result;
  }

  result = lookup_global_symbol (name, block, domain);
    return result;
}
*/

extern const struct language_defn cforall_language_defn =
{
  "cforall",				/* Language name */
  "CForAll",                /* Natural name */
  language_cforall,
  range_check_off,
  case_sensitive_on,
  array_row_major,
  macro_expansion_c,
  cforall_extensions,
  &exp_descriptor_c,
  c_parse,
  null_post_parser,
  c_printchar,			/* Print a character constant */
  c_printstr,			/* Function to print string constant */
  c_emit_char,			/* Print a single char */
  c_print_type,			/* Print a type using appropriate syntax */
  c_print_typedef,		/* Print a typedef using appropriate syntax */
  c_val_print,			/* Print a value using appropriate syntax */
  c_value_print,		/* Print a top-level value */
  default_read_var_value,	/* la_read_var_value */
  NULL,				/* Language specific skip_trampoline */
  NULL,				/* name_of_this */
  true,				/* la_store_sym_names_in_linkage_form_p */
  cp_lookup_symbol_nonlocal,	/* lookup_symbol_nonlocal */
  basic_lookup_transparent_type,/* lookup_transparent_type */
  gdb_demangle,				/* Language specific symbol demangler */
  cforall_sniff_from_mangled_name,
  NULL,				/* Language specific
				   class_name_from_physname */
  c_op_print_tab,		/* expression operators for printing */
  1,				/* c-style arrays */
  0,				/* String lower bound */
  default_word_break_characters,
  default_collect_symbol_completion_matches,
  c_language_arch_info,
  default_print_array_index,
  default_pass_by_reference,
  c_get_string,
  c_watch_location_expression,
  NULL,				/* la_get_symbol_name_matcher */
  iterate_over_symbols,
  default_search_name_hash,
  &c_varobj_ops,
  c_get_compile_context,
  c_compute_program,
  LANG_MAGIC
};
