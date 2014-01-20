----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    19:49:43 04/30/2013 
-- Design Name: 
-- Module Name:    timertest - Behavioral 
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

entity togglertest is
end togglertest;
 
architecture Behavioral of togglertest is
  signal t1, t2: std_logic := '0';
  signal o1, o2: std_logic;
begin
  uut1: entity work.toggler generic map('0') port map(t1, o1);
  uut2: entity work.toggler generic map('1') port map(t2, o2);

  tst_process: process
  begin
    wait for 5ns;
    assert (o1 = '0' and o2 = '1')
    report "Wrong initial value.";
  
    t1 <= '1';
    t2 <= '1';
    wait for 5ns;
    t1 <= '0';
    t2 <= '0';
    wait for 5ns;
    assert (o1 = '1' and o2 = '0')
    report "Wrong toggled value.";
    
    t1 <= '1';
    t2 <= '1';
    wait for 5ns;
    t1 <= '0';
    t2 <= '0';
    wait for 5ns;
    assert (o1 = '0' and o2 = '1')
    report "Wrong toggled value.";
    
    t1 <= '1';
    t2 <= '1';
    wait for 5ns;
    t1 <= '0';
    t2 <= '0';
    wait for 5ns;
    assert (o1 = '1' and o2 = '0')
    report "Wrong toggled value.";
	 wait;
  end process;
end architecture Behavioral;



library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity timertest is
end timertest;
 
architecture Behavioral of timertest is
  signal clk: std_logic := '0';
  constant clk_period: time := 5 ns;
  signal input: std_logic_vector(2 downto 0);
  signal rst: std_logic := '0';
  signal toggle: std_logic := '0';
  signal dir: std_logic := '0';
  signal output: std_logic_vector(2 downto 0);
  signal active: std_logic;
  signal ovf: std_logic;
begin
  uut: entity work.timer
    generic map(3)
    port map(input, clk, rst, toggle, dir,
             output, active, ovf);
             
  -- clock simulation
  clk_process: process
  begin
    clk <= '0';
    wait for clk_period / 2;
    clk <= '1';
    wait for clk_period / 2;
  end process;
  
  timer_process: process
  begin
    input <= "110";
    
    wait for 1ns;
    assert (active = '0')
    report "0) Off by default.";
  
    -- incrementing

    rst <= '1';
    dir <= '1';
    wait for clk_period;
    assert (active = '0') and (ovf = '0') and (output = "000")
    report "1) Bad reset values.";

    rst <= '0';
    dir <= '0';
    toggle <= '1';
    wait for clk_period;
    toggle <= '0';
    assert (active = '1') and (ovf = '0') and (output = "001")
    report "2) Bad values [increment].";
    
    wait for clk_period * 5;
    assert (active = '1') and (ovf = '0') and (output = "110")
    report "3) Bad values [increment].";
    
    wait for clk_period;
    assert (active = '0') and (ovf = '1') and (output = "110")
    report "4) Bad values [overflow].";
    
    
    -- turn off
    toggle <= '1';
    wait for clk_period;
    toggle <= '0';
    
    -- reset to top
    input <= "100";
    dir <= '0';
    rst <= '1';
    wait for clk_period;
    assert (active = '0') and (ovf = '0') and (output = "100")
    report "5) Bad reset values.";
    
    -- turn on
    rst <= '0';
    toggle <= '1';
    dir <= '1';
    wait for clk_period;
    toggle <= '0';
    assert (active = '1') and (ovf = '0') and (output = "011")
    report "6) Bad values [decrement].";
    
    wait for clk_period * 3;
    assert (active = '1') and (ovf = '0') and (output = "000")
    report "7) Bad values [decrement].";
    
    wait for clk_period;
    assert (active = '0') and (ovf = '1') and (output = "000")
    report "8) Bad values [decrement].";

    -- turn off
    toggle <= '1';
    wait for clk_period;
    toggle <= '0';
    
    -- reset to top
    input <= "100";
    dir <= '0';
    rst <= '1';
    wait for clk_period;
    
    -- turn on
    dir <= '1';
    rst <= '0';
    toggle <= '1';
    wait for clk_period;
    toggle <= '0';
    
    wait for clk_period * 2;
    assert (active = '1') and (ovf = '0') and (output = "001")
    report "9) Bad values [toggle test].";
    
    -- pause
    toggle <= '1';
    wait for clk_period;
    assert (active = '0') and (ovf = '0') and (output = "001")
    report "10) Bad values [toggle test].";
    
    toggle <= '0';
    wait for clk_period; 
    
    -- resume with different direction
    dir <= '0';
    toggle <= '1';
    wait for clk_period;
    assert (active = '1') and (ovf = '0') and (output = "010")
    report "11) Bad values [toggle test].";
    
    wait;
  end process;
end architecture Behavioral;