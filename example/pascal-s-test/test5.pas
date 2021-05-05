(*pascal teaser, access of components, bl 750807*)

Program tease1 (output);

Var 
  v: char;
  r: Record
    f: Record
      f: char
    End;
    g: Record
      f: char
    End;
  End;
  a: array [char] Of char;

Begin
(*tease*)
  v.t := r [ v ];
  a.f := v  ;
  a [ r . f .  f] :=  r  .  g  .  g;
  a [ r ] . f := r . 1 ;
  a ( 1 ) := r ;
  a [ '1', '1'  ] := v;
End.
