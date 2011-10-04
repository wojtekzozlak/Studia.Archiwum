{  ai.pas - Wojciech Zoltak - 18.04.2010
   module provides simple move generator, which uses
   simple damage-logic to make decisions }

unit ai;

interface
  uses engine;

  function generateMove(gameBoard: boardElements; isFirstMoving: boolean;
    currentArmyIndex: Integer) : order;

implementation

  function findArmyIndex(armyList: army; a: army) : integer;
  begin
    findArmyIndex := 1;
    while (armyList <> a) and (armyList <> nil) do begin
      armyList := armyList^.nextArmy;
      inc(findArmyIndex);
    end;
  end;


  function min(a: integer; b: integer) : integer;
  begin
    if a < b then min := a
    else min := b;
  end;


  function generateMove(gameBoard: boardElements; isFirstMoving: boolean;
    currentArmyIndex: Integer) : order;
  var
    newOrder: order;
    maxMin, bufMin, bufDmg: integer;
    bestTarget: army;
    currentTarget: army;
    currentArmy: army;
  begin
    currentArmy := indexToArmy(currentArmyIndex,
      curPlayerArmies(isFirstMoving, gameBoard)^);
    new(newOrder);
    maxMin := 0;
    bestTarget := nil;
    if isFirstMoving then
      currentTarget := gameBoard^.secondArmies
    else
      currentTarget := gameBoard^.firstArmies;
    while currentTarget <> nil do begin
      if canAttackArmy(currentArmyIndex, isFirstMoving, currentTarget, 
        gameBoard) then
      begin
        bufDmg := getDamage(currentArmy, currentTarget);
        bufMin := min(currentTarget^.health, bufDmg);
        if bufMin > maxMin then begin
          maxMin := bufMin;
          bestTarget := currentTarget;
        end;
      end;
      currentTarget := currentTarget^.nextArmy;
    end;
    if bestTarget <> nil then begin
      newOrder^.kind := ATTACK;
      newOrder^.destPos := bestTarget^.pos;
    end else begin
      newOrder^.kind := SKIP;
      newOrder^.destPos := nil;
    end;
    generateMove := newOrder;
  end;

end.
