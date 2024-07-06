% A function that detects the video type (whether the whole velocity graph
% moves or it is a moving slider
% INPUTS:
% currentVelocityFrame and previousVelocityFrame are the first two frames
% of the video
% OUTPUTS:
% xAxisLocation: The location of the x-axis of the velocity graph
% videoType: 1 means the whole graph moves, and 2 means it is a slider based video 
% developed by Ahmed Gawish January 2017
function [xAxisLocation, videoType] = setup4Velocity(currentVelocityFrame, previousVelocityFrame)
xWidth = size(currentVelocityFrame,2);
tempImg = currentVelocityFrame(: , round(0.1*xWidth): end-round(0.1*xWidth)); % ignore %10 of the columns from the begining and ending of the image to make sure less contribution of black pixels
varRows = var(tempImg, 0, 2);

% bw = tempImg==0;
bw = im2bw(tempImg, graythresh(tempImg));
bw = bw==0;
blackCount = sum(bw, 2);
toRemove = blackCount> 0.50 * size(bw,2);
varRows(toRemove) = NaN;
xAxisLocation = find(varRows == min(varRows), 1, 'first');

% ASSUMING THAT THE USER CAREFULLY SELECTS THE AREA OF INTEREST TO NOT
% INCLUDE ANY IRRELVANT DATA (e.g. THE AXES)
binaryFrame = im2bw(currentVelocityFrame, graythresh(currentVelocityFrame));
binarySum = sum(binaryFrame(:, 5:end-5));
[~, indx] = find(binarySum == 0);

frameDiff =  double(currentVelocityFrame) - double(previousVelocityFrame);
if sum(frameDiff(:))== 0%isempty(indx)
    videoType = 1; % whole graph moves
else
    videoType = 2; % silder
end

end
