
Program example(input, output);
{下面如果是空行会有bug}

var
Books : record
  title: array [1..50] of char;
  author: array [1..50] of char;
  subject: array [1..100] of char;
  bookid: integer;
end;
a : array [1..100] of char;
b : integer;
c : array [1..200, 1..300, 1..400] of real;
d : real;

Begin
  a[1] := Books.title[1];
  b := Books.bookid;
  b := d;
  d := Books.subject[1];
  d := c[0, 2, 1000];
  d := c[100, 100, 100];

End.
