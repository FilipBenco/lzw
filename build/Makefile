CXX           = g++
CXXFLAGS      = -m32 -pipe -g -Wall -W -D_REENTRANT $(DEFINES)
FILES         = ../main.cpp \
		../decoder.cpp \
		../encoder.cpp 
TARGET        = LZW
TESTFILES     = output.bmp \
                output.lzw


first: all

####### Build rules

all:
	$(CXX) $(CXXFLAGS) $(FILES) -o $(TARGET)
clean:
	rm -f $(TARGET)
	rm -f $(TESTFILES)
