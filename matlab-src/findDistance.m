function avgDist = findDistance(top, bottom)
% A function to calculate the distance between 2 given walls.
p_bot = polyfit(bottom(:,1),bottom(:,2),1);
if(isempty(top)|| isempty(bottom))
    avgDist = 0;
end

% find commom points between the two lines
[xCommon,iTop,iBottom] = intersect(top(:,1),bottom(:,1));
step = ceil(length(xCommon)*0.1); %
topLarge = interpolateMetoSpecificWidth(top,1,[-1000:1000]);
if isempty(xCommon)% no intersection between the two lines
    avgDist = 0;
else % there is an overlapping between top and bottom
    % find bottom line slop
    
    bottomPerpen = -1/p_bot(1);
    %     for i = 1 : step : length(xCommon)
    y_coor = bottom(iBottom,2)';
    y_coor_mat = repmat(y_coor,[length(topLarge),1]);
    deltaY = bsxfun(@minus, y_coor_mat, topLarge(:,2));%(y_coor_mat-diag(top(:,2))*ones(size(y_coor_mat)));
    
    x_coor = bottom(iBottom,1)';
    x_coor_mat = repmat(x_coor,[length(topLarge),1]);
    deltaX = bsxfun(@minus, x_coor_mat, topLarge(:,1));%(x_coor_mat-diag(top(:,1))*ones(size(x_coor_mat)));
    slopAll = (deltaY./deltaX)-bottomPerpen; 
    [~, ff] = min(abs(slopAll), [], 1);
    avgDist =  mean((sqrt((bottom(iBottom,1)- topLarge(ff,1)).^2 + (bottom(iBottom,2)- topLarge(ff,2)).^2)));%mean(abs(bottom(iBottom,2)- topLarge(ff,2)));
    
end

