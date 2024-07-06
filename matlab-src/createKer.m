function kernel = createKer()
size_of_kerel = 15;
sigma_gaussian_blur = (1/2)*size_of_kerel;
kernel_blur = fspecial('gaussian', size_of_kerel, sigma_gaussian_blur);
kernel_edge = [ -1*ones((size_of_kerel-1)/2, size_of_kerel); ...
    zeros(1, size_of_kerel); ...
    1*ones((size_of_kerel-1)/2, size_of_kerel)];
kernel = kernel_edge .* kernel_blur;
end