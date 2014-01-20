----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date:    14:04:01 05/21/2013 
-- Design Name: 
-- Module Name:    Main - Behavioral 
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

entity Main is
  port(mclk: in std_logic;
       btn:  in  std_logic_vector(3 downto 0);
       sw:   in  std_logic_vector(7 downto 0);
       led:  out std_logic_vector(7 downto 0);
       seg:  out std_logic_vector(7 downto 0);
       an:   out std_logic_vector(3 downto 0));
end Main;

architecture Behavioral of Main is
begin
  led <= (others => '0');
  seg <= (others => '1');
  an <= (others => '1');
end Behavioral;

