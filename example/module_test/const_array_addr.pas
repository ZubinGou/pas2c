
Program example(input, output);

Const test = 100 ;

Var x, y, z : integer;
  a, b : array[ 10..20, 0..30 ] Of integer ;
Function change( Var c, d : integer) : integer ;

Var t : integer;
Begin
  t := c;
  c := d;
  d := t;
  change := 1
End;
Begin
  read(x, y);
  z := change(x, y);
  write(x, y);
End.
