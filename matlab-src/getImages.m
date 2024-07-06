function [smoothedFrame , firstGradientImg, secondGradientImg] = getImages(currentFrame, ker, h)

smoothedFrame = imfilter(double(currentFrame),h ,'replicate');
% smoothedFrame = conv_fft2(double(currentFrame),h ,'reflect');
firstGradientImg = conv2(smoothedFrame,ker,'same');%* -UpperOrLower; % directional gradient image
% firstGradientImg = conv_fft2(smoothedFrame,ker,'same'); 
% secondGradientImg = conv2(firstGradientImg,ker,'same');
secondGradientImg = 0;
% normalization
firstGradientImg = (firstGradientImg./max(firstGradientImg(:)));
% firstGradientImg = circshift(firstGradientImg, sum(ker(:,10)==1));
% secondGradientImg = (secondGradientImg./max(secondGradientImg(:)));
smoothedFrame = smoothedFrame./max(smoothedFrame(:));
end