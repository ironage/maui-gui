% A function that calculates velocity measures
% INPUTS
% currVelcotiyFrame: only the velocity portion of the frame video frame
% frameNum: frame index
% xAxisLocation: the location of the x-axis of the velocity graph
% videoType: whether the whole graph moves or it is a slider based video
% firstMovingFrame: the index of the first moving frame
% xTrackingLocationIndividual: the x-coordinates of the current frame where
% the chages happens
% OUTPUTS
% maxPositive, avgPositive, maxNegative and avgNegative are the velocity
% measures
% xTrackingLocationIndividual: the x-coordinates of the current frame where
% the chages happen
% developed by Ahmed Gawish January 2017
function [maxPositive, avgPositive, maxNegative, avgNegative, xTrackingLocationIndividual] = processVelocityIntervals(currVelcotiyFrame, frameNum, xAxisLocation, videoType, firstMovingFrame, xTrackingLocationIndividual)
maxPositive = xAxisLocation;
avgPositive = xAxisLocation;
maxNegative = xAxisLocation;
avgNegative = xAxisLocation;
yHeight = size(currVelcotiyFrame, 1);
xWidth = size(currVelcotiyFrame, 2);
xTrackingLocationIndividual = xTrackingLocationIndividual(end);
% test for alignment with Kathryn Video
numOfReturnedMeasures = 20;

% currVelcotiyFrame(currVelcotiyFrame>170) = 30;


if videoType == 1 % the whole graph moves
    
    if  frameNum < firstMovingFrame % at the very begining of the video; almost everything is stationary
        xTrackingLocationIndividual = [ceil((frameNum-1) * (xWidth/firstMovingFrame))+1 : ceil(frameNum * (xWidth/firstMovingFrame))];
    else
        %         if mod(frameNum, 2) == 0
        %             td = 2;
        %         else
        %             td = 3;
        %         end
        xTrackingLocationIndividual = [xWidth - 2 - round(xWidth/firstMovingFrame): xWidth - 2];
        %           xTrackingLocationIndividual = [xWidth - 5 : xWidth - td];
    end
    
    binaryCol = im2bw(mat2gray(currVelcotiyFrame(:,xTrackingLocationIndividual)),graythresh(mat2gray(currVelcotiyFrame(:,xTrackingLocationIndividual))));
    [val , yTrackingLocationMaxIndividualPositive] = max(binaryCol(1:xAxisLocation-2, :));
    
    
    
    %     if sum(val) ~=0 % there is signal data
    maxPositive = yTrackingLocationMaxIndividualPositive;
    maxPositive(yTrackingLocationMaxIndividualPositive==1) = xAxisLocation;
    cumIntensity = cumsum(double(currVelcotiyFrame(1:xAxisLocation-2,xTrackingLocationIndividual)).* binaryCol(1:xAxisLocation-2,:)); % 2 is a margin to avoid counting the X-axis intensities
    averIntensity = cumIntensity >  repmat(cumIntensity(end,:)./2,[size(cumIntensity,1), 1]);
    [~, yTrackingLocationAvgIndividual] = max(averIntensity);
    avgPositive = yTrackingLocationAvgIndividual; % store in the array
    avgPositive(yTrackingLocationMaxIndividualPositive==1) = xAxisLocation;
    %     end
    %                 plot(xTrackingLocationIndividual, yTrackingLocationAvgIndividual, '.g', 'linewidth', 2);
    %                 plot(xTrackingLocation, yTrackingLocationMaxIndividual, '.r');
    
    %% Negative part of the graph
    [val , yTrackingLocationMaxIndividualNegative] = max(flipud(binaryCol(xAxisLocation+2:end, :)));
    %     if sum(val) ~= 0 % there is signal information
    maxNegative = yHeight - yTrackingLocationMaxIndividualNegative+2; % store in the array
    maxNegative(yTrackingLocationMaxIndividualNegative==1) = xAxisLocation;
    
    cumIntensity = cumsum(double(binaryCol(xAxisLocation+2:end,: )).*double(currVelcotiyFrame(xAxisLocation+2:end,xTrackingLocationIndividual))); % 2 is a margin
    averIntensity = cumIntensity>repmat(cumIntensity(end,:)./2,[size(cumIntensity,1), 1]);
    [~, yTrackingLocationAvgIndividualNegative] = max(averIntensity);
    yTrackingLocationAvgIndividualNegative = xAxisLocation + yTrackingLocationAvgIndividualNegative;
    avgNegative = yTrackingLocationAvgIndividualNegative;
    avgNegative(yTrackingLocationMaxIndividualNegative==1) = xAxisLocation;
end

if  videoType == 2 % the video has a moving slider
    
    
    sumFrame = sum(currVelcotiyFrame);
    [~, sliderLocation] = min(sumFrame); % new idea....
    if (sliderLocation - xTrackingLocationIndividual)>=1
        xTrackingLocationIndividual = [xTrackingLocationIndividual [xTrackingLocationIndividual+1 : sliderLocation-5]];
    else
        tempPoints = mod([[xTrackingLocationIndividual: xWidth] [1:sliderLocation-5]],xWidth);
        tempPoints(tempPoints==0) = xWidth;
        xTrackingLocationIndividual = tempPoints;%1;%[1: sliderLocation-5];
    end
    binaryCol = im2bw(mat2gray(currVelcotiyFrame(:,xTrackingLocationIndividual)),graythresh(mat2gray(currVelcotiyFrame(:,xTrackingLocationIndividual))));
    binaryCol = bwareaopen(binaryCol, 30);% remove samll noise objects from the binary image
    [val , yTrackingLocationMaxIndividualPositive] = max(binaryCol(1:xAxisLocation-2, :));
    
    %     if sum(val) ~=0 % there is signal data
    maxPositive = yTrackingLocationMaxIndividualPositive;
    maxPositive(yTrackingLocationMaxIndividualPositive==1) = xAxisLocation;
    cumIntensity = cumsum(double(currVelcotiyFrame(1:xAxisLocation-2,xTrackingLocationIndividual)).* binaryCol(1:xAxisLocation-2,:)); % 2 is a margin to avoid counting the X-axis intensities
    averIntensity = cumIntensity >  repmat(cumIntensity(end,:)./2,[size(cumIntensity,1), 1]);
    [~, yTrackingLocationAvgIndividual] = max(averIntensity);
    avgPositive = yTrackingLocationAvgIndividual; % store in the array
    avgPositive(yTrackingLocationMaxIndividualPositive==1) = xAxisLocation;
    %     end
    
    %% Negative part of the graph
    [val , yTrackingLocationMaxIndividualNegative] = max(flipud(binaryCol(xAxisLocation+2:end, :)));
    %     if sum(val) ~= 0 % there is signal information
    maxNegative = yHeight - yTrackingLocationMaxIndividualNegative+2; % store in the array
    maxNegative(yTrackingLocationMaxIndividualNegative==1) = xAxisLocation;
    
    cumIntensity = cumsum(double(binaryCol(xAxisLocation+2:end,: )).*double(currVelcotiyFrame(xAxisLocation+2:end,xTrackingLocationIndividual))); % 2 is a margin
    averIntensity = cumIntensity>repmat(cumIntensity(end,:)./2,[size(cumIntensity,1), 1]);
    [~, yTrackingLocationAvgIndividualNegative] = max(averIntensity);
    yTrackingLocationAvgIndividualNegative = xAxisLocation + yTrackingLocationAvgIndividualNegative;
    avgNegative = yTrackingLocationAvgIndividualNegative;
    avgNegative(yTrackingLocationMaxIndividualNegative==1) = xAxisLocation;
    
    
    %%
    %     else
    %          maxNegative = [ones(length(xTrackingLocationIndividual), 1)]'*xAxisLocation;
    %          avgNegative = maxNegative;
    %     end
    %     [maxPositive(frameNum)  avgPositive(frameNum)  maxNegative(frameNum)  avgNegative(frameNum)  xTrackingLocationIndividual]
    % xTrackingLocationIndividual = sliderLocation-5;
end

%  [length(maxPositive)  length(maxNegative) length(xTrackingLocationIndividual)]
%%
% only selecting numOfReturnedMeasures out of the found measurements
if(numOfReturnedMeasures<=length(maxPositive)) % only select a subset
    step = double(length(maxPositive)/numOfReturnedMeasures);
    indx = floor(cumsum(ones(1,numOfReturnedMeasures)*step));
    maxPositive = maxPositive(indx);
    avgPositive = avgPositive(indx);
    maxNegative = maxNegative(indx);
    avgNegative = avgNegative(indx);
    xTrackingLocationIndividual = xTrackingLocationIndividual(indx);
else % we need to extrapolate first
%             disp('ONLY ONE POINT exist');
    if (length(maxPositive) == 1) %only one item in the vector
        maxPositive = ones(1, numOfReturnedMeasures)* maxPositive;
        maxNegative = ones(1, numOfReturnedMeasures)* maxNegative;
        avgPositive = ones(1, numOfReturnedMeasures)* avgPositive;
        avgNegative = ones(1, numOfReturnedMeasures)* avgNegative;
        xTrackingLocationIndividual = ones(1, numOfReturnedMeasures)* xTrackingLocationIndividual;
    else
%           disp(['more points required than what is already exist' num2str(length(maxPositive))]);
        step  = (length(maxPositive)-1)/(numOfReturnedMeasures-1);
        tempV = [0 cumsum(ones(1,numOfReturnedMeasures-1)*step)];
        %         indx = tempV+ (xTrackingLocationIndividual(1)-tempV(1));
        indx = [xTrackingLocationIndividual(1)+tempV(1:end)];
        indx(end) = xTrackingLocationIndividual(end);
        vq1 = interp1(xTrackingLocationIndividual,maxPositive,indx, 'nearest');
        maxPositive = vq1;
        vq1 = interp1(xTrackingLocationIndividual,maxNegative,indx, 'nearest');
        maxNegative = vq1;
        vq1 = interp1(xTrackingLocationIndividual,avgPositive,indx, 'nearest');
        avgPositive = vq1;
        vq1 = interp1(xTrackingLocationIndividual,avgNegative,indx, 'nearest');
        avgNegative = vq1;
        
        xTrackingLocationIndividual = indx;
    end
    length(maxPositive)
end

end

