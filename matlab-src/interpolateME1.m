function M_new =interpolateME1(M,order)
M = mySort(M);
sz = abs((round(M(end,1))-round(M(1,1))))+1;
M_new = zeros(sz,2);%zeros(size(M,1),2);
M_new(:,1) = [round(M(1,1)):round(M(end,1))]';%M(:,1);
p = polyfit(M(:,1),M(:,2),order);
M_new(:,2) = polyval(p,M_new(:,1));
end