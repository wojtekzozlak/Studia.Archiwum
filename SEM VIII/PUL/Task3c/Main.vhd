--
--	Package File Template
--
--	Purpose: This package defines supplemental types, subtypes, 
--		 constants, and functions 
--
--   To use any of the example code shown below, uncomment the lines and modify as necessary
--

library IEEE;
use IEEE.STD_LOGIC_1164.all;

entity Main is
  port(mclk: in std_logic;
       btn:  in  std_logic_vector(3 downto 0);
       sw:   in  std_logic_vector(7 downto 0);
       led:  out std_logic_vector(7 downto 0);
       seg:  out std_logic_vector(7 downto 0);
       an:   out std_logic_vector(3 downto 0));
end Main;

architecture Behavioral of Main is
begin
  led <= (others => '0');
  seg <= (others => '1');
  an <= (others => '1');
end Behavioral;