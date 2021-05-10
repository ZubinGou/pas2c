
Program roman (output);

Var 
  a: array[1..100] Of integer;
  i: integer;
Begin
(*main program*)
  For i:=1 To 100 Do
    a[i] := 2 * i;
  For i:=1 To 30 Do
    writeln(a[3*i]);
End
(*roman*).