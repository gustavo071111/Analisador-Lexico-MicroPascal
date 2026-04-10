{ Teste 3: Loop while e string }
program contador;
var
  i : integer;
  nome : string;
begin
  nome := 'MicroPascal';
  i := 1;
  while i <= 5 do
  begin
    writeln(i);
    i := i + 1;
  end;
  write(nome);
end.