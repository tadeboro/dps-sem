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

# Some test cases
i1 = imread("test/t1.png");
[w1 n1] = watershed (i1);
figure 1
subplot (2, 1, 1)
imshow (i1);
title("Vhodna slika");
subplot (2, 1, 2)
imshow (ind2rgb (w1, map));
title("Rezultat samodejne izvedbe algoritma");
print("test/t1-result.png", "-dpng");

i2 = imread("test/t2.png");
[w2 n2] = watershed (i2);
figure 2
subplot (2, 1, 1)
imshow (i2);
title("Vhodna slika");
subplot (2, 1, 2)
imshow (ind2rgb (w2, map));
title("Rezultat samodejne izvedbe algoritma");
print("test/t2-result.png", "-dpng");

i3 = imread("test/t3.png");
[w3 n3] = watershed (i3);
figure 3
subplot (2, 1, 1)
imshow (i3);
subplot (2, 1, 2)
imshow (ind2rgb (w3, map));
title("Rezultat samodejne izvedbe algoritma");
print("test/t3-result.png", "-dpng");

i4 = imread ("test/t4.png");
[w4b n4b] = watershed (i4);
figure 4
subplot (2, 1, 1)
imshow (i4);
title("Vhodna slika");
subplot (2, 1, 2)
imshow (ind2rgb (w4b, map));
title("Rezultat samodejne izvedbe algoritma");
print("test/t4-result.png", "-dpng");

i4s = imread ("test/t4init.png");
[w4 n4] = watershed (i4, i4s);
figure 5
subplot (3, 1, 1)
imshow (i4);
title("Vhodna slika");
subplot (3, 1, 2)
imshow (i4s);
title("Izvori");
subplot (3, 1, 3)
imshow (ind2rgb (w4, map));
title("Rezultat vodene izvedbe algoritma");
print("test/t4-seed-1-result.png", "-dpng");

i4k = imread ("test/t4initalter.png");
[w5 n5] = watershed (i4, i4k);
figure 6
subplot (3, 1, 1)
imshow (i4);
title("Vhodna slika");
subplot (3, 1, 2)
imshow (i4k);
title("Izvori");
subplot (3, 1, 3)
imshow (ind2rgb (w5, map));
title("Rezultat vodene izvedbe algoritma");
print("test/t4-seed-2-result.png", "-dpng");
