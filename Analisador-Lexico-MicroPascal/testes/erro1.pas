{ Teste de erros léxicos: @, $ e % }
program erros;
var
  x : integer;
begin
  x := 10 @ 5;
  x := x $ 2;
  x := x % 3;
end.