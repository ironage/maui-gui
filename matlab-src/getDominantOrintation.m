function [dominantOrintation] = getDominantOrintation(vert)

% calculating the derivative of the input contour to get the directio of
% the tangent at each point
p = polyfit(vert(:,1),max(vert(:,2))-vert(:,2),1);% max(vert(:,2))-vert(:,2) filps vert upside down so that it looks like when is imposed on the image
k = polyder(p);
dev = zeros(size(vert));
dev(:,1) = vert(:,1);
dev(:,2) = polyval(k,dev(:,1));
tang = atan(dev(:,2));
tangDeg = radtodeg(tang)+180; % tangDeg is the direction of the tangent at each point of the contour
dominantOrintation = mode(round(tangDeg)); % dominant orientation of the whole wall
end