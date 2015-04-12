~/ludapache/bin/apxs -c -i -a ./mod_lud.c utils.c
~/ludapache/bin/apachectl stop
~/ludapache/bin/apachectl start
curl http://localhost/?fahsdf=dsafasd
