function [measure dominantOrint]= measureParallelization ( currVert , initVert)

dy = diff(currVert(:,2));
dx = diff(currVert(:,1));
slope = dy ./ dx;
slope(isnan(slope)) = 0;
avgSlope1 = mean(slope);
dominantOrint = avgSlope1; % average slope of currVert

dy = diff(initVert(:,2));
dx = diff(initVert(:,1));
slope = dy ./ dx;
slope(isnan(slope)) = 0;
avgSlope2 = mean(slope);

measure = abs(avgSlope2) - abs(avgSlope1); 
end