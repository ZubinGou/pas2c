Program knapsack (input,output);

var
        n,t,i,j : integer;
        f : array[0..1000,0..1000] of integer;
        w,v : array[0..1000] of integer;

function max(x,y : integer) : integer;
begin
        if x>y then max:=x
                else max:=y;
end;

begin
        read(t,n);
        for i:=1 to n do
        begin
          read(w[i], v[i]);
        end;
        for i:=1 to n do
                for j:=0 to t do
                begin
                        f[i,j]:=f[i-1,j];
                        if j>=w[i] then
                                f[i,j]:=max(f[i-1,j-w[i]]+v[i],f[i,j]);
                end;
        write(f[n,t]);
end.