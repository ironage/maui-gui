function newSig = removeBadSignal(noisySig, locs2remove)

smallestDistBtwBadFrames = 5;
distBetweenBadFrames = diff(locs2remove);
indx = find(distBetweenBadFrames <= smallestDistBtwBadFrames) +1 ;


for x0 = 1:length(indx)
    noisySig(locs2remove(indx(x0)-1):locs2remove(indx(x0))) = NaN; % Weired number
end

% sig(sig == -10) = NaN;
newSig = noisySig;

end