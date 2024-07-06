function [kernelSet] = create2SidedKernelsBank_using_Line(thetaStep)
%% A function that creates a bank of steered kenels at angles starting from 0 to 180 degrees with a step equals to thetaStep.
% Inputs
% kernelHeight: the height (number of rows) of the kernel
% kernelWidth : the width (number of columns) of the kernel. Due to the
%               rotations applied to the kernels, both KernelHeight and
%               kernelWidth may vary a little bit from one kernel to
%               another in the kernel bank.
%edgeThickness: the thickness (in pixels) of the edge/wall to be detected
% Output
% kernelSet   : A cell array that has the steered kernels starting from
%               index 2. The first element of the cell array contains the
%               thetaStep variable
% This function is developed by Ahmed Gawish Feb. 2016
% Last Modified March 2nd 2016

% templateKernel = mat2gray(ones(1,kernelWidth));
% templateKernel =  padarray(templateKernel,[2 2],0);

kernelSet = cell(1,round(180/thetaStep)+2);
kernelSet{1} = thetaStep;
index = 2;
% myKernel = creatDreivativeKernel(topStrongPoints,40,3);
%     kernel_thin = createSteerableGaussKernel(50 ,5, 0.05,0);
%     kernel_thin = conv2(kernel_thin,[-1 1]', 'same');
%     kernel_thin(kernel_thin<0) = -1;
%     kernel_thin(kernel_thin>0) =  1;
    
    kernel = createSteerableGaussKernel(20 ,3, 0.3,0);
    kernel = conv2(kernel,[-1 1]', 'same');
    kernel(kernel<0)=-1;
    kernel(kernel>0)=1;
for theta = 0:thetaStep:89
    blur = fspecial('gaussian', length(kernel), 0.5*length(kernel));
    blur = blur/max(blur(:));
    kernel = kernel .* blur;
    kernelSet{index} = kernel;
    kernelSet{end-index+2} = fliplr(kernel);
    
    kernel = imrotate(kernelSet{2},theta);
%     kernel_thin_R = imrotate(kernel_thin,theta);
    
    index = index+1;
    
end

end