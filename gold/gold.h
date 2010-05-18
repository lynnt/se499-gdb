// gold.h -- general definitions for gold   -*- C++ -*-

// Copyright 2006, 2007, 2008, 2009, 2010 Free Software Foundation, Inc.
// Written by Ian Lance Taylor <iant@google.com>.

// This file is part of gold.

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
// MA 02110-1301, USA.

#ifndef GOLD_GOLD_H
#define GOLD_GOLD_H

#include "config.h"
#include "ansidecl.h"

#include <cstddef>
#include <cstring>
#include <stdint.h>
#include <sys/types.h>

#ifndef ENABLE_NLS
  // The Solaris version of locale.h always includes libintl.h.  If we
  // have been configured with --disable-nls then ENABLE_NLS will not
  // be defined and the dummy definitions of bindtextdomain (et al)
  // below will conflict with the defintions in libintl.h.  So we
  // define these values to prevent the bogus inclusion of libintl.h.
# define _LIBINTL_H
# define _LIBGETTEXT_H
#endif

// Always include <clocale> first to avoid conflicts with the macros
// used when ENABLE_NLS is not defined.
#include <clocale>

#ifdef ENABLE_NLS
# include <libintl.h>
# define _(String) gettext (String)
# ifdef gettext_noop
#  define N_(String) gettext_noop (String)
# else
#  define N_(String) (String)
# endif
#else
# define gettext(Msgid) (Msgid)
# define dgettext(Domainname, Msgid) (Msgid)
# define dcgettext(Domainname, Msgid, Category) (Msgid)
# define textdomain(Domainname) while (0) /* nothing */
# define bindtextdomain(Domainname, Dirname) while (0) /* nothing */
# define _(String) (String)
# define N_(String) (String)
#endif

// Figure out how to get a hash set and a hash map.

#if defined(HAVE_TR1_UNORDERED_SET) && defined(HAVE_TR1_UNORDERED_MAP) \
    && defined(HAVE_TR1_UNORDERED_MAP_REHASH)

#include <tr1/unordered_set>
#include <tr1/unordered_map>

// We need a template typedef here.

#define Unordered_set std::tr1::unordered_set
#define Unordered_map std::tr1::unordered_map
#define Unordered_multimap std::tr1::unordered_multimap

#define reserve_unordered_map(map, n) ((map)->rehash(n))

#elif defined(HAVE_EXT_HASH_MAP) && defined(HAVE_EXT_HASH_SET)

#include <ext/hash_map>
#include <ext/hash_set>
#include <string>

#define Unordered_set __gnu_cxx::hash_set
#define Unordered_map __gnu_cxx::hash_map
#define Unordered_multimap __gnu_cxx::hash_multimap

namespace __gnu_cxx
{

template<>
struct hash<std::string>
{
  size_t
  operator()(std::string s) const
  { return __stl_hash_string(s.c_str()); }
};

template<typename T>
struct hash<T*>
{
  size_t
  operator()(T* p) const
  { return reinterpret_cast<size_t>(p); }
};

}

#define reserve_unordered_map(map, n) ((map)->resize(n))

#else

// The fallback is to just use set and map.

#include <set>
#include <map>

#define Unordered_set std::set
#define Unordered_map std::map
#define Unordered_multimap std::multimap

#define reserve_unordered_map(map, n)

#endif

#ifndef HAVE_PREAD
extern "C" ssize_t pread(int, void*, size_t, off_t);
#endif

#ifndef HAVE_FTRUNCATE
extern "C" int ftruncate(int, off_t);
#endif

#ifndef HAVE_MREMAP
#define MREMAP_MAYMOVE 1
extern "C" void *mremap(void *, size_t, size_t, int, ...);
#endif

#ifndef HAVE_FFSLL
extern "C" int ffsll(long long);
#endif

#if !HAVE_DECL_MEMMEM
extern "C" void *memmem(const void *, size_t, const void *, size_t);
#endif

#if !HAVE_DECL_STRNDUP
extern "C" char *strndup(const char *, size_t);
#endif

namespace gold
{

// General declarations.

class General_options;
class Command_line;
class Dirsearch;
class Input_objects;
class Mapfile;
class Symbol;
class Symbol_table;
class Layout;
class Task;
class Workqueue;
class Output_file;
template<int size, bool big_endian>
struct Relocate_info;

// Some basic types.  For these we use lower case initial letters.

// For an offset in an input or output file, use off_t.  Note that
// this will often be a 64-bit type even for a 32-bit build.

// The size of a section if we are going to look at the contents.
typedef size_t section_size_type;

// An offset within a section when we are looking at the contents.
typedef ptrdiff_t section_offset_type;

// The name of the program as used in error messages.
extern const char* program_name;

// This function is called to exit the program.  Status is true to
// exit success (0) and false to exit failure (1).
extern void
gold_exit(bool status) ATTRIBUTE_NORETURN;

// This function is called to emit an error message and then
// immediately exit with failure.
extern void
gold_fatal(const char* format, ...) ATTRIBUTE_NORETURN ATTRIBUTE_PRINTF_1;

// This function is called to issue an error.  This will cause gold to
// eventually exit with failure.
extern void
gold_error(const char* msg, ...) ATTRIBUTE_PRINTF_1;

// This function is called to issue a warning.
extern void
gold_warning(const char* msg, ...) ATTRIBUTE_PRINTF_1;

// This function is called to print an informational message.
extern void
gold_info(const char* msg, ...) ATTRIBUTE_PRINTF_1;

// Work around a bug in gcc 4.3.0.  http://gcc.gnu.org/PR35546 .  This
// can probably be removed after the bug has been fixed for a while.
#ifdef HAVE_TEMPLATE_ATTRIBUTES
#define TEMPLATE_ATTRIBUTE_PRINTF_4 ATTRIBUTE_PRINTF_4
#else
#define TEMPLATE_ATTRIBUTE_PRINTF_4
#endif

// This function is called to issue an error at the location of a
// reloc.
template<int size, bool big_endian>
extern void
gold_error_at_location(const Relocate_info<size, big_endian>*,
		       size_t, off_t, const char* format, ...)
  TEMPLATE_ATTRIBUTE_PRINTF_4;

// This function is called to issue a warning at the location of a
// reloc.
template<int size, bool big_endian>
extern void
gold_warning_at_location(const Relocate_info<size, big_endian>*,
			 size_t, off_t, const char* format, ...)
  TEMPLATE_ATTRIBUTE_PRINTF_4;

// This function is called to report an undefined symbol without
// a relocation (e.g., referenced by a dynamic object).  SYM is
// the undefined symbol.  The file name associated with the SYM
// is used to print a location for the undefined symbol.
extern void
gold_undefined_symbol(const Symbol*);

// This function is called to report an undefined symbol resulting
// from a relocation.  SYM is the undefined symbol.  RELINFO is the
// general relocation info.  RELNUM is the number of the reloc,
// and RELOFFSET is the reloc's offset.
template<int size, bool big_endian>
extern void
gold_undefined_symbol_at_location(const Symbol*,
		                  const Relocate_info<size, big_endian>*,
		                  size_t, off_t);

// This is function is called in some cases if we run out of memory.
extern void
gold_nomem() ATTRIBUTE_NORETURN;

// In versions of gcc before 4.3, using __FUNCTION__ in a template
// function can cause gcc to get confused about whether or not the
// function can return.  See http://gcc.gnu.org/PR30988.  Use a macro
// to avoid the problem.  This can be removed when we no longer need
// to care about gcc versions before 4.3.
#if defined(__GNUC__) && GCC_VERSION < 4003
#define FUNCTION_NAME static_cast<const char*>(__FUNCTION__)
#else 
#define FUNCTION_NAME __FUNCTION__
#endif

// This macro and function are used in cases which can not arise if
// the code is written correctly.

#define gold_unreachable() \
  (gold::do_gold_unreachable(__FILE__, __LINE__, FUNCTION_NAME))

extern void do_gold_unreachable(const char*, int, const char*)
  ATTRIBUTE_NORETURN;

// Assertion check.

#define gold_assert(expr) ((void)(!(expr) ? gold_unreachable(), 0 : 0))

// Print version information.
extern void
print_version(bool print_short);

// Get the version string.
extern const char*
get_version_string();

// Convert numeric types without unnoticed loss of precision.
template<typename To, typename From>
inline To
convert_types(const From from)
{
  To to = from;
  gold_assert(static_cast<From>(to) == from);
  return to;
}

// A common case of convert_types<>: convert to section_size_type.
template<typename From>
inline section_size_type
convert_to_section_size_type(const From from)
{ return convert_types<section_size_type, From>(from); }

// Queue up the first set of tasks.
extern void
queue_initial_tasks(const General_options&,
		    Dirsearch&,
		    const Command_line&,
		    Workqueue*,
		    Input_objects*,
		    Symbol_table*,
		    Layout*,
		    Mapfile*);

// Queue up the set of tasks to be done before
// the middle set of tasks.  Only used when garbage
// collection is to be done.
extern void
queue_middle_gc_tasks(const General_options&,
                      const Task*,
                      const Input_objects*,
                      Symbol_table*,
                      Layout*,
                      Workqueue*,
                      Mapfile*);

// Queue up the middle set of tasks.
extern void
queue_middle_tasks(const General_options&,
		   const Task*,
		   const Input_objects*,
		   Symbol_table*,
		   Layout*,
		   Workqueue*,
		   Mapfile*);

// Queue up the final set of tasks.
extern void
queue_final_tasks(const General_options&,
		  const Input_objects*,
		  const Symbol_table*,
		  Layout*,
		  Workqueue*,
		  Output_file* of);

inline bool
is_prefix_of(const char* prefix, const char* str)
{
  return strncmp(prefix, str, strlen(prefix)) == 0;
}

const char* const cident_section_start_prefix = "__start_";
const char* const cident_section_stop_prefix = "__stop_";

// Returns true if the name is a valid C identifier
inline bool
is_cident(const char* name)
{
  return (name[strspn(name,
	 	      ("0123456789"
		       "ABCDEFGHIJKLMNOPWRSTUVWXYZ"
		       "abcdefghijklmnopqrstuvwxyz"
		       "_"))]
	  == '\0');
}

// We sometimes need to hash strings.  Ideally we should use std::tr1::hash or
// __gnu_cxx::hash on some systems but there is no guarantee that either
// one is available.  For portability, we define simple string hash functions.

template<typename Char_type>
inline size_t
string_hash(const Char_type* s, size_t length)
{
  // This is the hash function used by the dynamic linker for
  // DT_GNU_HASH entries.  I compared this to a Fowler/Noll/Vo hash
  // for a C++ program with 385,775 global symbols.  This hash
  // function was very slightly worse.  However, it is much faster to
  // compute.  Overall wall clock time was a win.
  const unsigned char* p = reinterpret_cast<const unsigned char*>(s);
  size_t h = 5381;
  for (size_t i = 0; i < length * sizeof(Char_type); ++i)
    h = h * 33 + *p++;
  return h;
}

// Same as above except we expect the string to be zero terminated.

template<typename Char_type>
inline size_t
string_hash(const Char_type* s)
{
  const unsigned char* p = reinterpret_cast<const unsigned char*>(s);
  size_t h = 5381;
  for (size_t i = 0; s[i] != 0; ++i)
    {
      for (size_t j = 0; j < sizeof(Char_type); j++)
	h = h * 33 + *p++;
    }

  return h;
}

} // End namespace gold.

#endif // !defined(GOLD_GOLD_H)
