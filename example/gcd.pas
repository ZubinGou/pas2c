
Program example(input, output);
{啊的首发式发生}
 
Var x, y : integer;
Function gcd( a, b : integer) : integer;
Begin
  If b = 0 Then gcd := a
  Else gcd := gcd(b, a Mod b)
End;
Begin
  read(x, y);
  write(gcd(x, y))
End.
