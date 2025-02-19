//
//  Copyright (C) 2023  Nick Gasson
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include "util.h"
#include "hash.h"
#include "rt/printer.h"
#include "rt/structs.h"
#include "type.h"

#include <assert.h>
#include <stdlib.h>
#include <inttypes.h>

typedef void (*type_fn_t)(print_func_t *, const void *, size_t, print_flags_t);

typedef struct _print_func {
   printer_t *printer;
   type_fn_t  typefn;
} print_func_t;

typedef struct _printer {
   hash_t     *typemap;
   text_buf_t *buf;
} printer_t;

static const char std_logic_map[] = "UX01ZWLH-";

static void int_printer(print_func_t *f, const void *data, size_t size,
                        print_flags_t flags)
{
   assert(size <= 8);
   assert(is_power_of_2(size));

   const int radix = flags & PRINT_F_RADIX;

   if (radix == PRINT_F_HEX || radix == PRINT_F_BIN) {
      uint64_t wide = 0;
      switch (size) {
      case 1: wide = *(uint8_t *)data; break;
      case 2: wide = *(uint16_t *)data; break;
      case 4: wide = *(uint32_t *)data; break;
      case 8: wide = *(uint64_t *)data; break;
      }

      if (radix == PRINT_F_BIN) {
         for (int i = (size * 8) - 1; i >= 0; i--)
            tb_append(f->printer->buf, (wide & (UINT64_C(1) << i)) ? '1' : '0');
      }
      else
         tb_printf(f->printer->buf, "0x%"PRIx64, wide);
   }
   else {
      int64_t wide = 0;
      switch (size) {
      case 1: wide = *(int8_t *)data; break;
      case 2: wide = *(int16_t *)data; break;
      case 4: wide = *(int32_t *)data; break;
      case 8: wide = *(int64_t *)data; break;
      }

      tb_printf(f->printer->buf, "%"PRIi64, wide);
   }
}

static void std_logic_printer(print_func_t *f, const void *data, size_t size,
                              print_flags_t flags)
{
   assert(size == 1);
   const char str[] = { '\'', std_logic_map[*(uint8_t *)data], '\'' };
   tb_catn(f->printer->buf, str, ARRAY_LEN(str));
}

static void std_logic_vector_printer(print_func_t *f, const void *data,
                                     size_t size, print_flags_t flags)
{
   tb_append(f->printer->buf, '"');

   for (int i = 0; i < size; i++) {
      const uint8_t bit = *((uint8_t *)data + i);
      assert(bit < ARRAY_LEN(std_logic_map));
      tb_append(f->printer->buf, std_logic_map[bit]);
   }

   tb_append(f->printer->buf, '"');
}

printer_t *printer_new(void)
{
   printer_t *p = xcalloc(sizeof(printer_t));
   p->typemap = hash_new(128);
   p->buf     = tb_new();

   return p;
}

void printer_free(printer_t *p)
{
   hash_iter_t it = HASH_BEGIN;
   const void *key;
   void *value;
   while (hash_iter(p->typemap, &it, &key, &value))
      free(value);

   hash_free(p->typemap);
   tb_free(p->buf);
   free(p);
}

print_func_t *printer_for(printer_t *p, type_t type)
{
   print_func_t *f = hash_get(p->typemap, type);
   if (f != NULL)
      return f;

   type_t base = type_base_recur(type);
   if ((f = hash_get(p->typemap, base)) != NULL) {
      hash_put(p->typemap, type, f);
      return f;
   }

   f = xcalloc(sizeof(print_func_t));
   f->printer = p;

   switch (type_kind(base)) {
   case T_INTEGER:
      f->typefn = int_printer;
      break;
   case T_ENUM:
       switch (is_well_known(type_ident(base))) {
       case W_IEEE_LOGIC:
       case W_IEEE_ULOGIC:
          f->typefn = std_logic_printer;
          break;
       default:
          goto invalid;
       }
       break;
   case T_ARRAY:
      switch (is_well_known(type_ident(base))) {
      case W_IEEE_LOGIC_VECTOR:
      case W_IEEE_ULOGIC_VECTOR:
          f->typefn = std_logic_vector_printer;
          break;
       default:
          goto invalid;
       }
       break;
   default:
      goto invalid;
   }

   hash_put(p->typemap, type, f);
   return f;

 invalid:
   free(f);
   return NULL;
}

const char *print_signal(print_func_t *fn, rt_signal_t *s, print_flags_t flags)
{
   tb_rewind(fn->printer->buf);
   (*fn->typefn)(fn, s->shared.data, s->shared.size, flags);
   return tb_get(fn->printer->buf);
}
