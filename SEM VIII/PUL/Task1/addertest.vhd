--------------------------------------------------------------------------------
-- Company: University of Warsaw
-- Author: Marcin Peczarski
-- Description: test bench for adder demo example
-- Creation date: 28.01.2011
-- Last revision date: 24.02.2013
--------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
 
entity addertest is
end addertest;
 
architecture behavioral of addertest is
  -- Initialize inputs to default values.
  signal sw:  std_logic_vector(7 downto 0) := (others => '0');
  signal btn: std_logic_vector(3 downto 0) := (others => '0');
  -- Declare outputs.
  signal led: std_logic_vector(7 downto 0);
  signal seg: std_logic_vector(7 downto 0);
  signal an:  std_logic_vector(3 downto 0);
begin
  -- Instantiate the unit under test.
  uut: entity work.demo port map(sw => sw, btn => btn, led => led, seg => seg, an => an);

  -- Stimulate the unit under test.
  sw_proc: process
  begin
    wait for 5 ns;
    assert unsigned("0000" & sw(3 downto 0)) +
           unsigned("0000" & sw(7 downto 4)) = unsigned(led)
    report "Sum does not match.";
    wait for 5 ns;
    sw <= std_logic_vector(unsigned(sw) + x"01");
    -- In the following statement we still have the value of sw before increment.
    if (sw = x"ff") then
      wait;
    end if;
  end process;

  -- Cause that assert fails after 1005 ns.
  btn_proc: process
  begin		
    wait for 1003 ns;
    btn <= x"1";
    wait for 10 ns;
    btn <= x"0";
    wait;
  end process;
end architecture behavioral;
