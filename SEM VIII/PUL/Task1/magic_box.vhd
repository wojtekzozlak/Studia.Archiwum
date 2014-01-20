----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    22:55:37 04/22/2013 
-- Design Name: 
-- Module Name:    magic_box - Behavioral 
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

entity magic_box is
  generic(n: natural range 1 to 255 := 8);
  port(input: in std_logic_vector(n - 1 downto 0);
       fun: in std_logic_vector(1 downto 0);
       enable: std_logic;
       output: out std_logic_vector(n - 1 downto 0));
end magic_box;

architecture Behavioral of magic_box is
  signal max, min, median, count: std_logic_vector(n - 1 downto 0);
begin
  miner: entity work.n_min
      generic map(n) port map(input, min);
  maxer: entity work.n_max
      generic map(n) port map(input, max);
  counter: entity work.n_unary_counter
      generic map(n) port map(input, count);
  medianer: entity work.n_median
      generic map(n) port map(input, median);


  with enable & fun select
    output <= min when "100",
              max when "101",
              count when "111",
              median when "110",
              (others => '0') when others;

end Behavioral;

