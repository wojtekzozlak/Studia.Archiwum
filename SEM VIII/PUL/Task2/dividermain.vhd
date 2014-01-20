-- A simple frequency divider

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity divider is
  --                  1
  -- f_out = f_in ----------
  --              2(div + 1)
  generic(nbit: natural := 25;
          top:  natural := 24999999); -- f_out = 1Hz by f_in = 50 MHz
  port(clk_in:  in    std_logic;
       reset:   in    std_logic;
       clk_out: inout std_logic);
end entity divider;

architecture counter of divider is
  signal cnt: unsigned(nbit - 1 downto 0);
begin
  process(clk_in, reset)
  begin
    if reset = '0' then
      cnt <= (others => '0');
      clk_out <= '0';
    elsif clk_in'event and clk_in = '1' then
      if cnt = 0 then
        cnt <= to_unsigned(top, nbit);
        clk_out <= not clk_out;
      else
        cnt <= cnt - 1;
      end if;
    end if;
  end process;
end architecture counter;


-- Debouncer

library ieee;
use ieee.std_logic_1164.all;

entity debouncer is
  port(input:  in std_logic;
       clock:  in std_logic;
       output: out std_logic := '0');
end entity debouncer;

architecture counter of debouncer is
  signal count: natural range 1 to 1000 := 1;
  signal value: std_logic := '0';
begin
  output <= value;
	
  process (clock) is
  begin
    if rising_edge(clock) then
      if count = 1000 then
	value <= input;
	count <= 1;
      elsif value = input then
        count <= 1;
      else
        count <= count + 1;
      end if;
    end if;
  end process;
end counter;

architecture simple of debouncer is
  signal prev: std_logic := '0';
begin
  process (clock) is
  begin
    if clock'event and clock = '1' then
      if input = '1' and prev = '1' then
        output <= '1';
      elsif input = '0' and prev = '0' then
        output <= '0';
      else
        prev <= input;
      end if;
    end if;      
  end process;
end architecture simple;


-- Main entity

library ieee;
use ieee.std_logic_1164.all;

entity demo is
  port(mclk: in std_logic;
       btn0, btn3: in  std_logic;
       sw:   in  std_logic_vector(7 downto 0);
       led:  out std_logic_vector(7 downto 0) := (others => '0');
       seg:  out std_logic_vector(7 downto 0);
       an:   out std_logic_vector(3 downto 0));
end entity demo;

architecture simple of demo is
  signal dclk:  std_logic_vector(7 downto 0);
  signal disp:  std_logic_vector(19 downto 0);
  signal input: std_logic_vector(15 downto 0);
  signal output: std_logic_vector(15 downto 0);
  signal toggle: std_logic;
  signal active: std_logic;
begin
  khz_clock: entity work.divider generic map(16, 24999) port map(mclk, '1', dclk(3)); -- 1 KHz

  deb: entity work.debouncer
    port map(btn0, mclk, toggle);

  timer: entity work.timer
    generic map(16)
    port map(input, dclk(3), btn3, toggle, sw(0),
             output, active, led(7));

  b2d: entity work.bin2dec port map(output, disp);

  d: entity work.display port map(disp(19 downto 4), dclk(3), active, seg, an);

  led(6 downto 0) <= (others => '0');
  input(15 downto 9) <= sw(7 downto 1);
  input(8 downto 0) <= (others => '0');
end architecture simple;