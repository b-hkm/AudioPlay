TARGET=AudioPlay

CXX=g++

source = main.cpp

$(TARGET):
	$(CXX) $(source) -o $(TARGET)

clean:
	@echo "cleaning..."
	rm $(TARGET)
