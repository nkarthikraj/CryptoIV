# CryptoIV
command used to compile the flatbuffers
---------------------------------------
./flatc.exe --binary --schema --cpp --gen-mutable PropertyTree.fbs

Client send data stream continuesly thrh tcp socket.
Server recevies data stream and prints it in the screen.
boost asio library is used for managing socket.
