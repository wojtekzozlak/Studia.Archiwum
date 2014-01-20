---------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    14:49:43 04/22/2013 
-- Design Name: 
-- Module Name:    bit_shifter - Behavioral 
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

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx primitives in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity bit_shifter is
  port(input, sin, csin: in std_logic;
       output, sout, csout: out std_logic);
end bit_shifter;

architecture Behavioral of bit_shifter is
begin
  -- gives a shifted bit when there is one on input and someone can take it
  sout <= (input and csin);

  -- can take shifted bit when there is no input,
  -- or input bit is shifted
  csout <=  (not input) or (csin and input);
  
  -- outputs bit when takes one either from input or shift input
  output <= (input and not csin) or
             (((not input) or (csin and input)) and sin);
end Behavioral;


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity four_bit_packer is
  port(input: in std_logic_vector(3 downto 0);
       output: out std_logic_vector(3 downto 0));
end entity four_bit_packer;

architecture Xxx of four_bit_packer is
  signal s, cs: std_logic_vector(2 downto 0);
begin
  p1: entity work.bit_shifter port map(input(0), s(0), '0', output(0), open, cs(0));
  p2: entity work.bit_shifter port map(input(1), s(1), cs(0), output(1), s(0), cs(1));
  p3: entity work.bit_shifter port map(input(2), s(2), cs(1), output(2), s(1), cs(2));
  p4: entity work.bit_shifter port map(input(3), '0', cs(2), output(3), s(2), open);
end;


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity n_shifter is
  generic(n: natural range 1 to 255 := 8);
  port(input: in std_logic_vector(n - 1 downto 0);
       output: out std_logic_vector(n - 1 downto 0));
end entity n_shifter;

architecture Behavioral of n_shifter is
  signal s, cs: std_logic_vector(n downto 0);
begin
  s(n) <= '0';
  cs(0) <= '0';
  shifter: for i in 0 to n - 1 generate
     one_bit: entity work.bit_shifter 
	      port map(input(i), s(i + 1), cs(i), output(i), s(i), cs(i + 1));
  end generate shifter;
end;



library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity n_packer is
  generic(n: natural range 1 to 255 := 8);
  port(input: in std_logic_vector(n - 1 downto 0);
       output: out std_logic_vector(n - 1 downto 0));
end entity n_packer;

architecture Behavioral of n_packer is
  type AA is array(n downto 0) of std_logic_vector(n - 1 downto 0);
  signal s: AA;
begin
  shifter: for i in 0 to n - 1 generate
      single: entity work.n_shifter
		     generic map(n)
			  port map(s(i), s(i + 1));
  end generate shifter;
  
  s(0) <= input;
  output <= s(n);
end;