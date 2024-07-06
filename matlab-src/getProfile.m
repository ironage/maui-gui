function getProfile(img, minRange, MaxRange)
[x, y] = getline();
% figure, plot(img(round(y(1):y(end)),round(min(x))));
figure, plot([minRange: MaxRange], img(round(y(1)+minRange:y(1)+MaxRange),round(min(x))));
end