function [miniTopWall, miniBotWall, kerUpHeight, kerBotHeight] = autoInitializer( ROI , numPoints)
% A function to automatically find the upper and lower artery wall
% initialization. The returned initializations have number of points equal
% to numPoints.
% This function is developed by Ahmed Gawish--University of Waterloo, on
% June 23, 2016

if numPoints > size(ROI,2) % the user asks for initialization that is wider than the width of the ROI
    numPoints = size(ROI,2);
end

topStrongWall = double(zeros(size(ROI,2),2));
botStrongWall = topStrongWall;
% initilaize
topStrongWall(:,1) = 1:size(ROI,2);
botStrongWall(:,1) = topStrongWall(:,1);

% define the derivative kernel
ker = ones(3, 9);
ker(2,:) = 0;
ker(3,:) = -1;

% first derivative image
Gmag = conv2(ROI,ker,'same')*-1;

% normalize
Gmag = Gmag./max(max(Gmag));
ROI  = ROI./max(max(ROI));
% combie both the intesity and the gradient images
ROI_totatl = 0.3*Gmag + 0.7*ROI;
ROI_linear = reshape(ROI_totatl,[size(ROI_totatl,1)*size(ROI_totatl,2) 1]);
[pksTotal,locsTotal] = findpeaks(double(ROI_linear));
sz = size(ROI,1);
wallThickUp = 0;
wallThickBot = 0;
numPointsAboveUpperWall = 0;
numPointsunderLowerWall = 0;
ROI_BW = im2bw(ROI,graythresh(ROI));

for i= 1:size(ROI,2)
    
    gyg = (locsTotal>=((i-1)*sz +1) & locsTotal<=((i-1)*sz +sz));
    pks = pksTotal(gyg) ;
    locs = locsTotal(gyg)- (i-1)*sz;
    upperLocs = locs(locs <= sz/2);
    upperPks = pks(locs <= sz/2);
    for p = length(upperPks):-1:1
        if pks(find(pks == max(upperPks),1,'first'))<0.1
            topStrongWall(i,2) = 0;
            break;
        elseif pks(find(pks == upperPks(p),1,'first')) > pks(find(pks == max(upperPks),1,'first'))*0.6
            topStrongWall(i,2) = locs(find(pks == upperPks(p),1,'last'));
            break;
        else
            topStrongWall(i,2) = locs(find(pks == max(upperPks),1,'last'));
        end
    end
    if topStrongWall(i,2)~=0
        wallThickUp = wallThickUp+ sum(ROI_BW(1:round(topStrongWall(i,2)),i)==1);
    else
        wallThickUp = wallThickUp+ 0;
    end
    numPointsAboveUpperWall = numPointsAboveUpperWall+ round(topStrongWall(i,2));
end

Gmag = Gmag*-1;
Gmag = Gmag./max(max(Gmag));
ROI  = ROI./max(max(ROI));
ROI_totatl = 0.5*Gmag + 0.5*ROI;
ROI_linear = reshape(ROI_totatl,[size(ROI_totatl,1)*size(ROI_totatl,2) 1]);
[pksTotal,locsTotal] = findpeaks(double(ROI_linear));

for i= 1:size(ROI,2)
    gyg = (locsTotal>=((i-1)*sz +1) & locsTotal<=((i-1)*sz +sz));
    pks = pksTotal(gyg) ;
    locs = locsTotal(gyg)- (i-1)*sz;
    lowerLocs = locs(locs > sz/2);
    lowerPks = pks(locs > sz/2);
    for p = 1:length(lowerPks)
        if pks(find(pks == max(lowerPks),1,'first'))<0.1
            botStrongWall(i,2) = 0;
            break;
        elseif pks(find(pks == lowerPks(p),1,'first')) > pks(find(pks == max(lowerPks),1,'first'))*0.6
            botStrongWall(i,2) = locs(find(pks == lowerPks(p),1,'last'));
            break;
        else
            botStrongWall(i,2) = locs(find(pks == max(lowerPks),1,'last'));
        end
    end
    if botStrongWall(i,2)~=0
        wallThickBot = wallThickBot+ sum(ROI_BW(round(botStrongWall(i,2)):end,i)==1);
    else
        wallThickBot = wallThickBot+ 0;
    end
    numPointsunderLowerWall = numPointsunderLowerWall+ round(sz - botStrongWall(i,2));
end
topStrongWall(topStrongWall(:,2)==0,:)=[];
botStrongWall(botStrongWall(:,2)==0,:)=[];

wallThickUp = wallThickUp/numPointsAboveUpperWall;
wallThickBot = wallThickBot/numPointsunderLowerWall;
[kerUpHeight, kerBotHeight] = setKerHeight(wallThickUp, wallThickBot);
safeMargin = floor(0.05*length(topStrongWall));%0.05; % the precentage left from thr right n left of the upper and lower walls before the function returns the points
if length(topStrongWall)>5 % enough points
    %     stepSize = ceil((length(topStrongWall)/(numPoints+1)));
    stepSize = (((length(topStrongWall)-2*safeMargin)/(numPoints-1)))-1;
    topStrongWall(:,2) = medfilt1(topStrongWall(:,2),ceil(length(topStrongWall)*0.1));
    %     topStrongWall(:,2) = smooth(topStrongWall(:,2),3,'moving');
    
    %     peaksAreaUp = medfilt1(peaksAreaUp,round(length(topStrongWall)*0.3));
    %     topStrongWall = classifyWallNoise(topStrongWall);
    %     stepSize = floor((length(topStrongWall) - 2*safeMargin*length(topStrongWall))/(numPoints-1));
    %     miniTopWall = topStrongWall(ceil(length(topStrongWall)*safeMargin):stepSize:ceil(length(topStrongWall)-length(topStrongWall)*safeMargin),:);
    miniTopWall = topStrongWall(ceil([1:numPoints]*stepSize-stepSize +safeMargin +[0:numPoints-1]), :);
else
    miniTopWall = [];
    disp('no data in the upper part');
end

safeMargin = floor(0.05*length(botStrongWall));%0.05; % the precentage left from thr right n left of the upper and lower walls before the function returns the points
if length(botStrongWall)>=5 % enough points
    %     stepSize = ceil((length(botStrongWall)/(numPoints+1)));
    stepSize = (((length(botStrongWall)-2*safeMargin)/(numPoints-1)))-1;
    botStrongWall(:,2) = medfilt1(botStrongWall(:,2),ceil(length(botStrongWall)*0.1));
    %     botStrongWall(:,2) = smooth(botStrongWall(:,2),3,'moving');
    %     peaksAreaBot = medfilt1(peaksAreaBot,round(length(topStrongWall)*0.3));
    %     botStrongWall = classifyWallNoise(botStrongWall);
    %% selecting only numPoints from both walls
    %     stepSize = floor((length(botStrongWall) - 2*safeMargin*length(botStrongWall))/(numPoints-1));
    %     miniBotWall = botStrongWall(round(length(botStrongWall)*safeMargin):stepSize:round(length(botStrongWall)-length(botStrongWall)*safeMargin),:);
    miniBotWall = botStrongWall(ceil([1:numPoints]*stepSize-stepSize +safeMargin +[0:numPoints-1]), :);
else
    miniBotWall = [];
    disp('no data in the lower part');
end

end