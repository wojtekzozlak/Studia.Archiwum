----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    19:29:35 04/30/2013 
-- Design Name: 
-- Module Name:    Timer - Behavioral 
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

entity toggler is
  generic(def: std_logic);
  port(toggle: in std_logic;
       s: out std_logic := def);
end toggler;


architecture Behavioral of toggler is
  signal mem: std_logic := def;
begin
  process(toggle)
  begin
    if toggle'event and toggle = '1' then
      mem <= not mem;
      s <= not mem;
    end if;
  end process;
end;



--

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;


entity timer is
  generic(n: natural := 8);
  port(input: std_logic_vector(n - 1 downto 0);
       clk: in std_logic;
       rst: in std_logic;
       toggle: in std_logic;
       dir: in std_logic := '0';
       output: out std_logic_vector(n - 1 downto 0);
       active: out std_logic;
       ovf: out std_logic);
end timer;


architecture behavioral of timer is
  signal counter: unsigned(n - 1 downto 0) := (others => '0');
  signal s_active: std_logic;
  signal s_ovf: std_logic := '0';
  constant one: unsigned(n - 1 downto 0) := (0 => '1', others => '0');
begin
  tog: entity work.toggler
    generic map('0')
    port map(toggle, s_active);
  
  output <= std_logic_vector(counter);
  ovf <= s_ovf;
  process(clk, rst)
  begin
    -- reset
    if rst = '1' then
      case dir is
        when '1' => counter <= (others => '0');
        when others => counter <= unsigned(input);
      end case;
      s_ovf <= '0';
      active <= '0';

    -- overflow or not active
    elsif (s_ovf = '1') or (s_active = '0') then
      active <= '0';

    -- tick
    elsif clk'event and clk = '1' then
      if dir = '0' and (counter > 0) then
        counter <= counter - one;
        active <= '1';
      elsif dir = '1' and (counter < unsigned(input)) then
        counter <= counter + one;
        active <= '1';
      else
        s_ovf <= '1';
        active <= '0';
      end if;

    end if;
  end process;


end behavioral;