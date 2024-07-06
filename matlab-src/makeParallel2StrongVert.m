function weakVert = makeParallel2StrongVert(weakVert, strongVert)
% making weal wall parallel to the strong wall
smallStrongVert = weakVert;
y = strongVert(:,2);
smallStrongVert(:,2) = y([weakVert(:,1)-(weakVert(1,1)-1)]); 

meanVert  = smallStrongVert(:,2) - weakVert(:,2); 
meanValue = mean(meanVert);
weakVert(:,2) = smallStrongVert(:,2) - meanValue;
end