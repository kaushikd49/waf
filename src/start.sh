# run from src folder, otherwise segmentation fault
~/ludapache/bin/apxs -c -i -a ./mod_lud.c utils.c train_data.c -lm
~/ludapache/bin/apachectl stop
~/ludapache/bin/apachectl start
wget http://localhost/?fahsdf=dsafasd
