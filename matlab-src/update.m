function [topStrongLine, botStrongLine, OLD, topWeakLine, topIMT, botWeakLine, botIMT, topWallRef, botWallRef] =...
    update(frame, smoothKernel, derivateKernel, topStrongLine, botStrongLine, topStrongPoints, botStrongPoints, topWallRef, botWallRef)
% temporarly: set the unused parameters to zero

% if size(frame,3)>1
%     frame = rgb2gray(frame);
% end
frame = single(frame) / single(max(frame(:)));

distBtwMediaWalls = abs( mean(topStrongLine(:, 2)) - mean(botStrongLine(:, 2)));
[smoothedFrame, firstGradient, secondGradient] = getImages(frame, derivateKernel, smoothKernel);
[topWallRef, botWallRef ] = findBigBlackSpotNew(smoothedFrame ,topStrongLine, botStrongLine );%topStrongPoints, botStrongPoints);

topStrongLine = topWallRef;
botStrongLine = botWallRef;
topStrongLine = cleanUpVert(topWallRef, topStrongLine, 1);
%    range = getBestSearchRange(topWallRef, topStrongLine,1);
range = -15:15;
%  topStrongLine = classifyWallNoise(topStrongLine);
for iter = 1:2    
    topStrongLine = makeParallel2Init(topStrongLine, topStrongPoints);
    [ topStrongLine, ~ ] = findArteryWall_v7_imt(smoothedFrame, firstGradient, secondGradient, topStrongLine ,range, 1, 0, distBtwMediaWalls);
%      topStrongLine = cleanUpVert(topWallRef, topStrongLine, 1);
    topStrongLine = classifyWallNoise(topStrongLine);
     range = round(min(range/2)):max(range);
end

range = -2:2;
ker = creatDreivativeKernel(topStrongPoints,5,3);
smoothKernel = createSteerableGaussKernelNEW(topStrongPoints,5,1);
[smoothedFrame1, firstGradient1, ~] = getImages(frame, ker, smoothKernel);
% smoothedFrame = conv2(frame,smoothKernel,'same');
% firstGradient1 = conv2(smoothedFrame,ker,'same');

[ topStrongLine, topWeakLine ] = findArteryWall_v7_imt(smoothedFrame1, firstGradient1, secondGradient, topStrongLine ,range, 1, 1, distBtwMediaWalls);

botStrongLine = cleanUpVert(botWallRef, botStrongLine, -1);
% range = getBestSearchRange(botWallRef, botStrongLine, -1);  
range = -15:15;
%  botStrongLine = classifyWallNoise(botStrongLine);

for iter = 1:2
    botStrongLine = makeParallel2Init(botStrongLine, botStrongPoints);
    [ botStrongLine, ~ ] = findArteryWall_v7_imt(smoothedFrame, firstGradient, secondGradient, botStrongLine, range , -1, 0, distBtwMediaWalls);
%        botStrongLine = cleanUpVert(botWallRef, botStrongLine, -1);
     botStrongLine = classifyWallNoise(botStrongLine);
    range = min(range):round (max(range)/2);
end

range = -2:2;
[ botStrongLine, botWeakLine ] = findArteryWall_v7_imt(smoothedFrame1, firstGradient1, secondGradient, botStrongLine, range , -1, 1, distBtwMediaWalls);

if ~isempty(topStrongLine)&& ~isempty(botStrongLine)
     OLD = findDistance(topStrongLine, botStrongLine);
     if length(topWeakLine)>5
        topIMT = findDistance(topStrongLine, topWeakLine);
     else
         topIMT = 0;
     end
     if length(botWeakLine)>5
        botIMT = findDistance(botStrongLine, botWeakLine);
     else
         botIMT = 0;
     end
else
    OLD = 0;
end
%  OLD = 0;
%  topIMT = 0;
% botIMT = 0;
end