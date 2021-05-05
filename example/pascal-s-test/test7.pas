(*program 11.4
(*conversion to postfix form *)

Program postfix(input,output);

Var ch: char;

Procedure find;
(* finds next non-blank character*)
Begin
  Repeat
    read(ch)
  Until (ch<>' ') (*-and not eoln(input)-*)
End
(* find *);

Procedure expression;

Var op: char;
Procedure term;
Procedure factor;
Begin
(*factor*)
  If ch='(' Then (* (expression) *)
    Begin
      find;
      expression;
(*ch must now be ) *)
    End
  Else (*identifier*) write(ch);
  find
End
(*factor*);
Begin
(* term*)
  factor;
  While ch= '*' Do
    Begin
      find;
      factor;
      write('*')
    End
End
(*term*);

Begin
(*expression*)
  term;
  While (ch='+') Or (ch='-') Do
    Begin
      op := ch;
      find;
      term;
      write(op)
    End;
End
(*expression*);

Begin
(*postfix*)
  find;
  Repeat
    write(' ');
    expression;
    writeln
  Until ch= '.'
End
(*postfix*).
Data7
( a + b ) * ( c - d )
a  +  b  *  c  -  d
(a  +  b) * c  - d
a*a*a*a
b+c*(d+c*a*a)*b+a    .