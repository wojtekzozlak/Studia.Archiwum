----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    19:49:43 04/30/2013 
-- Design Name: 
-- Module Name:    timertest - Behavioral 
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
use ieee.numeric_std.all;


entity add3test is
end add3test;
 
architecture Behavioral of add3test is
  signal i: std_logic_vector(3 downto 0) := (others => '0');
  signal o: std_logic_vector(3 downto 0);
begin
  uut: entity work.add3 port map(i, o);

  tst_process: process
  begin
   i <= "0010";
   wait for 5ns;
   assert o = "0010"
   report "Bad add3 for 0010";

   i <= "0110";
   wait for 5ns;
   assert o = "1001"
   report "Bad add3 for 0110";

   wait;
  end process;
end architecture Behavioral;



library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;


entity bin2dectest is
end bin2dectest;
 
architecture Behavioral of bin2dectest is
  signal i: std_logic_vector(15 downto 0) := (others => '0');
  signal o: std_logic_vector(19 downto 0);
begin
  uut: entity work.bin2dec port map(i, o);

  tst_process: process
  begin
   i <= "0010101101100111";
   wait for 5ns;
	assert o = "00010001000100010001"
	report "bad result for 11111";
	
	i <= "0011000000111001";
	wait for 5ns;
	assert o = "00010010001101000101"
	report "bad for 12345";
	
	i <= "1110101001100110";
	wait for 5ns;
	assert o = "01100000000000000110"
	report "bad for 60006";

   wait;
  end process;
end architecture Behavioral;