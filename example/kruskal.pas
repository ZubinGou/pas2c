{采用并查集实现最小生成树}
Program kruskal (input,output);

var
  a, b, v, p : array[1..100] of integer;
  n, m, i, ans, tot, x, y : integer;

procedure sort(l,r : integer);
var
  i, j, x, y : integer;
begin
  i := l; j := r; x := v[(l+r) div 2];
  while i <= j do
    begin
      while v[i] < x do i := i+1;
      while x < v[j] do j := j-1;
      if not (i > j) then begin
        y := v[i]; v[i] := v[j]; v[j] := y;
        y := a[i]; a[i] := a[j]; a[j] := y;
        y := b[i]; b[i] := b[j]; b[j] := y;
        i := i+1; j := j-1;
      end;
    end;
  if l < j then sort(l,j);
  if i < r then sort(i,r);
end;

function doit(x : integer) : integer;
begin
  if p[x] = x then begin
    ans := x;
    doit := x;
  end
  else begin
    p[x] := ans;
    doit := doit(p[x]);
  end;
end;

begin
  read(n,m);
  for i := 1 to m do read(a[i], b[i], v[i]);
  sort(1, m);
  for i := 1 to n do p[i] := i;
  for i := 1 to m do begin
    x := doit(a[i]); y := doit(b[i]);
    if (x <> y) then begin
      p[x] := y;
      tot := tot+v[i];
    end;
  end;
  write(tot);
end.

{
6 15
0 1 4
0 2 4
1 2 2
1 0 4
2 0 4
2 1 2
2 3 3
2 5 2
2 4 4
3 2 3
3 4 3
4 2 4
4 3 3
5 2 2
5 4 3

ans: 10
}