function [wallKerUp, wallKerBot] = refWallKernel(img, initUp, initBot, kerHeight)

imgWidth = [1 : size(img,2)];
initUp = interpolateMetoSpecificWidth(initUp,1, imgWidth);
initBot = interpolateMetoSpecificWidth(initBot,1, imgWidth);

if round(min(initUp(:,2)))-1-floor(kerHeight/2)<1
    initUp(:,2)= initUp(:,2)+1-(round(min(initUp(:,2)))-1-floor(kerHeight/2));
    disp('the ROI is not covering enough area above the upper Artery. It is recommended to re-select your ROI');
%      error('Program exit')
end
if round(max(initBot(:,2)))+1+floor(kerHeight/2)> size(img,1)
    initBot(:,2) =  initBot(:,2)-1- ((round(max(initBot(:,2)))+1+floor(kerHeight/2))-size(img,1));
    disp('the ROI is not covering enough are under the lower Artery. It is recommended to re-select your ROI');
end

% create a bigger template for the kernel;
wallKerUp = double(zeros(size(img)));
wallKerBot = wallKerUp;

yMax = round(max(initUp(:,2)));
yMin = round(min(initUp(:,2)));

% assign value 1 to the pixels that lie between the two initialization
% lines  
for i = 1 : size(img,2)
    wallKerUp(round(initUp(i,2))-1-floor(kerHeight/2):round(initUp(i,2))-1,i) = +1;
    wallKerUp(round(initUp(i,2)),i) = 0;
    wallKerUp(round(initUp(i,2))+1:round(initUp(i,2))+1+floor(kerHeight/2),i) = -1;
    
    wallKerBot(round(initBot(i,2))-1-floor(kerHeight/2):round(initBot(i,2))-1,i) = -1;
    wallKerBot(round(initBot(i,2)),i) = 0;
    wallKerBot(round(initBot(i,2))+1:round(initBot(i,2))+1+floor(kerHeight/2),i) = +1;
end

wallKerUp(yMax+round(kerHeight/2)+1:end,:) = [];
wallKerUp(1:yMin-round(kerHeight/2)-1,:) = [];

yMax = round(max(initBot(:,2)));
yMin = round(min(initBot(:,2)));

wallKerBot(yMax+round(kerHeight/2)+1:end,:)  = [];
wallKerBot(1:yMin-round(kerHeight/2)-1,:) = [];

end
