function [refVertUp refVertLow] = findBigBlackSpot(img , lastConvergenceUp, lastConvergenceLow, intial_vertUp, intial_vertLow)

BW_otsu = im2bw(img, graythresh(img));
se1 = strel('rectangle',[5 21]);
BW_dilated = imdilate(BW_otsu,se1);
BW_cleaned= bwareaopen(BW_dilated, 300);
se = strel('disk',5); % big disk to make sure that the detected edges fall into the black region
BW_closed = imdilate(BW_cleaned,se);
cc = bwconncomp(~BW_closed);

% Measure the area of all the blobs.
measurements = regionprops(cc, 'Area');
allAreas = [measurements.Area];
% Find the biggest area and blob number
[maxArea, indexOfBiggestBlob] = max(allAreas);
newImg = ones(size(img));
newImg(cc.PixelIdxList{indexOfBiggestBlob})=0;

sz = size(img,1)/2;
e = edge(newImg);
[x y] = find(e==1);
% reducing the number of points to the length of the initialization
% wall
    x_up = y(x<sz);
    y_up = x(x<sz);
    
    x_low = y(x>sz);
    y_low = x(x>sz);
    

if length(x_up)<1 || mean(y_up)>sz || length(unique(x_up))< length(x_up)*0.8  % no values detected as the upper or lower boundar of the balck blob or it is too far
    x_up = lastConvergenceUp(:,1);
    y_up = lastConvergenceUp(:,2);
end
if length(x_low)<1 || mean(y_low)<sz  || length(unique(x_low))< length(x_low)*0.8% no values detected as the lower boundary of the balck blob or it's too far
    x_low = lastConvergenceLow(:,1);
    y_low = lastConvergenceLow(:,2);
end
validPoints = (x_up<=max(lastConvergenceUp(:,1)) & x_up>=min(lastConvergenceUp(:,1)));
x_up = x_up(validPoints);
% y_up = y_up(x_up);
y_up = y_up(validPoints);

validPoints = (x_low<=max(lastConvergenceLow(:,1)) & x_low>=min(lastConvergenceLow(:,1)));
x_low = x_low(validPoints);
% y_low = y_low(x_low);
y_low = y_low(validPoints);
refVertUp(:,1) =x_up;
refVertUp(:,2) =y_up;
refVertLow(:,1) =x_low;
refVertLow(:,2) =y_low;

refVertUp = interpolateME1(refVertUp, 1);
refVertLow = interpolateME1(refVertLow, 1);

refVertUp = reduceNumPoints(refVertUp);
refVertLow = reduceNumPoints(refVertLow);

% intial_vert = reduceNumPoints(intial_vert);

% making refVertUp parallel to the initialization;
meanVert  = intial_vertUp(:,2) - refVertUp(:,2);
meanValue = mean(meanVert);
refVertUp(:,2) = intial_vertUp(:,2) - meanValue;

meanVert  = intial_vertLow(:,2) - refVertLow(:,2);
meanValue = mean(meanVert);
refVertLow(:,2) = intial_vertLow(:,2) - meanValue;

%% check if Otsu thresholding leads to wrong walls
yMeasure = abs(mean(refVertUp(:,2)) - mean(lastConvergenceUp(:,2)));

end
