
Program example(input, output);
<<<<<<< HEAD
{下面如果是空行会有bug}

=======
{啊的首发式发生}
 
>>>>>>> 4ff4ae4 (fix lexer bug)
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
