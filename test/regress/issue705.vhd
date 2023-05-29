entity issue705 is
end entity;

library ieee;
use ieee.std_logic_1164.all;

architecture test of issue705 is
    signal x : std_logic_vector(1 to 3);
    signal y : natural;
    signal p : std_logic;
    signal q : natural;
    signal r : bit_vector(1 to 3);
    signal s : natural;
begin

    with x select? y <=
        1 when "111",
        2 when "000",
        3 when "1--",
        4 when "0--",
        5 when "--0",
        6 when others;

    p2: process is
    begin
        x <= "000";
        wait for 1 ns;
        assert y = 2;

        x <= "XXX";
        wait for 1 ns;
        assert y = 6;

        x <= "110";
        wait for 1 ns;
        assert y = 3;

        x <= "U10";
        wait for 1 ns;
        assert y = 5;

        p <= '0';
        wait for 1 ns;
        assert q = 99;

        p <= '1';
        wait for 1 ns;
        assert q = 99;

        p <= 'X';
        wait for 1 ns;
        assert q = 3;

        p <= 'U';
        wait for 1 ns;
        assert q = 3;

        r <= "010";
        wait for 1 ns;
        assert s = 6;

        r <= "111";
        wait for 1 ns;
        assert s = 1;

        wait;
    end process;

    p3: process (p) is
    begin
        q <= 0;
        case? p is
            when '0' to '1' => q <= 99;
            when '-' => q <= 3;
        end case?;
    end process;

    p4: process (r) is
    begin
        s <= 0;
        case? r is
            when "111" => s <= 1;
            when "000" => s <= 2;
            when others => s <= 6;
        end case?;
    end process;

end architecture;
