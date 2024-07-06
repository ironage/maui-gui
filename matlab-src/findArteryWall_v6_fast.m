function [vert_new_strong, vert_new_weak ]  = findArteryWall_v6_fast(smoothedFrame, firstGradientImg, secondGradientImg, oldVert, strongRange ,UpperOrLower)
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


oldVert = check4BoundaryCollision(oldVert , size(smoothedFrame,1), size(smoothedFrame,2));

originalVert = oldVert;
vert_new_strong = originalVert;
vert_new_weak = originalVert; % to store weak wall coordinates
strong = double(zeros(1,length(vert_new_strong)));

weak = double(zeros(1,length(vert_new_weak)));
minAvailableRange = ceil(min(min(min(oldVert(:,2)), abs(size(smoothedFrame,1)-max(oldVert(:)))),30))-2;
subImageRange = -minAvailableRange:minAvailableRange; % To consider only a smaller part of the gradient and intensity images

avgDiffValues = double(zeros(length(subImageRange),length(originalVert)));  % to store gradient data
avg2DiffValues = double(zeros(length(subImageRange),length(originalVert)));  % to store 2nd gradient data
avgIntensityValues = double(zeros(length(subImageRange),length(originalVert))); % to store Internsity data
avgDiffValues_final = double(zeros(length(subImageRange),length(originalVert))); % to store weighted average of intensity and gradient data
firstGradientImg = UpperOrLower*(firstGradientImg);
secondGradientImg = UpperOrLower*(secondGradientImg);

push = 3; % push the weal wall away from the strong wall
searchRange4WeakWall = [push:length(strongRange)]*UpperOrLower;  % search range for the weak wall
% disp = 0;
for  i= 1:1:length(originalVert)
    row = round(originalVert(i,2))+subImageRange;
    col = round(originalVert(i,1));
    avgDiffValues(:,i) = firstGradientImg(row,col);
    avg2DiffValues(:,i) = secondGradientImg(row,col);
    avgIntensityValues(:,i) = smoothedFrame(row,col);
    avgDiffValues_final(:,i) = (0.9 * avgDiffValues(:,i) + 0.0 * avg2DiffValues(:,i)+ 0.1 * avgIntensityValues(:,i));%...
end
avgDiffValues_final_linear = reshape(avgDiffValues_final,[size(avgDiffValues_final,1)*size(avgDiffValues_final,2) 1]);
[pksTotal,locsTotal] = findpeaks(avgDiffValues_final_linear); % find peaks of avgDiffValues_final
sz = size( avgDiffValues_final,1);
for i= 1:1:length(originalVert)
    %     i
    %[pks,locs] = findpeaks(avgDiffValues_final(:,i),'SortStr','descend'); % find peaks of avgDiffValues_final
    
    gyg = (locsTotal>=((i-1)*sz +1) & locsTotal<=((i-1)*sz +sz));
    pks = pksTotal(gyg) ;
    locs = locsTotal(gyg)- (i-1)*sz;
    % sort the peaks
    [pks t] = sort(pks,'descend');
    locs = locs(t);
    st = 0;
    locs1 = findSubsetPeaks(locs, st, length(subImageRange), strongRange); % find the peaks around the strong wall
    locsDist = abs (locs1 -  ceil(length(subImageRange)/2));
    [vv closestMax] = min(locsDist);
    if length(locs1)<1 
        st =    ceil(length(subImageRange)/2);
    elseif pks(closestMax)>=0.2*pks(1) && pks(1)>0.3 % both peaks are strong and strongest peak is geater than 0.5
        st = (min([locs1(closestMax), locs1(1)]*UpperOrLower))*UpperOrLower;%st = locs1(1);%
%     elseif pks(closestMax)>0.2*pks(1) && pks(closestMax)<0.5*pks(1)
%         st = locs1(1);%
    else
        st = locs1(closestMax);%st = (max(locs1*UpperOrLower))*UpperOrLower ;%st = (max([locs1(closestMax), locs1(1)]*UpperOrLower))*UpperOrLower ;%st = locs1(1);% 
        
    end
    strong(i) = subImageRange(st);
    
    %% finding the weak wall
    
    locs2 = findSubsetPeaks(locs,strong(i), length(subImageRange), searchRange4WeakWall);% finds the peaks around the weak wall
    % find the closest peak to the strongest peak;
    locsDist = abs (locs2 -  st);
    [vv closestMax] = min(locsDist);
    if length(locs2)<1 || vv >15 || vv<2 || pks(locs==locs2(closestMax))<(pks(1)*0.1)   % the weak point is far away from the strong edge by more than 10
        weak(i) = 0;
        continue
    end
    weak(i) = subImageRange(locs2(closestMax));
    pks = [];
    locs = [];
end

indd = weak==0;
vert_new_weak(indd,:)= [];
weak(indd)= [];

% adding the new moves (strong/weak) to the old walls
vert_new_weak(:,2) = vert_new_weak(:,2)+ weak' ;
vert_new_strong(:,2) = vert_new_strong(:,2)+ strong' ;

vert_new_weak(:,2) = smooth(vert_new_weak(:,2),0.4,'moving');
vert_new_strong(:,2) = smooth(vert_new_strong(:,2),0.4,'moving');

% % if the distance between the previous vert and the converged vert is more that 10 pixels then stick to the old vert
% if abs(mean(vert_new_strong(:,2)) - mean(oldVert(:,2)))>5
%     vert_new_strong(:,2) = oldVert(:,2);
% end

end