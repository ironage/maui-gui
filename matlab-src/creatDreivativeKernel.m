function [derivateKernel] = creatDreivativeKernel(initUp, kerWidth, kerHeight)
initUp = interpolateME1(initUp,1);
if round(min(initUp(:,2)))-1-floor(kerHeight/2)<1
    initUp(:,2)= initUp(:,2)+1-(round(min(initUp(:,2)))-1-floor(kerHeight/2));
end

derivateKernel = zeros(1000, kerWidth);
for i = 1: kerWidth
    derivateKernel(round(initUp(i,2))-floor(kerHeight/2):round(initUp(i,2))-1,i)= -1;
    derivateKernel(round(initUp(i,2))+1:round(initUp(i,2))+floor(kerHeight/2),i)= 1;
end
yMax = round(max(initUp(1:kerWidth,2)));
yMin = round(min(initUp(1:kerWidth,2)));
derivateKernel(yMax+round(kerHeight/2)+1:end,:) = [];
derivateKernel(1:yMin-round(kerHeight/2)-1,:) = [];
derivateKernel(:,kerWidth+1:end)=[];

end