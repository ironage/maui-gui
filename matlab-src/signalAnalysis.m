function signalAnalysis(sig, confidenceLevel)
% make sure the sigmal has the proper length for the wavelet analysis
numWaveletLevels = 4;
smallestDistBtwBadFrames = 5;
if mod(length(sig), 2^numWaveletLevels)~=0
    pad = ceil(length(sig)/ 2^numWaveletLevels) * 2^numWaveletLevels - length(sig);
%     pad = 2^numWaveletLevels - mod(length(sig), 2^numWaveletLevels) + 1;
    sig = padarray(sig ,[pad 0],'symmetric','pre');
end
sigMean = mean(sig);
sigVar = sig  - sigMean;
% scaling
sigVar = scaledata(sigVar, 0, 1);
sigVar = sigVar';
swc = swt(sig,numWaveletLevels,'sym8');
% figure,
% hold on
% subplot(6,1,1), plot(sig)
% for i = 2 : 6
%     subplot(6,1,i), plot(swc(i-1,:))
% end
% scaling
waveletSig = swc(1,:);
waveletSig = scaledata(waveletSig, 0, 1);
waveletSig = waveletSig* - 1;
totalSig = 1.0* waveletSig  + 0.0*sigVar;
[pks,locs] = findpeaks(totalSig, 'SortStr', 'descend');

% confidenceLevel = 0.60;
numPks2remove  = ceil((1-confidenceLevel)*length(pks));
pks2remove = pks(1:numPks2remove);
locs2remove = locs(1:numPks2remove);
% subplot(211), plot(sig);
% plot(sig, 'r');
% hold on
% plot(locs2remove, sig(locs2remove), 'or');
yl = ylim;
[locs2remove, t] = sort(locs2remove);
pks2remove = pks2remove(t);
% hold off;
%
% subplot(212), plot(waveletSig);
% hold on;
% plot(locs2remove, pks2remove, 'or');

distBetweenBadFrames = diff(locs2remove);
indx = find(distBetweenBadFrames <= smallestDistBtwBadFrames) +1 ;


for x0 = 1:length(indx)
    
    rectangle('position',[locs2remove(indx(x0)-1) yl(1) (locs2remove(indx(x0))-locs2remove(indx(x0)-1)) diff(yl)]  ,'EdgeColor','b');
    
end

newSig = removeBadSignal(sig, locs2remove);
plot(find(isnan(newSig)==1) , sig(find(isnan(newSig)==1)), 'xb');

end