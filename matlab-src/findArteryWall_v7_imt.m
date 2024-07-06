function [vert_new_strong, vert_new_weak ]  = findArteryWall_v7_imt(smoothedFrame, firstGradientImg, ~, oldVert, strongRange ,UpperOrLower, detectWeakArtery,distBtwMediaWalls)
% function [vert_new_strong, vert_new_weak ]  = findArteryWall_v3(currentFrame,oldVert, strongRange ,UpperOrLower, kernelSet)
%% A function that takes the segmentation of a certian artery wall in the previous frame and and returns the segmentation of the same wall in the current frame
% Parameters:
% currentFrame   : the current frame in which the artery wall needs to be
%                  detected/semented
% oldVert        : the segmentation of the artery wall in the previous image
% UpperOrLower   : Which type of artery wall needs to be detected. UpperOrLower is +1 if the wall is
%                  between a bright region at top and a dark region at bottom. UpperOrLower
%                  is -1 if the otherway. For example this wall 255 255 255 255 has UpperOrLower +1
%                                                                0   0   0   0
% vert_new_strong: the segmentation of the strong artery wall in the current frame
% vert_new_weak  : the segmentation of the weak artery wall in the current frame
% This function is developed by Ahmed Gawish Nov. 2015
% Last Modified April 8th 2016

imWidth = size(smoothedFrame,2);
imHeight = size(smoothedFrame,1);

oldVert = check4BoundaryCollision(oldVert, imHeight, imWidth);

originalVert = oldVert;
vert_new_strong = originalVert;
vert_new_weak = originalVert; % to store weak wall coordinates
strong = double(zeros(1,length(vert_new_strong)));

if detectWeakArtery
    weak = double(zeros(1,length(vert_new_weak)));
    searchRange4WeakWall = (3:50)*UpperOrLower; % search range for the weak wall
%     searchRange4WeakWall = (3: (uint8(distBtwMediaWalls)* 0.4))* UpperOrLower;
    weakRangeMin = min(searchRange4WeakWall);
    weakRangeMax = max(searchRange4WeakWall);
end

minAvailableRange = ceil(min(min(min(oldVert(:,2)), abs(imHeight-max(oldVert(:,2)))),30))-2; % Modified on April 24; as per Danielle feedback 
subImageRange = -minAvailableRange:minAvailableRange; % To consider only a smaller part of the gradient and intensity images

sz = length(subImageRange);
bigRangeMiddle = ceil(length(subImageRange)/2);
firstGradientImg = UpperOrLower*(firstGradientImg);
smallRangeMin = min(strongRange);
smallRangeMax = max(strongRange);

lin_idx = getAroundVert(originalVert, imHeight, minAvailableRange, minAvailableRange);
avgIntensityValues = smoothedFrame(lin_idx);%reshape(smoothedFrame(lin_idx), 2*minAvailableRange+1,length(originalVert));
avgDiffValues = firstGradientImg(lin_idx);%reshape(firstGradientImg(lin_idx), 2*minAvailableRange+1,length(originalVert));
% avgDiffValues_final_linear = (0.7 .* avgDiffValues+ 0.3 .* avgIntensityValues);
avgDiffValues_final_linear = (0.8 .* avgDiffValues+ 0.2 .* avgIntensityValues);

[pksTotal,locsTotal] = findpeaks(avgDiffValues_final_linear); % find peaks of avgDiffValues_final
loopIndx =  2:2:length(originalVert) ; 
% pksMatrix = zeros(sz,length(originalVert)); 
 
for i= loopIndx%2:2:length(originalVert)
    gyg = (locsTotal>=((i-1)*sz +1) & locsTotal<=((i-1)*sz +sz));
    pks = pksTotal(gyg) ;
    locs = locsTotal(gyg)- (i-1)*sz;
    [pks, t] = sort(pks,'descend');
    locs = locs(t);
    st = 0;
    pksIndx = findSubsetPeaks(locs, st, bigRangeMiddle, smallRangeMin, smallRangeMax); % find the peaks around the strong wall
    locs1 = locs(pksIndx);
    locsDist = abs (locs1 -  bigRangeMiddle);
    [~, closestMax] = min(locsDist);
    if isempty(locs1)
        st =    bigRangeMiddle;%ceil(length(subImageRange)/2);
    elseif length(locs1)==1
       st = locs1(1);
    elseif pks(closestMax)>=0.2*pks(1) && pks(1)>0.3 % both peaks are strong and strongest peak is geater than 0.5
%       st = (locs1(pks== min([pks(locs1== locs1(closestMax)), pks(locs1==locs1(1))]))*UpperOrLower)*UpperOrLower;%st = locs1(1);%   
        if UpperOrLower==1
            st = min([locs1(closestMax), locs1(1)]);
        else
            st = max([locs1(closestMax), locs1(1)]);
        end
%       st = (min([locs1(closestMax), locs1(1)]*UpperOrLower))*UpperOrLower;%st = locs1(1);%ORIGINAL SELECTION
        %     elseif pks(closestMax)>0.2*pks(1) && pks(closestMax)<0.5*pks(1)
        %         st = locs1(1);%
    else
        st = locs1(closestMax);%st = (max(locs1*UpperOrLower))*UpperOrLower ;%st = (max([locs1(closestMax), locs1(1)]*UpperOrLower))*UpperOrLower ;%st = locs1(1);%
        
    end
    strong(i) = subImageRange(st);
    if (detectWeakArtery)
        %% finding the weak wall
        subPeakIndx = findSubsetPeaks(locs,strong(i), bigRangeMiddle, weakRangeMin, weakRangeMax);% finds the peaks around the strong wall
        locs2 = locs(subPeakIndx);
        pks2 = pks(subPeakIndx);
        % find the closest peak to the strongest peak;
        %         locsDist = abs (locs2 -  st);
        %         [vv, closestMax] = min(locsDist);
       % [~,iLocs,iLocs2] = intersect(locs,locs2); % common locations between locs and locs2
        %locs2(pks(iLocs2)<0.1) = [];% remove weak peaks
        locs2(pks2<0.08) = [];
        pks2(pks2<0.08) = [];
        %         IMTpks = pks(locs2);
        
        if isempty(locs2) %|| vv >15 || vv<2 || pks(locs==locs2(closestMax))<(strong(i)*0.1)   % the weak point is far away from the strong edge by more than 10
            weak(i) = 0;
            continue
        end
        
        %IMTpks(IMTpks<0.05)=[];
        strongWallPeak = pks(find(locs == st,1,'first'));
       [~, maxWeakWallIndx] = max(pks2);
%         for p = length(locs2): -1 : 1  
            %if pks(find(locs == locs2(p),1,'last')) > strongWallPeak*0.2
            if pks2(maxWeakWallIndx) > strongWallPeak*0.2
                weak(i) = subImageRange(locs2(maxWeakWallIndx));
                %weak(i) = subImageRange(locs2(p));
%                 break;
            else
                weak(i) = 0;%subImageRange(locs2(1));% subImageRange(locs2(closestMax));
            end
%         end
        %         pks = [];
        %         locs = [];
        weak(i-1) = weak(i);
    end
  %  strong(i-1) = strong(i);
end

strong(loopIndx-1) = strong(loopIndx);
strong(end)=strong(end-1);

% adding the new moves (strong/weak) to the old walls
vert_new_strong(:,2) = vert_new_strong(:,2)+ strong' ;
vert_new_strong(:,2) = smooth(vert_new_strong(:,2),0.4,'moving');
if detectWeakArtery
    weak(end)=weak(end-1);
    vert_new_weak(:,2) = vert_new_strong(:,2)+ weak' ;
   
    indd = weak==0;
    vert_new_weak(indd,:)= [];
    weak(indd)= []; 
    vert_new_weak(:,2) = smooth(vert_new_weak(:,2),0.4,'moving');
end


yMoves = abs(vert_new_strong(:,2) - oldVert(:,2));
if mean(yMoves)> 0.1* imHeight
    vert_new_strong(:,2) = oldVert(:,2);
% else
%     vert_new_strong = removeHigeVar(vert_new_strong, oldVert);
end

if ~isempty(vert_new_weak)
%     vert_new_weak = classifyWallNoise(vert_new_weak);
    vert_new_weak = makeParallel2StrongVert(vert_new_weak, vert_new_strong);
end
%% another way to calculate the weak wall is to constantly move the whole detected strong wall down (in case of the upper wall)
% or up (in case of the lower wall) in order to maximiza/minimuize a
% certian cost function



% if (detectWeakArtery == 1)
%     strongIntensity = getAroundVert(vert_new_strong, size(smoothedFrame,1), 0, 0);
%     strongIntensitySubImg = reshape(smoothedFrame(strongIntensity), 1,length(vert_new_strong));
%     if(UpperOrLower == 1) % upper wall
%         margin = 20;
%         vert_new_strong_shifted = vert_new_strong;
%         vert_new_strong_shifted(:,2) = vert_new_strong_shifted(:,2)+2;
%         vert_new_weak = vert_new_strong;
%         weakWallIntensityIndx = getAroundVert(vert_new_strong_shifted, size(smoothedFrame,1), 0, margin);
%         intensitySubImg = reshape(smoothedFrame(weakWallIntensityIndx), margin+1,length(vert_new_strong));
%         gradientSubImg = reshape(firstGradientImg(weakWallIntensityIndx), margin+1,length(vert_new_strong));
%         combinedSubImg = 0.3 * intensitySubImg + 0.7 * gradientSubImg;
%         [~, moves] = max(combinedSubImg);
%         linearInd = sub2ind(size(intensitySubImg), moves, [1:size(intensitySubImg,2)]);
%         intensityLine = intensitySubImg(linearInd);
%         vert_new_weak(:,2) = vert_new_strong(:,2)+ moves'+2; % just push it a little bit further; 3
%         % remove outliers
%         vert_new_weak(intensityLine<0.1*strongIntensitySubImg,:) = [];
%         
%     elseif (UpperOrLower == -1)% lower wall
%         margin = 20;
%         vert_new_strong_shifted = vert_new_strong;
%         vert_new_strong_shifted(:,2) = vert_new_strong_shifted(:,2)-2;
%         vert_new_weak = vert_new_strong;
%         weakWallIntensityIndx = getAroundVert(vert_new_strong_shifted, size(smoothedFrame,1), margin, 0 );
%         intensitySubImg = reshape(smoothedFrame(weakWallIntensityIndx), margin+1,length(vert_new_strong));
%         gradientSubImg = reshape(firstGradientImg(weakWallIntensityIndx), margin+1,length(vert_new_strong));
%         combinedSubImg = 0.3 * intensitySubImg + 0.7 * gradientSubImg;
%         combinedSubImg = flipud(combinedSubImg);
%         intensitySubImg = flipud(intensitySubImg);
%         [~, moves] = max(combinedSubImg);
%         linearInd = sub2ind(size(intensitySubImg), moves, [1:size(intensitySubImg,2)]);
%         intensityLine = intensitySubImg(linearInd);
%         vert_new_weak(:,2) = vert_new_strong(:,2)- moves'-2; % just push it a little bit further; 3
%         % remove outliers
%         vert_new_weak(intensityLine<0.1*strongIntensitySubImg,:) = [];
%     end
%  
%     if ~isempty(vert_new_weak)
%         vert_new_weak = makeParallel2StrongVert(vert_new_weak, vert_new_strong);
%         %     vert_new_weak = classifyWallNoise(vert_new_weak);
%     end
% end
end