
Program sort (input,output);

Var a : array[0..4] Of integer;
  x,y: integer;
Procedure readarray;

Var i : integer;
Begin
  i := 0;
  While i<5 Do
    Begin
      read(y);
      a[i] := y;
      i := i+1
    End
End;
Procedure quicksort (l,h:integer);

Var i,j,k,m: integer;
Begin
  i := l;
  j := h;
  k := a[i];
  If l<h Then
    Begin
      While i<j Do
        Begin
          While (a[j]>=k) And (i<j) Do
            Begin
              j := j-1
            End;
          a[i] := a[j];
          While (a[i]<=k) And (i<j) Do
            Begin
              i := i+1
            End;
          a[j] := a[i]
        End;
      a[i] := k;
      quicksort(l,i-1);
      quicksort(j+1,h)
    End
  Else
    m := 0
End;
Begin
  x := 0;
  readarray;
  quicksort(0,4);
  While x<5 Do
    Begin
      y := a[x];
      {writeln(y)}
      writeln(y);
      x := x+1
    End
End.