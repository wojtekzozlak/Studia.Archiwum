----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    12:40:42 05/01/2013 
-- Design Name: 
-- Module Name:    displaytest - Behavioral 
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

entity displaytest is
end displaytest;

architecture Behavioral of displaytest is
  signal clk: std_logic := '0';
  constant clk_period: time := 5 ns;
  signal input: std_logic_vector(31 downto 0) := (others => '0');
  signal seg: std_logic_vector(7 downto 0);
  signal an: std_logic_vector(3 downto 0);
begin
  uut: entity work.display port map(input, clk, seg, an);

  -- clock simulation
  clk_process: process
  begin
    clk <= '0';
    wait for clk_period / 2;
    clk <= '1';
    wait for clk_period / 2;
  end process;
  
  test_process: process
  begin
    input <= "11111111000000001010101010001000";
    wait for 5ns;
    
    
    wait;
  end process;

end Behavioral;

