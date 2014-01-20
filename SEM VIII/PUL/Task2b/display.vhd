----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    12:13:38 05/01/2013 
-- Design Name: 
-- Module Name:    display - Behavioral 
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
use IEEE.NUMERIC_STD.ALL;

entity display is
  port(input: in std_logic_vector(15 downto 0);
       clk: in std_logic;
		 active: in std_logic;
       seg: out std_logic_vector(7 downto 0);
       an: out std_logic_vector(3 downto 0));
end display;

architecture Behavioral of display is
  signal i: unsigned(1 downto 0) := "00";
  type SEGMENT is array(3 downto 0) of std_logic_vector(3 downto 0);
  constant SEGMENTS: SEGMENT := ("0111", "1011", "1101", "1110");
  type DIGIT is array(9 downto 0) of std_logic_vector(6 downto 0);
  constant DIGITS: DIGIT :=
    ("0010000",  -- 9
     "0000000",  -- 8
	  "1111000",  -- 7
	  "0000010",  -- 6
	  "0010010",  -- 5
	  "0011001",  -- 4
	  "0110000",  -- 3
	  "0100100",  -- 2
	  "1111001",  -- 1
	  "1000000"); -- 0
begin
  process(clk)
  begin
    if clk'event and clk = '1' then
      case i is
        when "11" => i <= "00";
        when others => i <= i + "01";
      end case;
      
      an <= SEGMENTS(TO_INTEGER(i));
      seg(6 downto 0) <= DIGITS(TO_INTEGER(unsigned(input(TO_INTEGER(i) * 4 + 3 downto TO_INTEGER(i) * 4))));

      case i is
        when "10" => seg(7) <= '0';
		  when "00" => seg(7) <= not active;
        when others => seg(7) <= '1';
      end case;
    end if;
  end process;
end Behavioral;
