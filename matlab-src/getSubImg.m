function [smallerImg , xMin, yMin, xWidth, yHeight] = getSubImg(img)
set(gcf, 'units','normalized','outerposition',[0 0 1 1]);
imshow(img);
h = imrect; setColor(h,'red')
position = wait(h);
cMin = uint32(position(1));
rMin = uint32(position(2));
cMax = uint32(position(1)) + uint32(position(3));
rMax = uint32(position(2)) + uint32(position(4));
delete(h);
ROI = img(rMin:rMax,cMin:cMax);

if  rMax> size(img,1) || cMax > size(img,2)
    disp(' the rectangle dimensions exceed image dimension' );
    smallerImg = [];
else
    smallerImg = img(rMin:rMax, cMin:cMax);
end
xMin = uint32(position(1));
yMin = uint32(position(2));
xWidth = uint32(position(3));
yHeight = uint32(position(4));
end