function currVert = makeParallel2Init(currVert, initVert)
% making blackVert parallel to the initialization;
initVert = interpolateMetoSpecificWidth(initVert, 1, currVert(:,1));
meanVert  = initVert(:,2) - currVert(:,2);
meanValue = mean(meanVert);
currVert(:,2) = initVert(:,2) - meanValue;
end