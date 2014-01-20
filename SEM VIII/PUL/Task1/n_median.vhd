----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    16:46:25 04/22/2013 
-- Design Name: 
-- Module Name:    n_median - Behavioral 
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

entity bit_anihilator is
  port(input, lin, rin, cin: in std_logic;
       output, lout, rout, cout: out std_logic);
end bit_anihilator;

architecture Behavioral of bit_anihilator is
begin
  output <= input and ((lin and rin) or not cin);
  
  lout <= lin or input;
  rout <= rin or input;

  cout <= lin and rin and input;
end Behavioral;


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.STD_LOGIC_MISC.ALL;

entity median_step is
  generic(n: natural range 1 to 255 := 8);
  port(input: in std_logic_vector(n - 1 downto 0);
       output: out std_logic_vector(n - 1 downto 0));
end entity median_step;

architecture Behavioral of median_step is
  signal l, r: std_logic_vector(n downto 0);
  signal c_in, c_out: std_logic_vector(n - 1 downto 0);
begin
  l(0) <= '0';
  r(n) <= '0';
  anihilator: for i in 0 to n - 1 generate
      single: entity work.bit_anihilator
          port map(input(i), l(i), r(i + 1), c_out(i),
                   output(i), l(i + 1), r(i), c_in(i));
  end generate;
  c_out <= (others => or_reduce(c_in));
end Behavioral;


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity n_median is
  generic(n: natural range 1 to 255 := 8);
  port(input: in std_logic_vector(n - 1 downto 0);
       output: out std_logic_vector(n - 1 downto 0));
end entity n_median;

architecture Behavioral of n_median is
  type AA is array(n / 2 + 1 downto 0) of std_logic_vector(n - 1 downto 0);
  signal s: AA;
begin
  median: for i in 0 to n / 2 generate
    step: entity work.median_step
		  generic map(n)
		  port map(s(i), s(i + 1));
  end generate median;
  
  s(0) <= input;
  output <= s(n / 2 + 1);
end;
