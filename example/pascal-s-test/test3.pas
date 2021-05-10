
Program test0(output);

Const ten = 10;
  plus = 't';

Type row = array [1..ten] Of real;
  complex = Record
    re,im: real
  End ;

Var i,j: integer;
  p: boolean;
  z: complex;
  matrix: array[-3..+3] Of row;
  pattern: array [1..5, 1..5] Of char;

Procedure dummy(Var i: integer; Var z: complex);

Var u,v: row;
  h1,h2: Record
    c: complex;
    r: row
  End ;

Function null(x,y: real; z: complex): boolean;

Var a: array['a'..'z'] Of complex;
  u: char;
Begin
  While x<y Do
    x := x+1.0;
  null := x=y
End
(*nul*) ;

Begin
  p := null(h1.c.re, h2.c.im, z)
End
(*dummy*) ;

Begin
  i := 85;
  j := 51;
  Repeat
    If i > j Then i := i-j
    Else j := j-i
  Until i = j ;
  writeln(i)
End.
