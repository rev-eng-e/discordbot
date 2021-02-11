magick %1.%2 -resize 50%% %1-256.%2
magick %1-256.%2 -resize 50%% %1-128.%2
magick %1-128.%2 -resize 50%% %1-64.%2
magick %1-64.%2 -resize 50%% %1-32.%2
magick %1-32.%2 -resize 50%% %1-16.%2