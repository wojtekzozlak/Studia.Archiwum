<?php

function make_seed()
{
  list($usec, $sec) = explode(' ', microtime());
  return (float) $sec + ((float) $usec * 100000);
}
srand(make_seed());
	$n = 7;
	$m = 16;
	for($i = 0; $i<$m; $i++)
	{
		$x = rand() % $n;
		$y = rand() % $n;
		while($x == $y || isset($z[$x][$y]) || isset($z[$y][$x]))
		{
		  $x = rand() % $n;
		  $y = rand() % $n;
		}
		$z[$x][$y] = true;
	}
	
	print "$n $m\n";
	foreach($z as $i => $tab)
	  foreach($tab as $j => $v)
	    echo ($i+1)." ".($j+1)."\n";
	

?>
