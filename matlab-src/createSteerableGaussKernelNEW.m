function smoothKernel = createSteerableGaussKernelNEW(initUp, kerWidth, kerHeight)
initUp = interpolateME1(initUp,1);
if round(min(initUp(:,2)))-1-floor(kerHeight/2)<1
    initUp(:,2)= initUp(:,2)+1-(round(min(initUp(:,2)))-1-floor(kerHeight/2));
end

smoothKernel = zeros(1000, kerWidth);
for i = 1: kerWidth
    smoothKernel(round(initUp(i,2))-floor(kerHeight/2):round(initUp(i,2)),i)= 1;
    smoothKernel(round(initUp(i,2)):round(initUp(i,2))+floor(kerHeight/2),i)= 1;
end
yMax = round(max(initUp(1:kerWidth,2)));
yMin = round(min(initUp(1:kerWidth,2)));
smoothKernel(yMax+round(kerHeight/2):end,:) = [];
smoothKernel(1:yMin-round(kerHeight/2),:) = [];
smoothKernel(:,kerWidth+1:end)=[];

end
