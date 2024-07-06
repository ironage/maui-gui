
clear; clc; close all;
load video02

newData = 0;
if newData
    figure; imshow(data(:,:,1));
    [x, y] = ginputc;
    save dataSoFar;
else
    load dataSoFar;
end

imshow(data(:,:,1)); hold on;
plot(x,y,'ro', 'linewidth', 2);
plot(mean(x),mean(y),'mo', 'linewidth', 2);


dy = diff([ y(1); y ]);
dx = diff([ x(1); x ]);
slope = dy ./ dx;
slope(isnan(slope)) = 0;
% plot(dy/dx);

% kernel = [1 -1];
% gradSignal = conv(y,kernel,'same');

figure;
subplot(211); plot(y, 'ko-');
subplot(212); plot(slope, 'bo-');


figure; scatterhist(x,y)

P = polyfit(x,y,1);
yfit = polyval(P,x);
plot(x,yfit,'bo', 'linewidth', 2);
