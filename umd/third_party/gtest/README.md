# GoogleTest and GoogleMock libraries

Version used 1.11.0

## Generating

To create required files:

* Download googletest version 1.11.0 (https://github.com/google/googletest)
* Run python script to create fused version

```bash
wget https://github.com/google/googletest/archive/release-1.11.0.tar.gz
tar -zxvf release-1.11.0.tar.gz
cd googletest-release-1.11.0/gtest/googletest/scripts
python2 ./fuse_gmock_files.py <vpu-kmd>/validation/third_party/
```
