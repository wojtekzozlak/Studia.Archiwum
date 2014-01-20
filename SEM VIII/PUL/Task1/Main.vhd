----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    10:15:33 04/20/2013 
-- Design Name: 
-- Module Name:    main - Behavioral 
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

entity main is
  port(sw:  in  std_logic_vector(7 downto 0);
       btn: in  std_logic_vector(3 downto 0);
       led: out std_logic_vector(7 downto 0);
		 seg: out std_logic_vector(7 downto 0);
		 an:  out std_logic_vector(3 downto 0));
end main;

architecture Behavioral of main is
  signal enabled, error: std_logic;
  signal fun: std_logic_vector(1 downto 0);
  signal count: unsigned(3 downto 0);
  type SYMBOLS is array(9 downto 0) of std_logic_vector(7 downto 0);
  constant S: SYMBOLS :=
    ("10000110",  -- E
	  "10000000",  -- 8
	  "11111000",  -- 7
	  "10000010",  -- 6
	  "10010010",  -- 5
	  "10011001",  -- 4
	  "10110000",  -- 3
	  "10100100",  -- 2
	  "11111001",  -- 1
	  "11000000"); -- 0
begin
  box: entity work.magic_box
    generic map(8)
    port map(sw, fun, enabled, led);

  with btn select
    error <= '0' when "1000",
	          '0' when "0100",
				 '0' when "0010",
				 '0' when "0001",
				 '0' when "0000",
				 '1' when others;
  
  enabled <= '0' when error = '1' or btn = "0000" else
             '1';
  
  with btn select
    fun <= "00" when "0001",
           "01" when "0010",
           "10" when "0100",
           "11" when others;

			  
  an <= "1110";
  count <= ("000" & sw(0)) + ("000" & sw(1)) + ("000" & sw(2)) + ("000" & sw(3)) +
           ("000" & sw(4)) + ("000" & sw(5)) + ("000" & sw(6)) + ("000" & sw(7));

  seg <= S(9) when error = '1' else
	      S(TO_INTEGER(count));
  
end Behavioral;
