function vertClean = cleanUpVert( refVert, currVert ,upORdown)
vertClean = currVert;
y = currVert(:,2);
if length(refVert)~= length(currVert)
    refVert = interpolateMetoSpecificWidth(refVert, 1, currVert(:,1));
end
if upORdown == 1 % upper wall
    % remove all point in currVert that are lower than refVert
    dist =refVert(:,2) - currVert(:,2);
elseif upORdown == -1 % lower wall
    % remove all point in currVert that are higher than refVert
    dist = currVert(:,2) - refVert(:,2);
end
tempY = y;
tempY(dist < 0 | dist>30)=[]; % do not allow the currVert to be more that 30 pixels away of the refVert
if ~isempty(tempY) % tempY is not empty
    y(dist < 0 | dist>30) =mean(tempY);
else
    y(dist <0 | dist>30) = mean(refVert(:,2)) - upORdown*15; % put current vert 20 pixels away from the refrence vert
end
   
vertClean(:,2) = y;
end