function h = createSteerableGaussKernel(size ,std1, std2,theta)
padMatrix = 50;
h = fspecial( 'gaussian', [1 size+padMatrix], std1 );
v = fspecial( 'gaussian', [size+padMatrix 1], std2 ); % vertical filter
f1 = v * h;
f = imrotate(f1,theta,'bilinear', 'crop');
% h= f;
h = f(padMatrix/2:end-padMatrix/2,padMatrix/2:end-padMatrix/2);
% blur = fspecial('gaussian', size(h), 5);
% h = h .* blur;

% h = imrotate(f,theta);
%whos h
%m = imresize(h,[5 5]);
% figure, surf(h);

%%% new try

end