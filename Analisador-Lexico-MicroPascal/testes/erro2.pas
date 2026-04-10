{ Erro 2: Comentario nao fechado }
program teste;
var
  x : integer;
begin
  x := 42;
    { este comentario nunca fecha...}
  writeln(x);
end.