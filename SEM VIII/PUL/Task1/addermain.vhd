----------------------------------------------------------------------------------
-- Company: University of Warsaw
-- Author: Marcin Peczarski
-- 
-- Created:  25.01.2011
-- Modified: 18.02.2012 
-- Revised:  24.02.2013
-- Target device: Basys 2, Spartan3E 100K
-- Tool version: ISE 14.4
--
-- Description: PLD lecture - combinatorial circuits
-- lecture demo 
----------------------------------------------------------------------------------

-- Different implementations of one-bit adder

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity one_bit_adder is
  port(a, b, cin: in std_logic;
       cout, s: out std_logic);
end entity one_bit_adder;

architecture a1 of one_bit_adder is
begin
  s <= a xor b xor cin;
  cout <= (a and b) or (a and cin) or (b and cin);
end architecture a1;

architecture a2 of one_bit_adder is
  signal input: std_logic_vector (2 downto 0);
begin
  input <= a & b & cin;
  s <= '1' when input = "001" or input = "010" or input = "100" or
                input = "111" else '0';
  cout <= cin when std_match(input, "-11") else
          cin when std_match(input, "1-1") else
          '1' when std_match(input, "11-") else
          '0';
end architecture a2;

architecture a3 of one_bit_adder is
  signal input: std_logic_vector(2 downto 0);
  signal output: std_logic_vector(1 downto 0);
begin
  input <= a & b & cin;
  s <= output(0);
  cout <= output(1);
  with input select
    output <= "00" when "000",
              "01" when "001",
              "01" when "010",
              "10" when "011",
              "01" when "100",
              "10" when "101",
              "10" when "110",
              "11" when others;
end architecture a3;

architecture a4 of one_bit_adder is
begin
  -- It is possible to define combinatorial circuit using process,
  -- but this is not recommended.
  process (a, b, cin) is
  begin
    if ((a xor b xor cin) = '1') then
      s <= '1';
    else
      s <= '0';
    end if;
    if (a = '1' and b = '1') then
      cout <= '1';
    elsif (a = '1' and cin = '1') then
      cout <= '1';
    elsif (b = '1' and cin = '1') then
      cout <= '1';
    else
      cout <= '0';
    end if;
  end process;
end architecture a4;

-- Four-bit adder

library ieee;
use ieee.std_logic_1164.all;

entity four_bit_adder is
  port(a, b: in std_logic_vector(3 downto 0);
       cin: in std_logic;
       cout: out std_logic;
       s: out std_logic_vector(3 downto 0));
end entity four_bit_adder;

architecture structural of four_bit_adder is
  signal c: std_logic_vector(3 downto 1);
begin
  s0: entity work.one_bit_adder(a1) port map(a(0), b(0), cin,  c(1), s(0));
  s1: entity work.one_bit_adder(a2) port map(a(1), b(1), c(1), c(2), s(1));
  s2: entity work.one_bit_adder(a3) port map(a(2), b(2), c(2), c(3), s(2));
  s3: entity work.one_bit_adder(a4) port map(a(3), b(3), c(3), cout, s(3));
end architecture structural;

-- n-bit adder

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity n_bit_adder is
  generic(n: natural range 2 to 255 := 8);
  port(a, b: in std_logic_vector(n - 1 downto 0);
       cin: in std_logic;
       cout: out std_logic;
       s: out std_logic_vector(n - 1 downto 0));
end entity n_bit_adder;

architecture structural of n_bit_adder is
  component general_one_bit_adder is
    port(a, b, cin: in std_logic;
         cout, s: out std_logic);
  end component general_one_bit_adder;
  for all: general_one_bit_adder use entity work.one_bit_adder(a1);
  signal c: std_logic_vector(n downto 0);
begin
  c(0) <= cin;
  cout <= c(n);
  adder: for i in 0 to n - 1 generate
    one_bit: general_one_bit_adder port map(a(i), b(i), c(i), c(i + 1), s(i));
  end generate adder;
end architecture structural;

architecture functional of n_bit_adder is
  -- Previous version of ISim allowed negative indices, present ISim does not.
  -- signal sum: unsigned(n downto -1);
  signal sum: unsigned(n + 1 downto 0);
  -- constant zeros: unsigned(n downto 1) := (others => '0');
begin
  -- Wrong
  -- sum <= unsigned(a) + unsigned(b) + (zeros & cin);
  -- Correct
  sum <= ('0' & unsigned(a) & cin) + ('0' & unsigned(b) & '1');
  s <= std_logic_vector(sum(n downto 1));
  cout <= sum(n + 1);
end architecture functional;

-- 7-segment display coder (incomplete) with some dummy signals

library ieee;
use ieee.std_logic_1164.all;

entity hex2seg is
  port(hex:    in    std_logic_vector(3 downto 0);
       enable: inout std_logic;  -- dummy signal
       seg:    out   std_logic_vector(7 downto 0);
       strobe: out   std_logic); -- dummy signal
end entity hex2seg;

architecture dummy of hex2seg is
  type seg_descr_type is array (0 to 1) of std_logic_vector(7 downto 0);
  constant seg_descr: seg_descr_type := (x"c7", x"89"); -- 0 is L, 1 is H
begin
  with hex select
    enable <= '0' when x"0",
              '1' when others;
  with enable select
    seg <= seg_descr(0) when '0',    -- display L
           seg_descr(1) when others; -- display H
  strobe <= hex(0);
end architecture dummy;

-- Adder test entity

library ieee;
use ieee.std_logic_1164.all;

entity demo is
  port(sw:  in  std_logic_vector(7 downto 0); 
       btn: in  std_logic_vector(3 downto 0);
       led: out std_logic_vector(7 downto 0);
       seg: out std_logic_vector(7 downto 0);
       an:  out std_logic_vector(3 downto 0));
end entity demo;

architecture simple of demo is
begin
  --add: entity work.four_bit_adder
  --  port map(sw(3 downto 0), sw(7 downto 4), '0', led(4), led(3 downto 0));
  --add: entity work.n_bit_adder(structural)
  --  generic map(4)
  --  port map(sw(3 downto 0), sw(7 downto 4), '0', led(4), led(3 downto 0));
  add: entity work.n_bit_adder(functional)
    generic map(4)
    port map(sw(3 downto 0), sw(7 downto 4), '0', led(4), led(3 downto 0));
  btn2seg: entity work.hex2seg port map(btn, open, seg, led(7));
  an <= "1110";
  led(6 downto 5) <= (others => '0');
end architecture simple;
