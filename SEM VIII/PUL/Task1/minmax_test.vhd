library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
 
entity minmax_test is
end minmax_test;
 
architecture behavioral of minmax_test is
  -- Initialize inputs to default values.
  signal sw:  std_logic_vector(7 downto 0) := (others => '0');
  -- Declare outputs.
  signal led: std_logic_vector(7 downto 0);
begin
  -- Instantiate the unit under test.
  uut: entity work.n_max
    generic map(8)
    port map(input => sw, output => led);

  -- Stimulate the unit under test.
  sw_proc: process
  begin
    wait for 5 ns;
   
   sw <= "10000000";
   wait for 5 ns;
   assert "10000000" = led
   report "Wrong maximum.";

   sw <= "10101010";
   wait for 5 ns;
   assert "00000010" = led
   report "Wrong maximum.";
   
   sw <= "11111111";
   wait for 5 ns;
   assert "00000001" = led
   report "Wrong maximum.";
 
    wait;
  end process;
end architecture behavioral;


--
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity main_test is
end main_test;
 
architecture behavioral of main_test is
  -- Initialize inputs to default values.
  signal sw:  std_logic_vector(7 downto 0) := (others => '0');
  -- Declare outputs.
  signal led: std_logic_vector(7 downto 0);
begin
  -- Instantiate the unit under test.
  uut: entity work.n_max
    generic map(8)
    port map(input => sw, output => led);

  -- Stimulate the unit under test.
  sw_proc: process
  begin
    wait for 5 ns;
   
   sw <= "10000000";
   wait for 5 ns;
   assert "10000000" = led
   report "Wrong maximum.";

   sw <= "10101010";
   wait for 5 ns;
   assert "00000010" = led
   report "Wrong maximum.";
   
   sw <= "11111111";
   wait for 5 ns;
   assert "00000001" = led
   report "Wrong maximum.";
 
    wait;
  end process;
end architecture behavioral;