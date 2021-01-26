# rm /usr/local/nvidia/lib/ligmodelaggregator_host.so
cp -r secure-aggregation/server/build/host/libmodelaggregator_host.so /usr/local/nvidia/lib
cd secure-aggregation/python-package
python3 setup.py install
cd -
./startup/start.sh
