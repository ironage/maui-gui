function  newStraightenWall=  forceWallsParallelization(Wall2beStraighten, theta)
% making blackVert parallel to the initialization;
% meanVert  = straightWall(:,2) - Wall2beStraighten(:,2);
% meanValue = mean(meanVert);
% Wall2beStraighten(:,2) = straightWall(:,2) - meanValue;
x = Wall2beStraighten(:,1);
y = Wall2beStraighten(:,2);
% newStraightenWall = Wall2beStraighten;
dy = diff([Wall2beStraighten(1,2) ; Wall2beStraighten(:,2)]);
dx = diff([Wall2beStraighten(1,1) ; Wall2beStraighten(:,1)]);
slope = dy ./ dx;
slope(isnan(slope)) = 0;

varianceSlope = abs(slope - theta);
newStraightenWall(:,1) = x(varianceSlope > mode(varianceSlope));
newStraightenWall(:,2) = y(varianceSlope > mode(varianceSlope));

%figure, plot(varianceSlope)
% temp = Wall2beStraighten;
% temp(varianceSlope >mean(varianceSlope))= [];
% temp = interpolateMetoSpecificWidth(temp, 1, x);
% yy= temp(:,2);
% y(varianceSlope > mean(varianceSlope)) = yy(varianceSlope >mean(varianceSlope));
% newStraightenWall(:,2) = y;
% pause();

end