function [smoothKernel, derivateKernel, topStrongLine, botStrongLine, topRefWall, botRefWall] =  setup(topStrongPoints, botStrongPoints)

topStrongLine = interpolateME1(topStrongPoints,1);
botStrongLine = interpolateME1(botStrongPoints,1);
topRefWall = topStrongLine;
botRefWall = botStrongLine;
%  topRefWall(:,2) = topRefWall(:,2)+10; 
%  botRefWall(:,2) = botRefWall(:,2)-10; 
thetaStep = 10;
kernelSet = create2SidedKernelsBank_using_Line(thetaStep);
dominantOrintation = getDominantOrintation(topStrongLine);
%derivateKernel = kernelSet{mod(round(dominantOrintation/thetaStep), round(180/thetaStep))+2}; % select the best steerable kernel that will be used to calculate the directional gradient
derivateKernel = creatDreivativeKernel(topStrongPoints,20,15);
% derivateKernel{1} = creatDreivativeKernel(topStrongPoints,40,heightUp);
% derivateKernel{2} = creatDreivativeKernel(topStrongPoints,40,3);
smoothKernel = createSteerableGaussKernelNEW(topStrongPoints,10,3); %createSteerableGaussKernel(20 ,10, 0.5,dominantOrintation); 
end