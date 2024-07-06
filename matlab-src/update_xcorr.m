function [topStrongLine, botStrongLine, OLD, topWeakLine, topIMT, botWeakLine, botIMT, topWallRef, botWallRef, signature] =...
    update_xcorr(frame, smoothKernel, derivateKernel, topStrongLine, botStrongLine, topStrongPoints, botStrongPoints, topWallRef, botWallRef, signature)
% temporarly: set the unused parameters to zero

% if size(frame,3)>1
%     frame = rgb2gray(frame);
% end
frame = single(frame) / single(max(frame(:)));

[smoothedFrame, firstGradient, secondGradient] = getImages(frame, derivateKernel, smoothKernel);
[topWallRef, botWallRef ] = findBigBlackSpotNew(smoothedFrame ,topStrongLine, botStrongLine );%topStrongPoints, botStrongPoints);

topStrongLine = topWallRef;
botStrongLine = botWallRef;
topStrongLine = cleanUpVert(topWallRef, topStrongLine, 1);
%    range = getBestSearchRange(topWallRef, topStrongLine,1);
range = -15:15;
%  topStrongLine = classifyWallNoise(topStrongLine);
%%%for iter = 1:2    
 %%%   topStrongLine = makeParallel2Init(topStrongLine, topStrongPoints);
    [ topStrongLine, ~, signature ] = findArteryWall_v7_imt_xcorr(smoothedFrame, firstGradient, secondGradient, topStrongLine ,range, 1, 0, signature);
%      topStrongLine = cleanUpVert(topWallRef, topStrongLine, 1);
 %%%   topStrongLine = classifyWallNoise(topStrongLine);
  %%%   range = round(min(range/2)):max(range);
%%%end

%%%range = -2:2;
%%%ker = creatDreivativeKernel(topStrongPoints,9,3);
%%%firstGradient1 = conv2(smoothedFrame,ker,'same');
%%%[ topStrongLine, topWeakLine, signature ] = findArteryWall_v7_imt_xcorr(smoothedFrame, firstGradient1, secondGradient, topStrongLine ,range, 1, 1, signature);

% botStrongLine = cleanUpVert(botWallRef, botStrongLine, -1);
% range = getBestSearchRange(botWallRef, botStrongLine, -1);  
%%%range = -15:15;
%  botStrongLine = classifyWallNoise(botStrongLine);

%%%for iter = 1:2
  %%%  botStrongLine = makeParallel2Init(botStrongLine, botStrongPoints);
    [ botStrongLine, ~, signature ] = findArteryWall_v7_imt_xcorr(smoothedFrame, firstGradient, secondGradient, botStrongLine, range , -1, 0, signature);
%        botStrongLine = cleanUpVert(botWallRef, botStrongLine, -1);
 %%%    botStrongLine = classifyWallNoise(botStrongLine);
%     range = min(range):round (max(range)/2);
%%%end

%%%range = -2:2;
%%%[ botStrongLine, botWeakLine, signature ] = findArteryWall_v7_imt_xcorr(smoothedFrame, firstGradient1, secondGradient, botStrongLine, range , -1, 1, signature);
%%%
%%%if ~isempty(topStrongLine)&& ~isempty(botStrongLine)
  %%%   OLD = findDistance(topStrongLine, botStrongLine);
  %%%   if length(topWeakLine)>5
  %%%      topIMT = findDistance(topStrongLine, topWeakLine);
  %%%   else
 %%%        topIMT = 0;
%%%     end
%%%     if length(botWeakLine)>5
%%%        botIMT = findDistance(botStrongLine, botWeakLine);
%%%     else
%%%         botIMT = 0;
%%%     end
%%%else
%%%    OLD = 0;
%%%end
 OLD = 0;
 topIMT = 0;
botIMT = 0;
topWeakLine = 0;
botWeakLine = 0;
end