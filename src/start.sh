# run from src folder, otherwise segmentation fault
~/ludapache/bin/apxs -c -i -a ./mod_lud.c utils.c train_data.c
~/ludapache/bin/apachectl stop
~/ludapache/bin/apachectl start
curl http://localhost/?fahsdf=dsafasd
