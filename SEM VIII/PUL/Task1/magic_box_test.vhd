----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    23:08:56 04/22/2013 
-- Design Name: 
-- Module Name:    magic_box_test - Behavioral 
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
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity magic_box_test is
end magic_box_test;

architecture Behavioral of magic_box_test is
  signal i: std_logic_vector(8 downto 0) := (others => '0');
  signal o: std_logic_vector(8 downto 0);
  signal e: std_logic;
  signal fun: std_logic_vector(1 downto 0);
begin
  uut: entity work.magic_box
    generic map(9)
    port map(i, fun, e, o);

  proc: process
  begin
    e <= '1';
    i <= "101010101";

    -- min
    fun <= "00";
    wait for 5 ns;
    assert (o = "000000001")
    report "Bad maximum.";
	 
    -- max
    fun <= "01";
    wait for 5 ns;
    assert (o = "111111111")
    report "Bad maximum.";

    -- sum
    fun <= "11";
    wait for 5 ns;
    assert (o = "000011111")
    report "Bad sum.";
    
    -- median
    fun <= "10";
    wait for 5 ns;
    assert (o = "000010000")
    report "Bad median.";
 
    wait;
  end process;
end architecture Behavioral;

