
Program roman (output);

Var i : integer;

Procedure romanout(i: integer);
(* prints roman representation of i *)

Var 
  pos: integer;
  scale: array [1..7] Of 
         Record
           amount: integer;
           sign : char
         End;
Begin
(*romanout*)
  scale[1]. amount := 1;
  scale[1]. sign := 'I';
  scale[2]. amount := 5;
  scale[2]. sign := 'V';
  scale[3]. amount := 10;
  scale[3]. sign := 'X';
  scale[4]. amount := 50;
  scale[4]. sign := 'L';
  scale[5]. amount := 100;
  scale[5]. sign := 'C';
  scale[6]. amount := 500;
  scale[6]. sign := 'D';
  scale[7]. amount := 1000;
  scale[7]. sign := 'M';
  write(' ');
  For pos := 7 Downto 1 Do
    While  i>= scale[pos].amount Do
      Begin
        i := i- scale[pos].amount;
        write(scale[pos].sign)
      End;
End
(*romanout*);

Begin
(*main program*)
  i := 1;
  While i<= 5000 Do
    Begin
      write(i);
      romanout(i);
      writeln;
      i := i*2
    End
End
(*roman*).