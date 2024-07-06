% A function that detects the first moving frame in a video
% INPUTS:
% currentVelocityFrame and previousVelocityFrame are two consecutive frames
% of the video( e.g. 50, 49)
% indx: the frame number of currentVelocityFrame
% OUTPUT:
% ind: the index of the current frame if a movement detected, else it is -1 
% developed by Ahmed Gawish January 2017

function ind = check4FirstMovingFrame(currentVelocityFrame,previousVelocityFrame, indx)
imgDiff = abs(currentVelocityFrame - previousVelocityFrame);
imgDiff = im2bw(mat2gray(imgDiff), graythresh(mat2gray(imgDiff)));
amonutChange = sum(imgDiff(:));
if amonutChange <10 % less than 10 pixels
    ind = -1;
else
    ind = indx;
end

end