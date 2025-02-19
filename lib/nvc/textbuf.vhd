-------------------------------------------------------------------------------
--  Copyright (C) 2023  Nick Gasson
--
--  Licensed under the Apache License, Version 2.0 (the "License");
--  you may not use this file except in compliance with the License.
--  You may obtain a copy of the License at
--
--     http://www.apache.org/licenses/LICENSE-2.0
--
--  Unless required by applicable law or agreed to in writing, software
--  distributed under the License is distributed on an "AS IS" BASIS,
--  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
--  See the License for the specific language governing permissions and
--  limitations under the License.
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- An efficient string builder
-------------------------------------------------------------------------------

package textbuf is
    type str_ptr_t is access string;

    type text_buf_t is record
        len   : natural;
        alloc : natural;
        buf   : str_ptr_t;
    end record;

    procedure tb_cat (tb : out text_buf_t; str : in string);

    impure function to_string (tb : inout text_buf_t) return string;
end package;
