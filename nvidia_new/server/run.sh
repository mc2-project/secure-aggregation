rm /usr/local/nvidia/lib/ligmodelaggregator_host.so
cp -r kvah/server/build/host/libmodelaggregator_host.so /usr/local/nvidia/lib
cd kvah/client
python3 setup.py install
cd -
cd kvah/server/host
python3 setup.py install
cd -
./startup/start.sh

