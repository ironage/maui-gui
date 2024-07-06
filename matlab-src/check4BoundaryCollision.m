function newVert = check4BoundaryCollision(currVert , height, width)
% A function to check whether the currentVert has any y values that are close 
% to (10 pixels) the upper or lower boudaries of the image, and modifiy them  
% Note: this function does not check for the collision that might happen at
% the two sides of the image
%
% Developed: Ahmed Gawish April 27, 2016

protectionMargin = 10;% how far the newVert should be away from the upper and lower boudaries of the image
miin = protectionMargin;% minimum allawable y
maax = height - protectionMargin;% maximum allawable y
newVert = currVert;
y = currVert(:,2);
y_temp = y;
yCloseIndxUp =  y < protectionMargin;
yCloseIndxDown = y > (height -protectionMargin);

y_temp(yCloseIndxUp) = 0; 
y_temp(yCloseIndxDown) = 0; 
y_temp(y_temp == 0) = [];
if min(y_temp)> protectionMargin; 
miin = min(y_temp);
end
if max(y_temp)< (height - protectionMargin)
maax = max(y_temp); 
end
% replacing the close y cooridantes
y(yCloseIndxUp)= miin;
y(yCloseIndxDown)=  maax; 
 newVert(:,2) = y;
end
