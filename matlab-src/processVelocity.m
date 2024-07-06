function [maxPositive, avgPositive, maxNegative, avgNegative, xTrackingLocationIndividual] = processVelocity(currVelcotiyFrame, frameNum, xAxisLocation, videoType, firstMovingFrame, xTrackingLocationIndividual)
maxPositive = xAxisLocation;
avgPositive = xAxisLocation;
maxNegative = xAxisLocation;
avgNegative = xAxisLocation;
yHeight = size(currVelcotiyFrame, 1);
xWidth = size(currVelcotiyFrame, 2);
if videoType == 1 % the whole graph moves
    
    if  frameNum < firstMovingFrame % at the very begining of the video; almost everything is stationary
        xTrackingLocationIndividual = round(frameNum * (xWidth/firstMovingFrame));
    else
         xTrackingLocationIndividual = xWidth - 5; 
    end
%         BW = im2bw(currVelcotiyFrame, graythresh(currVelcotiyFrame));
%         BW = bwareaopen(BW,20); %remove small noise objects (e.g the ticks on the axis)
%         E = bwmorph(edge(BW), 'bridge');
%         [ ~, maxPositive] = max(BW(1:xAxisLocation-2,xTrackingLocationIndividual)); % Also coud be max(BW);
% %         tempVar = yTrackingLocationMaxWholePositive ==1;
% %         yTrackingLocationMaxWholePositive(tempVar) = xAxisLocation;
%         
%         cumIntensity = cumsum(double(BW(1:xAxisLocation-2,xTrackingLocationIndividual)).*double(currVelcotiyFrame(1:xAxisLocation-2,xTrackingLocationIndividual))); % 5 is a margin
%         averIntensity = cumIntensity>repmat(cumIntensity(end,:)./2,[size(cumIntensity,1), 1]);
%         [~, avgPositive] = max(averIntensity);
% %         yTrackingLocationAvgWholePositive(tempVar) = xAxisLocation;
%         
%         % Negative velocity
%         [ ~, maxNegative] = max(flipud(BW(xAxisLocation+2:end,xTrackingLocationIndividual)));% flip the lower part of the image
%         % (i.e. underneath the x-axis) to easily use max function, and then do the math to accuratly determin the exact location
% %         tempVar = yTrackingLocationMaxWholeNegative ~=1;
%         maxNegative = size(BW,1) - maxNegative+2;
% %         yTrackingLocationMaxWholeNegative(~tempVar) =  xAxisLocation;
%         %                     yTrackingLocationMaxWholeNegative = xAxisLocation+yTrackingLocationMaxWholeNegative;
%         cumIntensity = cumsum(double(BW(xAxisLocation+2:end,xTrackingLocationIndividual)).*double(currVelcotiyFrame(xAxisLocation+2:end,xTrackingLocationIndividual))); % 5 is a margin
%         averIntensity = cumIntensity>repmat(cumIntensity(end,:)./2,[size(cumIntensity,1), 1]);
%         [~, avgNegative] = max(averIntensity);
%         avgNegative = xAxisLocation + avgNegative;
% %         yTrackingLocationAvgWholeNegative(~tempVar) =  xAxisLocation;
%         
%         % interpolate the caculated points amongest the array
% %         tempArr = cumsum(ones(1,totalFrameNum-1)*(length(yTrackingLocationMaxWholePositive)/(totalFrameNum-1)));
% %         maxPositive(1:totalFrameNum-1) = yTrackingLocationMaxWholePositive(round(tempArr));
% %         avgPositive(1:totalFrameNum-1) = yTrackingLocationAvgWholePositive(round(tempArr));
% %         maxNegative(1:totalFrameNum-1) = yTrackingLocationMaxWholeNegative(round(tempArr));
% %         avgNegative(1:totalFrameNum-1) = yTrackingLocationAvgWholeNegative(round(tempArr));
%     else % the graph started to move
        
%         tempArr = cumsum(ones(1,frameNum-1)*(length(yTrackingLocationMaxWholePositive)/(frameNum-1)));
%         maxPositive(1:frameNum-1) = yTrackingLocationMaxWholePositive(round(tempArr));
%         avgPositive(1:frameNum-1) = yTrackingLocationAvgWholePositive(round(tempArr));
%         maxNegative(1:frameNum-1) = yTrackingLocationMaxWholeNegative(round(tempArr));
%         avgNegative(1:frameNum-1) = yTrackingLocationAvgWholeNegative(round(tempArr));
        
%         xTrackingLocationIndividual = xWidth - 5; % A little margin to avoid the vertical axis at the right side
        binaryCol = im2bw(mat2gray(currVelcotiyFrame(:,xTrackingLocationIndividual)),graythresh(mat2gray(currVelcotiyFrame(:,xTrackingLocationIndividual))));
        [val , yTrackingLocationMaxIndividualPositive] = max(binaryCol(1:xAxisLocation-2));
        
        
        % claculate average intensity value at the same location (at the very left of the moving curve).
        %                 BW = im2bw(mat2gray(currVelcotiyFrame(:,xTrackingLocationIndividual)),graythresh(mat2gray(currVelcotiyFrame(:,xTrackingLocationIndividual))));
        %                 cumIntensity = cumsum(double(BW(1:xAxisLocation-5,:)).*double(currVelcotiyFrame(1:xAxisLocation-5,xTrackingLocationIndividual))); % 5 is a margin
        
        
        if val ~=0 % there is signal data
            maxPositive = yTrackingLocationMaxIndividualPositive;
            cumIntensity = cumsum(double(currVelcotiyFrame(1:xAxisLocation-2,xTrackingLocationIndividual))); % 2 is a margin to avoid counting the X-axis intensities
            averIntensity = cumIntensity >  cumIntensity(end)./2; %repmat(cumIntensity(end,:)./2,[size(cumIntensity,1), 1]);
            [~, yTrackingLocationAvgIndividual] = max(averIntensity);
            avgPositive = yTrackingLocationAvgIndividual; % store in the array
        end
        %                 plot(xTrackingLocationIndividual, yTrackingLocationAvgIndividual, '.g', 'linewidth', 2);
        %                 plot(xTrackingLocation, yTrackingLocationMaxIndividual, '.r');
        
        %% Negative part of the graph
        [val , yTrackingLocationMaxIndividualNegative] = max(flipud(binaryCol(xAxisLocation+2:end)));
        if val ~= 0 % there is signal information
            maxNegative = yHeight+1 - yTrackingLocationMaxIndividualNegative+2; % store in the array
            cumIntensity = cumsum(double(binaryCol(xAxisLocation+2:end)).*double(currVelcotiyFrame(xAxisLocation+2:end,xTrackingLocationIndividual))); % 2 is a margin
            averIntensity = cumIntensity>repmat(cumIntensity(end,:)./2,[size(cumIntensity,1), 1]);
            [~, yTrackingLocationAvgIndividualNegative] = max(averIntensity);
            yTrackingLocationAvgIndividualNegative = xAxisLocation + yTrackingLocationAvgIndividualNegative;
            avgNegative = yTrackingLocationAvgIndividualNegative;
        end       

end

if  videoType == 2 % the video has a moving slider
    
    sumFrame = sum(currVelcotiyFrame);
    [~, sliderLocation] = min(sumFrame); % new idea....
    
    xTrackingLocationIndividual = sliderLocation-7; % a margin to make sure that image pixels intensity are not masked with the slider.
    if xTrackingLocationIndividual<1
        xTrackingLocationIndividual = 1;
    end
    binaryCol = im2bw(mat2gray(currVelcotiyFrame(:,xTrackingLocationIndividual)),graythresh(mat2gray(currVelcotiyFrame(:,xTrackingLocationIndividual))));
    [val , yTrackingLocationMaxIndividualPositive] = max(binaryCol(1:xAxisLocation-2));
    
    if val ~=0 % there is signal data
        maxPositive = yTrackingLocationMaxIndividualPositive;
        cumIntensity = cumsum(double(currVelcotiyFrame(1:xAxisLocation-2,xTrackingLocationIndividual))); % 2 is a margin to avoid counting the X-axis intensities
        averIntensity = cumIntensity >  cumIntensity(end)./2; %repmat(cumIntensity(end,:)./2,[size(cumIntensity,1), 1]);
        [~, yTrackingLocationAvgIndividual] = max(averIntensity);
        avgPositive = yTrackingLocationAvgIndividual; % store in the array
    end
    
    %% Negative part of the graph
    [val , yTrackingLocationMaxIndividualNegative] = max(flipud(binaryCol(xAxisLocation+2:end)));
    if val ~= 0 % there is signal information
        maxNegative = yHeight - yTrackingLocationMaxIndividualNegative+2; % store in the array
        cumIntensity = cumsum(double(binaryCol(xAxisLocation+2:end)).*double(currVelcotiyFrame(xAxisLocation+2:end,xTrackingLocationIndividual))); % 2 is a margin
        averIntensity = cumIntensity>repmat(cumIntensity(end,:)./2,[size(cumIntensity,1), 1]);
        [~, yTrackingLocationAvgIndividualNegative] = max(averIntensity);
        yTrackingLocationAvgIndividualNegative = xAxisLocation + yTrackingLocationAvgIndividualNegative;
        avgNegative = yTrackingLocationAvgIndividualNegative;
    end
%     [maxPositive(frameNum)  avgPositive(frameNum)  maxNegative(frameNum)  avgNegative(frameNum)  xTrackingLocationIndividual]
end

end

