----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    21:31:42 04/22/2013 
-- Design Name: 
-- Module Name:    n_vector_or - Behavioral 
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
use IEEE.STD_LOGIC_MISC.ALL;

entity n_vector_or is
  generic(n: natural range 1 to 255 := 8);
  port(input: in std_logic_vector(n - 1 downto 0);
       output: out std_logic_vector(n - 1 downto 0));
end entity n_vector_or;

architecture Behavioral of n_vector_or is
begin
  output <= (others => or_reduce(input));
end Behavioral;
