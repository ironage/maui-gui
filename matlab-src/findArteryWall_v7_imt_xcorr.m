function [vert_new_strong, vert_new_weak, signature ]  = findArteryWall_v7_imt_xcorr(smoothedFrame, firstGradientImg, secondGradient, oldVert, strongRange ,UpperOrLower, detectWeakArtery, signature)
% function [vert_new_strong, vert_new_weak ]  = findArteryWall_v3(currentFrame,oldVert, strongRange ,UpperOrLower, kernelSet)
%% A function that takes the segmentation of a certian artery wall in the previous frame and and returns the segmentation of the same wall in the current frame
% Parameters:
% currentFrame   : the current frame in which the artery wall needs to be
%                  detected/semented
% oldVert        : the segmentation of the artery wall in the previous image
% UpperOrLower   : Which type of artery wall needs to be detected. UpperOrLower is +1 if the wall is
%                  between a bright region at top and a dark region at bottom. UpperOrLower
%                  is -1 if the otherway. For example this wall 255 255 255 255 has UpperOrLower +1
%                                                             0   0   0   0
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
    searchRange4WeakWall = (3:30)*UpperOrLower; % search range for the weak wall
    weakRangeMin = min(searchRange4WeakWall);
    weakRangeMax = max(searchRange4WeakWall);
end
currSignature (:,1) = smoothedFrame(oldVert(1,2)-15:oldVert(1,2)+15, oldVert(1,1)); 
currSignature (:,2) = smoothedFrame(oldVert(end,2)-15:oldVert(end,2)+15, oldVert(end,1));
if UpperOrLower == 1
    [r,lags] = xcorr(signature(:,1),currSignature(:,1));
    signature(:,1) = currSignature(:,1);
    vertShiftFirst = lags(r ==max(r));
    [r,lags] = xcorr(signature(:,2),currSignature(:,2));
    signature(:,2) = currSignature(:,2);
    vertShiftLast = lags(r ==max(r));
    
    vert_new_strong = interpolateME1( [oldVert(1,1), oldVert(1,2)-(vertShiftFirst*UpperOrLower); oldVert(end,1), oldVert(end,2)-(vertShiftLast*UpperOrLower)] ,1);
imshow(smoothedFrame)
hold on
plot(vert_new_strong(:,1), vert_new_strong(:,2),'r'); 

else
   [r,lags] = xcorr(signature(:,3),currSignature(:,1));
    signature(:,3) = currSignature(:,1);
    vertShiftFirst = lags(r ==max(r));
    [r,lags] = xcorr(signature(:,4),currSignature(:,2));
    signature(:,4) = currSignature(:,2);
    vertShiftLast = lags(r ==max(r));
end


% plot(oldVert(1,1), oldVert(1,2),'.r');
% plot(oldVert(end,1), oldVert(end,2),'.r');
% 
% plot(oldVert(1,1), oldVert(1,2)-(vertShiftFirst*UpperOrLower),'.y');
% plot(oldVert(end,1), oldVert(end,2)-(vertShiftLast*UpperOrLower),'.y');
% pause();
% minAvailableRange = ceil(min(min(min(oldVert(:,2)), abs(imHeight-max(oldVert(:,2)))),30))-2; % Modified on April 24; as per Danielle feedback 
% subImageRange = -minAvailableRange:minAvailableRange; % To consider only a smaller part of the gradient and intensity images
% 
% sz = length(subImageRange);
% bigRangeMiddle = ceil(length(subImageRange)/2);
% firstGradientImg = UpperOrLower*(firstGradientImg);
% smallRangeMin = min(strongRange);   
% smallRangeMax = max(strongRange);
% 
% lin_idx = getAroundVert(originalVert, imHeight, minAvailableRange, minAvailableRange);
% avgIntensityValues = smoothedFrame(lin_idx);%reshape(smoothedFrame(lin_idx), 2*minAvailableRange+1,length(originalVert));
% avgDiffValues = firstGradientImg(lin_idx);%reshape(firstGradientImg(lin_idx), 2*minAvailableRange+1,length(originalVert));
% % avgDiffValues_final_linear = (0.7 .* avgDiffValues+ 0.3 .* avgIntensityValues);
% avgDiffValues_final_linear = (0.3 .* avgDiffValues+ 0.7 .* avgIntensityValues);
% 
% [pksTotal,locsTotal] = findpeaks(avgDiffValues_final_linear); % find peaks of avgDiffValues_final
% loopIndx =  2:2:length(originalVert) ; 
% % pksMatrix = zeros(sz,length(originalVert)); 
%  
% for i= loopIndx%2:2:length(originalVert)
%     gyg = (locsTotal>=((i-1)*sz +1) & locsTotal<=((i-1)*sz +sz));
%     pks = pksTotal(gyg) ;
%     locs = locsTotal(gyg)- (i-1)*sz;
%     [pks, t] = sort(pks,'descend');
%     locs = locs(t);
%     st = 0;
%     pksIndx = findSubsetPeaks(locs, st, bigRangeMiddle, smallRangeMin, smallRangeMax); % find the peaks around the strong wall
%     locs1 = locs(pksIndx);
%     locsDist = abs (locs1 -  bigRangeMiddle);
%     [~, closestMax] = min(locsDist);
%     if length(locs1)<1
%         st =    bigRangeMiddle;%ceil(length(subImageRange)/2);
%     elseif length(locs1)==1
%        st = locs1(1);
%     elseif pks(closestMax)>=0.2*pks(1) && pks(1)>0.3 % both peaks are strong and strongest peak is geater than 0.5
% %       st = (locs1(pks== min([pks(locs1== locs1(closestMax)), pks(locs1==locs1(1))]))*UpperOrLower)*UpperOrLower;%st = locs1(1);%   
%         st = max([locs1(closestMax), locs1(1)]);
% %       st = (min([locs1(closestMax), locs1(1)]*UpperOrLower))*UpperOrLower;%st = locs1(1);%ORIGINAL SELECTION
%         %     elseif pks(closestMax)>0.2*pks(1) && pks(closestMax)<0.5*pks(1)
%         %         st = locs1(1);%
%     else
%         st = locs1(closestMax);%st = (max(locs1*UpperOrLower))*UpperOrLower ;%st = (max([locs1(closestMax), locs1(1)]*UpperOrLower))*UpperOrLower ;%st = locs1(1);%
%         
%     end
%     strong(i) = subImageRange(st);
%     if (detectWeakArtery)
%         %% finding the weak wall
%         subPeakIndx = findSubsetPeaks(locs,strong(i), bigRangeMiddle, weakRangeMin, weakRangeMax);% finds the peaks around the weak wall
%         locs2 = locs(subPeakIndx);
%         pks2 = pks(subPeakIndx);
%         % find the closest peak to the strongest peak;
%         %         locsDist = abs (locs2 -  st);
%         %         [vv, closestMax] = min(locsDist);
%        % [~,iLocs,iLocs2] = intersect(locs,locs2); % common locations between locs and locs2
%         %locs2(pks(iLocs2)<0.1) = [];% remove weak peaks
%         locs2(pks2<0.05) = [];
%         pks2(pks2<0.05) = [];
%         %         IMTpks = pks(locs2);
%         
%         if length(locs2)<1 %|| vv >15 || vv<2 || pks(locs==locs2(closestMax))<(strong(i)*0.1)   % the weak point is far away from the strong edge by more than 10
%             weak(i) = 0;
%             continue
%         end
%         
%         %IMTpks(IMTpks<0.05)=[];
%         strongWallPeak = pks(find(locs == st,1,'first'));
%         for p = length(locs2): -1 : 1  
%             %if pks(find(locs == locs2(p),1,'last')) > strongWallPeak*0.2
%             if pks2(p) > strongWallPeak*0.2
%                 weak(i) = subImageRange(locs2(p));
%                 break;
%             else
%                 weak(i) = 0;%subImageRange(locs2(1));% subImageRange(locs2(closestMax));
%             end
%         end
%         %         pks = [];
%         %         locs = [];
%         weak(i-1) = weak(i);
%     end
%   %  strong(i-1) = strong(i);
% end
% 
% strong(loopIndx-1) = strong(loopIndx);
% strong(end)=strong(end-1);
% 
% % adding the new moves (strong/weak) to the old walls
% vert_new_strong(:,2) = vert_new_strong(:,2)+ strong' ;
% vert_new_strong(:,2) = smooth(vert_new_strong(:,2),0.4,'moving');
% if detectWeakArtery
%     weak(end)=weak(end-1);
%     vert_new_weak(:,2) = vert_new_strong(:,2)+ weak' ;
%    
%     indd = weak==0;
%     vert_new_weak(indd,:)= [];
%     weak(indd)= []; 
%     vert_new_weak(:,2) = smooth(vert_new_weak(:,2),0.4,'moving');
% end
% 
% 
% yMoves = abs(vert_new_strong(:,2) - oldVert(:,2));
% if mean(yMoves)> 0.1* imHeight
%     vert_new_strong(:,2) = oldVert(:,2);
% % else
% %     vert_new_strong = removeHigeVar(vert_new_strong, oldVert);
% end
% 
% if ~isempty(vert_new_weak)
%     vert_new_weak = makeParallel2StrongVert(vert_new_weak, vert_new_strong);
%     %     vert_new_weak = classifyWallNoise(vert_new_weak);
% end

end