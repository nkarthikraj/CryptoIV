# CryptoIV
command used to compile the flatbuffers
---------------------------------------
./flatc.exe --binary --schema --cpp --gen-mutable PropertyTree.fbs

Client send data stream continuesly thrh tcp socket.<br>
Server recevies data stream and prints it in the screen.<br>
boost asio library is used for managing socket.<br>

