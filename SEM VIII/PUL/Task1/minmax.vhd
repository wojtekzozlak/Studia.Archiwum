----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    19:00:26 04/19/2013 
-- Design Name: 
-- Module Name:    minmax - Behavioral 
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

entity max_bit_propagator is
  port(cin, input: in std_logic;
       cout, output: out std_logic);
end entity max_bit_propagator;

architecture Behavioral of max_bit_propagator is
begin
  output <= not cin or (cin and input);
  cout <= cin and (not input);
end architecture Behavioral;


library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity min_bit_propagator is
  port(lin, rin, input: in std_logic;
       lout, rout, output: out std_logic);
end entity min_bit_propagator;

architecture Behavioral of min_bit_propagator is
begin
  output <= (rin and not lin) or (rin and input);
  rout <= rin and (not input);
  lout <= lin and (not input);
end architecture Behavioral;


library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity n_min is
  generic(n: natural range 1 to 255 := 8);
  port(input: in std_logic_vector(n - 1 downto 0);
       output: out std_logic_vector(n - 1 downto 0));
end entity n_min;


architecture Behavioral of n_min is
  signal l, r: std_logic_vector(n downto 0);
begin
  r(0) <= '1';
  l(n) <= '1';
  
  propagator: for i in 0 to n - 1 generate
    one_bit: entity work.min_bit_propagator
	   port map(l(i + 1), r(i), input(i), l(i), r(i + 1), output(i));
  end generate propagator;
end architecture Behavioral;




library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity n_max is
  generic(n: natural range 1 to 255 := 8);
  port(input: in std_logic_vector(n - 1 downto 0);
       output: out std_logic_vector(n - 1 downto 0));
end entity n_max;


architecture Behavioral of n_max is
  signal c: std_logic_vector(n downto 0);	
begin
  c(n) <= '1';
  propagator: for i in 0 to n - 1 generate
    one_bit: entity work.max_bit_propagator
	   port map(c(n - i), input(n - i - 1), c(n - i - 1), output(n - i - 1));
  end generate propagator;
end architecture Behavioral;