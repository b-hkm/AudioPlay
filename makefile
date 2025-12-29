TARGET=AudioPlay

CXX=g++

source = main.cpp

$(TARGET):
	$(CXX) $(source) -o $(TARGET) -O1

clean:
	@echo "cleaning..."
	rm $(TARGET)
