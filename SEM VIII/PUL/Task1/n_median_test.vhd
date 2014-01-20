----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    22:39:11 04/22/2013 
-- Design Name: 
-- Module Name:    n_median_test - Behavioral 
-- Project Name: 
-- Target Devices: 
-- Tool versions: 
-- Description: 
--
-- Dependencies: 
--
-- Revision: 
-- Revision 0.01 - File Created
-- Additional Comments: 
--
----------------------------------------------------------------------------------

-- Test for median_step component.

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity median_step_test is
end median_step_test;

architecture behavioral of median_step_test is
  signal i: std_logic_vector(7 downto 0) := (others => '0');
  signal o: std_logic_vector(7 downto 0);
begin
  uut: entity work.median_step
    generic map(8)
    port map(input => i, output => o);

  sw_proc: process
  begin
    i <= "11111111";
    wait for 5 ns;
    assert o = "01111110"
    report "Bad step";

    i <= "10101010";
    wait for 5 ns;
    assert o = "00101000"
    report "Bad step";

    i <= "10000000";
    wait for 5 ns;
    assert o = "10000000"
    report "Bad step";

    i <= "00100001";
    wait for 5 ns;
    assert o = "00100001"
    report "Bad step";

    wait;
  end process;
end architecture behavioral;


-- Test for n_median component.

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity n_median_test is
end n_median_test;

architecture behavioral of n_median_test is
  signal i: std_logic_vector(7 downto 0) := (others => '0');
  signal o: std_logic_vector(7 downto 0);
begin
  uut: entity work.n_median
    generic map(8)
    port map(input => i, output => o);

  sw_proc: process
  begin
    i <= "11111111";
    wait for 5 ns;
    assert o = "00011000"
    report "Bad step";

    i <= "10101010";
    wait for 5 ns;
    assert o = "00101000"
    report "Bad step";

    i <= "10000000";
    wait for 5 ns;
    assert o = "10000000"
    report "Bad step";

    i <= "01111111";
    wait for 5 ns;
    assert o = "00001000"
    report "Bad step";

    wait;
  end process;
end architecture behavioral;
