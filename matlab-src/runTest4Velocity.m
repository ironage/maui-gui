function vert = runTest4Velocity()
close all; clc; warning off;
display = 1;

path=strcat('D:\Ahmed Gawish\PDF\MAUI\Ahmed\velocity videos\');
[FileName,PathName] = uigetfile('*.avi','select a file', 'D:\Ahmed Gawish\PDF\MAUI\Ahmed\Joel videos\');
videoFile = [PathName FileName];
vidObject = VideoReader(videoFile);
mov = read(vidObject);
for numIteration =1 :1
    wholeMaxPoisitive = 0;
    wholeAvgPoisitive = 0;
    wholeMaxNegative = 0;
    wholeAvgNegative = 0;
    trackingLocs = 0;
    numOfReturnedMeasures = 20;
    I_all = rgb2gray(mov(:,:,:,1));
    I_clean = single(I_all) / single(max(I_all(:)));
    
    %    [currentVelocityFrame, xMin, yMin, xWidth, yHeight] = getSubImg(I_clean);
    
    %% testing the consistency of the returned results
    if numIteration ==1
        [currentVelocityFrame, xMin, yMin, xWidth, yHeight] = getSubImg(I_clean);
        currentVelocityFrameO = currentVelocityFrame;
        xMinO = xMin;
        yMinO = yMin;
        xWidthO = xWidth;
        yHeightO = yHeight;
    end
    currentVelocityFrame = currentVelocityFrameO;
    xMin = xMinO;
    yMin = yMinO;
    xWidth = xWidthO;
    yHeight = yHeightO;
    %%
    
    previousMov = rgb2gray(mov(:,:,:,2));
    previousMov = single(previousMov)/single(max(previousMov(:)));
    previousVelocityFrame = previousMov;
    previousVelocityFrame = previousVelocityFrame(yMin:(yMin+yHeight) , xMin:(xMin+xWidth));
    numOfFrames = ceil(vidObject.FrameRate* vidObject.Duration);
    [xAxisLocation, videoType] = setup4Velocity(currentVelocityFrame, previousVelocityFrame);
    videoType
    firstMovingFrame =-1;
    ind = 2;
    while firstMovingFrame ==-1
        currMov = mov(:,:,:,ind);
        currentVelocityFrame = rgb2gray(currMov);
        currentVelocityFrame = currentVelocityFrame(yMin:(yMin+yHeight) , xMin:(xMin+xWidth));
        previousMov = mov(:,:,:,ind-1);
        previousVelocityFrame = rgb2gray(previousMov);
        previousVelocityFrame = previousVelocityFrame(yMin:(yMin+yHeight) , xMin:(xMin+xWidth));
        firstMovingFrame = check4FirstMovingFrame(currentVelocityFrame,previousVelocityFrame, ind);
        ind = ind+1;
    end
    xTrackingLocationIndividual = 1;
    close all
    vert = zeros(numOfFrames, 4);
    timeVector = 0;
    for i = 1 : numOfFrames
        
        currMov = mov(:,:,:,i);
        currentVelocityFrame = rgb2gray(currMov);
        currentVelocityFrame = currentVelocityFrame(yMin:(yMin+yHeight) , xMin:(xMin+xWidth));
        %     previousMov = mov(:,:,:,i-1);
        %     previousVelocityFrame = rgb2gray(previousMov);
        %     previousVelocityFrame = previousVelocityFrame(yMin:(yMin+yHeight) , xMin:(xMin+xWidth));
%         if i == 350
%              display=1;
%         end
        
        [maxPositive, avgPositive, maxNegative, avgNegative, xTrackingLocationIndividual] = processVelocityIntervals(currentVelocityFrame, i, xAxisLocation, videoType, firstMovingFrame, xTrackingLocationIndividual);
        
        %         [ i length(xTrackingLocationIndividual)]
        %         timeVector = [timeVector (i-1)+[1/length(maxPositive):1/length(maxPositive):1]];
        timeVector = [timeVector [i/vidObject.FrameRate i/vidObject.FrameRate+cumsum(ones(1,numOfReturnedMeasures-1)*(1/(numOfReturnedMeasures*vidObject.FrameRate)))]];
        f1 = figure(1); hold on
        if display
            imshow(currentVelocityFrame);
            hold on
            %         if xTrackingLocationIndividual == -1 %stationary graph
            %             tempX_axis = cumsum(ones(1,numOfFrames)*(size(currentVelocityFrame, 2)/(numOfFrames)));
            %             plot(round(tempX_axis), maxPositive, '-o r', 'LineWidth',5,'MarkerSize',2);
            %             plot(round(tempX_axis), avgPositive, '-o y', 'LineWidth',5,'MarkerSize',2);
            %             plot(round(tempX_axis), maxNegative, '-o b', 'LineWidth',5,'MarkerSize',2);
            %             plot(round(tempX_axis), avgNegative, '-o g', 'LineWidth',5,'MarkerSize',2);
            %         else
            
            plot( xTrackingLocationIndividual, maxPositive, '*r','LineWidth',5, 'MarkerSize',1);
            plot( xTrackingLocationIndividual, avgPositive, '*b','LineWidth',5, 'MarkerSize',1);
            plot( xTrackingLocationIndividual, maxNegative, '*c','LineWidth',5, 'MarkerSize',1);
            plot( xTrackingLocationIndividual, avgNegative, '*y','LineWidth',5, 'MarkerSize',1);
            %         saveas(f1,strcat('D:\Ahmed Gawish\PDF\MAUI\Ahmed\velocity videos\joel\Mikel-Trimmed D-U Videos for Analysis (3.1)\','ES3.1_30%_No2_avi.avi trimmed\0', num2str(i),'.png'));
                        pause();
            if(mod(i,5)==0)
                delete(get(gca,'Children'));
            end
            %             close all;
            %         end
            
        end
        % %     maxNegative
        wholeMaxPoisitive = [wholeMaxPoisitive  maxPositive];
        wholeAvgPoisitive = [wholeAvgPoisitive avgPositive];
        wholeMaxNegative = [wholeMaxNegative maxNegative];
        wholeAvgNegative = [wholeAvgNegative avgNegative];
        trackingLocs = [trackingLocs xTrackingLocationIndividual];
        %     vert(i,1) = maxPositive;
        %     vert(i,2) = avgPositive;
        %     vert(i,3) = maxNegative;
        %     vert(i,4) = avgNegative;
    end
    wholeMaxPoisitive = xAxisLocation - wholeMaxPoisitive;
    wholeAvgPoisitive = xAxisLocation - wholeAvgPoisitive;
    wholeAvgNegative = xAxisLocation - wholeAvgNegative;
    wholeMaxNegative = xAxisLocation - wholeMaxNegative;
    %% Calibration step
    % this calibration factor is for Joel and Mikel data
    % 69 pixels = 100 cm/s   that is    1 pixel = (100/69) cm/s
    wholeMaxPoisitive = wholeMaxPoisitive * (100/69)*-1;
%     wholeAvgPoisitive = wholeAvgPoisitive * (100/69)*-1;
%     wholeMaxNegative = wholeMaxNegative * (100/69)*-1;
%     wholeAvgNegative = wholeAvgNegative * (100/69)*-1;
    
    length(wholeMaxPoisitive)
    %     pause();
end
% figure, plot(wholeMaxPoisitive);
% hold on
% plot(wholeAvgPoisitive+wholeAvgNegative);
% % plot(wholeAvgNegative);
% plot(wholeMaxNegative);
%
% %figure, plot(trackingLocs,'.r')
% wholeData(:,1) = timeVector;
% wholeData(:,2) = wholeMaxPoisitive ;
% wholeData(:,3) = wholeAvgPoisitive+wholeAvgNegative ;
% wholeData(:,4) = wholeMaxNegative;
% filename = 'ES3.1_Baseline.avi trimmed.xlsx';
% xlswrite(filename,wholeData);

