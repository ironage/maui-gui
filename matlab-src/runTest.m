function runTest()
    close all; clc; warning off;

path=strcat('C:\AHMED GAWISH\');
[FileName,PathName] = uigetfile({'*.avi'; '*.bmp'},'select a file', 'C:\AHMED GAWISH\');

videoFile = [PathName FileName];
isVideo = false;
if ~isempty(strfind(upper(FileName), 'AVI'))
    vidObject = VideoReader(videoFile);
    mov = read(vidObject,1); 
    frameNumber = ceil(vidObject.FrameRate* vidObject.Duration);
    isVideo = true;

else
    mov = imread(videoFile);
    frameNumber = 30;
end
%     profile clear;
%     profile on; 
%     
    dispFig = 1;
    I_all = rgb2gray(mov);
%     I_all = rgb2gray(mov(:,:,:,1));
    I_clean = single(I_all) / single(max(I_all(:)));
    [I_clean_mini, xMin, yMin, xWidth, yHeight] = getSubImg(I_clean);
%     xMin= 220;
%     yMin= 134;
%     xWidth= 120;
%     yHeight = 144;
%     xMin= 391;
%     yMin= 207;
%     xWidth= 180;
%     yHeight = 226;
    I_clean_mini = I_clean(yMin:yMin+yHeight, xMin:xMin+xWidth);
    
    [topInit, botInit] = autoInitializer( I_clean_mini , 5);
    topStrongPoints = topInit;
    botStrongPoints = botInit;
    imshow(I_clean_mini,[])
    hold on
    plot(topStrongPoints(:,1), topStrongPoints(:,2), '.c');
    plot(botStrongPoints(:,1), botStrongPoints(:,2), '.c');
    pause();
    close all;
%     I_clean = I_clean_mini;
%     imshow(I_clean,[])
    tic
    [smoothKernel, derivateKernel, topStrongLine, botStrongLine, topRefWall, botRefWall] =  setup(topStrongPoints, botStrongPoints);
     
    %     [derivateKernel, ~] = refWallKernel(I_clean, topStrongPoints, botStrongPoints);
    comparison = double(zeros(3, frameNumber-1));
    for frameNum = 1:frameNumber-1
        
        if isVideo
            mov = read(vidObject,frameNum);
        else
            
        end
%         frame = rgb2gray(mov(:,:,:,frameNum));
        frame = rgb2gray(mov);
        
        %          frame = frame(rMin:rMax,cMin:cMax,:);
        frame = frame(yMin:(yMin+yHeight), xMin:(xMin+xWidth));
%         [smoothedFrame, firstGradient, ~] = getImages(frame, derivateKernel, smoothKernel);
%         flow = estimateFlow(opticFlow,frame);

   
        [topStrongLine, botStrongLine, OLD, topWeakLine, topIMT, botWeakLine, botIMT, topRefWall, botRefWall] ...
            = update(frame, smoothKernel, derivateKernel, topStrongLine, botStrongLine, interpolateME1(topStrongPoints,1), interpolateME1(botStrongPoints,1), topRefWall, botRefWall);
            
%         comparison(1, frameNum) = OLD;
%         comparison(2, frameNum) = topIMT;
%         comparison(3, frameNum) = botIMT;

        if(dispFig)
            if(mod(frameNum,1)==0)     % display updated contours every 3rd frame
                imshow(frame); hold on;
                title(['frame number = ' num2str(frameNum) ' / ' num2str(frameNumber)]);
                plot(topStrongLine(:,1),topStrongLine(:,2), '.g', 'linewidth', 2);
                plot(botStrongLine(:,1),botStrongLine(:,2), '.g', 'linewidth', 2);
                plot(topWeakLine(:,1),topWeakLine(:,2), '.r', 'linewidth', 2);
                plot(botWeakLine(:,1),botWeakLine(:,2), '.r', 'linewidth', 2);
%                 plot(flow,'DecimationFactor',[5 5],'ScaleFactor',10)
                
%                 plot(topWallRef(:,1),topWallRef(:,2), 'r');
%                 plot(botWallRef(:,1),botWallRef(:,2), 'r');
                %
                %                             plot(topStrongPoints(:,1),topStrongPoints(:,2), '.g', 'linewidth', 2);
                %                             plot(botStrongPoints(:,1),botStrongPoints(:,2), '.g', 'linewidth', 2);
                pause(0.00001);
                if(mod(frameNum,20)==0)
                    delete(get(gca,'Children'));
                end
            end
        end
        
    end
%     subplot(311), plot( comparison(1,:), 'r')
%     subplot(312), plot( comparison(2,:), 'b')
%     subplot(313), plot( comparison(3,:), 'b')
topStrongLine(:,1) = topStrongLine(:,1)+double(xMin);
topStrongLine(:,2) = topStrongLine(:,2)+double(yMin);
botStrongLine(:,1) = botStrongLine(:,1)+double(xMin);
botStrongLine(:,2) = botStrongLine(:,2)+double(yMin);

dlmwrite('top3.txt', topStrongLine);
dlmwrite('bottom3.txt', botStrongLine);

toc
% profile viewer