----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    22:20:29 05/20/2013 
-- Design Name: 
-- Module Name:    doubledabble - Behavioral 
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


entity add3 is
  port(i: in std_logic_vector(3 downto 0);
       o: out std_logic_vector(3 downto 0));
end entity add3;

architecture Behavioral of add3 is
begin
   o <= i when unsigned(i) < x"5" else
        std_logic_vector(unsigned(i) + to_unsigned(3, 4)); 
end;


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

entity bin2dec is
  port(input: in std_logic_vector(15 downto 0);
       output: out std_logic_vector(19 downto 0));
end entity bin2dec;


architecture Behavioral of bin2dec is
  type temp_array is array (16 downto 0) of std_logic_vector (36 downto 0);
  signal temp: temp_array;
begin
  temp(0)(15 downto 0) <= input;
  temp(0)(36 downto 16) <= (others => '0');
  layer_for: for i in 0 to 15 generate
    add3_for: for j in 0 to 4 generate
      add3: entity work.Add3 port map(temp(i)(15 + 4 * (j + 1) downto 16 + 4 * j),
                                      temp(i + 1)(16 + 4 * (j + 1) downto 17 + 4 * j));
    end generate add3_for;
    temp(i + 1)(16 downto 1) <= temp(i)(15 downto 0);
    temp(i + 1)(0) <= '0';
  end generate layer_for;
  output <= temp(16)(35 downto 16);
end Behavioral;