function [pksTotal locsTotal] = clacPeaks(smoothedFrame, firstGradientImg, secondGradientImg, oldVert)

avgDiffValues = double(zeros(length(subImageRange),length(oldVert)));  % to store gradient data
avg2DiffValues = double(zeros(length(subImageRange),length(oldVert)));  % to store 2nd gradient data
avgIntensityValues = double(zeros(length(subImageRange),length(oldVert))); % to store Internsity data
avgDiffValues_final = double(zeros(length(subImageRange),length(oldVert))); % to store weighted average of intensity and gradient data
firstGradientImg = UpperOrLower*(firstGradientImg);
secondGradientImg = UpperOrLower*(secondGradientImg);

for  i= 1:1:length(oldVert)
    row = round(oldVert(i,2))+subImageRange;
    col = round(oldVert(i,1));
    avgDiffValues(:,i) = firstGradientImg(row,col);
    avg2DiffValues(:,i) = secondGradientImg(row,col);
    avgIntensityValues(:,i) = smoothedFrame(row,col);
    avgDiffValues_final(:,i) = (0.9 * avgDiffValues(:,i) + 0.0 * avg2DiffValues(:,i)+ 0.1 * avgIntensityValues(:,i));%...
end
avgDiffValues_final_linear = reshape(avgDiffValues_final,[size(avgDiffValues_final,1)*size(avgDiffValues_final,2) 1]);
[pksTotal,locsTotal] = findpeaks(avgDiffValues_final_linear); % find peaks of avgDiffValues_final
end