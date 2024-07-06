function newVert = removeHigeVar(vert, refVert)
newVert = vert;
yChange = vert(:,2) - refVert(:,2);
yOld = refVert(:,2);
yCoord = vert(:,2);
[~, indx] = sort(abs(yChange));
% remove the first 10 percent of the points in y; these have the biggest
% change in y
yCoord(indx(1:round(length(vert)*0.1))) = yOld(indx(1:round(length(vert)*0.1)))+ mean(yChange(round(length(vert)*0.1):end));
newVert(:,2) = yCoord;
end