<?php
  $iteracje = 50;
  $programy_na_iteracje = 100;
  $kill_iteracji = 5;

  $N = 1000;
  $max_K = 10;
  $max_T = 0;
  $max_zasobow = 2*$N/3;

  for($i = 0; $i < $iteracje; $i++)
  {
    $aaa = $programy_na_iteracje;
    while($aaa > 0)
    {
      $il = min(rand(1, 20), $aaa);
      $aaa -= $il;
      for($j = 0; $j < $il; $j++)
      {
        $_k = rand(1, $max_K);
        $_t = rand(0, $max_T);
        $_z = rand(1, $max_zasobow);
        $zasoby = array();
        for($k = 0; $k < $_z; $k++) $zasoby[rand(1, $N)] = true;
        $_z = "";
        foreach($zasoby as $k => $v) $_z .= " $k";
        $zasoby = implode(" ", $zasoby);
        echo "../client $_k $_t $_z &\n";
      }
      echo "sleep 1\n";
    }
    echo "sleep $kill_iteracji\n";
    echo "kill -s SIGINT `pidof client`\n";
  }
  echo "../client 1 10 ";
  for($i = 1; $i <= $N; $i++) echo " $i";
  echo "\n";
?>
