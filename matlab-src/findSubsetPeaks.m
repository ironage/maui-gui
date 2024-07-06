function [subsetPeaksIndx] = findSubsetPeaks(peaks, previousPeak, bigRangeMiddle, smallRangeMin, smallRangeMax)
%% A function that finds sub-peaks in a larger array of peaks
%Inputs
% peaks        : a list of peak locations in the signal
% previousPeak : the location of the previous peak 
% bigRange     : the range of search for the large signal
% smallRange   : the range in which the new subpeaks will be found
%subsetPeaks   : the found peaks in the smallRange area.
subsetPeaksIndx = peaks>= (bigRangeMiddle+previousPeak+smallRangeMin) & peaks <= (bigRangeMiddle+previousPeak+smallRangeMax);
