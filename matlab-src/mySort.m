function M_new = mySort(M)
M_new = zeros(size(M));
x=M(:,1);
y=M(:,2);
[x t]=sort(x);
y=y(t);
M_new(:,1)=x;
M_new(:,2)=y;

end