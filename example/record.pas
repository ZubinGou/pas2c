
Program example(input, output);
{下面如果是空行会有bug}

var
Books : record
  title: array [1..50] of char;
  author: array [1..50] of char;
  subject: array [1..100] of char;
  bookid: integer;
end;
a : array [1..100, 1..100] of char;
b : integer;

Begin
  b := Books.bookid
End.
