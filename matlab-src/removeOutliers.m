function newVert = removeOutliers( refVert , currVert )
%% A function to remove currVert outliers based on the dominant orientation of refVert
% tang1 = atan(refVert(:,2));
% tang2 = atan(currVert(:,2));
% avgTang1 = mean(tang1);
y = currVert(:,2);
x = currVert(:,1);
append = y(1);
dy = diff(y);
for i = 1: round(length(y)*0.5) % 30 percent of th
    dy_abs = abs(dy);
   % dx = diff([ x(1); x ]);
    %slope = dy ./ dx;
    %slope(isnan(slope)) = 0;
    figure; title(['iteration ' num2str(i)]);
    subplot(311); plot(y, 'ko-');
    subplot(312); plot(dy, 'ro-');
    %subplot(313); plot(slope, 'bo-');
    if dy(dy_abs == max(dy_abs))>0
        append = y(1);
       % append = y(dy_abs == max(dy_abs));
        dy = diff([append; y]);
    else
        append = y(end);
        dy = diff([y ;append]);
    end
    y(dy_abs == max(dy_abs)) = [];
    x(dy_abs == max(dy_abs)) = [];
    pause();
    
end
newVert(:,1) = x;
newVert(:,2) = y;
end