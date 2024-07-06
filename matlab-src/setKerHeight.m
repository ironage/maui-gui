function [kerUpHeight, kerBotHeight] = setKerHeight(upperWallThickness, lowerWallThickness)
maxHeight = 15;
kerUpHeight = ceil((maxHeight*upperWallThickness)/3)*3;
kerBotHeight = ceil((maxHeight*lowerWallThickness)/3)*3;
end