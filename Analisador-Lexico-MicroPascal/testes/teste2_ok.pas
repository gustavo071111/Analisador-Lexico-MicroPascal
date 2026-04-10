{ Teste 2: Variaveis reais e condicionais }
program calculo;
var
  a, b : real;
  resultado : real;
begin
  a := 3.14;
  b := 2.71;
  if a > b then
    resultado := a - b
  else
    resultado := b - a;
  writeln(resultado);
end.