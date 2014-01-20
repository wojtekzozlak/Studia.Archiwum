----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    00:23:48 04/23/2013 
-- Design Name: 
-- Module Name:    n_minmax_test - Behavioral 
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

entity n_min_test is
end n_min_test;

architecture behavioral of n_min_test is
  signal i:  std_logic_vector(10 downto 0) := (others => '0');
  signal o: std_logic_vector(10 downto 0);
begin
  uut: entity work.n_min
    generic map(11)
    port map(input => i, output => o);

  proc: process
  begin
    i <= "00000000000";
    wait for 5 ns;
    assert "00000000000" = o
    report "Wrong min.";
  
    i <= "10101010101";
    wait for 5 ns;
    assert "00000000001" = o
    report "Wrong min.";

    i <= "10000000000";
    wait for 5 ns;
    assert "11111111111" = o
    report "Wrong min.";

    i <= "11111111000";
    wait for 5 ns;
    assert "00000001111" = o
    report "Wrong min.";

    wait;
  end process;
end architecture behavioral;



library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity n_max_test is
end n_max_test;

architecture behavioral of n_max_test is
  signal i:  std_logic_vector(10 downto 0) := (others => '0');
  signal o: std_logic_vector(10 downto 0);
begin
  uut: entity work.n_max
    generic map(11)
    port map(input => i, output => o);

  proc: process
  begin
    i <= "00000000000";
    wait for 5 ns;
    assert "00000000000" = o
    report "Wrong max.";

    i <= "10101010101";
    wait for 5 ns;
    assert "11111111111" = o
    report "Wrong max.";

    i <= "10000000000";
    wait for 5 ns;
    assert "11111111111" = o
    report "Wrong max.";

    i <= "00011111111";
    wait for 5 ns;
    assert "00011111111" = o
    report "Wrong max.";

    wait;
  end process;
end architecture behavioral;


