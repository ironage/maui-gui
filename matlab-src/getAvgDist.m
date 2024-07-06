function range = getAvgDist( refVert, currVert, upORdown)
% a function to compute the average distance between the current wall and
% the referecne wall
maxSearchRane = 10;
distance =  round(abs(abs(currVert(:,2)) - abs(mean(refVert(:,2))))) + 1; % 1 is to prevent the distance from being zero;  
if distance > maxSearchRane
%     distance = 10;
    range = -maxSearchRane:maxSearchRane;
elseif upORdown==1
    range = -maxSearchRane : distance;
else
    range = -distance : maxSearchRane;
end

end