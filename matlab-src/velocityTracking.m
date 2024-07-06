for videoCase = [020];% 03 04 05 06 07 08 09 010 011 012]
    close all; clear all; clc; warning off;
    %     videoFile =['D:\Ahmed Gawish\PDF\MAUI\Ahmed\video0' num2str(videoCase) '.AVI']; %'D:\Ahmed Gawish\PDF\MAUI\Ahmed\problem\Michelle.avi';%
    path=strcat('D:\Ahmed Gawish\PDF\MAUI\Ahmed\velocity videos\');
    videoType = 0;
    % vid_writer = VideoWriter(strcat(path,'newfile1.mp4'),'Motion JPEG AVI');
    % open(vid_writer);
    [FileName,PathName] = uigetfile('*.avi','select a file', 'D:\Ahmed Gawish\PDF\MAUI\Ahmed\velocity videos\Kathryn Nov 17 -2016\bamca-pilot-02BAMCA-PILOT-02\VAS20160805101347');
    % videoFile = 'D:\Ahmed Gawish\PDF\MAUI\Ahmed\velocity videos\Kathryn Nov 17 -2016\bamca-pilot-02BAMCA-PILOT-02\VAS20160805101347\201608051124370021VAS.AVI';
    videoFile = [PathName FileName];
    vidObject = VideoReader(videoFile);
    mov = read(vidObject);
    I_all = rgb2gray(mov(:,:,:,1));
    I_clean = single(I_all) / single(max(I_all(:)));
    [I_clean_mini, xMin, yMin, xWidth, yHeight] = getSubImg(I_clean);
    I_clean_mini = I_clean(yMin:yMin+yHeight, xMin:xMin+xWidth);
    imshow(I_clean_mini,[])
    frameNumber = ceil(vidObject.FrameRate* vidObject.Duration);
    
    yPlotPositive = ones(1,frameNumber);
    yPlotNegative = ones(1,frameNumber);
    yPlotAvgPositive = ones(1,frameNumber);
    yPlotAvgNegative = ones(1,frameNumber);
    xPlot = ones(1,frameNumber);
    %%
    % determine the zero line (x-axis)
    tempImg = I_clean_mini(: , round(0.2*size(I_clean_mini,2)): end-round(0.2*size(I_clean_mini,2))); % ignore %20 of the columns from the begining and ending of the image to make sure less contribution of black pixels
    %     sumRows = sum(I_clean_mini, 2);
    %     sumRows = sumRows./max(sumRows(:)); % normalization
    %     sumRows = 1-sumRows;
    varRows = var(tempImg, 0, 2);
    
    %     varRows = varRows./max(varRows(:)); % normalization
    
    bw = tempImg==0;
    blackCount = sum(bw, 2);
    toRemove = blackCount> 0.50 * size(bw,2);
    varRows(toRemove) = NaN;
    xAxisLocation = find(varRows == min(varRows), 1, 'first');
    % remove columns with large number of zeros
    xAxisIntensity = I_clean_mini(xAxisLocation, :);
    
    %     axisRow(axisRow == 0) = NaN;
    xDiff = abs(diff(xAxisIntensity));
    xDiff = xDiff/max(xDiff(:));
    
    % exactLocs = find(xDiff>0.5, 2, 'first');
    % ASSUMING THAT THE USER CAREFULLY SELECTS THE AREA OF INTEREST TO NOT
    % INCLUDE ANY IRRELVANT DATA (e.g. THE AXES)
    exactLocs = [1 size(I_clean_mini,2)];
    hold on;
    %     plot([1: size(I_clean_mini,2)], xAxisIndx, '.r');
%     plot(exactLocs, xAxisLocation,  '*y');
    %% determine the exact locations of the begining and end of the x-axis
    
    %     pause();
    close all;
    %     f1 = figure(1); hold on
    firstTime = true;
    for frameNum = 2:frameNumber-1
        frameNum
        currMov = mov(:,:,:,frameNum);
        prevMov = mov(:,:,:,frameNum-1);
        currFrame = rgb2gray(currMov);
        prevFrame = rgb2gray(prevMov);
%         currFrame = currFrame(yMin:(yMin+yHeight-(yHeight-xAxisLocation)), xMin:(xMin+xWidth));
%         prevFrame = prevFrame(yMin:(yMin+yHeight-(yHeight-xAxisLocation)), xMin:(xMin+xWidth));
        currFrame = currFrame(yMin:(yMin+yHeight) , xMin:(xMin+xWidth));
        prevFrame = prevFrame(yMin:(yMin+yHeight) , xMin:(xMin+xWidth));
        
        %         imgDiff = abs(currFrame(: ,exactLocs(2)-10: exactLocs(2)-5 ));
        imgDiff = abs(currFrame - prevFrame);
        
        imgDiff = im2bw(mat2gray(imgDiff), graythresh(mat2gray(imgDiff)));
        sliderOrWholeFrame = sum(imgDiff(:));
        imshow(currFrame);
        %         figure, imshow(subImg);
        hold on
        subImg = currFrame(: ,exactLocs(1): exactLocs(2) );
        xTrackingLocation = [1: size(subImg,2)];
        if videoType == 1 || sliderOrWholeFrame > 0.01 * (size(imgDiff,1)*size(imgDiff,2))||sliderOrWholeFrame < 10  % The whole graph moves
%             subImg = currFrame(: ,exactLocs(1): exactLocs(2) );
%             xTrackingLocation = [1: size(subImg,2)];
            videoType = 1;
            if  sliderOrWholeFrame < 10 % no movement -- at the begining of the video
                if firstTime == true  % all data here needs to be processed only one time
                    BW = im2bw(subImg, graythresh(subImg));
                    BW = bwareaopen(BW,20); %remove small noise objects (e.g the ticks on the axis)
                    E = bwmorph(edge(BW), 'bridge');
                    [ ~, yTrackingLocationMaxWholePostive] = max(BW(1:xAxisLocation-2,:)); % Also coud be max(BW);
                    cumIntensity = cumsum(double(BW(1:xAxisLocation-2,:)).*double(subImg(1:xAxisLocation-2,:))); % 5 is a margin
                    averIntensity = cumIntensity>repmat(cumIntensity(end,:)./2,[size(cumIntensity,1), 1]);
                    [~, yTrackingLocationAvgWholePostive] = max(averIntensity);
                    % Negative velocity
                    [ ~, yTrackingLocationMaxWholeNegative] = max(flipud(BW(xAxisLocation+2:end,:)));% flip the lower part of the image
                    % (i.e. underneath the x-axis) to easily use max function, and then do the math to accuratly determin the exact location 
                    yTrackingLocationMaxWholeNegative = (size(BW,1)-xAxisLocation+2) - yTrackingLocationMaxWholeNegative;
                    yTrackingLocationMaxWholeNegative = xAxisLocation+yTrackingLocationMaxWholeNegative;
                    cumIntensity = cumsum(double(BW(xAxisLocation+2:end,:)).*double(subImg(xAxisLocation+2:end,:))); % 5 is a margin
                    averIntensity = cumIntensity>repmat(cumIntensity(end,:)./2,[size(cumIntensity,1), 1]);
                    [~, yTrackingLocationAvgWholeNegative] = max(averIntensity);
                    yTrackingLocationAvgWholeNegative = xAxisLocation+yTrackingLocationAvgWholeNegative;
                    
                end
                firstTime = false;
                % calculate the average intensity.
                %                 plot(xTrackingLocation, yTrackingLocationMaxWhole, '.r', 'linewidth', 2);
                %                 plot(xTrackingLocation, yTrackingLocationAvgWhole, '.g', 'linewidth', 2);
                
            else % the graph started to move
                %sampling the stationary results
                tempArr = cumsum(ones(1,frameNum-1)*(length(yTrackingLocationMaxWhole)/(frameNum-1)));
                yPlotPositive(1:frameNum-1) = yTrackingLocationMaxWhole(round(tempArr));
                yPlotAvgPositive(1:frameNum-1) = yTrackingLocationAvgWhole(round(tempArr));
                
                % claculate Max intensity at a particular point (at the very left of the moving curve).
                xTrackingLocationIndividual = find(sum(imgDiff)~=0, 1 , 'last') - 5; % little margin to avoid the vertical axis at the right side
                [~ , yTrackingLocationMaxIndividual] = max(im2bw(mat2gray(currFrame(:,xTrackingLocationIndividual)),graythresh(mat2gray(currFrame(:,xTrackingLocationIndividual)))));
                yPlotPositive(frameNum) = yTrackingLocationMaxIndividual; % store in the array
                % claculate average intensity value at the same location (at the very left of the moving curve).
                %                 BW = im2bw(mat2gray(currFrame(:,xTrackingLocationIndividual)),graythresh(mat2gray(currFrame(:,xTrackingLocationIndividual))));
                %                 cumIntensity = cumsum(double(BW(1:xAxisLocation-5,:)).*double(subImg(1:xAxisLocation-5,xTrackingLocationIndividual))); % 5 is a margin
                cumIntensity = cumsum(double(subImg(1:xAxisLocation-5,xTrackingLocationIndividual))); % 5 is a margin to avoid counting the X-axis intensities
                averIntensity = cumIntensity >  cumIntensity(end)./2; %repmat(cumIntensity(end,:)./2,[size(cumIntensity,1), 1]);
                [val, yTrackingLocationAvgIndividual] = max(averIntensity);
                yPlotAvgPositive(frameNum) = yTrackingLocationAvgIndividual; % store in the array
                %                 plot(xTrackingLocationIndividual, yTrackingLocationAvgIndividual, '.g', 'linewidth', 2);
                %                 plot(xTrackingLocation, yTrackingLocationMaxIndividual, '.r');
            end
        elseif  sliderOrWholeFrame <= 0.01 * (size(imgDiff,1)*size(imgDiff,2)) || sliderOrWholeFrame >10 ||videoType == 2  % the video has a moving slider
            videoType = 2;
            CC = bwconncomp(imgDiff);
            numPixels = cellfun(@numel,CC.PixelIdxList);
            [biggest,idx] = max(numPixels);
            if ~isempty(idx)
                
                imgDiff = imgDiff.*0;
                imgDiff(CC.PixelIdxList{idx}) = 1;
                [x, y] = find(imgDiff==1);%ind2sub(size(imgDiff),CC.PixelIdxList{idx});
                % Find max intensity
                yPlotPositive(frameNum) = min(x);
                xPlot(frameNum) = y(find(x == yPlotPositive(frameNum),1));
                
                % Find average intensity  
                % One challeng here is that in the current frame the slider
                % location in the image is totally black which allows us to
                % calculate the max intensity (which is simply the peak of
                % the black region) but not the average. Hence, average intensity
                % for the current frame will be calculated from the previous
                % frame using the xTrackingLocationIndividualPrevious variable
                
                xTrackingLocationIndividual =  xPlot(frameNum);
                xTrackingLocationIndividualPrevious =  xPlot(frameNum-1);
                cumIntensity = cumsum(double(subImg(1:xAxisLocation-5,xTrackingLocationIndividualPrevious))); % 5 is a margin to avoid counting the X-axis intensities
                averIntensity = cumIntensity >  cumIntensity(end)./2; %repmat(cumIntensity(end,:)./2,[size(cumIntensity,1), 1]);
                [val, yTrackingLocationAvgIndividual] = max(averIntensity);
                yPlotAvgPositive(frameNum) = yTrackingLocationAvgIndividual; % store in the array
                yPlotAvgPositive(frameNum-1) = yPlotAvgPositive(frameNum);
                %plot(xPlot(frameNum) , yPlot(frameNum), 'r');
                
            else % For some reason, no moving slider can be detected
                yPlotPositive(frameNum) = yPlotPositive(frameNum-1);
                xPlot(frameNum) = xPlot(frameNum-1);
                yPlotAvgPositive(frameNum) = yPlotAvgPositive(frameNum-1);
            end
        end
        
        %         saveas(f1,strcat(path,'plots\0',num2str(frameNum),'.png'));
        %                 pause();
        %                 if(mod(frameNum,5)==0)
        %                     delete(get(gca,'Children'));
        %                 end
        %         close all;
        %             elseif ch == false %% empty
        %                 ch = true;
        %
    plot(xTrackingLocationIndividual,yPlotPositive(frameNum),'-r');
    hold on
    plot(xTrackingLocationIndividual,yPlotAvgPositive(frameNum),'-g');
    pause(0.1);
    end
    figure, plot([1:frameNumber],max(yPlotPositive(:))-yPlotPositive);
    hold on
    plot([1:frameNumber],max(yPlotPositive(:))-yPlotAvgPositive);
end