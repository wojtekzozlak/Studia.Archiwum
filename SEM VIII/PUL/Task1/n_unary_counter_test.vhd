----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    22:43:48 04/22/2013 
-- Design Name: 
-- Module Name:    n_unary_counter_test - Behavioral 
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

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity n_shifter_test is
end n_shifter_test;

architecture behavioral of n_shifter_test is
  signal i:  std_logic_vector(7 downto 0) := (others => '0');
  signal o: std_logic_vector(7 downto 0);
begin
  uut: entity work.n_shifter
    generic map(8)
    port map(input => i, output => o);

  proc: process
  begin
    i <= "10011001";
    wait for 5 ns;
    assert "01001101" = o
    report "Wrong count.";

    i <= "10000000";
    wait for 5 ns;
    assert "01000000" = o
    report "Wrong count.";

    i <= "11111111";
    wait for 5 ns;
    assert "11111111" = o
    report "Wrong count.";

    i <= "11110001";
    wait for 5 ns;
    assert "01111001" = o
    report "Wrong count.";
 
    wait;
  end process;
end architecture behavioral;



library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity n_unary_counter_test is
end n_unary_counter_test;

architecture behavioral of n_unary_counter_test is
  signal sw:  std_logic_vector(7 downto 0) := (others => '0');
  signal led: std_logic_vector(7 downto 0);
begin
  uut: entity work.n_unary_counter
    generic map(8)
    port map(input => sw, output => led);

  proc: process
  begin
    sw <= "10011001";
    wait for 5 ns;
    assert "00001111" = led
    report "Wrong count.";

    sw <= "10000000";
    wait for 5 ns;
    assert "00000001" = led
    report "Wrong count.";

    sw <= "11111111";
    wait for 5 ns;
    assert "11111111" = led
    report "Wrong count.";

    sw <= "11110001";
    wait for 5 ns;
    assert "00011111" = led
    report "Wrong count.";
 
    wait;
  end process;
end architecture behavioral;


