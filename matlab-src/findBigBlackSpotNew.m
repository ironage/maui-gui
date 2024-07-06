function [refVertUp, refVertLow ] = findBigBlackSpotNew(img , initUp, initLow)

refVertUp = initUp;
refVertLow = initLow;
imgHeight = size(img,1);
halfHeight = ceil(imgHeight/2);
imgWidth = size(img,2);

indx = 1;
resUp = zeros(halfHeight,1);
resBot = resUp;
[wallKerUp, wallKerBot] = refWallKernel(img, initUp, initLow, 21);
shiftUp = 0;
shiftBot = 0;
if mod(size(wallKerUp,1),2)==0
    shiftUp = -1;
end
if mod(size(wallKerBot,1),2)==0
    shiftBot = -1;
end

padUp = floor(size(wallKerUp,1)/2); % the number of rows added to the currentFrame. A number of Pad rows are added to the currentFrame
padBot = floor(size(wallKerBot,1)/2);
img = cat(1, zeros(padUp,imgWidth),img);
img = cat(1, img, zeros(padBot,imgWidth));

for i = padUp+1: halfHeight
    count = wallKerUp .* img((i-padUp:i+padUp+shiftUp),:);
    resUp(indx) = sum(count(:));
    count = wallKerBot .* img((i+halfHeight-padBot:i+halfHeight+padBot+shiftBot),:);
    resBot(indx) = sum(count(:));
    indx = indx+1;
end

[~, locs] = findpeaks(resUp, 'SortStr','descend');
topShift = locs(1)- mean(initUp(:,2));

[~, locs1] = findpeaks(resBot, 'SortStr','descend');
botShift = locs1(1)+halfHeight - mean(initLow(:,2));

refVertUp(:,2) = initUp(:,2) + topShift;
refVertLow(:,2) = initLow(:,2) + botShift;

end
