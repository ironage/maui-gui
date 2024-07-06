function [vert_new_strong, vert_new_weak ]  = findArteryWall_v3(smoothedFrame, firstGradientImg, secondGradientImg, oldVert, strongRange ,UpperOrLower)
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
    avgDiffValues(:,i) = firstGradientImg(round(originalVert(i,2))+subImageRange,round(originalVert(i,1)));
    avg2DiffValues(:,i) = secondGradientImg(round(originalVert(i,2))+subImageRange,round(originalVert(i,1)));
    avgIntensityValues(:,i) = smoothedFrame(round(originalVert(i,2))+subImageRange,round(originalVert(i,1)));
    
    avgDiffValues_final(:,i) = (0.7 * avgDiffValues(:,i) + 0.0 * avg2DiffValues(:,i)+ 0.3 * avgIntensityValues(:,i));%...
    [pks,locs] = findpeaks(avgDiffValues_final(:,i),'SortStr','descend'); % find peaks of avgDiffValues_final
    st = 0;
    locs1 = findSubsetPeaks(locs, st, length(subImageRange), strongRange); % find the peaks around the strong wall
    locsDist = abs (locs1 -  ceil(length(subImageRange)/2));
    [vv closestMax] = min(locsDist);
    if length(locs1)<1
        st =    ceil(length(subImageRange)/2);
    elseif pks(closestMax)>0.3*pks(1)
        st =locs1(closestMax);% st = locs1(1);%
    else
        st = locs1(1);
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
end

indd = weak==0;
vert_new_weak(indd,:)= [];
weak(indd)= [];

% adding the new moves (strong/weak) to the old walls
vert_new_weak(:,2) = vert_new_weak(:,2)+ weak' ;
vert_new_strong(:,2) = vert_new_strong(:,2)+ strong' ;

vert_new_weak(:,2) = smooth(vert_new_weak(:,2),0.5,'moving');
vert_new_strong(:,2) = smooth(vert_new_strong(:,2),0.5,'moving');

end