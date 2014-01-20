--------------------------------------------------------------------------------
-- Company: University of Warsaw
-- Author: Marcin Peczarski
-- Description: test bench for divider demo example
-- Creation date: 01.02.2011
-- Modification date: 18.03.2013
--------------------------------------------------------------------------------
library ieee;
use ieee.std_logic_1164.all;

entity dividertest is
end dividertest;
 
architecture behavioral of dividertest is
  -- Initialize inputs to default values.
  signal mclk: std_logic := '0';
  signal btn:  std_logic_vector(3 downto 0) := (others => '0');
  -- Declare outputs.
  signal led: std_logic_vector(7 downto 0);
  signal seg: std_logic_vector(7 downto 0);
  signal an:  std_logic_vector(3 downto 0);
  -- Clock period definitions
  constant mclk_period: time := 20 ns; -- 50 MHz 
begin
  -- Instantiate the unit under test.
  uut: entity work.demo port map(mclk, btn, led, seg, an);

  -- Clock process definition
  mclk_process: process
  begin
    mclk <= '0';
    wait for mclk_period / 2;
    mclk <= '1';
    wait for mclk_period / 2;
  end process;
 
  -- Reset process
  reset_proc: process
  begin		
    -- Hold reset state for 35 ns.
    btn <= (others => '1');
    wait for 7 * mclk_period / 4;
    btn(0) <= '0';
    btn <= (others => '0');
    wait;
  end process;
end architecture behavioral;
