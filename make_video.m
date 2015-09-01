#!/usr/bin/env octave

# Arguments
arguments = argv ();
file  = arguments{1};
video = arguments{2};
from  = str2double(arguments{3});
do_mask = false
no_subplots = 2
if size (arguments, 1) > 3
  mask = arguments{4}
  do_mask = true
  no_subplots = 3
end

# Prepare environment
rmdir ("tmp", "s");
mkdir "tmp";

# Color map, used for displaying basins
map = [
  1.0 0.0 0.0
  0.0 1.0 0.0
  0.0 0.0 1.0
  1.0 1.0 0.0
  1.0 0.0 1.0
  0.0 1.0 1.0
  1.0 1.0 1.0
  0.5 0.5 0.5
];

# Create video frames
i2 = imread (file);
s = size(i2);
limit = s(1) * s(2)
subplot (no_subplots, 1, 1)
imshow (i2);
title("Vhodna slika");
if do_mask
  m2 = imread (mask);
  subplot (no_subplots, 1, 2)
  imshow (m2);
  title("Izvori");
end
subplot (no_subplots, 1, no_subplots)
for k = from:limit
  if do_mask
    [w2 n2] = watershed (i2, m2, k);
  else
    [w2 n2] = watershed (i2, k);
  end
  imshow (ind2rgb (w2, map));
  title("Rezultat izvedbe algoritma");
  print(["tmp/result-" int2str(k) ".png"], "-dpng");
end

# Create video
system (["avconv -f image2 -start_number " int2str(from) " -i tmp/result-%d.png " video]);
