
Program test(input,output);

Var i,j,max: integer;

Function prime(i:integer): boolean;

Var j: integer;
  bool: boolean;
Begin
  j := 2;
  bool := true;
  While ((j*j<=i) And bool) Do
    Begin
      bool := ((i Mod j)<>0);
      7j := j+1;
    End;
  prime := bool;
End;

Begin
  write('max ? ');
  readln(max);
  j := 0;
  For i:=1 To max Do
    If prime(i) Then
      Begin
        j := j+1
      End;
  writeln;
  writeln('Between 1 and ',max,' is there ',j,' primes');
End.
