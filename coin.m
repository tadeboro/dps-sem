pkg load image

# Load image and convert to grayscale
i1 = imread ("sample/coins.png");
i1g = rgb2gray(i1);
imshow (i1);
print("sample/coins-input.png", "-dpng");

# Remove background
i2 = 1 - (i1g < 230);
imshow (i2);
print("sample/coins-bg-rem.png", "-dpng");

# Calculate distance to the background
d = bwdist (i2);
i3 = d / max (max (d));
imshow (i3);
print("sample/coins-distance.png", "-dpng");

# Threshold distance
i4_a = i3 > .8; # Seeds for coins
i4_b = i3 == 0; # Seed for background
i4 = i4_a + i4_b;
imshow (i4);
print("sample/coins-seeds.png", "-dpng");

# Execute watershed
[i5, no_colors] = watershed (i1g, i4);
imshow (label2rgb (i5, jet (), "w", "shuffle"));
print("sample/coins-watershed.png", "-dpng");

# Extract objects
for k = 2:no_colors
  s = regionprops (i5 == k, "BoundingBox");
  bbox = int32(s(1).BoundingBox);
  l = bbox(1);
  r = bbox(1) + bbox(3) - 1;
  t = bbox(2);
  b = bbox(2) + bbox(4) - 1;
  mask = i5(t:b, l:r) == k;
  obj = i1(t:b, l:r, :) .* mask;
  imwrite (obj, ["sample/obj_" int2str(k) ".png"]);
end
