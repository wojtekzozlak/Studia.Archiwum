{ game.pas - Wojciech Zoltak - 18.04.2010
  program runs "bitwa" game using engine module to ingame state handling }

program game;

uses engine, ai;

procedure makeMove(gameBoard: boardElements; isFirstMoving: boolean;
  currentArmyIndex: integer; currentOrder: order);
begin
  if currentOrder^.kind = MOVE then
    if canMoveTo(currentArmyIndex, isFirstMoving, currentOrder^.destPos, gameBoard) then
      moveTo(currentArmyIndex, isFirstMoving, currentOrder^.destPos, gameBoard)
    else
      wrongMove()
  else if currentOrder^.kind = ATTACK then
    if canAttackArmy(currentArmyIndex, isFirstMoving, currentOrder^.armyToAttack, gameBoard) then
      dealDamage(currentArmyIndex, isFirstMoving, currentOrder^.armyToAttack, gameBoard)
    else
      wrongMove()
  else skipMove();
end;


procedure initCurrentArmy(gameBoard: boardElements; var currentArmy: army;
  var currentArmyIndex: integer; var isFirstMoving: boolean; var quitGame: boolean);
begin
  if (gameBoard^.firstArmies <> nil) and (gameBoard^.secondArmies <> nil) then
  begin
    isFirstMoving := true;
    currentArmy := gameBoard^.firstArmies;
    currentArmyIndex := 1;
  end else
    quitGame := true;
end;


procedure setNextArmy(gameBoard: boardElements; var currentArmy: army;
  var currentArmyIndex: Integer; var isFirstMoving: boolean; var quitGame: boolean);
begin
  if isFirstMoving and (gameBoard^.secondArmies = nil) or
    (not isFirstMoving) and (gameBoard^.firstArmies = nil) then
  begin
    quitGame := true;
    exit;
  end;
  currentArmy := currentArmy^.nextArmy;
  inc(currentArmyIndex);
  if currentArmy = nil then
    if isFirstMoving then
    begin
      isFirstMoving := false;
      currentArmy := gameBoard^.secondArmies;
      currentArmyIndex := 1;
    end else
    begin
      isFirstMoving := true;
      currentArmy := gameBoard^.firstArmies;
      currentArmyIndex := 1;
    end;
  if currentArmy = nil then quitGame := true;
end;


var
  gameBoard: boardElements;
  currentCommand: userCommand;
  currentOrder: order;
  currentArmy: army;
  currentArmyIndex: Integer;
  quitGame: boolean;
  isFirstMoving: boolean;
begin
  quitGame := false;
  while (not quitGame) and (not EOF) do
  begin
    currentCommand := readCommand();
    case currentCommand^.kind of
      READBOARD:
      begin
        gameBoard:= currentCommand^.newBoard;
        initCurrentArmy(gameBoard, currentArmy, currentArmyIndex, isFirstMoving, quitGame);
      end;
      SHOWBOARD: drawBoard(gameBoard);
      QUIT: quitGame := true;
      PLAY:
      begin
        currentOrder := getOrder(gameBoard, currentCommand^.destPos, isFirstMoving);
        makeMove(gameBoard, isFirstMoving, currentArmyIndex, currentOrder);
        setNextArmy(gameBoard, currentArmy, currentArmyIndex, isFirstMoving, quitGame);
        destroyOrder(currentOrder);
      end;
      GENMOVE:
      begin
        currentOrder := generateMove(gameBoard, isFirstMoving, currentArmyIndex);
        drawGeneratedMove(currentOrder);
        destroyOrder(currentOrder);
      end
    end;
     destroyCommand(currentCommand);
  end;
  destroyBoard(gameBoard);
end.
