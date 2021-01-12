rm /usr/local/nvidia/lib/libmodelaggregator_host.so
cp -r secure-aggregation/server/build/host/libmodelaggregator_host.so /usr/local/nvidia/lib
cd secure-aggregation/client
python3 setup.py install
cd -
cd secure-aggregation/server/host
python3 setup.py install
cd -
./startup/start.sh

