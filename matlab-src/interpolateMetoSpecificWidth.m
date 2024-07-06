function M_new =interpolateMetoSpecificWidth(M,order, xCoordinate)
M_new = zeros(length(xCoordinate),2);
M_new(:,1)=round(xCoordinate);
p=polyfit(M(:,1),M(:,2),order);
M_new(:,2)=polyval(p,M_new(:,1));
end