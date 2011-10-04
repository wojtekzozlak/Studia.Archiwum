{ engine.pas - Wojciech Zoltak - 18.04.2010
  main module of "bitwa" game - provides functions to I/O operations
  and everything necessary to change state of game }
unit engine;

interface
  uses sysutils;

  const
    MAX_SIZE = 100;
    ORD_SA = Ord('a'); { in case of changing units-name-conception }
    ORD_1A = Ord('a');
    ORD_2A = Ord('A');
    ORD_0 = OrD('0');
  
  const { fields char representation }
    FIELD_N = '.';
    FIELD_G = '^';
    FIELD_W = '=';

  const { units stats }
    UNITS_ZR : array [1..6] of integer = (3, 3, 6, 4, 8, 5);
    UNITS_ZMI : array [1..6] of integer = (1, 2, 1, 3, 1, 1);
    UNITS_ZMA : array [1..6] of integer = (1, 2, 2, 5, 1, 1);
    UNITS_SN : array [1..6] of integer = (2, 3, 5, 6, 4, 7);
    UNITS_SG : array [1..6] of integer = (3, 4, 0, 0, 4, 7);
    UNITS_SW : array [1..6] of integer = (0, 0, 0, 0, 4, 7);
    UNITS_P  : array [1..6] of integer = (0, 0, 2, 1, 1, 2);

  type position = ^positionDescription; { recursive types! }
  positionDescription=record
    x: integer;
    y: integer;
  end;

  type field = ^fieldDescription;
  army = ^armyDescription;  
  fieldDescription=record
    pos: position;
    a: army; 
    kind: integer; { 1-3 }
  end;
  armyDescription=record
    kind: integer; { 1 - 6 }
    pos: field;
    health: integer;
    prevArmy: army; { = nil if first army }
    nextArmy: army; { = nil if last army }
    owner: boolean; { = true if first player }
  end;

  type armyListStart = ^army;

  type board = array[1..MAX_SIZE, 1..MAX_SIZE] of field;

  type boardElements = ^boardDescription;
  boardDescription=record
    firstArmies: army;
    secondArmies: army;
    boardHeight: integer;
    boardWidth: integer;
    terrain: board;
  end;

  type orderKind = (SKIP, MOVE, ATTACK);

  type order = ^orderDescription;
  orderDescription=record
    kind: orderKind;
    destPos: field;
    armyToAttack: army;
  end;

  type commandKind = (READBOARD, SHOWBOARD, PLAY, QUIT, GENMOVE);

  type userCommand = ^commandDescription;
  commandDescription=record
    kind: commandKind;
    destPos: position; { = nil if SKIP}
    newBoard: boardElements; { <> nil if READBOARD }
  end;

  { dealDamage[A, D, wA] deals combat damage dealt by [A] to [D] }
  procedure dealDamage(attackingArmyIndex: integer; isFirstMoving: boolean;
    var defendingArmy: army; gameBoard: boardElements);

  { moveTo[A, F] moves army [A] into field [F] }
  procedure moveTo(movingArmyIndex: integer; isFirstMoving: boolean;
           destPos: field; gameBoard: boardElements);

  { skipMove[] skips move }
  procedure skipMove();

  { canMoveTo[A, F, B] checks if army [A] can move into field [F]
   on [B] board }
  function canMoveTo(movingArmyIndex: integer; isFirstMoving: boolean;
            destPos: field; gameBoard: boardElements) : boolean;

  { canAttackArmy[A, D] checks if army [A] can attack army [D] }
  function canAttackArmy(attackingArmyIndex: integer; isFirstMoving: boolean;
              armyToAttack: army; gameBoard: boardElements) : boolean;
  { getDamage[A, D] returns the damage dealt during the attack on [D] by [A]}
  function getDamage(attacking: army; defending: army) : integer;

  { readCommand[] reads command from input and return userCommand object }
  function readCommand() : userCommand;

  { drawBoard[B] draws board [B] into output }
  procedure drawBoard(gameBoard: boardElements);

  { getOrder[B, dP, P] makes a order object from board [B], destination [dP]
   and active player [P] for active army }
  function getOrder(gameBoard: boardElements; destPos: position;
    isFirstMoving: boolean) : order;

  { wrongMove[] generates "invalid move" output }
  procedure wrongMove();

  { drawGeneratedMove[O] draws move requested by GENMOVE into output }
  procedure drawGeneratedMove(generatedOrder: order);

  { destroyBoard[B] disposes board [B] with included fields and armies from the
    memory and sets as nil }
  procedure destroyBoard(var gameBoard: boardElements);

  { destroyOrder[O] disposes order [O] from the memory and
    sets as nil }
  procedure destroyOrder(var o: order);

  { destroyCommand[C] disposes userCommand [C] with included field from memory }
  procedure destroyCommand(var command: userCommand);

  { indexToArmy[i, a] returns [i]-army from [a] army list }
  function indexToArmy(i: integer; armies: army) : army;
  
  { curPlayerArmies[fst, b] returns current player army list
    using [fst] boolean player flag and [b] board }
  function curPlayerArmies(isFirstMoving: boolean;
    gameBoard: boardElements) : armyListStart;


implementation

  { hidden types } 

  type positionsList = ^positionListPart;
  positionListPart=record
    prevPart: positionsList; { nil if begin of list }
    nextPart: positionsList; { nil if end of list }
    pos: position;
  end;

  type dualPosList = ^dualPosListDescription;
  dualPosListDescription=record
    b: positionsList;
    e: positionsList;
    size: integer;
  end;

  
  { represenation conversion }  

  function tKindToChar(kind: integer) : char;
  begin
    case kind of
      1 : tKindToChar := FIELD_N;
      2 : tKindToChar := FIELD_G;
      3 : tKindToChar := FIELD_W;
    end;
  end;


  function aKindToChar(kind: integer; playerOne: boolean) : char;
  begin
    if playerOne then aKindToChar := Chr(ORD_1A - 1 + kind)
    else aKindToChar := Chr(ORD_2A - 1 + kind);
  end;
  
  function posToString(pos: position) : string;
  begin
    posToString := Chr(pos^.x + ORD_SA - 1) + intToStr(pos^.y);
  end;


  function indexToArmy(i: integer; armies: army) : army;
  begin
      indexToArmy := armies;
      while i > 1 do begin
        indexToArmy := indexToArmy^.nextArmy;
        dec(i);
      end;
  end;
  
  function curPlayerArmies(isFirstMoving: boolean;
    gameBoard: boardElements) : armyListStart;
  begin
    if isFirstMoving then
      curPlayerArmies := addr(gameBoard^.firstArmies)
    else
      curPlayerArmies := addr(gameBoard^.secondArmies);
  end;

  function secPlayerArmies(isFirstMoving: boolean;
    gameBoard: boardElements) : armyListStart;
  begin
    secPlayerArmies := curPlayerArmies(not isFirstMoving, gameBoard);
  end;


  { types constructors and destructors }

  function makePosition(x: integer; y: integer) : position;
  begin
    new(makePosition);
    makePosition^.x := x;
    makePosition^.y := y;
  end;


  function makeField(x: integer; y: integer; a: army; kind: char) : field;
  begin
    new(makeField);
    makeField^.pos := makePosition(x, y);
    makeField^.a := a;
    case kind of
      FIELD_N : makeField^.kind := 1;
      FIELD_G : makeField^.kind := 2;
      FIELD_W : makeField^.kind := 3;
    end;
  end;


  function makeArmy(kind: integer; pos: field; health: integer;
    prevArmy: army; nextArmy: army; owner: boolean) : army;
  begin
    new(makeArmy);
    makeArmy^.kind := kind;
    makeArmy^.pos := pos;
    makeArmy^.health := health;
    makeArmy^.prevArmy := prevArmy;
    makeArmy^.nextArmy := nextArmy;
    makeArmy^.owner := owner;
  end;


  procedure addNewArmy(var first: army; var last: army; kind: char;
    health: char; f: field; owner: boolean);
  var
    b_kind: integer;
    b_health: integer;
  begin
    b_kind := Ord(lowerCase(kind)) - ORD_1A + 1;
    b_health := Ord(health) - ORD_0;
    if last = nil then begin
      last := makeArmy(b_kind, f, b_health, nil, nil, owner);
      first := last;
    end else begin
      last^.nextArmy := makeArmy(b_kind, f, b_health, last, nil, owner);
      last := last^.nextArmy;
    end;
  end;


  procedure destroyArmy(var a: army; var playerFirst: army);
  begin
    a^.pos^.a := nil;
    if a^.nextArmy <> nil then a^.nextArmy^.prevArmy := a^.prevArmy;
    if a^.prevArmy <> nil then a^.prevArmy^.nextArmy := a^.nextArmy;
    if a = playerFirst then playerFirst := a^.nextArmy;
    dispose(a);
    a := nil;
  end;


  procedure destroyBoard(var gameBoard: boardElements);
  var
    x, y: integer;
    bArmy: army;
  begin
    while gameBoard^.firstArmies <> nil do begin
      bArmy := gameBoard^.firstArmies;
      destroyArmy(bArmy, gameBoard^.firstArmies);
    end;
    while gameBoard^.secondArmies <> nil do begin
      bArmy := gameBoard^.secondArmies;
      destroyArmy(bArmy, gameBoard^.secondArmies);
    end;
    for y := 1 to gameBoard^.boardHeight do
      for x := 1 to gameBoard^.boardWidth do begin
         dispose(gameBoard^.terrain[x][y]^.pos);
         dispose(gameBoard^.terrain[x][y]);
      end;
    dispose(gameBoard);
    gameBoard := nil;
  end;

  procedure destroyOrder(var o: order);
  begin
    dispose(o);
    o := nil;
  end;

  procedure destroyCommand(var command: userCommand);
  begin
    if command^.destPos <> nil then dispose(command^.destPos);
    dispose(command);
    command := nil;
  end;


  { rest }

  function readNewBoard() : boardElements;
  var
    gameBoard: boardElements;
    x: integer;
    y: integer;
    line: string;
    shift: integer;
    b_field: field;
    b_army: army;
    lastFirstArmy: army;
    lastSecondArmy: army;

    procedure initVars();
    begin
    new(gameBoard);
      gameBoard^.firstArmies := nil;
      gameBoard^.secondArmies := nil;
      lastFirstArmy := nil;
      lastSecondArmy := nil;
    end;

    procedure readField();
    begin
      shift := (x-1)*4;
      b_field := makeField(x, y, nil, line[2+shift]);
      gameBoard^.terrain[x][y] := b_field;
      b_army := nil;
      case line[1+shift] of
        'a', 'b', 'c', 'd', 'e', 'f' : begin
          addNewArmy(gameBoard^.firstArmies, lastFirstArmy,
            line[1+shift], line[3+shift], b_field, true);
          b_army := lastFirstArmy;
        end;
        'A', 'B', 'C', 'D', 'E', 'F' : begin
          addNewArmy(gameBoard^.secondArmies, lastSecondArmy,
            line[1+shift], line[3+shift], b_field, false);
          b_army := lastSecondArmy;
        end;
      end;
      b_field^.a := b_army;
    end;
    
  begin
    initVars();
    readln(gameBoard^.boardHeight);
    readln(gameBoard^.boardWidth);
    for y := 1 to gameBoard^.boardHeight do
    begin
      readln(line);
      for x := 1 to gameBoard^.boardWidth do
        readField();
    end;
    readNewBoard := gameBoard;
  end;


  function getDamage(attacking: army; defending: army) : integer;
  begin
    case attacking^.pos^.kind of
      1 : getDamage := UNITS_SN[attacking^.kind];
      2 : getDamage := UNITS_SG[attacking^.kind];
      3 : getDamage := UNITS_SW[attacking^.kind];
    end;
    if (attacking^.kind in [1..2]) and (defending^.kind in [1..2]) then inc(getDamage);
    getDamage := getDamage - UNITS_P[defending^.kind];
    if getDamage < 0 then getDamage := 0;
  end;

  procedure dealDamage(attackingArmyIndex: integer; isFirstMoving: boolean;
    var defendingArmy: army; gameBoard: boardElements);
  var
    damage: integer;
    attackingArmy: army;
    attackerArmies: armyListStart;
    defenderArmies: armyListStart;

  begin
    attackerArmies := curPlayerArmies(isFirstMoving, gameBoard);
    defenderArmies := secPlayerArmies(isFirstMoving, gameBoard);
    attackingArmy := indexToArmy(attackingArmyIndex, attackerArmies^);
    damage := getDamage(attackingArmy, defendingArmy);
    defendingArmy^.health := defendingArmy^.health - damage;
    if defendingArmy^.health <= 0 then
      destroyArmy(defendingArmy, defenderArmies^);
    write(#10, #10);
  end;


  procedure moveTo(movingArmyIndex: integer; isFirstMoving: boolean;
    destPos: field; gameBoard: boardElements);
  var
    movingArmy: army;
  begin
    movingArmy := indexToArmy(movingArmyIndex,
      curPlayerArmies(isFirstMoving, gameBoard)^);
    movingArmy^.pos^.a := nil;
    movingArmy^.pos := destPos;
    destPos^.a := movingArmy;
    write(#10, #10);
  end;


  procedure skipMove();
  begin
    write(#10, #10);
  end;


  function canMoveTo(movingArmyIndex: integer; isFirstMoving: boolean;
    destPos: field; gameBoard: boardElements) : boolean;

  type 
    fA = array [1..MAX_SIZE, 1..MAX_SIZE] of Boolean;
  var
    fieldTab: fA;
    posList: dualPosList;
    flag: boolean;
 
    procedure push(pos: position);
    var
       part: positionsList;
    begin
      new(part);
      part^.pos := pos;
      part^.nextPart := nil;
      if posList^.b = nil then begin
        part^.prevPart := nil;
        posList^.b := part;
        posList^.e := part;
      end else begin
        posList^.e^.nextPart := part;
        part^.prevPart := posList^.e;
        posList^.e := part;
      end;
      inc(posList^.size)
    end;

    function pop() : position;
    var
      handle: positionsList;
    begin
      pop := posList^.b^.pos;
      handle := posList^.b;
      posList^.b := handle^.nextPart;
      if posList^.b = nil then posList^.e := nil
      else posList^.b^.prevPart := nil;
      dispose(handle);
      dec(posList^.size)
    end;

    function inBoard(f: field; b: boardElements) : boolean;
    begin
      if (f^.pos^.x in [1..b^.boardWidth]) and
        (f^.pos^.y in [1..b^.boardHeight]) then inBoard := true
      else inBoard := false;
    end;
    
    function goodFieldType(a: army; f: field) : boolean;
    begin
      goodFieldType := false;
      if (f^.a <> nil) and (f^.a^.owner <> a^.owner) then exit;
      case f^.kind of
        1 : goodFieldType := true; { all units }
        2 : if (a^.kind in [1..2]) or (a^.kind in [5..6]) then
          goodFieldType := true ; { infantry and aviation }
        3 : if a^.kind in [5..6] then goodFieldType := true; { aviation }
      end;
    end;

    procedure checkout(a: army; gameBoard: boardElements; destPos: field);
    var
      pos: position;

      procedure grunt(a: army; x: integer; y: integer; gameBoard: boardElements);
      begin
        if (x in [1..gameBoard^.boardWidth]) and
           (y in [1..gameBoard^.boardHeight]) and
           (not fieldTab[x][y]) and
           goodFieldType(a, gameBoard^.terrain[x][y]) then
        begin
          fieldTab[x][y] := true;
          push(gameBoard^.terrain[x][y]^.pos);
        end;
      end;

    begin
      pos := pop();
      if (pos^.x = destPos^.pos^.x) and (pos^.y = destPos^.pos^.y) then begin
         flag := true;
         exit;
      end;
      grunt(a, pos^.x + 1, pos^.y, gameBoard);
      grunt(a, pos^.x - 1, pos^.y, gameBoard);
      grunt(a, pos^.x, pos^.y + 1, gameBoard);
      grunt(a, pos^.x, pos^.y - 1, gameBoard);
    end;

  var
    x, y: integer;
    iter: integer;
    movingArmy: army;
  begin
    movingArmy := indexToArmy(movingArmyIndex,
      curPlayerArmies(isFirstMoving, gameBoard)^);
    if (not inBoard(destPos, gameBoard)) or
      (not goodFieldType(movingArmy, destPos)) then
    begin
      canMoveTo := false;
      exit;
    end;
    for y := 1 to gameBoard^.boardHeight do
      for x := 1 to gameBoard^.boardWidth do fieldTab[x][y] := false;
    new(posList);
    posList^.b := nil;
    posList^.e := nil;
    posList^.size := 0;
    flag := false;
    push(movingArmy^.pos^.pos);
    fieldTab[movingArmy^.pos^.pos^.x][movingArmy^.pos^.pos^.y] := true;
    iter := 0;
    while (iter <= UNITS_ZR[movingArmy^.kind]) and (posList^.size > 0) do begin
      x := posList^.size;
      while x > 0 do begin
        checkout(movingArmy, gameBoard, destPos);
        dec(x);
        if flag then break;
      end;
      inc(iter);
      if flag then break;
    end;
    while posList^.size > 0 do pop();
    dispose(posList);
    canMoveTo := flag;
  end;


  function canAttackArmy(attackingArmyIndex: integer; isFirstMoving: boolean;
    armyToAttack: army; gameBoard: boardElements) : boolean;
  var
    d: integer;
    aKind: integer;
    attackingArmy: army;
  begin
    attackingArmy := indexToArmy(attackingArmyIndex,
      curPlayerArmies(isFirstMoving, gameBoard)^);
    canAttackArmy := false;
    if (attackingArmy^.kind in [1, 3, 6]) and (armyToAttack^.kind in [5..6]) then exit; 
    aKind := attackingArmy^.kind;
    d := 0;
    d := abs(armyToAttack^.pos^.pos^.x - attackingArmy^.pos^.pos^.x);
    d := d + abs(armyToAttack^.pos^.pos^.y - attackingArmy^.pos^.pos^.y);
    if (d >= UNITS_ZMI[aKind]) and (d <= UNITS_ZMA[aKind]) then
      canAttackArmy := true;
    if attackingArmy^.owner = armyToAttack^.owner then
      canAttackArmy := false;
  end;


  function readCommand() : userCommand;
  var
    currentCommand: userCommand;
    input: string;
    playX: integer;
    playY: integer;

    procedure commReadboard();
    begin
      currentCommand^.kind := READBOARD;
      currentCommand^.newBoard := readNewBoard();
      write('=', #10, #10);
    end;
    
    procedure commShowboard();
    begin
      currentCommand^.kind := SHOWBOARD;
      write('=', #10);
    end;
    
    procedure commPlay();
    begin
      currentCommand^.kind := PLAY;
      if copy(input, 6, 5) = 'skip' then currentCommand^.destPos := nil
      else begin
        playX := Ord(input[6]) - ORD_SA + 1;
        val(copy(input, 7, 4), playY);
        currentCommand^.destPos := makePosition(playX, playY);
      end;
      write('=');
    end;
    
    procedure commGenmove();
    begin
      currentCommand^.kind := GENMOVE;
      write('= ');
    end;
    
    procedure commQuit();
    begin
      currentCommand^.kind := QUIT;
      write('=', #10, #10);      
    end;

  begin
    new(currentCommand);
    currentCommand^.destPos := nil;
    currentCommand^.newBoard := nil;
    readln(input);
    if input = 'readboard' then commReadboard()
    else if input = 'showboard' then commShowboard()
    else if copy(input, 1, 5) = 'play ' then commPlay()
    else if input = 'genmove' then commGenmove()
    else commQuit();
    readCommand := currentCommand;
  end;


  procedure drawBoard(gameBoard: boardElements);
  var
    x: integer;
    y: integer;
    tChar: char;
    aChar: char;
    hChar: char;
  begin
    for y := 1 to gameBoard^.boardHeight do begin
      for x := 1 to gameBoard^.boardWidth do begin
        if gameBoard^.terrain[x][y]^.a <> nil then begin
          aChar := aKindToChar(gameBoard^.terrain[x][y]^.a^.kind,
            gameBoard^.terrain[x][y]^.a^.owner);
          hChar := Chr(gameBoard^.terrain[x][y]^.a^.health + ORD_0);
        end else begin
          aChar := FIELD_N;
          hChar := FIELD_N;
        end;
        tChar := tKindToChar(gameBoard^.terrain[x][y]^.kind);
        write(aChar, tChar, hChar);
        if x <> gameBoard^.boardWidth then write(' ');
      end;
      write(#10);
    end;
    write(#10);
  end;


  function getOrder(gameBoard: boardElements; destPos: position;
    isFirstMoving: boolean) : order;
  var
    currentOrder: order;
  begin
    new(currentOrder);
    if destPos = nil then currentOrder^.kind := SKIP
    else begin
      currentOrder^.destPos := gameBoard^.terrain[destPos^.x][destPos^.y];
      if currentOrder^.destPos^.a <> nil then begin
        currentOrder^.kind := ATTACK;
        currentOrder^.armyToAttack := currentOrder^.destPos^.a;
      end else currentOrder^.kind := MOVE;
    end;
    getOrder := currentOrder;
  end;


  procedure wrongMove();
  begin
    writeln(' invalid move', #10);
  end;


  procedure drawGeneratedMove(generatedOrder: order);
  begin
    if generatedOrder^.kind = SKIP then write('skip')
    else begin
      write(posToString(generatedOrder^.destPos^.pos));
    end;
    write(#10, #10);
  end;

end.
