protoc.exe --cpp_out=. .\src\proto.proto
copy afx.txt + .\src\proto.pb.h inc.h
copy inc.h .\src\proto.pb.h 

